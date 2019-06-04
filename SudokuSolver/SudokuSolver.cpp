// SudokuSolver.cpp : Defines the entry point for the application.
//

#include "SudokuSolver.h"

#define SIZE 9
#define QUAD_SIZE 3

#include <math.h>
#include <assert.h>
#include <cstring>
#include <iostream>

static wchar_t buf[256];
typedef int TSource[SIZE][SIZE];


TSol::TSol()
{
	Clear();
}
void TSol::Clear()
{
	high = -1;
	v = 0;
}
void TSol::Add(int val)
{
	assert(val >= 1 && val <= 9);
	int t = 1 << (val);
	if (v&t)return;
	high++;
	v |= t;
}
void TSol::Remove(int& sol_need, int val)
{
	assert(val >= 1 && val <= 9);
	int t = 1 << (val);
	if (!(v&t))return;
	high--;
	if (high == 0)sol_need--;
	v ^= t;
}
int TSol::High()
{
	return high;
}
bool TSol::Exists(int val)
{
	assert(val >= 1 && val <= 9);
	return v & (1 << (val));
}
unsigned int TSol::Get()
{
	assert(high == 0);
	unsigned long index = 0;
	auto temp = v;
	while (temp >>= 1) {
		index++;
	}
	assert(index != 0);
	return index;
}


int TSudoku::BitCount(int v)
{
	v = v - ((v >> 1) & 0x55555555); // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333); // temp
	return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
}

void TSudoku::InitCellSolutions(int i, int k)
{
	int mask = 0;
	for (int f = 0; f < SIZE; f++)
		if (sol[i][f].high == 0) mask |= sol[i][f].v;
	for (int f = 0; f < SIZE; f++)
		if (sol[f][k].high == 0) mask |= sol[f][k].v;
	int min_i = (i / QUAD_SIZE) * QUAD_SIZE;
	int min_k = (k / QUAD_SIZE) * QUAD_SIZE;
	for (int ii = min_i; ii < min_i + 3; ii++)
	{
		if (sol[ii][min_k + 0].high == 0) mask |= sol[ii][min_k + 0].v;
		if (sol[ii][min_k + 1].high == 0) mask |= sol[ii][min_k + 1].v;
		if (sol[ii][min_k + 2].high == 0) mask |= sol[ii][min_k + 2].v;
	}
	mask = (~mask) & 1022;
	sol[i][k].v = (mask);
	sol[i][k].high = (BitCount(mask)) - 1;
}
bool TSudoku::InitCellsSolutions()
{
	//
	int row[SIZE], col[SIZE], quad[QUAD_SIZE][QUAD_SIZE];
	for (int i = 0; i < SIZE; i++)
	{
		row[i] = 0;
		for (int f = 0; f < SIZE; f++)
			if (sol[i][f].high == 0) row[i] |= sol[i][f].v;
	}
	for (int k = 0; k < SIZE; k++)
	{
		col[k] = 0;
		for (int f = 0; f < SIZE; f++)
			if (sol[f][k].high == 0) col[k] |= sol[f][k].v;
	}
	for (int i = 0; i < QUAD_SIZE; i++)
	{
		for (int k = 0; k < QUAD_SIZE; k++)
		{
			int min_i = i * QUAD_SIZE;
			int min_k = k * QUAD_SIZE;
			quad[i][k] = 0;
			for (int ii = min_i; ii < min_i + 3; ii++)
			{
				if (sol[ii][min_k + 0].high == 0) quad[i][k] |= sol[ii][min_k + 0].v;
				if (sol[ii][min_k + 1].high == 0) quad[i][k] |= sol[ii][min_k + 1].v;
				if (sol[ii][min_k + 2].high == 0) quad[i][k] |= sol[ii][min_k + 2].v;
			}
		}
	}
	int mask;
	for (int i = 0; i < SIZE; i++)
	{
		for (int k = 0; k < SIZE; k++)
			if (sol[i][k].high != 0)
			{
				mask = (~(row[i] | col[k] | quad[i / 3][k / 3])) & 1022;
				sol[i][k].v = mask;
				sol[i][k].high = (BitCount(mask)) - 1;
				if (sol[i][k].high == 0)sol_need--;
				if (sol[i][k].v == 0)
					return false;
			}
	}
	return true;
}
void TSudoku::ValidateRow(int i)
{
	//проверяем встречается ли потенциальное число в строке один раз
	//и делаем его решением если такое имеется
	int f_k = -1;
	for (int n = 1; n <= SIZE; n++)
	{
		TSol* found = NULL;
		for (int k = 0; k < SIZE; k++)
		{
			if (sol[i][k].high > 0)
			{
				if (sol[i][k].Exists(n))
				{
					if (found == NULL)
					{
						f_k = k;
						found = &sol[i][k];
					}
					else
					{
						found = NULL;
						goto end_search;
					}
				}
			}
			else
			{
				assert(sol[i][k].high == 0);
				if (sol[i][k].Exists(n))
					goto end_search;
			}
		}
	end_search:
		if (found != NULL)
		{
			sol_need--;
			found->Clear();
			found->Add(n);
		}
		continue;
	}
}
void TSudoku::ValidateCol(int k)
{
	//проверяем встречается ли потенциальное число в столбце один раз
	//и делаем его решением если такое имеется
	for (int n = 1; n <= SIZE; n++)
	{
		TSol* found = NULL;
		for (int i = 0; i < SIZE; i++)
		{
			if (sol[i][k].high > 0)
			{
				if (sol[i][k].Exists(n))
				{
					if (found == NULL)
						found = &sol[i][k];
					else
					{
						found = NULL;
						goto end_search;
					}
				}
			}
			else
			{
				assert(sol[i][k].high == 0);
				if (sol[i][k].Exists(n))
					goto end_search;
			}
		}
	end_search:
		if (found != NULL)
		{
			sol_need--;
			found->Clear();
			found->Add(n);
		}
		continue;
	}
}
void TSudoku::ValidateQuad(int i, int k)
{
	//проверяем встречается ли потенциальное число в квадрате один раз
	//и делаем его решением если такое имеется
	int min_i = i * 3;
	int min_k = k * 3;
	for (int f = 1; f <= SIZE; f++)
	{
		TSol* found = NULL;
		for (int ii = min_i; ii < min_i + 3; ii++)
		{
			for (int kk = min_k; kk < min_k + 3; kk++)
			{
				if (sol[ii][kk].high > 0)
				{
					if (sol[ii][kk].Exists(f))
					{
						if (found == NULL)
							found = &sol[ii][kk];
						else
						{
							found = NULL;
							goto end_search;
						}
					}
				}
				else
				{
					assert(sol[ii][kk].high == 0);
					if (sol[ii][kk].Exists(f))
						goto end_search;
				}
			}
		}end_search:
		if (found != NULL)
		{
			sol_need--;
			found->Clear();
			found->Add(f);
		}
		continue;
	}
}
void TSudoku::ValidateQuads()
{
	for (int i = 0; i < QUAD_SIZE; i++)
	{
		for (int k = 0; k < QUAD_SIZE; k++)
		{
			ValidateQuad(i, k);
		}
	}
}
void TSudoku::ValidateRows()
{
	for (int i = 0; i < SIZE; i++)
		ValidateRow(i);
}
void TSudoku::ValidateCols()
{
	for (int k = 0; k < SIZE; k++)
		ValidateRow(k);
}
void TSudoku::RemoveNumFromRow(int i, int num, int exclude_min, int exclude_max)
{
	for (int f = 0; f < SIZE; f++)
		if (f<exclude_min || f>exclude_max)
			if (sol[i][f].high > 0)sol[i][f].Remove(sol_need, num);
}
void TSudoku::RemoveNumFromCol(int k, int num, int exclude_min, int exclude_max)
{
	for (int f = 0; f < SIZE; f++)
		if (f<exclude_min || f>exclude_max)
			if (sol[f][k].high > 0)sol[f][k].Remove(sol_need, num);
}
void TSudoku::ValidateQuadRowCol(int i, int k)
{
	int min_i = i * QUAD_SIZE;
	int min_k = k * QUAD_SIZE;
	int r[3];
	r[0] = 0;
	r[1] = 0;
	r[2] = 0;
	//проверяем в каких строках какие цифры присутствуют
	for (int ii = 0; ii < 3; ii++)
	{
		r[ii] |= sol[min_i + ii][min_k + 0].v;
		r[ii] |= sol[min_i + ii][min_k + 1].v;
		r[ii] |= sol[min_i + ii][min_k + 2].v;
	}
	//если цифра присутствует только в одной строке квадрата то удаляем ее из строк остальных квадратов
	int m = 1;
	for (int f = 1; f <= 9; f++)
	{
		m <<= 1;
		if ((r[0] & m) && !(r[1] & m) && !(r[2] & m))
		{
			RemoveNumFromRow(min_i + 0, f, min_k, min_k + 2);
		}
		if (!(r[0] & m) && (r[1] & m) && !(r[2] & m))
		{
			RemoveNumFromRow(min_i + 1, f, min_k, min_k + 2);
		}
		if (!(r[0] & m) && !(r[1] & m) && (r[2] & m))
		{
			RemoveNumFromRow(min_i + 2, f, min_k, min_k + 2);
		}
	}
	r[0] = 0;
	r[1] = 0;
	r[2] = 0;
	//проверяем в каких столбцах какие цифры присутствуют
	for (int ii = 0; ii < 3; ii++)
	{
		r[ii] |= sol[min_i + 0][min_k + ii].v;
		r[ii] |= sol[min_i + 1][min_k + ii].v;
		r[ii] |= sol[min_i + 2][min_k + ii].v;
	}
	//если цифра присутствует только в одном столбце квадрата то удаляем ее из столбцов остальных квадратов
	m = 1;
	for (int f = 1; f <= 9; f++)
	{
		m <<= 1;
		if ((r[0] & m) && !(r[1] & m) && !(r[2] & m))
		{
			RemoveNumFromCol(min_k + 0, f, min_i, min_i + 2);
		}
		if (!(r[0] & m) && (r[1] & m) && !(r[2] & m))
		{
			RemoveNumFromCol(min_k + 1, f, min_i, min_i + 2);
		}
		if (!(r[0] & m) && !(r[1] & m) && (r[2] & m))
		{
			RemoveNumFromCol(min_k + 2, f, min_i, min_i + 2);
		}
	}
}
void TSudoku::ValidateQuadsRowCol()
{
	for (int i = 0; i < QUAD_SIZE; i++)
	{
		for (int k = 0; k < QUAD_SIZE; k++)
		{
			ValidateQuadRowCol(i, k);
		}
	}
}

TSudoku::TSudoku()
{
	sol_need = -1;
	has_source = false;
}
void TSudoku::Init(TSource* source)
{
	has_source = true;
	sol_need = SIZE * SIZE;
	for (int i = 0; i < SIZE; i++)
	{
		for (int k = 0; k < SIZE; k++)
		{
			sol[i][k].Clear();
			if ((*source)[i][k] > 0)
			{
				sol_need--;
				sol[i][k].Add((*source)[i][k]);
			}
			else
			{
				sol[i][k].high = 8;
				sol[i][k].v = 1022;
			}
		}
	}
}
void TSudoku::Init(TSol* use_sol, int use_sol_need)
{
	has_source = true;
	sol_need = use_sol_need;
	memcpy(&sol, use_sol, SIZE*SIZE * sizeof(TSol));
}

bool TSudoku::BuildSubLevel(TSolution* &result)
{
#ifndef NDEBUG
	{
		TSource result;
		ConvertToResult(&sol, &result);
	}
#endif

	TSudoku sub_level;
	TCountTableElement table[SIZE];
	for (int i = 0; i < SIZE; i++)
	{
		for (int k = 0; k < SIZE; k++)
			if (sol[i][k].high > 0)
				table[sol[i][k].High()].Add(&sol[i][k]);
	}
	for (int c = 1; c < SIZE; c++)
	{
		for (int t = 1; t <= table[c].high; t++)
		{
			for (int n = 1; n <= SIZE; n++)
				if (table[c].sols[t]->Exists(n))
				{
					TSol temp = *table[c].sols[t];
					table[c].sols[t]->Clear();
					table[c].sols[t]->Add(n);

					sub_level.Init(&sol[0][0], sol_need - 1);
					if (sub_level.Calculate(result))
					{
						Init(&sub_level.sol[0][0], sub_level.sol_need);
						result = &sol;
						return true;
					}
					else
					{
						*table[c].sols[t] = temp;
					}
				}
		}
	}
	return false;
}
bool TSudoku::Calculate(TSolution* &result)
{
	assert(has_source);
	do
	{
		int last_sol = sol_need;
		if (false)
		{
			if (!InitCellsSolutions())
				return false;
		}
		else
		{
			for (int i = 0; i < SIZE; i++)
			{
				for (int k = 0; k < SIZE; k++)
					if (sol[i][k].high > 0)
					{
						InitCellSolutions(i, k);
						if (sol[i][k].high == 0)sol_need--;
						if (sol[i][k].high == -1)
							return false;
					}
			}
		}
		if (last_sol == sol_need)
			ValidateQuadsRowCol();
		if (last_sol == sol_need)
			ValidateRows();
		if (last_sol == sol_need)
			ValidateCols();
		if (last_sol == sol_need)
			ValidateQuads();
		for (int i = 0; i < 9; i++)
		{
			for (int k = 0; k < 9; k++)
				if (sol[i][k].high == -1)
					return false;
		}
		if (last_sol == sol_need)
			return BuildSubLevel(result);
	} while (sol_need != 0);
	result = &sol;
	return true;
}
bool TSudoku::ValidateSourceInBigCell(TSource* source, int num, int i, int k)
{
	assert(num >= 1 && num <= 9);
	bool exists = false;
	int min_i = i * 3;
	int min_k = k * 3;
	for (int ii = min_i; ii < min_i + 3; ii++)
	{
		for (int kk = min_k; kk < min_k + 3; kk++)
		{
			if ((*source)[ii][kk] < 0 || (*source)[ii][kk]>9)
				return false;
			if ((*source)[ii][kk] != 0 && (*source)[ii][kk] == num)
			{
				if (exists)
				{
					swprintf(buf, 256, L"Repeating value in small quad: row = %i, col = %i", ii, kk);
					return false;
				}
				else exists = true;
			}

		}
	}
	return true;
}
bool TSudoku::ValidateSource(TSource* source, bool check_zero)
{
	for (int n = 1; n <= 9; n++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			bool exists = false;
			for (int i = 0; i < SIZE; i++)
			{
				if ((*source)[i][c] == 0 && check_zero)
				{
					swprintf(buf, 256, L"Zero in result: row = %i, col = %i", i, c);
					return false;
				}
				if ((*source)[i][c] != 0 && (*source)[i][c] == n)
				{
					if (exists)
					{
						swprintf(buf, 256, L"Repeating value \"%i\" in col = %i", (*source)[i][c], c);
						return false;
					}
					else exists = true;
				}
			}
			exists = false;
			for (int k = 0; k < SIZE; k++)
				if ((*source)[c][k] != 0 && (*source)[c][k] == n)
				{
					if (exists)
					{
						swprintf(buf, 256, L"Repeating value \"%i\" in row = %i", (*source)[c][k], c);
						return false;
					}
					else exists = true;
				}
		}
		for (int i = 0; i < 3; i++)
		{
			for (int k = 0; k < 3; k++)
			{
				if (!ValidateSourceInBigCell(source, n, i, k))
					return false;
			}
		}
	}
	return true;
}

void  TSudoku::ConvertToResult(TSolution* r, TSource* result)
{
	for (int i = 0; i < SIZE; i++)
		for (int k = 0; k < SIZE; k++)
			(*result)[i][k] = (*r)[i][k].high == 0 ? (*r)[i][k].Get() : 0;
}

bool TSudoku::CalculateIterations(TSource* source, TSource* result)
{
	memset(result, 0, sizeof(int)*SIZE*SIZE);
	if (!ValidateSource(source))
	{
		std::cout << "Source data isn't correct!";
		return false;
	}
	TSolution* r = NULL;

		Init(source);
		if (!Calculate(r))
		{
			std::cout << "Solution not found!";
			return false;
		}

	if (!r)
		return false;
	ConvertToResult(r, result);
	if (!ValidateSource(result, true))
	{
		std::cout << "ERROR: Solution found, but it isn't correct!";
		return false;
	}
	return true;
}

