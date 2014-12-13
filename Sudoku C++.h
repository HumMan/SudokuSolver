
// Sudoku C++.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CSudokuCApp:
// О реализации данного класса см. Sudoku C++.cpp
//

class CSudokuCApp : public CWinApp
{
public:
	CSudokuCApp();

// Переопределение
	public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CSudokuCApp theApp;