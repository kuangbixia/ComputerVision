// CNoiseDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImgProcess1.h"
#include "CNoiseDlg.h"
#include "afxdialogex.h"


// CNoiseDlg 对话框

IMPLEMENT_DYNAMIC(CNoiseDlg, CDialogEx)

CNoiseDlg::CNoiseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NOISE, pParent)
{

}

CNoiseDlg::~CNoiseDlg()
{
}

void CNoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_NOISETYPE, mNoiseType);
}


BEGIN_MESSAGE_MAP(CNoiseDlg, CDialogEx)
END_MESSAGE_MAP()


// CNoiseDlg 消息处理程序
