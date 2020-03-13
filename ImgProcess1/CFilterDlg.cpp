// CFilterDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImgProcess1.h"
#include "CFilterDlg.h"
#include "afxdialogex.h"
#include "ImgProcess1Dlg.h"


// CFilterDlg 对话框

IMPLEMENT_DYNAMIC(CFilterDlg, CDialogEx)

CFilterDlg::CFilterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILTER, pParent)
{
	dlg = NULL;
}

CFilterDlg::~CFilterDlg()
{
}

void CFilterDlg::filter(void* p)
{
	dlg = new CImgProcess1Dlg();
	dlg = (CImgProcess1Dlg*)p;

	int thread = dlg->mThreadType.GetCurSel();

	switch (thread) {
	case 0: // WIN多线程
	{
		filter_WIN();
		break;
	}
	case 1: // OpenMP
	{
		filter_OPENMP();
		break;
	}
	}
}

void CFilterDlg::filter_WIN()
{
	int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;
	int h = dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;
	int w = dlg->m_pImgShow->GetWidth() / dlg->m_nThreadNum;
	for (int i = 0; i < dlg->m_nThreadNum; ++i)
	{
		dlg->m_pThreadParam[i].startIndex = i * subLength;
		dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;
		dlg->m_pThreadParam[i].maxSpan = MAX_SPAN;
		dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
		// windows MFC 创建线程
		switch (mFilterType.GetCurSel()) {
		case 0: // 自适应中值滤波
		{
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::medianFilter, &dlg->m_pThreadParam[i]);
			break;
		}
		case 1: // todo:平滑线性滤波
		{
			break;
		}
		case 2: // todo:高斯滤波
		{
			break;
		}
		case 3: // todo:维纳滤波
		{
			break;
		}
		}
	}
}

void CFilterDlg::filter_OPENMP()
{
	// 每个线程处理的像素数
	int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;

	// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(m_nThreadNum)
	for (int i = 0; i < dlg->m_nThreadNum; ++i)
	{
		dlg->m_pThreadParam[i].startIndex = i * subLength;
		dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;
		dlg->m_pThreadParam[i].maxSpan = MAX_SPAN;
		dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
		switch (mFilterType.GetCurSel()) {
		case 0: // 自适应中值滤波
		{
			ImageProcess::medianFilter(&dlg->m_pThreadParam[i]);
			break;
		}
		case 1: // todo:平滑线性滤波
		{
			break;
		}
		case 2: // todo:高斯滤波
		{
			break;
		}
		case 3: // todo:维纳滤波
		{
			break;
		}
		}
	}
}

void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FILTERTYPE, mFilterType);
}

BOOL CFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	mFilterType.InsertString(0, _T("自适应中值滤波"));
	mFilterType.InsertString(1, _T("平滑线性滤波"));
	mFilterType.InsertString(2, _T("高斯滤波"));
	mFilterType.InsertString(3, _T("维纳滤波"));
	mFilterType.SetCurSel(0);
	return 0;
}


BEGIN_MESSAGE_MAP(CFilterDlg, CDialogEx)
END_MESSAGE_MAP()


// CFilterDlg 消息处理程序
