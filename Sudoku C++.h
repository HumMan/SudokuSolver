
// Sudoku C++.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CSudokuCApp:
// � ���������� ������� ������ ��. Sudoku C++.cpp
//

class CSudokuCApp : public CWinApp
{
public:
	CSudokuCApp();

// ���������������
	public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CSudokuCApp theApp;