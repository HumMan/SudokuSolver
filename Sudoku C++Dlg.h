
// Sudoku C++Dlg.h : ���� ���������
//

#pragma once

#include "sudoku.h"

// ���������� ���� CSudokuCDlg
class CSudokuCDlg : public CDialog
{
// ��������
public:
	CSudokuCDlg(CWnd* pParent = NULL);	// ����������� �����������

// ������ ����������� ����
	enum { IDD = IDD_SUDOKUC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// ��������� DDX/DDV


// ����������
protected:
	HICON m_hIcon;
	TSudoku sudoku;
	TSource source;

	// ��������� ������� ����� ���������
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
