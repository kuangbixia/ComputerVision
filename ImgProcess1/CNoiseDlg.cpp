// CNoiseDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImgProcess1.h"
#include "CNoiseDlg.h"
#include "afxdialogex.h"
#include "ImgProcess1Dlg.h"

#include<omp.h>


// CNoiseDlg 对话框

IMPLEMENT_DYNAMIC(CNoiseDlg, CDialogEx)

CNoiseDlg::CNoiseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NOISE, pParent)
{
	dlg = NULL;
}

CNoiseDlg::~CNoiseDlg()
{
}

void CNoiseDlg::addNoise(void* p)
{
	dlg = new CImgProcess1Dlg();
	dlg = (CImgProcess1Dlg*)p;
	switch (mNoiseType.GetCurSel()) {
	case 0: // 椒盐噪声
	{
		saltNoise();
	}
	break;

	case 1: // 高斯噪声
	{
		gaussianNoise();
	}
	break;

	}
}

void CNoiseDlg::saltNoise()
{
	int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;

	int thread = dlg->mThreadType.GetCurSel();

	switch (thread) {
	case 0: // WIN多线程
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;

			AfxBeginThread((AFX_THREADPROC)&ImageProcess::saltNoise, &dlg->m_pThreadParam[i]);
		}
	}
	break;

	case 1: // OpenMP
	{
		// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
			for (int i = 0; i < dlg->m_nThreadNum; i++) {
				dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
				dlg->m_pThreadParam[i].startIndex = i * subLength;
				dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
					(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;

				ImageProcess::saltNoise(&dlg->m_pThreadParam[i]);
			}
	}
	break;

	}
}

void CNoiseDlg::gaussianNoise()
{
	int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;
	CString mText, sText;
	mEditMean.GetWindowTextW(mText);
	mEditStddev.GetWindowTextW(sText);
	if (mText.IsEmpty() || sText.IsEmpty()) {
		dlg->printLine(CString("还没有输入高斯噪声处理参数。"));
		return;
	}
	float mean = _ttof(mText);
	float stddev = _ttof(sText);

	int thread = dlg->mThreadType.GetCurSel();

	switch (thread) {
	case 0: // WIN多线程
	{
		for (int i = 0; i < dlg->m_nThreadNum; ++i)
		{
			dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;
			
			dlg->m_pThreadParam[i].mean = mean;
			dlg->m_pThreadParam[i].stddev = stddev;
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::gaussianNoise, &dlg->m_pThreadParam[i]);
		}
	}
	break;

	case 1: // OpenMP
	{
		// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(dlg->m_nThreadNum)
		for (int i = 0; i < dlg->m_nThreadNum; i++) {
			dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
			dlg->m_pThreadParam[i].startIndex = i * subLength;
			dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;

			dlg->m_pThreadParam[i].mean = mean;
			dlg->m_pThreadParam[i].stddev = stddev;

			ImageProcess::gaussianNoise(&dlg->m_pThreadParam[i]);
		}
	}
	break;

	}
}


void CNoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_NOISETYPE, mNoiseType);
	DDX_Control(pDX, IDC_EDIT_MEAN, mEditMean);
	DDX_Control(pDX, IDC_EDIT_STDDEV, mEditStddev);
}

BOOL CNoiseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	mNoiseType.InsertString(0, _T("椒盐噪声"));
	mNoiseType.InsertString(1, _T("高斯噪声"));
	mNoiseType.SetCurSel(0);
	return 0;
}


BEGIN_MESSAGE_MAP(CNoiseDlg, CDialogEx)
END_MESSAGE_MAP()


// CNoiseDlg 消息处理程序
