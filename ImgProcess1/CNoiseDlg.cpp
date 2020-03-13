// CNoiseDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImgProcess1.h"
#include "CNoiseDlg.h"
#include "afxdialogex.h"
#include "ImgProcess1Dlg.h"


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

	int thread = dlg->mThreadType.GetCurSel();

	switch (thread) {
	case 0: // WIN多线程
	{
		addNoise_WIN();
		break;
	}
	case 1: // OpenMP
	{
		addNoise_OPENMP();
		break;
	}
	}
}

void CNoiseDlg::addNoise_WIN()
{
	int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;
	for (int i = 0; i < dlg->m_nThreadNum; ++i)
	{
		dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
		dlg->m_pThreadParam[i].startIndex = i * subLength;
		dlg->m_pThreadParam[i].endIndex = i != dlg->m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;
		// windows MFC 创建线程
		switch (mNoiseType.GetCurSel()) {
		case 0: // 椒盐噪声
		{
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::saltNoise, &dlg->m_pThreadParam[i]);
			break;
		}
		case 1: // todo:高斯噪声
		{
			break;
		}
		}
	}
}

void CNoiseDlg::addNoise_OPENMP()
{
	// 每个线程处理的像素数
	int subLength = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() / dlg->m_nThreadNum;

	// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(m_nThreadNum)
	for (int i = 0; i < dlg->m_nThreadNum; i++) {
		dlg->m_pThreadParam[i].img = dlg->m_pImgShow;
		dlg->m_pThreadParam[i].startIndex = i * subLength;
		if (i != dlg->m_nThreadNum - 1) {
			dlg->m_pThreadParam[i].endIndex = (i + 1) * subLength - 1;
		}
		else {
			dlg->m_pThreadParam[i].endIndex = dlg->m_pImgShow->GetWidth() * dlg->m_pImgShow->GetHeight() - 1;
		}
		switch (mNoiseType.GetCurSel()) {
		case 0: // 椒盐噪声
		{
			ImageProcess::saltNoise(&dlg->m_pThreadParam[i]);
			break;
		}
		case 1: // todo:高斯噪声
		{
			break;
		}
		}
	}
}

void CNoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_NOISETYPE, mNoiseType);
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
