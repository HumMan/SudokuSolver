#pragma once


#define SIZE 9
#define QUAD_SIZE 3

#include "math.h"
#include "assert.h"


//#define ROW_COL_OPTIM

static wchar_t buf[256];
typedef int TSource[SIZE][SIZE];
class TSudoku
{
	_forceinline int BitCount(int v)
	{
		v = v - ((v >> 1) & 0x55555555); // reuse input as temporary
		v = (v & 0x33333333) + ((v >> 2) & 0x33333333); // temp
		return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
	}
	struct TSol
	{
	public:
		int high;
		unsigned int v;
		_forceinline TSol()
		{
			Clear();
		}
		_forceinline void Clear()
		{
			high=-1;
			v=0;
		}
		_forceinline void Add(int val)
		{	
			assert(val>=1&&val<=9);
			int t=1<<(val);
			if(v&t)return;
			high++;
			v|=t;
		}
		_forceinline void Remove(int& sol_need,int val)
		{
			assert(val>=1&&val<=9);
			int t=1<<(val);
			if(!(v&t))return;
			high--;
			if(high==0)sol_need--;
			v^=t;
		}
		int High()
		{
			return high;
		}
		_forceinline bool Exists(int val)
		{
			assert(val>=1&&val<=9);
			return v&(1<<(val));
		}
		_forceinline unsigned int Get()
		{
			assert(high==0);
			unsigned long index=0;
			_BitScanForward(&index,v);
			assert(index!=0);
			return index;
		}
	};
	typedef TSol TSolution[SIZE][SIZE];
private:	
	TSolution sol;
	void InitCellSolutions(int i, int k)
	{
		int mask=0;
		for (int f = 0; f < SIZE; f++) 
			if (sol[i][f].high == 0) mask|=sol[i][f].v;
		for (int f = 0; f < SIZE; f++)
			if (sol[f][k].high == 0) mask|=sol[f][k].v;
		int min_i = (i / QUAD_SIZE) * QUAD_SIZE;
		int min_k = (k / QUAD_SIZE) * QUAD_SIZE;
		for (int ii = min_i; ii < min_i+3; ii++)
		{
			if (sol[ii][min_k+0].high == 0) mask|=sol[ii][min_k+0].v;
			if (sol[ii][min_k+1].high == 0) mask|=sol[ii][min_k+1].v;
			if (sol[ii][min_k+2].high == 0) mask|=sol[ii][min_k+2].v;
		}
		mask=(~mask)&1022;
		sol[i][k].v=(mask);
		sol[i][k].high=(BitCount(mask))-1;
	}
	bool InitCellsSolutions()
	{
		//
		int row[SIZE],col[SIZE],quad[QUAD_SIZE][QUAD_SIZE];
		for (int i = 0; i < SIZE; i++) 
		{
			row[i]=0;
			for (int f = 0; f < SIZE; f++) 
				if (sol[i][f].high == 0) row[i]|=sol[i][f].v;
		}
		for (int k = 0; k < SIZE; k++) 
		{
			col[k]=0;
			for (int f = 0; f < SIZE; f++)
				if (sol[f][k].high == 0) col[k]|=sol[f][k].v;
		}
		for (int i = 0; i < 3; i++) 
		{
			for (int k = 0; k < 3; k++) 
			{
				int min_i = i * QUAD_SIZE;
				int min_k = k * QUAD_SIZE;
				quad[i][k]=0;
				for (int ii = min_i; ii < min_i+3; ii++)
				{
					if (sol[ii][min_k+0].high == 0) quad[i][k]|=sol[ii][min_k+0].v;
					if (sol[ii][min_k+1].high == 0) quad[i][k]|=sol[ii][min_k+1].v;
					if (sol[ii][min_k+2].high == 0) quad[i][k]|=sol[ii][min_k+2].v;
				}
			}
		}
		int mask;
		for (int i = 0; i < SIZE; i++) 
		{
			for (int k = 0; k < SIZE; k++) 
				if(sol[i][k].high>0)
				{
					mask=(~(row[i]|col[k]|quad[i/3][k/3]))&1022;
					sol[i][k].v=mask;
					sol[i][k].high=(BitCount(mask))-1;
					if(sol[i][k].high==0)sol_need--;
					if(sol[i][k].v==0)
						return false;
				}
		}
		return true;
	}
	void ValidateRow( int i)
	{
		//проверяем встречается ли потенциальное число в строке один раз
		//и делаем его решением если такое имеется
#ifndef ROW_COL_OPTIM
		int f_k=-1;
		for(int n=1;n<=9;n++)
		{
			TSol* found=NULL;
			for (int k = 0; k < SIZE; k++) 
			{
				if(sol[i][k].high>0)
				{
					if(sol[i][k].Exists(n))
					{
						if(found==NULL)
						{
							f_k=k;
							found=&sol[i][k];
						}
						else
						{
							found=NULL;
							goto end_search;
						}
					}
				}else
				{
					assert(sol[i][k].high==0);
					if(sol[i][k].Exists(n))
						goto end_search;
				}
			}
			end_search:
			if(found!=NULL)
			{
				sol_need--;
				found->Clear();
				found->Add(n);
			}
			continue;
		}
#else
		int curr=0;//TODO данный способ хоть и побыстрее но он не реагирует на изменения сразу же
		int one=0;
		int several=0;
		for (int k = 0; k < SIZE; k++) 
		{
			curr=sol[i][k].v;
			if(sol[i][k].high>0)
			{
				one=(curr&~several)|((~curr)&one);
				several|=curr;
			}
		}
		for (int k = 0; k < SIZE; k++) 
		{
			curr=sol[i][k].v;
			if(sol[i][k].high>0)
			{
				int m=one&sol[i][k].v;
				if(m)
				{
					sol_need--;
					sol[i][k].high=0;
					sol[i][k].v=m;
				}
			}
		}
#endif
	}
	void ValidateCol( int k)
	{
		//проверяем встречается ли потенциальное число в столбце один раз
		//и делаем его решением если такое имеется
#ifndef ROW_COL_OPTIM
		for(int n=1;n<=9;n++)
		{
			TSol* found=NULL;
			for (int i = 0; i < SIZE; i++) 
			{
				if(sol[i][k].high>0)
				{
					if(sol[i][k].Exists(n))
					{
						if(found==NULL)
							found=&sol[i][k];
						else
						{
							found=NULL;
							goto end_search;
						}
					}
				}else
				{
					assert(sol[i][k].high==0);
					if(sol[i][k].Exists(n))
						goto end_search;
				}
			}
			end_search:
			if(found!=NULL)
			{
				sol_need--;
				found->Clear();
				found->Add(n);
			}
			continue;
		}
#else
		int curr=0;//TODO данный способ хоть и побыстрее но он не реагирует на изменения сразу же
		int one=0;
		int several=0;
		for (int i = 0; i < SIZE; i++) 
		{
			curr=sol[i][k].v;
			if(sol[i][k].high>0)
			{
				one=(curr&~several)|((~curr)&one);
				several|=curr;
			}
		}
		for (int i = 0; i < SIZE; i++) 
		{
			curr=sol[i][k].v;
			if(sol[i][k].high>0)
			{
				int m=one&sol[i][k].v;
				if(m)
				{
					sol_need--;
					sol[i][k].high=0;
					sol[i][k].v=m;
				}
			}
		}
#endif
	}
	void ValidateQuad( int i, int k)
	{
		//проверяем встречается ли потенциальное число в квадрате один раз
		//и делаем его решением если такое имеется
#ifndef ROW_COL_OPTIM
		int min_i = i * 3;
		int min_k = k * 3;
		for(int f=1;f<=9;f++)
		{
			TSol* found=NULL;
			for (int ii = min_i; ii < min_i+3; ii++)
			{
				for (int kk = min_k; kk < min_k+3; kk++)
				{
					if(sol[ii][kk].high>0)
					{
						if(sol[ii][kk].Exists(f))
						{
							if(found==NULL)
								found=&sol[ii][kk];
							else
							{
								found=NULL;
								goto end_search;
							}
						}
					}else
					{
						assert(sol[ii][kk].high==0);
						if(sol[ii][kk].Exists(f))
							goto end_search;
					}
				}
			}end_search:
			if(found!=NULL)
			{
				sol_need--;
				found->Clear();
				found->Add(f);
			}
			continue;
		}
#else
		int curr=0;//TODO данный способ хоть и побыстрее но он не реагирует на изменения сразу же
		int one=0;
		int several=0;
		int min_i = i * QUAD_SIZE;
		int min_k = k * QUAD_SIZE;
		for (int ii = min_i; ii < min_i+3; ii++)
		{
			for (int kk = min_k; kk < min_k+3; kk++)
			{
				curr=sol[ii][kk].v;
				if(sol[ii][kk].high>0)
				{
					one=(curr&~several)|((~curr)&one);
					several|=curr;
				}
			}
		}
		for (int ii = min_i; ii < min_i+3; ii++)
		{
			for (int kk = min_k; kk < min_k+3; kk++)
			{
				curr=sol[ii][kk].v;
				if(sol[ii][kk].high>0)
				{
					int m=one&sol[ii][kk].v;
					if(m)
					{
						sol_need--;
						sol[ii][kk].high=0;
						sol[ii][kk].v=m;
					}
				}
			}
		}
#endif
	}
	void ValidateQuads()
	{
		for (int i = 0; i < QUAD_SIZE; i++)
		{
			for (int k = 0; k < QUAD_SIZE; k++)
			{
				ValidateQuad(i,k);
			}
		}
	}
	void ValidateRows()
	{
		for (int i = 0; i < SIZE; i++) 
			ValidateRow(i);
	}
	void ValidateCols()
	{
		for (int k = 0; k < SIZE; k++) 
			ValidateRow(k);
	}
	void RemoveNumFromRow(int i,int num,int exclude_min,int exclude_max)
	{
		for (int f = 0; f < SIZE; f++) 
			if(f<exclude_min||f>exclude_max)
				if (sol[i][f].high > 0)sol[i][f].Remove(sol_need,num);
	}
	void RemoveNumFromCol(int k,int num,int exclude_min,int exclude_max)
	{
		for (int f = 0; f < SIZE; f++) 
			if(f<exclude_min||f>exclude_max)
				if (sol[f][k].high > 0)sol[f][k].Remove(sol_need,num);
	}
	void ValidateQuadRowCol( int i, int k)
	{
		int min_i = i * QUAD_SIZE;
		int min_k = k * QUAD_SIZE;
		int r[3];
		r[0]=0;
		r[1]=0;
		r[2]=0;
		//проверяем в каких строках какие цифры присутствуют
		for (int ii = 0; ii <3; ii++)
		{
			r[ii]|=sol[min_i+ii][min_k+0].v;
			r[ii]|=sol[min_i+ii][min_k+1].v;
			r[ii]|=sol[min_i+ii][min_k+2].v;
		}
		//если цифра присутствует только в одной строке квадрата то удаляем ее из строк остальных квадратов
		int m=1; 
		for(int f=1;f<=9;f++)
		{
			m<<=1;
			if((r[0]&m)&&!(r[1]&m)&&!(r[2]&m))
			{
				RemoveNumFromRow(min_i+0,f,min_k,min_k+2);
			}
			if(!(r[0]&m)&&(r[1]&m)&&!(r[2]&m))
			{
				RemoveNumFromRow(min_i+1,f,min_k,min_k+2);
			}
			if(!(r[0]&m)&&!(r[1]&m)&&(r[2]&m))
			{
				RemoveNumFromRow(min_i+2,f,min_k,min_k+2);
			}			
		}
		r[0]=0;
		r[1]=0;
		r[2]=0;
		//проверяем в каких столбцах какие цифры присутствуют
		for (int ii = 0; ii <3; ii++)
		{
			r[ii]|=sol[min_i+0][min_k+ii].v;
			r[ii]|=sol[min_i+1][min_k+ii].v;
			r[ii]|=sol[min_i+2][min_k+ii].v;
		}
		//если цифра присутствует только в одном столбце квадрата то удаляем ее из столбцов остальных квадратов
		m=1; 
		for(int f=1;f<=9;f++)
		{
			m<<=1;
			if((r[0]&m)&&!(r[1]&m)&&!(r[2]&m))
			{
				RemoveNumFromCol(min_k+0,f,min_i,min_i+2);
			}
			if(!(r[0]&m)&&(r[1]&m)&&!(r[2]&m))
			{
				RemoveNumFromCol(min_k+1,f,min_i,min_i+2);
			}
			if(!(r[0]&m)&&!(r[1]&m)&&(r[2]&m))
			{
				RemoveNumFromCol(min_k+2,f,min_i,min_i+2);
			}			
		}
	}
	void ValidateQuadsRowCol()
	{
		for (int i = 0; i < QUAD_SIZE; i++)
		{
			for (int k = 0; k < QUAD_SIZE; k++)
			{
				ValidateQuadRowCol(i,k);
			}
		}
	}
	int sol_need;
	bool has_source;
public:
	TSudoku()
	{
		sol_need=-1;
		has_source=false;
	}
	void Init(TSource* source)
	{
		has_source=true;
		sol_need=SIZE*SIZE;
		for (int i = 0; i < SIZE; i++)
		{
			for (int k = 0; k < SIZE; k++)
			{
				sol[i][k].Clear();
				if ((*source)[i][k] > 0)
				{
					sol_need--;
					sol[i][k].Add((*source)[i][k]);
				}else
				{
					sol[i][k].high=8;
					sol[i][k].v=1022;
				}
			}
		}
	}
	void Init(TSol* use_sol,int use_sol_need)
	{
		has_source=true;
		sol_need=use_sol_need;
		memcpy(&sol,use_sol,SIZE*SIZE*sizeof(TSol));
	}
	struct TCountTableElement
	{
		TSol* sols[SIZE*SIZE];
		int high;
		void Add(TSol* use_sol)
		{
			sols[++high]=use_sol;
		}
		TCountTableElement()
		{
			high=-1;
		}
	};
	bool BuildSubLevel(TSolution* &result)
	{
		TSudoku sub_level;
		TCountTableElement table[9];
		for (int i = 0; i < SIZE; i++)
		{
			for (int k = 0; k < SIZE; k++)
				if(sol[i][k].high>0)
					table[sol[i][k].High()].Add(&sol[i][k]);
		}
		for (int c = 1; c < 9; c++)
		{
			for (int t = 1; t <= table[c].high; t++)
			{
				for (int n = 1; n <= 9; n++)
					if(table[c].sols[t]->Exists(n))
					{
						TSol temp=*table[c].sols[t];
						table[c].sols[t]->Clear();
						table[c].sols[t]->Add(n);

						sub_level.Init(&sol[0][0],sol_need-1);
						if(sub_level.Calculate(result))
						{
							Init(&sub_level.sol[0][0],sub_level.sol_need);
							result=&sol;
							return true;
						}else
						{
							*table[c].sols[t]=temp;
						}
					}
			}
		}
		return false;
	}
	bool Calculate(TSolution* &result)
	{
		assert(has_source);
		do
		{
			int last_sol=sol_need;
			for (int i = 0; i < 9; i++)
			{
				for (int k = 0; k < 9; k++)
					if(sol[i][k].high>0)
					{
						sol[i][k].Clear();
						InitCellSolutions(i,k);
						if(sol[i][k].high==0)sol_need--;
						if(sol[i][k].high==-1)
							return false;
					}
			}
			//if(!InitCellsSolutions())
			//	return false;
			//if(last_sol==sol_need)
			//	ValidateQuadsRowCol();
			if(last_sol==sol_need)
				ValidateRows();
			if(last_sol==sol_need)
				ValidateCols();	
			if(last_sol==sol_need)
				ValidateQuads();
			for (int i = 0; i < 9; i++)
			{
				for (int k = 0; k < 9; k++)
					if(sol[i][k].high==-1)
						return false;
			}
			if(last_sol==sol_need)
				return BuildSubLevel(result);
		} while (sol_need!=0);
		result=&sol;
		return true;
	}
	static bool ValidateSourceInBigCell(TSource* source,int num, int i, int k)
	{
		assert(num>=1&&num<=9);
		bool exists=false;
		int min_i = i * 3;
		int min_k = k * 3;
		for (int ii = min_i; ii < min_i+3; ii++)
		{
			for (int kk = min_k; kk < min_k+3; kk++)
			{
				if((*source)[ii][kk]<0||(*source)[ii][kk]>9)
					return false;
				if ((*source)[ii][kk]!= 0 && (*source)[ii][kk]==num)
				{
					if(exists)
					{
						swprintf(buf,256,L"Repeating value in small quad: row = %i, col = %i",ii,kk);
						return false;
					}
					else exists=true;
				}

			}
		}
		return true;
	}
	static bool ValidateSource(TSource* source,bool check_zero=false)
	{
		for(int n=1;n<=9;n++)
		{
			for (int c = 0; c < SIZE; c++)
			{
				bool exists=false;
				for (int i = 0; i < SIZE; i++)
				{
					if((*source)[i][c]== 0 && check_zero)
					{
						swprintf(buf,256,L"Zero in result: row = %i, col = %i",i,c);
						return false;
					}
					if ((*source)[i][c]!= 0 && (*source)[i][c]==n)
					{
						if(exists)
						{
							swprintf(buf,256,L"Repeating value in col = %i",c);
							return false;
						}
						else exists=true;
					}
				}
				exists=false;
				for (int k = 0; k < SIZE; k++)
					if ((*source)[c][k]!= 0 && (*source)[c][k]==n)
					{
						if(exists)
						{
							swprintf(buf,256,L"Repeating value in row = %i",c);
							return false;
						}
						else exists=true;
					}
			}
			for (int i = 0; i < 3; i++)
			{
				for (int k = 0; k < 3; k++)
				{
					if(!ValidateSourceInBigCell(source,n,i,k))return false;
				}
			}
		}
		return true;
	}
	void CalculateIterations(float& full_time,float& avg_time,int iterations_count,TSource* source,TSource* result)
	{
		memset(result,0,sizeof(int)*SIZE*SIZE);
		if(!ValidateSource(source))
		{
			MessageBox(0,_T("Source data isn't correct!"),_T("Error!"),MB_ICONERROR);
			MessageBox(0,(LPCWSTR)buf,_T("Info!"),MB_ICONERROR);
			return;
		}
		int time_before = GetTickCount();
		TSolution* r=NULL;
		for (int s = 1; s <= iterations_count; s++)
		{
			Init(source);
			if(!Calculate(r))
			{
				MessageBox(0,_T("Solution not found!"),_T("Error!"),MB_ICONERROR);
				return;
			}
		}
		if(!r)return;
		for (int i = 0; i < 9; i++)
			for (int k = 0; k < 9; k++)
				(*result)[i][k] = (*r)[i][k].high==0?(*r)[i][k].Get():0;
		if(!ValidateSource(result,true))
		{
			MessageBox(0,_T("Solution found, but it isn't correct!"),_T("Error!"),MB_ICONERROR);
			MessageBox(0,(LPCWSTR)buf,_T("Info!"),MB_ICONERROR);
			return;
		}
		int time_elapsed = GetTickCount() - time_before;
		full_time=time_elapsed / 1000.0f;
		avg_time= time_elapsed / (float)iterations_count;
	}
};
