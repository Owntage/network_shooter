#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <vector>

typedef std::vector<std::vector<double> > matrix_t;
typedef std::vector<double> vector_t;

double matrixSum(matrix_t& matrix, int width, int height);


#endif