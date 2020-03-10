// CInterpolationDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImgProcess1.h"
#include "CInterpolationDlg.h"
#include "afxdialogex.h"


// CInterpolationDlg 对话框

IMPLEMENT_DYNAMIC(CInterpolationDlg, CDialogEx)

CInterpolationDlg::CInterpolationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INTERPOLATION, pParent)
{

}

CInterpolationDlg::~CInterpolationDlg()
{
}

void CInterpolationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInterpolationDlg, CDialogEx)
END_MESSAGE_MAP()


// CInterpolationDlg 消息处理程序
