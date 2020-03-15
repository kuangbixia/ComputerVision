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

	if (dlg->m_pImgTemp != NULL) {
		dlg->m_pImgTemp->Destroy();
		delete dlg->m_pImgTemp;
		dlg->m_pImgTemp = NULL;
	}
	dlg->m_pImgTemp = new CImage();
	dlg->imageCopy(dlg->m_pImgShow, dlg->m_pImgTemp);

	switch (thread) {
	case 0: // WIN多线程
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			// windows MFC 创建线程
			switch (mFilterType.GetCurSel()) {
			case 0: // 自适应中值滤波
			{
				int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;
				dlg->m_pThreadParam[i].startIndex = i * subLength;
				dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
					(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;

				dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
				dlg->m_pThreadParam[i].maxSpan = MAX_SPAN;
				AfxBeginThread((AFX_THREADPROC)&ImageProcess::medianFilter, &dlg->m_pThreadParam[i]);
				break;
			}
			case 1: // 加权均值线性滤波
			{
				int subLength = dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() / dlg->m_nThreadNum;
				dlg->m_pThreadParam[i].startIndex = i * subLength;
				dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
					(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

				dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
				dlg->m_pThreadParam[i].src = dlg->m_pImgShow;
				AfxBeginThread((AFX_THREADPROC)&ImageProcess::meanFilter, &dlg->m_pThreadParam[i]);
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
		break;
	}
	case 1: // OpenMP
	{
		// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			switch (mFilterType.GetCurSel()) {
			case 0: // 自适应中值滤波
			{
				int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;
				dlg->m_pThreadParam[i].startIndex = i * subLength;
				dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
					(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;

				dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
				dlg->m_pThreadParam[i].maxSpan = MAX_SPAN;
				ImageProcess::medianFilter(&dlg->m_pThreadParam[i]);
				break;
			}
			case 1: // 加权均值滤波
			{
				int subLength = dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() / dlg->m_nThreadNum;
				dlg->m_pThreadParam[i].startIndex = i * subLength;
				dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
					(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

				dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
				dlg->m_pThreadParam[i].src = dlg->m_pImgShow;
				ImageProcess::meanFilter(&dlg->m_pThreadParam[i]);
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
		break;
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
	mFilterType.InsertString(1, _T("加权均值滤波"));
	mFilterType.InsertString(2, _T("高斯滤波"));
	mFilterType.InsertString(3, _T("维纳滤波"));
	mFilterType.SetCurSel(0);
	return 0;
}


BEGIN_MESSAGE_MAP(CFilterDlg, CDialogEx)
END_MESSAGE_MAP()


// CFilterDlg 消息处理程序
