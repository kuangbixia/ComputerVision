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
	DDX_Control(pDX, IDC_COMBO_SorR, mScaleOrRotate);
	DDX_Control(pDX, IDC_EDIT_XSCALE, mEditXscale);
	DDX_Control(pDX, IDC_EDIT_YSCALE, mEditYscale);
	DDX_Control(pDX, IDC_EDIT_ANGLE, mEditAngle);
}

BOOL CInterpolationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	mScaleOrRotate.InsertString(0, _T("缩放"));
	mScaleOrRotate.InsertString(1, _T("旋转"));
	mScaleOrRotate.SetCurSel(0);
	return 0;
}

void CInterpolationDlg::scale(void* p)
{
	dlg = new CImgProcess1Dlg();
	dlg = (CImgProcess1Dlg*)p;

	CString xText, yText;
	mEditXscale.GetWindowTextW(xText);
	mEditYscale.GetWindowTextW(yText);
	if (xText.IsEmpty()||yText.IsEmpty()) {
		dlg->printLine(CString("还没有输入缩放系数。"));
		return;
	}
	float x = _ttof(xText);
	float y = _ttof(yText);


	if (dlg->m_pImgTemp != NULL) {
		dlg->m_pImgTemp->Destroy();
		delete dlg->m_pImgTemp;
		dlg->m_pImgTemp = NULL;
	}
	dlg->m_pImgTemp = new CImage();
	dlg->imageCopy(dlg->m_pImgShow, dlg->m_pImgTemp);


	int width = (x < 1 ? x : 1) * dlg->m_pImgTemp->GetWidth();
	int height = (y < 1 ? y : 1) * dlg->m_pImgTemp->GetHeight();
	// 每个线程处理的像素数
	//int subLength = m_pImgTemp->GetWidth() * m_pImgTemp->GetHeight() / m_nThreadNum;
	int subLength = width * height / dlg->m_nThreadNum;

	int thread = dlg->mThreadType.GetCurSel();
	switch (thread) {
	case 0: // WIN多线程
	{
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
		break;
	}
	case 1: // OpenMP
	{
		// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; i++) {
			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;
			dlg->m_pThreadParam[i].xscale = x;
			dlg->m_pThreadParam[i].yscale = y;
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ? (i + 1) * subLength - 1 : width * height - 1;
			ImageProcess::cubicScale(&dlg->m_pThreadParam[i]);
		}
		break;
	}
	}
}

void CInterpolationDlg::rotate(void* p)
{
	dlg = new CImgProcess1Dlg();
	dlg = (CImgProcess1Dlg*)p;
	CString aText;
	mEditAngle.GetWindowTextW(aText);
	if (aText.IsEmpty()) {
		dlg->printLine(CString("还没输入旋转角度。"));
		return;
	}
	float alpha = _ttof(aText);
	alpha = alpha * acos(-1) / 180; // 转换弧度制

	if (dlg->m_pImgTemp != NULL) {
		dlg->m_pImgTemp->Destroy();
		delete dlg->m_pImgTemp;
		dlg->m_pImgTemp = NULL;
	}
	dlg->m_pImgTemp = new CImage();
	dlg->imageCopy(dlg->m_pImgShow, dlg->m_pImgTemp);

	int width = dlg->m_pImgTemp->GetWidth();
	int height = dlg->m_pImgTemp->GetHeight();
	// 每个线程处理的像素数
	int subLength = width * height / dlg->m_nThreadNum;

	int thread = dlg->mThreadType.GetCurSel();
	switch (thread) {
	case 0: // WIN多线程
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;
			dlg->m_pThreadParam[i].alpha = alpha;
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ? (i + 1) * subLength - 1 : width * height - 1;

			// windows MFC 创建线程
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::cubicRotate, &dlg->m_pThreadParam[i]);
		}
		break;
	}
	case 1: // OpenMP
	{
		// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; i++) {
			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;
			dlg->m_pThreadParam[i].alpha = alpha;
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ? (i + 1) * subLength - 1 : width * height - 1;
			ImageProcess::cubicRotate(&dlg->m_pThreadParam[i]);
		}
		break;
	}
	}
}



BEGIN_MESSAGE_MAP(CInterpolationDlg, CDialogEx)
END_MESSAGE_MAP()


// CInterpolationDlg 消息处理程序