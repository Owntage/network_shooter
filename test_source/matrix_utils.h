#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <vector>
#include <utility>

typedef std::vector<std::vector<double> > matrix_t;
typedef std::vector<double> vector_t;

double matrixSum(matrix_t& matrix, int width, int height);
std::pair<vector_t, matrix_t> gauss(matrix_t& matrix);
void generateRandom(matrix_t& matrix);
void generateHilbert(matrix_t& matrix);
double norm(matrix_t matrix);
vector_t yakobi(matrix_t& matrix);
vector_t zeidel(matrix_t& matrix);
matrix_t generateIdentity(int size);


#endif