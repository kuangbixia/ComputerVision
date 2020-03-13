// CInterpolationDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImgProcess1.h"
#include "CInterpolationDlg.h"
#include "afxdialogex.h"
#include "ImgProcess1Dlg.h"


// CInterpolationDlg 对话框

IMPLEMENT_DYNAMIC(CInterpolationDlg, CDialogEx)

CInterpolationDlg::CInterpolationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INTERPOLATION, pParent)
{
	dlg = NULL;
}

CInterpolationDlg::~CInterpolationDlg()
{
}

void CInterpolationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CInterpolationDlg::scale(void* p)
{
	float x = 0.5, y = 0.5;

	dlg = new CImgProcess1Dlg();
	dlg = (CImgProcess1Dlg*)p;

	if (dlg->m_pImgTemp != NULL) {
		dlg->m_pImgTemp->Destroy();
		delete dlg->m_pImgTemp;
		dlg->m_pImgTemp = NULL;
	}
	dlg->m_pImgTemp = new CImage();

	/*int alpha = m_pImgShow->GetBPP() == 32 ? 1 : 0;
	bool createYes = m_pImgTemp->Create(int(x * m_pImgShow->GetWidth()), int(y * m_pImgShow->GetHeight()), m_pImgShow->GetBPP(), alpha);
	m_pImgShow->StretchBlt(m_pImgTemp->GetDC(), CRect(0, 0, m_pImgTemp->GetWidth(), m_pImgTemp->GetHeight()), CRect(0, 0, m_pImgTemp->GetWidth(), m_pImgTemp->GetHeight()));
	m_pImgTemp->ReleaseDC();*/

	dlg->m_pImgTemp->Load(dlg->strFilePath);

	int thread = dlg->mThreadType.GetCurSel();
	switch (thread) {
	case 0: // WIN多线程
	{
		scale_WIN(x, y);
		break;
	}
	case 1: // OpenMP
	{
		scale_OPENMP(x, y);
		break;
	}
	}
}

void CInterpolationDlg::scale_WIN(float x, float y)
{
	int width = (x < 1 ? x : 1) * dlg->m_pImgTemp->GetWidth();
	int height = (y < 1 ? y : 1) * dlg->m_pImgTemp->GetHeight();

	// 每个线程处理的像素数
	//int subLength = m_pImgTemp->GetWidth() * m_pImgTemp->GetHeight() / m_nThreadNum;
	int subLength = width * height / dlg->m_nThreadNum;


	for (int i = 0; i < dlg->m_nThreadNum; ++i)
	{
		dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
		dlg->m_pThreadParam[i].src = dlg->m_pImgShow;
		dlg->m_pThreadParam[i].xscale = x;
		dlg->m_pThreadParam[i].yscale = y;
		dlg->m_pThreadParam[i].startIndex = i * subLength;
		dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ? (i + 1) * subLength - 1 : width * height - 1;

		// windows MFC 创建线程
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::cubicScale, &dlg->m_pThreadParam[i]);
	}
}

void CInterpolationDlg::scale_OPENMP(float x, float y)
{
	int width = (x < 1 ? x : 1) * dlg->m_pImgTemp->GetWidth();
	int height = (y < 1 ? y : 1) * dlg->m_pImgTemp->GetHeight();

	// 每个线程处理的像素数
	//int subLength = m_pImgTemp->GetWidth() * m_pImgTemp->GetHeight() / m_nThreadNum;
	int subLength = width * height / dlg->m_nThreadNum;

	// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(m_nThreadNum)
	for (int i = 0; i < dlg->m_nThreadNum; i++) {
		dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
		dlg->m_pThreadParam[i].src = dlg->m_pImgShow;
		dlg->m_pThreadParam[i].xscale = x;
		dlg->m_pThreadParam[i].yscale = y;
		dlg->m_pThreadParam[i].startIndex = i * subLength;
		dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ? (i + 1) * subLength - 1 : width * height - 1;
		ImageProcess::cubicScale(&dlg->m_pThreadParam[i]);
	}
}


BEGIN_MESSAGE_MAP(CInterpolationDlg, CDialogEx)
END_MESSAGE_MAP()


// CInterpolationDlg 消息处理程序