#include "SudokuSolver.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>

using namespace std;
namespace fs = std::filesystem;

void read_from_stream(std::ifstream &is, TSource &result)
{
	for (int i = 0; i < SIZE; i++)
	{
		char c[SIZE];
		is.read(c, SIZE);
		for (int k = 0; k < SIZE; k++)
		{
			result[i][k] = c[k] - '0';
		}
		is.read(c, 1);
	}
}

void wrtie_to_stream(std::ofstream &stream, TSource &result)
{
	for (int i = 0; i < SIZE; i++)
	{
		char c[SIZE];
		for (int k = 0; k < SIZE; k++)
		{
			c[k] = '0' + result[i][k];
		}
		stream.write(c, SIZE);
		
		stream << std::endl;
	}
}

int main()
{
	fs::path input_path = "to_solve";
	fs::path results_path = "results";
	for (const auto & entry : fs::directory_iterator(input_path))
	{
		auto path = entry.path();
		cout << "Solving '" << path << "'..." << endl;
		clock_t begin = clock();

		std::ifstream is(path);
		TSource source;
		read_from_stream(is, source);
		is.close();

		TSudoku sudoku;
		TSource result;
		auto resolved = sudoku.CalculateIterations(&source, &result);
		clock_t end = clock();
		if (resolved)
		{
			std::ofstream out_stream(results_path / entry.path().filename(), ios::out);
			wrtie_to_stream(out_stream, result);
			out_stream.close();
			cout << "solved in " << (double(end - begin) / CLOCKS_PER_SEC) << " sec" << endl;
		}
		else
		{
			cout << "not solved in " << (double(end - begin) / CLOCKS_PER_SEC) << " sec" << endl;
		}	
	}

	cout << "Hello CMake." << endl;
	return 0;
}
