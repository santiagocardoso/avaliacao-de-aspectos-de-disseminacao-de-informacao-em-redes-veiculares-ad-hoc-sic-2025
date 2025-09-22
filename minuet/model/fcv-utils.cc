#include "fcv-utils.h"

#include <cmath>
#include <ctime>
#include <chrono>

#include "ns3/simulator.h"
#include "ns3/vector.h"

#include <fstream>
#include <string>
#include <vector>
#include <map>

void LogAllScores(const std::string& methodName, 
                  const std::vector<int>& ids, 
                  const std::vector<std::vector<double>>& criteriaMatrix, 
                  const std::vector<double>& finalScores, 
                  uint64_t tempo) 
{
    const std::string folderPath = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/";
    const std::string filePath = folderPath + "score_history_" + methodName + ".csv";

    std::ofstream logFile(filePath, std::ios::app);
    
    if (!logFile.is_open()) {
        std::cerr << "ERRO: Não foi possível abrir o arquivo de log para escrita: " << filePath << std::endl;
        return;
    }
    
    logFile.seekp(0, std::ios::end);
    if (logFile.tellp() == 0) {
        logFile << "ns,ID,N,D,AS,PH,DT,"
                << "KM,LD,F,YV,AP,"
                << "CS,TQ,FE,VCF\n";
    }
    
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i >= criteriaMatrix.size() || i >= finalScores.size()) continue;
        logFile << tempo << "," << ids[i];
        for (const double& criterionValue : criteriaMatrix[i]) {
            logFile << "," << criterionValue;
        }
        logFile << "," << finalScores[i] << "\n";
    }

    logFile.close();
}

// Veículos leves (Potências x Faixas etárias)
//                                  P | (18-29 30-39 40-49 50-59 60-69 70-79 80+) 
float faixa_matrix_leves[20][8] = {{0  , 18  , 30  , 40  , 50  , 60  , 70  , 80},
								   {60 , 1   , 1   , 1   , 1   , 0.75, 0.75, 0.25},
								   {70 , 1   , 1   , 1   , 1   , 0.75, 0.75, 0.25},
								   {80 , 1   , 1   , 1   , 1   , 0.75, 0.75, 0.25},
								   {90 , 1   , 1   , 1   , 1   , 0.75, 0.75, 0.25},
								   {100, 1   , 1   , 1   , 1   , 0.75, 0.75, 0.25},
								   {110, 0.75, 1   , 1   , 1   , 0.75, 0.75, 0.25},
								   {120, 0.75, 1   , 1   , 1   , 0.75, 0.75, 0.25},
								   {130, 0.75, 1   , 1   , 1   , 0.75, 0.5 , 0.25},
								   {140, 0.75, 1   , 1   , 1   , 0.75, 0.5 , 0.25},
								   {150, 0.75, 1   , 1   , 1   , 0.75, 0.5 , 0.25},
								   {160, 0.5 , 1   , 1   , 1   , 0.5 , 0.5 , 0   },
								   {170, 0.5 , 1   , 1   , 1   , 0.5 , 0.5 , 0   },
	 							   {180, 0.5 , 0.75, 1   , 1   , 0.5 , 0.5 , 0   },
		 						   {190, 0.25, 0.75, 1   , 1   , 0.5 , 0.5 , 0   },
			 					   {200, 0.25, 0.75, 1   , 0.75, 0.5 , 0.25, 0   },
				 				   {225, 0   , 0.75, 1   , 0.75, 0.25, 0   , 0   },
					 			   {250, 0   , 0.5 , 0.75, 0.75, 0.25, 0   , 0   },
						 		   {275, 0   , 0.5 , 0.75, 0.75, 0.25, 0   , 0   },
							 	   {300, 0   , 0.5 , 0.75, 0.75, 0.25, 0   , 0   },
};

// Veículos pesados (Potências x Faixas etárias)
//                                 (18-29   30-49  50-59  60-64  65-66  67-69  70+)
float faixa_matrix_pesados[2][7] = {{18    , 30   , 50   , 60   , 65   , 67   , 70},
                                    {0.75  , 1    , 1    , 0.75 , 0.5  , 0.25 , 0}};

void printMatrix(vector<vector<double>> &matrix) {
	for (const auto &row : matrix) {
		for (double val : row)
			cout << val << " ";
		cout << endl;
	}
}

bool createDirectory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (mkdir(path.c_str(), 0777) != 0) {
            return false;
        }
    }
    return true;
}

double VectorDistance(const ns3::Vector &v1, const ns3::Vector &v2) {
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

double mapeamentoRodoviaRua(double x, double y) {
    if ((x >= 0 && x < 1400) && (y >= 0 && y < 1333))
        return 1;
    else if ((x >= 1400 && x < 2800) && (y >= 0 && y < 1333))
        return 1;
    else if ((x >= 2800 && x < 4200) && (y >= 0 && y < 1333))
        return 1;
    else if ((x >= 4200 && x < 5600) && (y >= 0 && y < 1333))
        return 1;
    else if ((x >= 5860 && x < 7000) && (y >= 0 && y < 1333))
        return 1;
	else if ((x >= 7000 && x < 8400) && (y >= 0 && y < 1333))
        return 0;
    else if ((x >= 8400 && x < 9800) && (y >= 0 && y < 1333))
        return 1;
    else if ((x >= 9800 && x < 11200) && (y >= 0 && y < 1333))
        return 0;
    else if ((x >= 11200 && x < 12600) && (y >= 0 && y < 1333))
        return 1;
    else if ((x >= 12600 && x <= 14000) && (y >= 0 && y < 1333))
        return 1;
    else if ((x >= 0 && x < 1400) && (y >= 1333 && y < 2666))
        return 1;
    else if ((x >= 1400 && x < 2800) && (y >= 1333 && y < 2666))
        return 1;
    else if ((x >= 2800 && x < 4200) && (y >= 1333 && y < 2666))
        return 1;
    else if ((x >= 4200 && x < 5600) && (y >= 1333 && y < 2666))
        return 1;
    else if ((x >= 5860 && x < 7000) && (y >= 1333 && y < 2666))
        return 1;
	else if ((x >= 7000 && x < 8400) && (y >= 1333 && y < 2666))
        return 0;
    else if ((x >= 8400 && x < 9800) && (y >= 1333 && y < 2666))
        return 1;
    else if ((x >= 9800 && x < 11200) && (y >= 1333 && y < 2666))
        return 0;
    else if ((x >= 11200 && x < 12600) && (y >= 1333 && y < 2666))
        return 1;
    else if ((x >= 12600 && x <= 14000) && (y >= 1333 && y < 2666))
        return 1;
    else if ((x >= 0 && x < 1400) && (y >= 2666 && y < 4000))
        return 1;
    else if ((x >= 1400 && x < 2800) && (y >= 2666 && y < 4000))
        return 1;
    else if ((x >= 2800 && x < 4200) && (y >= 2666 && y < 4000))
        return 1;
    else if ((x >= 4200 && x < 5600) && (y >= 2666 && y < 4000))
        return 1;
    else if ((x >= 5860 && x < 7000) && (y >= 2666 && y < 4000))
        return 1;
	else if ((x >= 7000 && x < 8400) && (y >= 2666 && y < 4000))
        return 1;
    else if ((x >= 8400 && x < 9800) && (y >= 2666 && y < 4000))
        return 1;
    else if ((x >= 9800 && x < 11200) && (y >= 2666 && y < 4000))
        return 1;
    else if ((x >= 11200 && x < 12600) && (y >= 2666 && y < 4000))
        return 1;
    else if ((x >= 12600 && x <= 14000) && (y >= 2666 && y < 4000))
        return 1;
    else if ((x >= 0 && x < 1400) && (y >= 4000 && y < 5333))
        return 1;
    else if ((x >= 1400 && x < 2800) && (y >= 4000 && y < 5333))
        return 1;
    else if ((x >= 2800 && x < 4200) && (y >= 4000 && y < 5333))
        return 1;
    else if ((x >= 4200 && x < 5600) && (y >= 4000 && y < 5333))
        return 0;
    else if ((x >= 5860 && x < 7000) && (y >= 4000 && y < 5333))
        return 1;
	else if ((x >= 7000 && x < 8400) && (y >= 4000 && y < 5333))
        return 0;
    else if ((x >= 8400 && x < 9800) && (y >= 4000 && y < 5333))
        return 0;
    else if ((x >= 9800 && x < 11200) && (y >= 4000 && y < 5333))
        return 1;
    else if ((x >= 11200 && x < 12600) && (y >= 4000 && y < 5333))
        return 1;
    else if ((x >= 12600 && x <= 14000) && (y >= 4000 && y < 5333))
        return 1;
    else if ((x >= 0 && x < 1400) && (y >= 5333 && y < 6666))
        return 1;
    else if ((x >= 1400 && x < 2800) && (y >= 5333 && y < 6666))
        return 1;
    else if ((x >= 2800 && x < 4200) && (y >= 5333 && y < 6666))
        return 1;
    else if ((x >= 4200 && x < 5600) && (y >= 5333 && y < 6666))
        return 1;
    else if ((x >= 5860 && x < 7000) && (y >= 5333 && y < 6666))
        return 0;
	else if ((x >= 7000 && x < 8400) && (y >= 5333 && y < 6666))
        return 0;
    else if ((x >= 8400 && x < 9800) && (y >= 5333 && y < 6666))
        return 0;
    else if ((x >= 9800 && x < 11200) && (y >= 5333 && y < 6666))
        return 1;
    else if ((x >= 11200 && x < 12600) && (y >= 5333 && y < 6666))
        return 1;
    else if ((x >= 12600 && x <= 14000) && (y >= 5333 && y < 6666))
        return 1;
    else if ((x >= 0 && x < 1400) && (y >= 6666 && y < 8000))
        return 0;
    else if ((x >= 1400 && x < 2800) && (y >= 6666 && y < 8000))
        return 0;
    else if ((x >= 2800 && x < 4200) && (y >= 6666 && y < 8000))
        return 0;
    else if ((x >= 4200 && x < 5600) && (y >= 6666 && y < 8000))
        return 0;
    else if ((x >= 5860 && x < 7000) && (y >= 6666 && y < 8000))
        return 0;
	else if ((x >= 7000 && x < 8400) && (y >= 6666 && y < 8000))
        return 0;
    else if ((x >= 8400 && x < 9800) && (y >= 6666 && y < 8000))
        return 0;
    else if ((x >= 9800 && x < 11200) && (y >= 6666 && y < 8000))
        return 1;
    else if ((x >= 11200 && x < 12600) && (y >= 6666 && y < 8000))
        return 1;
    else if ((x >= 12600 && x <= 14000) && (y >= 6666 && y < 8000))
        return 1;
    else if ((x >= 0 && x < 1400) && (y >= 8000 && y < 9333))
        return 0;
    else if ((x >= 1400 && x < 2800) && (y >= 8000 && y < 9333))
        return 1;
    else if ((x >= 2800 && x < 4200) && (y >= 8000 && y < 9333))
        return 1;
    else if ((x >= 4200 && x < 5600) && (y >= 8000 && y < 9333))
        return 0;
    else if ((x >= 5860 && x < 7000) && (y >= 8000 && y < 9333))
        return 0;
	else if ((x >= 7000 && x < 8400) && (y >= 8000 && y < 9333))
        return 0;
    else if ((x >= 8400 && x < 9800) && (y >= 8000 && y < 9333))
        return 0;
    else if ((x >= 9800 && x < 11200) && (y >= 8000 && y < 9333))
        return 0;
    else if ((x >= 11200 && x < 12600) && (y >= 8000 && y < 9333))
        return 1;
    else if ((x >= 12600 && x <= 14000) && (y >= 8000 && y < 9333))
        return 1;
    else if ((x >= 0 && x < 1400) && (y >= 9333 && y < 10666))
        return 1;
    else if ((x >= 1400 && x < 2800) && (y >= 9333 && y < 10666))
        return 1;
    else if ((x >= 2800 && x < 4200) && (y >= 9333 && y < 10666))
        return 1;
    else if ((x >= 4200 && x < 5600) && (y >= 9333 && y < 10666))
        return 1;
    else if ((x >= 5860 && x < 7000) && (y >= 9333 && y < 10666))
        return 1;
	else if ((x >= 7000 && x < 8400) && (y >= 9333 && y < 10666))
        return 0;
    else if ((x >= 8400 && x < 9800) && (y >= 9333 && y < 10666))
        return 1;
    else if ((x >= 9800 && x < 11200) && (y >= 9333 && y < 10666))
        return 1;
    else if ((x >= 11200 && x < 12600) && (y >= 9333 && y < 10666))
        return 1;
    else if ((x >= 12600 && x <= 14000) && (y >= 9333 && y < 10666))
        return 1;
	else if ((x >= 0 && x < 1400) && (y >= 10666 && y <= 12000))
        return 1;
    else if ((x >= 1400 && x < 2800) && (y >= 10666 && y <= 12000))
        return 1;
    else if ((x >= 2800 && x < 4200) && (y >= 10666 && y <= 12000))
        return 1;
    else if ((x >= 4200 && x < 5600) && (y >= 10666 && y <= 12000))
        return 1;
    else if ((x >= 5860 && x < 7000) && (y >= 10666 && y <= 12000))
        return 1;
	else if ((x >= 7000 && x < 8400) && (y >= 10666 && y <= 12000))
        return 0;
    else if ((x >= 8400 && x < 9800) && (y >= 10666 && y <= 12000))
        return 1;
    else if ((x >= 9800 && x < 11200) && (y >= 10666 && y <= 12000))
        return 1;
    else if ((x >= 11200 && x < 12600) && (y >= 10666 && y <= 12000))
        return 1;
    else if ((x >= 12600 && x <= 14000) && (y >= 10666 && y <= 12000))
        return 1;
    else
        return 0;
}

double CalculateDistanceTraveledScore(int km) {
    if (km <= 1000) return 1.0;
    if (km <= 2000) return 0.75;
    if (km <= 3000) return 0.5;
    return 0.25;
}

double CalculateAgeVsPotencyScore(int pv, int im, int lp) {
    if (!lp) { // Veículos Leves
        // Itera sobre as linhas de potência (começando de i=1 para pular o cabeçalho)
        for (int i = 1; i < 20; i++) {
            if (pv == faixa_matrix_leves[i][0]) {
                // Itera sobre as faixas etárias, exceto a última
                for (int j = 1; j < 7; j++) { // Loop vai de j=1 a j=6
                    if (im >= faixa_matrix_leves[0][j] && im < faixa_matrix_leves[0][j + 1]) {
                        return faixa_matrix_leves[i][j];
                    }
                }
                // Trata a última faixa etária (80+) separadamente
                if (im >= faixa_matrix_leves[0][7]) {
                    return faixa_matrix_leves[i][7];
                }
            }
        }
    } else { // Veículos Pesados
        // Itera sobre as faixas etárias, exceto a última
        for (int i = 0; i < 6; i++) { // Loop vai de i=0 a i=5
            if (im >= faixa_matrix_pesados[0][i] && im < faixa_matrix_pesados[0][i + 1]) {
                return faixa_matrix_pesados[1][i];
            }
        }
        // Trata a última faixa etária (70+) separadamente
        if (im >= faixa_matrix_pesados[0][6]) {
            return faixa_matrix_pesados[1][6];
        }
    }
    return 0.0;
}

double CalculateVehicleAgeScore(int manufactureYear) {
    //auto currentTime = std::chrono::system_clock::now();
    //std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    //std::tm* currentTM = std::localtime(&currentTime_t);
    //int currentYear = currentTM->tm_year + 1900;
    int currentYear = 2025;

    int vehicleAge = std::max(0, currentYear - manufactureYear);

    if (vehicleAge <= 10) {
        return 1.00;
    } else if (vehicleAge <= 20) {
        return 0.75;
    } else if (vehicleAge <= 40) {
        return 0.50;
    } else { // vehicleAge > 40
        return 0.25;
    }
}

double CalculateTimeTraveledScore(int tt) {
    if (tt <= 30) return 1.0;
    if (tt <= 60) return 0.75;
    if (tt <= 150) return 0.5;
    return 0.25;
}

double CalculateTimeLicensedScore(int tc) {
    if (tc > 10) return 1.0;
    if (tc > 5) return 0.75;
    if (tc > 2) return 0.5;
    if (tc >= 1) return 0.25;
    return 0.0;
}

double CalculatePeakTimeScore(int hd, int hn, int tt) {
    if (tt <= 0) return 0.0;
    return (1 - hd / (double)tt) * 0.25 + (1 - hn / (double)tt) * 0.75;
}

double CalculatePenaltyHistoryScore(int mv, int mq, int mm, int mvmax, int mmax) {
    if (mvmax == 0 || mmax == 0) {
        // Se não há multas no cluster, um motorista sem multas é perfeito.
        return (mv == 0) ? 1.0 : 0.5; // Retorna 1.0 se o motorista não tem multas, 0.5 se tem
    }
    
    int lmmax = 12;
    double score = pow((1 - mv / (double)mvmax), mq / (double)mmax) * (1 - mm / (double)lmmax);

    return std::max(0.0, score);
}

double CalculateTireQualityScore(int qp) {
    if (qp > 30) return 1.00;
    else if (qp >= 16) return 0.75;
    else return 0.0;
}

double CalculateFuelEfficiencyScore(int ec) {
    switch (ec) {
        case 0: return 1.0;
        case 1: return 0.8;
        case 2: return 0.7;
        case 3: return 0.4;
        default: return 0.2;
    }
}

double CalculateAverageSpeedScore(bool road, double magnitudeMyVelocity) {
    if (road) { // Rodovias
        if (magnitudeMyVelocity >= 60 && magnitudeMyVelocity < 110)
            return 1.0;
        else if (magnitudeMyVelocity >= 110 && magnitudeMyVelocity < 150)
            return 0.25;
        else
            return 0;
    } else { // Ruas
        if (magnitudeMyVelocity >= 15 && magnitudeMyVelocity < 40)
            return 1.0;
        else if (magnitudeMyVelocity >= 40 && magnitudeMyVelocity < 60)
            return 0.75;
        else if (magnitudeMyVelocity >= 60 && magnitudeMyVelocity < 80)
            return 0.25;
        else
            return 0;
    }
}
