// CFourierDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImgProcess1.h"
#include "CFourierDlg.h"
#include "afxdialogex.h"


// CFourierDlg 对话框

IMPLEMENT_DYNAMIC(CFourierDlg, CDialogEx)

CFourierDlg::CFourierDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FOURIER, pParent)
{

}

CFourierDlg::~CFourierDlg()
{
}

void CFourierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFourierDlg, CDialogEx)
END_MESSAGE_MAP()


// CFourierDlg 消息处理程序
