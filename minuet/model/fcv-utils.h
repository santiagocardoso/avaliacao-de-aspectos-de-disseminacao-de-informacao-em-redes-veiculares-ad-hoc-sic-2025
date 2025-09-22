#ifndef FCV_UTILS_H
#define FCV_UTILS_H

#include "ns3/vector.h"

#include <vector>
#include <map>
#include <string>
#include <sys/stat.h>
#include <cmath>
#include <ctime>
#include <chrono>
#include <iostream>

using namespace std;

void printMatrix(vector<vector<double>> &matrix);
bool createDirectory(const std::string& path);
double VectorDistance(const ns3::Vector &v1, const ns3::Vector &v2);
double mapeamentoRodoviaRua(double x, double y);

void LogAllScores(const std::string& methodName, 
                  const std::vector<int>& ids, 
                  const std::vector<std::vector<double>>& criteriaMatrix, 
                  const std::vector<double>& finalScores, 
                  uint64_t tempo);

// Funções de avaliação de atributos veiculares
double CalculateAverageSpeedScore(bool road, double magnitudeMyVelocity);
double CalculateDistanceTraveledScore(int km);
double CalculateAgeVsPotencyScore(int pv, int im, int lp);
double CalculateVehicleAgeScore(int manufactureYear);
double CalculateTimeTraveledScore(int tt);
double CalculateTimeLicensedScore(int tc);
double CalculatePeakTimeScore(int hd, int hn, int tt);
double CalculatePenaltyHistoryScore(int mv, int mq, int mm, int mvmax, int mmax);
double CalculateTireQualityScore(int qp);
double CalculateFuelEfficiencyScore(int ec);

// Matrizes globais
extern float faixa_matrix_leves[20][8];
extern float faixa_matrix_pesados[2][7];

#endif // FCV_UTILS_H
