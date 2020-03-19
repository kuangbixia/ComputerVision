// CFourierDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImgProcess1.h"
#include "CFourierDlg.h"
#include "afxdialogex.h"
#include "ImgProcess1Dlg.h"


// CFourierDlg 对话框

IMPLEMENT_DYNAMIC(CFourierDlg, CDialogEx)

CFourierDlg::CFourierDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FOURIER, pParent)
{
	dlg = NULL;
}

CFourierDlg::~CFourierDlg()
{
}

void CFourierDlg::fourier(void* p)
{
	dlg = new CImgProcess1Dlg();
	dlg = (CImgProcess1Dlg*)p;

	dlg->printLine(CString("正在进行暴力傅立叶变换..."));

	if (dlg->m_pImgTemp != NULL) {
		dlg->m_pImgTemp->Destroy();
		delete dlg->m_pImgTemp;
		dlg->m_pImgTemp = NULL;
	}
	dlg->m_pImgTemp = new CImage();
	dlg->imageCopy(dlg->m_pImgShow, dlg->m_pImgTemp);

	int subLength = dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() / dlg->m_nThreadNum;

	switch (dlg->mThreadType.GetCurSel()) {
	case 0:
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;

			AfxBeginThread((AFX_THREADPROC)&ImageProcess::fourierTransform, &dlg->m_pThreadParam[i]);
		}
	}
	break;
	case 1:
	{
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;

			ImageProcess::fourierTransform(&dlg->m_pThreadParam[i]);
		}
	}
	break;
	case 2:
	{
		dlg->m_pThreadParam[0].img = dlg->m_pImgTemp;
		dlg->m_pThreadParam[0].src = dlg->m_pImgShow;

		ImageProcess::fourierTransformCL(&dlg->m_pThreadParam[0]);
	}
	break;
	}
}

void CFourierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFourierDlg, CDialogEx)
END_MESSAGE_MAP()


// CFourierDlg 消息处理程序
