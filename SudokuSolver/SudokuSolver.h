#pragma once


#define SIZE 9
#define QUAD_SIZE 3

typedef int TSource[SIZE][SIZE];

///одна из клеток и её возможные варианты
struct TSol
{
public:
	///кол-во бит в маске - 1
	int high;
	///битовая маска для вариантов чисел в ячейке
	unsigned int v;

	TSol();
	void Clear();
	void Add(int val);
	void Remove(int& sol_need, int val);
	int High();
	bool Exists(int val);
	unsigned int Get();
};

typedef TSol TSolution[SIZE][SIZE];

///сетка с числами судоку
class TSudoku
{
	static int BitCount(int v);
	
private:
	TSolution sol;
	int sol_need;
	bool has_source;
	void InitCellSolutions(int i, int k);
	void ValidateRow(int i);
	void ValidateCol(int k);
	void ValidateQuad(int i, int k);
	void ValidateQuads();
	void ValidateRows();
	void ValidateCols();
	void NakedInColumns();
	void NakedInRows();
	void RemoveNumFromRow(int i, int num, int exclude_min, int exclude_max);
	void RemoveNumFromCol(int k, int num, int exclude_min, int exclude_max);
	void ValidateQuadRowCol(int i, int k);
	void ValidateQuadsRowCol();

	void ConvertToResult(TSolution* r, TSource* result);
public:
	TSudoku();
	void Init(TSource* source);
	void Init(TSol* use_sol, int use_sol_need);
	struct TCountTableElement
	{
		TSol* sols[SIZE*SIZE];
		int high;
		void Add(TSol* use_sol)
		{
			sols[++high] = use_sol;
		}
		TCountTableElement()
		{
			high = -1;
		}
	};
	bool BuildSubLevel(TSolution* &result, bool first_level = false);
	bool Calculate(TSolution* &result, bool is_sublevel=true);
	static bool ValidateSourceInBigCell(TSource* source, int num, int i, int k);
	static bool ValidateSource(TSource* source, bool check_zero = false);
	bool CalculateIterations(TSource* source, TSource* result);
};

