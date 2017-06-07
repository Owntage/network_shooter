#include "matrix_utils.h"
#include <iostream>
#include <string>
#include <algorithm>

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

void multiplyRow(matrix_t& m, int line, double multiplier)
{
	for (int i = 0; i < m[line].size(); i++) m[line][i] *= multiplier;
}

void swapRows(matrix_t& m, int first, int second)
{
	if (first == second) return;
	vector_t tempRow(m[0].size());
	for (int i = 0; i < m[0].size(); i++) tempRow[i] = m[first][i];
	for (int i = 0; i < m[0].size(); i++) tempRow[i] = m[first][i] = m[second][i];
	for (int i = 0; i < m[0].size(); i++) tempRow[i] = m[second][i] = tempRow[i];
}

void subtractRows(matrix_t& m, int first, int second)
{
	for(int i = 0; i < m[0].size(); i++)
	{
		m[first][i] -= m[second][i];
	}
}

void printMatrix(matrix_t& m)
{
	std::cout << "matrix: " << std::endl;
	for(int i = 0; i < m.size(); i++)
	{
		for(int j = 0; j < m[i].size(); j++)
		{
			std::string output = std::to_string(m[i][j]);
			std::cout << output;
			for (int k = output.size(); k < 10; k++) std::cout << " ";
		}
		std::cout << std::endl;
	}
}

void generateRandom(matrix_t& m)
{
	for(int i = 0; i < m.size(); i++)
	{
		for(int j = 0; j < m[i].size(); j++)
		{
			m[i][j] = ((double)(rand() % 100000)) / 1000.0;
		}
	}
	printMatrix(m);
}

matrix_t generateIdentity(int size)
{
	matrix_t m;
	m.resize(size);
	for (int i = 0; i < size; i++) m[i].resize(size);
	for(int i = 0; i < m.size(); i++)
	{
		for(int j = 0; j < m.size(); j++)
		{
			if (i == j)
			{
				m[i][j] = 1.0;
			}
			else
			{
				m[i][j] = 0.0;
			}
				
		}
	}
	return m;
}

double columnSum(matrix_t& m, int column)
{
	double res = 0;
	for(int i = 0; i < m.size(); i++)
	{
		res += abs(m[i][column]);
	}
	return res;
}

double norm(matrix_t m) //first norm
{
	double res = columnSum(m, 0);
	for(int i = 1; i < m.size(); i++)
	{
		res = std::max(res, columnSum(m, i));
	}
	return res;
}

std::pair<vector_t, matrix_t> gauss(matrix_t& matrix)
{
	matrix_t reverted = generateIdentity(matrix.size());


	for(int i = 0; i < matrix.size(); i++)
	{
		int nonZeroIndex = i;
		for(int j = i; j < matrix.size(); j++)
		{
			if(matrix[j][i] != 0.0f)
			{
				nonZeroIndex = j;
				break;
			}
		}
		swapRows(matrix, i, nonZeroIndex);
		swapRows(reverted, i, nonZeroIndex);
		for(int j = i; j < matrix.size(); j++)
		{
			if (matrix[j][i] == 0.0) continue;
			multiplyRow(matrix, j, 1.0 / matrix[j][i]);
			multiplyRow(reverted, j, 1.0 / matrix[j][i]);
		}
		for(int j = i + 1; j < matrix.size(); j++)
		{
			if (matrix[j][i] == 0.0) continue;
			subtractRows(matrix, j, i);
			subtractRows(reverted, j, i);
		}
	}

	int offset = 0;
	for(int i = matrix.size() - 1; i >= 0; i--)
	{
		for(int j = 0; j < i; j++)
		{
			if (matrix[j][matrix.size() - 1 - offset] == 0.0f) continue;
			multiplyRow(matrix, j, matrix[i][matrix.size() - 1 - offset] / matrix[j][matrix.size() - 1 - offset]);
			multiplyRow(reverted, j, matrix[i][matrix.size() - 1 - offset] / matrix[j][matrix.size() - 1 - offset]);
			subtractRows(matrix, j, i);
			subtractRows(reverted, j, i);
		}
		multiplyRow(matrix, i, 1.0 / matrix[i][matrix.size() - 1 - offset]);
		multiplyRow(reverted, i, 1.0 / matrix[i][matrix.size() - 1 - offset]);
		offset++;
	}

	matrix_t result_m;
	vector_t result_v;
	printMatrix(matrix);
	return std::make_pair(result_v, reverted);
}