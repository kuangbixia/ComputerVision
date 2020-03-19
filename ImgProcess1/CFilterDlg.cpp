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

	switch (mFilterType.GetCurSel()) {
	case 0: // 自适应中值滤波
	{
		medianFilter();
	}
	break;

	case 1: // 加权均值滤波
	{
		meanFilter();
	}
	break;

	case 2: // 高斯滤波
	{
		gaussianFilter();
	}
	break;

	case 3: // 维纳滤波
	{
		wienerFilter();
	}
	break;

	case 4: // 双边滤波
	{
		bilateralFilter();
	}
	break;
	}
}

void CFilterDlg::medianFilter()
{
	dlg->printLine(CString("正在进行自适应中值滤波..."));
	int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;

	int thread = dlg->mThreadType.GetCurSel();
	switch (thread) {
	case 0: // WIN多线程
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
			dlg->m_pThreadParam[i].maxSpan = MAX_SPAN;

			AfxBeginThread((AFX_THREADPROC)&ImageProcess::medianFilter, &dlg->m_pThreadParam[i]);
		}
	}
	break;

	case 1: // OpenMP
	{
		// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
			dlg->m_pThreadParam[i].maxSpan = MAX_SPAN;

			ImageProcess::medianFilter(&dlg->m_pThreadParam[i]);
		}
	}
	break;
	}

}


void CFilterDlg::meanFilter()
{
	dlg->printLine(CString("正在进行均值滤波..."));
	if (dlg->m_pImgTemp != NULL) {
		dlg->m_pImgTemp->Destroy();
		delete dlg->m_pImgTemp;
		dlg->m_pImgTemp = NULL;
	}
	dlg->m_pImgTemp = new CImage();
	dlg->imageCopy(dlg->m_pImgShow, dlg->m_pImgTemp);

	int subLength = dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() / dlg->m_nThreadNum;

	int thread = dlg->mThreadType.GetCurSel();
	switch (thread) {
	case 0: // WIN多线程
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;

			AfxBeginThread((AFX_THREADPROC)&ImageProcess::meanFilter, &dlg->m_pThreadParam[i]);
		}
	}
	break;

	case 1: // OpenMP
	{
		// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
			dlg->m_pThreadParam[i].maxSpan = MAX_SPAN;

			ImageProcess::medianFilter(&dlg->m_pThreadParam[i]);
		}
	}
	break;
	}
}

void CFilterDlg::gaussianFilter()
{
	CString sText;
	mEditStddev.GetWindowTextW(sText);
	if (sText.IsEmpty()) {
		AfxMessageBox(CString("还没有输入标准差。"));
		return;
	}
	dlg->printLine(CString("正在进行高斯滤波，标准差：") + sText + CString(" ..."));

	if (dlg->m_pImgTemp != NULL) {
		dlg->m_pImgTemp->Destroy();
		delete dlg->m_pImgTemp;
		dlg->m_pImgTemp = NULL;
	}
	dlg->m_pImgTemp = new CImage();
	dlg->imageCopy(dlg->m_pImgShow, dlg->m_pImgTemp);

	int subLength = dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() / dlg->m_nThreadNum;

	int thread = dlg->mThreadType.GetCurSel();
	switch (thread) {
	case 0: // WIN多线程
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;
			
			dlg->m_pThreadParam[i].stddev = _ttof(sText);

			AfxBeginThread((AFX_THREADPROC)&ImageProcess::gaussianFilter, &dlg->m_pThreadParam[i]);
		}
	}
	break;

	case 1: // OpenMP
	{
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;
			
			dlg->m_pThreadParam[i].stddev = _ttof(sText);

			ImageProcess::gaussianFilter(&dlg->m_pThreadParam[i]);
		}
	}
	break;


	}
}

void CFilterDlg::wienerFilter()
{
	dlg->printLine(CString("正在进行维纳滤波..."));
	if (dlg->m_pImgTemp != NULL) {
		dlg->m_pImgTemp->Destroy();
		delete dlg->m_pImgTemp;
		dlg->m_pImgTemp = NULL;
	}
	dlg->m_pImgTemp = new CImage();
	dlg->imageCopy(dlg->m_pImgShow, dlg->m_pImgTemp);

	int subLength = dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() / dlg->m_nThreadNum;

	int thread = dlg->mThreadType.GetCurSel();
	switch (thread) {
	case 0: // WIN多线程
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;

			AfxBeginThread((AFX_THREADPROC)&ImageProcess::wienerFilter, &dlg->m_pThreadParam[i]);
		}
	}
	break;

	case 1: // OpenMP
	{
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;

			ImageProcess::wienerFilter(&dlg->m_pThreadParam[i]);
		}
	}
	break;
	}
}

void CFilterDlg::bilateralFilter()
{
	CString dText, rText;
	mEditSigmaD.GetWindowTextW(dText);
	mEditSigmaR.GetWindowTextW(rText);
	if (dText.IsEmpty() || rText.IsEmpty()) {
		AfxMessageBox(CString("还没有输入系数。"));
		return;
	}
	dlg->printLine(CString("正在进行双边滤波，sigma_d：") + dText + CString("，sigma_r：") + rText + CString(" ..."));

	if (dlg->m_pImgTemp != NULL) {
		dlg->m_pImgTemp->Destroy();
		delete dlg->m_pImgTemp;
		dlg->m_pImgTemp = NULL;
	}
	dlg->m_pImgTemp = new CImage();
	dlg->imageCopy(dlg->m_pImgShow, dlg->m_pImgTemp);

	int subLength = dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() / dlg->m_nThreadNum;

	int thread = dlg->mThreadType.GetCurSel();
	switch (thread) {
	case 0: // WIN多线程
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;

			dlg->m_pThreadParam[i].sigma_d = _ttof(dText);
			dlg->m_pThreadParam[i].sigma_r = _ttof(rText);

			AfxBeginThread((AFX_THREADPROC)&ImageProcess::bilateralFilter, &dlg->m_pThreadParam[i]);
		}
	}
	break;

	case 1: // OpenMP
	{
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgTemp->GetWidth() * dlg->m_pImgTemp->GetHeight() - 1;

			dlg->m_pThreadParam[i].img = dlg->m_pImgTemp;
			dlg->m_pThreadParam[i].src = dlg->m_pImgShow;

			dlg->m_pThreadParam[i].sigma_d = _ttof(dText);
			dlg->m_pThreadParam[i].sigma_r = _ttof(rText);

			ImageProcess::bilateralFilter(&dlg->m_pThreadParam[i]);
		}
	}
	break;
	}
}


void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FILTERTYPE, mFilterType);
	DDX_Control(pDX, IDC_EDIT_STDDEV, mEditStddev);
	DDX_Control(pDX, IDC_COMBO_THREAD, mEditSigmaD);
	DDX_Control(pDX, IDC_EDIT_SIGMAR, mEditSigmaR);
}

BOOL CFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	mFilterType.InsertString(0, _T("自适应中值滤波"));
	mFilterType.InsertString(1, _T("加权均值滤波"));
	mFilterType.InsertString(2, _T("高斯滤波"));
	mFilterType.InsertString(3, _T("维纳滤波"));
	mFilterType.InsertString(4, _T("双边滤波"));
	mFilterType.SetCurSel(0);
	return 0;
}


BEGIN_MESSAGE_MAP(CFilterDlg, CDialogEx)
END_MESSAGE_MAP()


// CFilterDlg 消息处理程序
