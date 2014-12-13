
// Sudoku C++Dlg.h : файл заголовка
//

#pragma once

#include "sudoku.h"

// диалоговое окно CSudokuCDlg
class CSudokuCDlg : public CDialog
{
// Создание
public:
	CSudokuCDlg(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
	enum { IDD = IDD_SUDOKUC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;
	TSudoku sudoku;
	TSource source;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	int count;
	float full_time;
	float avg_time;
	CString source_text;
	CString result_text;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnStnClickedAverageTime();
};
