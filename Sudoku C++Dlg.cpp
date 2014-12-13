
// Sudoku C++Dlg.cpp : файл реализации
//

#include "stdafx.h"
#include "Sudoku C++.h"
#include "Sudoku C++Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// диалоговое окно CSudokuCDlg

CSudokuCDlg::CSudokuCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSudokuCDlg::IDD, pParent)
	, count(20000)
	, full_time(0)
	, avg_time(0)
	, source_text(_T(""))
	, result_text(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSudokuCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, count);
	DDX_Text(pDX, IDC_FULL_TIME, full_time);
	DDX_Text(pDX, IDC_AVERAGE_TIME, avg_time);
	DDX_Text(pDX, IDC_EDIT2, source_text);
	DDX_Text(pDX, IDC_EDIT3, result_text);
}

BEGIN_MESSAGE_MAP(CSudokuCDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CSudokuCDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSudokuCDlg::OnBnClickedButton2)
	ON_STN_CLICKED(IDC_AVERAGE_TIME, &CSudokuCDlg::OnStnClickedAverageTime)
END_MESSAGE_MAP()


// обработчики сообщений CSudokuCDlg

BOOL CSudokuCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Задает значок для этого диалогового окна. Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок. Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CSudokuCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CSudokuCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString ShowSudoku(int* sudoku)
{
	char buf[1000];
	int c=0;
	for(int i=0;i<SIZE;i++)
	{
		if(i%3==0&&i!=0)
				c+=sprintf_s(&buf[c],1000-c,"\r\n");
		for(int k=0;k<SIZE;k++)
		{
			if(k%3==0)
				c+=sprintf_s(&buf[c],1000-c,"   ");
			c+=sprintf_s(&buf[c],1000-c,"%i",sudoku[i*SIZE+k]);
		}
		c+=sprintf_s(&buf[c],1000-c,"\r\n");
	}
	return CString(buf);
}

void CSudokuCDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CFileDialog dlg(TRUE, NULL, _T("*.*"), 0);
	if(IDOK == dlg.DoModal())
	{
		CFile file(dlg.GetPathName(),CFile::modeRead);
		
		for(int i=0;i<SIZE;i++)
		{
			char c[SIZE];
			file.Read(&c,SIZE);
			for(int k=0;k<SIZE;k++)
			{
				source[i][k]=c[k]-'0';
			}
			file.Read(&c,2);
		}
		file.Close();
		source_text=ShowSudoku(&source[0][0]);
		UpdateData(false);
	}
}

void CSudokuCDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	TSource result;
	sudoku.CalculateIterations(full_time,avg_time,count,&source,&result);
	result_text=ShowSudoku(&result[0][0]);
	UpdateData(false);
}
void CSudokuCDlg::OnStnClickedAverageTime()
{
	// TODO: Add your control notification handler code here
}
