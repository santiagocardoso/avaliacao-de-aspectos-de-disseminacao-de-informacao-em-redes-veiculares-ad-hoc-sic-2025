#include "mcda-utils.h"

// TOPSIS Functions =======================================================================================================
vector<vector<double>> normalize(vector<vector<double>> &matrix) {
	size_t rows = matrix.size();
	size_t cols = matrix[0].size();
	
	vector<vector<double>> normalizedMatrix(rows, vector<double> (cols));

	for (size_t j = 0; j < cols; j++) {
		// Euclidean norm
		double columnNorm = 0;
		for (const auto &row : matrix) {
			columnNorm += row[j] * row[j];
		}
		columnNorm = sqrt(columnNorm);

		if (columnNorm > 0) {
			for (size_t i = 0; i < rows; i++) {
				normalizedMatrix[i][j] = matrix[i][j] / columnNorm;
			}
		}
	}

	return normalizedMatrix;
}

vector<vector<double>> multiplyWeights(vector<vector<double>> &matrix, vector<double> &weights) {
	size_t rows = matrix.size();
	size_t cols = matrix[0].size();
	
	vector<vector<double>> weightedMatrix(rows, vector<double> (cols));

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			weightedMatrix[i][j] = matrix[i][j] * weights[j];
		}
	}

	return weightedMatrix;
}

vector<double> idealSolution(vector<vector<double>> &matrix) {
	size_t cols = matrix[0].size();

	vector<double> ideal(cols, numeric_limits<double>::min());

	for (const auto &row : matrix) {
		for (size_t j = 0; j < cols; j++) {
			ideal[j] = max(ideal[j], row[j]);
		}
	}

	return ideal;
}

vector<double> negativeIdealSolution(vector<vector<double>> &matrix) {
	size_t cols = matrix[0].size();

	vector<double> negativeIdeal(cols, numeric_limits<double>::max());

	for (const auto &row : matrix) {
		for (size_t j = 0; j < cols; j++) {
			negativeIdeal[j] = min(negativeIdeal[j], row[j]);
		}
	}

	return negativeIdeal;
}

vector<double> euclideanDistance(vector<vector<double>> &matrix, vector<double> &target) {
    size_t rows = matrix.size();
    vector<double> distances(rows, 0.0);

    for (size_t i = 0; i < rows; i++) {
        double sum_of_squares = 0.0;
        for (size_t j = 0; j < target.size(); j++) {
            double diff = matrix[i][j] - target[j];
            sum_of_squares += diff * diff;
        }
        distances[i] = sqrt(sum_of_squares);
    }

    return distances;
}

vector<double> closenessCoefficients(vector<double> &euclideanDistanceIdeal, vector<double> &negativeEuclidianDistanceIdeal) {
	size_t size = euclideanDistanceIdeal.size();
	vector<double> coefficients(size);

	for (size_t i = 0; i < size; i++) {
        double denominator = euclideanDistanceIdeal[i] + negativeEuclidianDistanceIdeal[i];
        if (denominator == 0.0) coefficients[i] = 0.0;
        else coefficients[i] = negativeEuclidianDistanceIdeal[i] / denominator;
	}
	return coefficients;
}

int bestAlternative(vector<double> coefficients) {
	return distance(coefficients.begin(), max_element(coefficients.begin(), coefficients.end()));
}

// AHP Functions ==========================================================================================================
vector<vector<double>> getDefaultAHPJudgmentMatrix() {
    return {
        {1, 1.0/7, 1.0/5, 1, 5, 5, 3, 1.0/3, 3, 1.0/3, 1.0/5, 1.0/3, 5},
        {7, 1, 1, 5, 9, 9, 9, 2, 9, 5, 1, 1, 9},
        {5, 1, 1, 3, 5, 5, 7, 1, 7, 3, 1, 1.0/3, 7},
        {1, 1.0/5, 1.0/5, 1, 2, 3, 5, 1.0/3, 3, 1, 1.0/5, 1.0/5, 7},
        {1.0/5, 1.0/9, 1.0/5, 1.0/2, 1, 1.0/3, 1, 1.0/5, 3, 1.0/3, 1.0/3, 1.0/3, 5},
        {1.0/5, 1.0/9, 1.0/5, 1.0/3, 3, 1, 3, 1.0/5, 3, 1.0/3, 1.0/5, 1.0/3, 3},
        {1.0/3, 1.0/9, 1.0/7, 1.0/5, 1, 1.0/3, 1, 1.0/3, 1.0/3, 1.0/5, 1.0/9, 1.0/7, 1},
        {3, 1.0/2, 1, 3, 5, 5, 3, 1, 7, 3, 1.0/3, 1, 5},
        {1.0/3, 1.0/9, 1.0/7, 1.0/3, 1.0/3, 1.0/3, 3, 1.0/7, 1, 1.0/3, 1.0/9, 1.0/3, 3},
        {3, 1.0/5, 1.0/3, 1, 3, 3, 5, 1.0/3, 3, 1, 1.0/3, 1.0/5, 5},
        {5, 1, 1, 5, 3, 5, 9, 3, 9, 3, 1, 3, 9},
        {3, 1, 3, 5, 3, 3, 7, 1, 3, 5, 1.0/3, 1, 7},
        {1.0/5, 1.0/9, 1.0/7, 1.0/7, 1.0/5, 1.0/3, 1, 1.0/5, 1.0/3, 1.0/5, 1.0/9, 1.0/7, 1}
    };
}

vector<vector<double>> normalizeAHPMatrix(vector<vector<double>> &judgmentMatrix) {
	size_t size = judgmentMatrix.size();
	vector<vector<double>> normalizedMatrix(size, vector<double>(size));

	for (size_t j = 0; j < size; j++) {
		double colSum = 0;
		for (size_t i = 0; i < size; i++) {
			colSum += judgmentMatrix[i][j];
		}
		for (size_t i = 0; i < size; i++) {
			normalizedMatrix[i][j] = judgmentMatrix[i][j] / colSum;
		}
	}

	return normalizedMatrix;
}

vector<double> calculateAHPWeights(vector<vector<double>> &normalizedMatrix) {
	size_t size = normalizedMatrix.size();
	vector<double> weights(size, 0.0);

	for (size_t i = 0; i < size; i++) {
		weights[i] = accumulate(normalizedMatrix[i].begin(), normalizedMatrix[i].end(), 0.0) / size;
	}

	return weights;
}

vector<double> scoreAHPAlternatives(vector<vector<double>> &evaluationMatrix, vector<double> &weights) {
	size_t rows = evaluationMatrix.size();
	size_t cols = weights.size();
	vector<double> scores(rows, 0.0);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			scores[i] += evaluationMatrix[i][j] * weights[j];
		}
	}

	return scores;
}

// PROMETHEE Functions ====================================================================================================
double preferenceFunction(double diff, double p) {
    if (diff <= 0) return 0.0;
    if (diff >= p) return 1.0;
    return diff / p;
}

vector<vector<double>> calculatePreferenceMatrix(const vector<vector<double>>& evaluationMatrix, const vector<double>& weights, double preferenceThreshold) {
    int n = evaluationMatrix.size();
    int m = evaluationMatrix[0].size();
    vector<vector<double>> preferenceMatrix(n, vector<double>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            double preferenceSum = 0.0;
            for (int k = 0; k < m; ++k) {
                double diff = evaluationMatrix[i][k] - evaluationMatrix[j][k];
                preferenceSum += weights[k] * preferenceFunction(diff, preferenceThreshold);
            }
            preferenceMatrix[i][j] = preferenceSum;
        }
    }

    return preferenceMatrix;
}

vector<double> calculatePositiveFlows(const vector<vector<double>>& preferenceMatrix) {
    int n = preferenceMatrix.size();
    vector<double> phiPlus(n, 0.0);
	if (n <= 1) return phiPlus;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            phiPlus[i] += preferenceMatrix[i][j];
    for (double& val : phiPlus) val /= (n - 1);
    return phiPlus;
}

vector<double> calculateNegativeFlows(const vector<vector<double>>& preferenceMatrix) {
    int n = preferenceMatrix.size();
    vector<double> phiMinus(n, 0.0);
	if (n <= 1) return phiMinus;
    for (int j = 0; j < n; ++j)
        for (int i = 0; i < n; ++i)
            phiMinus[j] += preferenceMatrix[i][j];
    for (double& val : phiMinus) val /= (n - 1);
    return phiMinus;
}

vector<double> calculateNetFlows(const vector<double>& phiPlus, const vector<double>& phiMinus) {
    int n = phiPlus.size();
    vector<double> netFlow(n, 0.0);
    for (int i = 0; i < n; ++i)
        netFlow[i] = phiPlus[i] - phiMinus[i];
    return netFlow;
}

int bestAlternativePromethee(const vector<double>& netFlows) {
    return max_element(netFlows.begin(), netFlows.end()) - netFlows.begin();
}
