#include "matrix_utils.h"
#include <iostream>
#include <string>
#include <algorithm>

//gauss

double myMax(double first, double second)
{
	return first > second ? first : second;
}

double myAbs(double a)
{
	return a > 0 ? a : -a;
}

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

void generateDiag(matrix_t& m)
{
	generateRandom(m);
	for(int i = 0; i < m.size(); i++)
	{
		m[i][i] += 5000.0 + ((double)(rand() % 100));
	}
}

void generateSymetric(matrix_t& m)
{
	generateRandom(m);
	for(int i = 0; i < m.size(); i++)
	{
		for(int j = 0; j < m.size(); j++)
		{
			m[i][j] /= 1000.0;
		}
	}
	
	for(int i = 0; i < m.size(); i++)
	{
		for(int j = 0; j < m.size(); j++)
		{
			m[i][j] = m[j][i];
		}
	}
}

void generateRandom(matrix_t& m)
{
	for(int i = 0; i < m.size(); i++)
	{
		for(int j = 0; j < m[i].size(); j++)
		{
			m[i][j] = ((double)(rand() % 100));
		}
	}
	printMatrix(m);
}

void generateHilbert(matrix_t& m)
{
	for(int i = 0; i < m.size(); i++)
	{
		for(int j = 0; j < m.size(); j++)
		{
			m[i][j] = 1.0 / (i + j + 1.0);
		}
	}
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
		res += myAbs(m[i][column]);
	}
	return res;
}

double norm(matrix_t m) //first norm
{
	double res = columnSum(m, 0);
	for(int i = 1; i < m.size(); i++)
	{
		res = myMax(res, columnSum(m, i));
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
			double temp = 1.0 / matrix[j][i];
			multiplyRow(matrix, j, temp);
			multiplyRow(reverted, j, temp);
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
			double temp = matrix[i][matrix.size() - 1 - offset] / matrix[j][matrix.size() - 1 - offset];
			multiplyRow(matrix, j, temp);
			multiplyRow(reverted, j, temp);
			subtractRows(matrix, j, i);
			subtractRows(reverted, j, i);
		}
		double temp = 1.0 / matrix[i][matrix.size() - 1 - offset];
		multiplyRow(matrix, i, temp);
		multiplyRow(reverted, i, temp);
		offset++;
	}

	vector_t result_v;
	printMatrix(matrix);
	return std::make_pair(result_v, reverted);
}

//yakobi

double vec_norm(vector_t vector)
{
	double res = 0;
	for (int i = 0; i < vector.size() - 1; i++) res += myAbs(vector[i]);
	return res;
}

vector_t get_next_x(matrix_t& matrix, vector_t& prev)
{
	vector_t res;
	res.resize(matrix.size());
	for(int i = 0; i < matrix.size(); i++)
	{
		double sum = 0.0;
		for(int j = 0; j < matrix.size(); j++)
		{
			if (i == j) continue;
			sum += matrix[i][j] * prev[j];
		}
		res[i] = 1.0 / matrix[i][i] * (matrix[i].back() - sum);
	}
	return res;
}

vector_t vec_substract(vector_t first, vector_t second)
{
	vector_t res;
	res.resize(first.size());
	for(int i = 0; i < res.size(); i++)
	{
		res[i] = first[i] - second[i];
	}
	return res;
}

vector_t vec_add(vector_t first, vector_t second)
{
	vector_t res;
	res.resize(first.size());
	for (int i = 0; i < res.size(); i++)
	{
		res[i] = first[i] + second[i];
	}
	return res;
}

vector_t yakobi(matrix_t& matrix)
{
	std::cout << "norm: " << norm(matrix) << std::endl;
	float eps = 0.000000001;
	vector_t x0;
	x0.resize(matrix.size());
	for(int i = 0; i < matrix.size(); i++)
	{
		x0[i] = matrix[i].back() / matrix[i][i];
	}

	int it_count = 0;
	while (true)
	{
		it_count++;
		vector_t x1 = get_next_x(matrix, x0);
		if (myAbs(vec_norm(vec_substract(x0, x1))) < eps || it_count > 100000)
		{
			std::cout << "yakobi iterations: " << it_count << std::endl;
			return x1;
		}
			
		x0 = x1;
	}
}

//zeidel

double count_c(matrix_t& m, int i, int j)
{
	if (i == j) return 0;
	return -m[i][j] / m[i][i];
}

double count_d(matrix_t& m, int i)
{
	return m[i].back() / m[i][i];
}

vector_t zeidel(matrix_t& matrix)
{
	float eps = 0.00000001;
	vector_t x0;
	x0.resize(matrix.size());
	for (int i = 0; i < matrix.size(); i++)
	{
		x0[i] = matrix[i].back() / matrix[i][i];
	}
	

	int it_count = 0;
	while(true)
	{
		it_count++;
		vector_t x1;
		x1.resize(x0.size());
		
		for (int i = 0; i < x0.size(); i++)
		{
			double sum = 0;
			for (int j = 0; j < x0.size(); j++)
			{
				if (j < i)
				{
					sum += count_c(matrix, i, j) * x1[j];
				}
				else
				{
					sum += count_c(matrix, i, j) * x0[j];
				}
			}
			sum += count_d(matrix, i);
			x1[i] = sum;
		}
		if (myAbs(vec_norm(vec_substract(x0, x1))) < eps ||  it_count > 100000)
		{
			std::cout << "zeidel iterations: " << it_count << std::endl;
			return x1;
		}
			
		x0 = x1;

	}
}

//descent

vector_t matToVec(matrix_t m, vector_t vec)
{
	vector_t result;
	result.resize(m.size(), 0);
	for(int i = 0; i < result.size(); i++)
	{
		for(int j = 0; j < result.size(); j++)
		{
			result[i] += m[i][j] * vec[j];
		}
	}
	return result;
}

double dotProduct(vector_t first, vector_t second)
{
	double res = 0;
	for(int i = 0; i < first.size(); i++)
	{
		res += first[i] * second[i];
	}
	return res;
}

vector_t vec_multiply(vector_t first, double second)
{
	vector_t res;
	res.resize(first.size());
	for (int i = 0; i < res.size(); i++)
	{
		res[i] = first[i] * second;
	}
	return res;
}

vector_t descent(matrix_t& matrix)
{
	vector_t x0;
	x0.resize(matrix.size(), 0);

	vector_t r0;
	vector_t initial_b;
	r0.resize(x0.size());
	for(int i = 0; i < r0.size(); i++)
	{
		r0[i] = matrix[i].back();
	}
	initial_b = r0;

	vector_t z0 = r0;

	int it_count = 0;
	while (true)
	{
		it_count++;
		double ak = dotProduct(r0, r0) / dotProduct(matToVec(matrix, z0), z0);
		vector_t x1 = vec_add(x0, vec_multiply(z0, ak));
		vector_t r1 = vec_substract(r0, vec_multiply(matToVec(matrix, z0), ak));
		double bk = dotProduct(r1, r1) / dotProduct(r0, r0);
		vector_t z1 = vec_add(r1, vec_multiply(z0, bk));

		x0 = x1;
		r0 = r1;
		z0 = z1;

		if (it_count > 100000)
		{
			std::cout << "descent iterations: " << it_count << std::endl;
			return x0;
		}
			
		
		double k = vec_norm(r0) / vec_norm(initial_b);
		if (k < 0.0001)
		{
			std::cout << "descent iterations: " << it_count << std::endl;
			return x0;
		}
	}

}