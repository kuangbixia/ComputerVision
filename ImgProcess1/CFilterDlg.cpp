// CFilterDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImgProcess1.h"
#include "CFilterDlg.h"
#include "afxdialogex.h"


// CFilterDlg 对话框

IMPLEMENT_DYNAMIC(CFilterDlg, CDialogEx)

CFilterDlg::CFilterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILTER, pParent)
{

}

CFilterDlg::~CFilterDlg()
{
}

void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FILTERTYPE, mFilterType);
}


BEGIN_MESSAGE_MAP(CFilterDlg, CDialogEx)
END_MESSAGE_MAP()


// CFilterDlg 消息处理程序
