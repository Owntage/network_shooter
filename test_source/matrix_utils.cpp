#include "matrix_utils.h"

double matrixSum(std::vector<std::vector<double> >& matrix, int width, int height)
{
	double result = 0.0;
	for(int i = 0; i < width; i++)
	{
		for(int j = 0; j < height; j++)
		{
			result += matrix[i][j];
		}
	}
	return result;
}