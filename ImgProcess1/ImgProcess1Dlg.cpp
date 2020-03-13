
// ImgProcess1Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ImgProcess1.h"
#include "ImgProcess1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CImgProcess1Dlg 对话框

CImgProcess1Dlg::CImgProcess1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMGPROCESS1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pImgSrc = NULL;
	m_pImgShow = NULL;
	m_pImgTemp = NULL;
	curPage = 0;
	m_nThreadNum = 1;
	// 不初始化将导致写入访问权限冲突
	m_pThreadParam = new ThreadParam[MAX_THREAD];
}

void CImgProcess1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, mTabControl);
	DDX_Control(pDX, IDC_EDIT_PICINFO, mEditInfo);
	DDX_Control(pDX, IDC_PICTURE_LEFT, mPicCtrlLeft);
	DDX_Control(pDX, IDC_PICTURE_RIGHT, mPicCtrlRight);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, mEditOutput);
	DDX_Control(pDX, IDC_COMBO_THREAD, mThreadType);
}

void CImgProcess1Dlg::setTab()
{
	// Tab分页

	TCITEM tcItem[TABNUM];
	int i;
	for (i = 0; i < TABNUM; i++) {
		tcItem[i].mask = TCIF_TEXT;
	}

	i = 0;
	tcItem[i++].pszText = _T("三阶插值");
	tcItem[i++].pszText = _T("傅里叶变换");
	tcItem[i++].pszText = _T("噪声");
	tcItem[i++].pszText = _T("滤波");
	tcItem[i++].pszText = _T("双边滤波");

	for (i = 0; i < TABNUM; i++) {
		mTabControl.InsertItem(i, &tcItem[i]);
	}

	// 把新建对话框添加到tab control

	i = 0;
	// 分页的显示范围
	CRect tabRc;
	mTabControl.GetClientRect(tabRc);
	tabRc.top += 25;

	m_pageInterpolation.Create(IDD_INTERPOLATION, &mTabControl);
	m_pageInterpolation.MoveWindow(&tabRc);
	pPages[i++] = &m_pageInterpolation;
	m_pageFourier.Create(IDD_FOURIER, &mTabControl);
	m_pageFourier.MoveWindow(&tabRc);
	pPages[i++] = &m_pageFourier;
	m_pageNoise.Create(IDD_NOISE, &mTabControl);
	m_pageNoise.MoveWindow(&tabRc);
	m_pageNoise.mNoiseType.InsertString(0, _T("椒盐噪声"));
	m_pageNoise.mNoiseType.InsertString(1, _T("高斯噪声"));
	m_pageNoise.mNoiseType.SetCurSel(0);
	pPages[i++] = &m_pageNoise;
	m_pageFilter.Create(IDD_FILTER, &mTabControl);
	m_pageFilter.MoveWindow(&tabRc);
	m_pageFilter.mFilterType.InsertString(0, _T("自适应中值滤波"));
	m_pageFilter.mFilterType.InsertString(1, _T("平滑线性滤波"));
	m_pageFilter.mFilterType.InsertString(2, _T("高斯滤波"));
	m_pageFilter.mFilterType.InsertString(3, _T("维纳滤波"));
	m_pageFilter.mFilterType.SetCurSel(0);
	pPages[i++] = &m_pageFilter;
	m_pageBilateral.Create(IDD_BILATERAL, &mTabControl);
	m_pageBilateral.MoveWindow(&tabRc);
	pPages[i++] = &m_pageBilateral;

	// 初始化tab control显示的窗口
	for (i = 0; i < TABNUM; i++) {
		if (i == curPage) {
			pPages[i++]->ShowWindow(SW_SHOW);
		}
		else {
			pPages[i]->ShowWindow(SW_HIDE);
		}
	}
}

void CImgProcess1Dlg::Open(void* p)
{
	CImgProcess1Dlg* dlg = (CImgProcess1Dlg*)p;
	if (dlg->m_pImgSrc != NULL) {
		int picHeight = dlg->m_pImgSrc->GetHeight();
		int picWidth = dlg->m_pImgSrc->GetWidth();
		CRect picCtrlRect;
		CRect displayRect;

		// CDC————设备上下文对象
		CDC* pDC = dlg->mPicCtrlLeft.GetDC();
		// SetStretchBltMode函数，设置指定设备环境中的位图拉伸模式
		SetStretchBltMode(pDC->m_hDC, STRETCH_HALFTONE);

		// 窗口区分为 client 客户区域 和 非客户区
		dlg->mPicCtrlLeft.GetClientRect(&picCtrlRect);
		if (picHeight <= picCtrlRect.Height() && picWidth <= picCtrlRect.Width()) {
			displayRect = CRect(picCtrlRect.TopLeft(), CSize(picWidth, picHeight));
		}
		else {
			float xScale = (float)picCtrlRect.Width() / (float)picWidth;
			float yScale = (float)picCtrlRect.Height() / (float)picHeight;
			float scale = xScale <= yScale ? xScale : yScale;
			displayRect = CRect(picCtrlRect.TopLeft(), CSize((int)picWidth * scale, (int)picHeight * scale));
		}
		// 从源矩形中复制一个位图到目标矩形，必要时按目标设备设置的模式进行图像的拉伸或压缩。
		dlg->m_pImgSrc->StretchBlt(pDC->m_hDC, displayRect, SRCCOPY);

		// CDC消耗很大，要及时释放
		dlg->ReleaseDC(pDC);
	}
	return;
}

UINT CImgProcess1Dlg::Update(void* p)
{
	while (1)
	{
		Sleep(200);
		CImgProcess1Dlg* dlg = (CImgProcess1Dlg*)p;

		if (dlg->m_pImgShow != NULL) {
			int picHeight = dlg->m_pImgShow->GetHeight();
			int picWidth = dlg->m_pImgShow->GetWidth();
			CRect picCtrlRect;
			CRect displayRect;

			// CDC————设备上下文对象
			CDC* pDC = dlg->mPicCtrlRight.GetDC();
			// SetStretchBltMode函数，设置指定设备环境中的位图拉伸模式
			SetStretchBltMode(pDC->m_hDC, STRETCH_HALFTONE);

			// 窗口区分为 client 客户区域 和 非客户区
			dlg->mPicCtrlRight.GetClientRect(&picCtrlRect);
			if (picHeight <= picCtrlRect.Height() && picWidth <= picCtrlRect.Width()) {
				displayRect = CRect(picCtrlRect.TopLeft(), CSize(picWidth, picHeight));
			}
			else {
				float xScale = (float)picCtrlRect.Width() / (float)picWidth;
				float yScale = (float)picCtrlRect.Height() / (float)picHeight;
				float scale = xScale <= yScale ? xScale : yScale;
				displayRect = CRect(picCtrlRect.TopLeft(), CSize((int)picWidth * scale, (int)picHeight * scale));
			}

			// 从源矩形中复制一个位图到目标矩形，必要时按目标设备设置的模式进行图像的拉伸或压缩。
			dlg->m_pImgShow->StretchBlt(pDC->m_hDC, displayRect, SRCCOPY);
			// CDC消耗很大，要及时释放
			dlg->ReleaseDC(pDC);
		}
	}
	return 0;
}

void CImgProcess1Dlg::imageCopy(CImage* pImgSrc, CImage* pImgDrt)
{
	int MaxColors = pImgSrc->GetMaxColorTableEntries();
	RGBQUAD* ColorTab;
	ColorTab = new RGBQUAD[MaxColors];

	CDC* pDCsrc, * pDCdrc;
	if (!pImgDrt->IsNull())
	{
		pImgDrt->Destroy();
	}
	int alpha = pImgSrc->GetBPP() == 32 ? 1 : 0;
	pImgDrt->Create(pImgSrc->GetWidth(), pImgSrc->GetHeight(), pImgSrc->GetBPP(), alpha);

	if (pImgSrc->IsIndexed())
	{
		pImgSrc->GetColorTable(0, MaxColors, ColorTab);
		pImgDrt->SetColorTable(0, MaxColors, ColorTab);
	}

	pDCsrc = CDC::FromHandle(pImgSrc->GetDC());
	pDCdrc = CDC::FromHandle(pImgDrt->GetDC());
	pDCdrc->BitBlt(0, 0, pImgSrc->GetWidth(), pImgSrc->GetHeight(), pDCsrc, 0, 0, SRCCOPY);
	pImgSrc->ReleaseDC();
	pImgDrt->ReleaseDC();
	delete ColorTab;
}

// scale

void CImgProcess1Dlg::scale()
{
	float x = 0.5, y = 0.5;

	if (m_pImgTemp != NULL) {
		m_pImgTemp->Destroy();
		delete m_pImgTemp;
		m_pImgTemp = NULL;
	}
	m_pImgTemp = new CImage();

	//int alpha = m_pImgShow->GetBPP() == 32 ? 1 : 0;
	//bool createYes = m_pImgTemp->Create(int(x * m_pImgShow->GetWidth()), int(y * m_pImgShow->GetHeight()), m_pImgShow->GetBPP(), alpha);
	m_pImgTemp->Load(strFilePath);

	int thread = mThreadType.GetCurSel();
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

void CImgProcess1Dlg::scale_WIN(float x, float y)
{
	int width = (x < 1 ? x : 1) * m_pImgTemp->GetWidth();
	int height = (y < 1 ? y : 1) * m_pImgTemp->GetHeight();

	// 每个线程处理的像素数
	//int subLength = m_pImgTemp->GetWidth() * m_pImgTemp->GetHeight() / m_nThreadNum;
	int subLength = width * height / m_nThreadNum;


	for (int i = 0; i < m_nThreadNum; ++i)
	{
		m_pThreadParam[i].img = m_pImgTemp;
		m_pThreadParam[i].src = m_pImgShow;
		m_pThreadParam[i].xscale = x;
		m_pThreadParam[i].yscale = y;
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ? (i + 1) * subLength - 1 : width * height - 1;

		// windows MFC 创建线程
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::cubicScale, &m_pThreadParam[i]);
	}
}

void CImgProcess1Dlg::scale_OPENMP(float x, float y)
{
	int width = (x < 1 ? x : 1) * m_pImgTemp->GetWidth();
	int height = (y < 1 ? y : 1) * m_pImgTemp->GetHeight();

	// 每个线程处理的像素数
	//int subLength = m_pImgTemp->GetWidth() * m_pImgTemp->GetHeight() / m_nThreadNum;
	int subLength = width * height / m_nThreadNum;

	// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(m_nThreadNum)
	for (int i = 0; i < m_nThreadNum; i++) {
		m_pThreadParam[i].img = m_pImgTemp;
		m_pThreadParam[i].src = m_pImgShow;
		m_pThreadParam[i].xscale = x;
		m_pThreadParam[i].yscale = y;
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ? (i + 1) * subLength - 1 : width * height - 1;
		ImageProcess::cubicScale(&m_pThreadParam[i]);
	}
}


// add noise

void CImgProcess1Dlg::addNoise()
{
	int thread = mThreadType.GetCurSel();

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

void CImgProcess1Dlg::addNoise_WIN()
{
	int subLength = m_pImgShow->GetWidth() * m_pImgShow->GetHeight() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i)
	{
		m_pThreadParam[i].img = m_pImgShow;
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgShow->GetWidth() * m_pImgShow->GetHeight() - 1;
		// windows MFC 创建线程
		switch (m_pageNoise.mNoiseType.GetCurSel()) {
		case 0: // 椒盐噪声
		{
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::saltNoise, &m_pThreadParam[i]);
			break;
		}
		case 1: // todo:高斯噪声
		{
			break;
		}
		}
	}
}

void CImgProcess1Dlg::addNoise_OPENMP()
{
	// 每个线程处理的像素数
	int subLength = m_pImgShow->GetWidth() * m_pImgShow->GetHeight() / m_nThreadNum;

	// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(m_nThreadNum)
	for (int i = 0; i < m_nThreadNum; i++) {
		m_pThreadParam[i].img = m_pImgShow;
		m_pThreadParam[i].startIndex = i * subLength;
		if (i != m_nThreadNum - 1) {
			m_pThreadParam[i].endIndex = (i + 1) * subLength - 1;
		}
		else {
			m_pThreadParam[i].endIndex = m_pImgShow->GetWidth() * m_pImgShow->GetHeight() - 1;
		}
		switch (m_pageNoise.mNoiseType.GetCurSel()) {
		case 0: // 椒盐噪声
		{
			ImageProcess::saltNoise(&m_pThreadParam[i]);
			break;
		}
		case 1: // todo:高斯噪声
		{
			break;
		}
		}
	}
}


// filter

void CImgProcess1Dlg::filter()
{
	int thread = mThreadType.GetCurSel();

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

void CImgProcess1Dlg::filter_WIN()
{
	int subLength = m_pImgShow->GetWidth() * m_pImgShow->GetHeight() / m_nThreadNum;
	int h = m_pImgShow->GetHeight() / m_nThreadNum;
	int w = m_pImgShow->GetWidth() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i)
	{
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgShow->GetWidth() * m_pImgShow->GetHeight() - 1;
		m_pThreadParam[i].maxSpan = MAX_SPAN;
		m_pThreadParam[i].img = m_pImgShow;
		// windows MFC 创建线程
		switch (m_pageFilter.mFilterType.GetCurSel()) {
		case 0: // 自适应中值滤波
		{
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::medianFilter, &m_pThreadParam[i]);
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

void CImgProcess1Dlg::filter_OPENMP()
{
	// 每个线程处理的像素数
	int subLength = m_pImgShow->GetWidth() * m_pImgShow->GetHeight() / m_nThreadNum;

	// 把for循环分给各个线程执行！！和windows多线程不一样
#pragma omp parallel for num_threads(m_nThreadNum)
	for (int i = 0; i < m_nThreadNum; ++i)
	{
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgShow->GetWidth() * m_pImgShow->GetHeight() - 1;
		m_pThreadParam[i].maxSpan = MAX_SPAN;
		m_pThreadParam[i].img = m_pImgShow;
		switch (m_pageFilter.mFilterType.GetCurSel()) {
		case 0: // 自适应中值滤波
		{
			ImageProcess::medianFilter(&m_pThreadParam[i]);
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


// 消息接收

LRESULT CImgProcess1Dlg::OnScaleThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	static int scaleThreadCount = 0;

	if ((int)wParam == 1) // 0：发送消息，1：接收消息
	{
		if (m_nThreadNum == ++scaleThreadCount) // 每个线程都处理完
		{
			// 为下一次处理初始化
			scaleThreadCount = 0;

			imageCopy(m_pImgTemp, m_pImgShow);


			CString text2;
			mEditOutput.GetWindowTextW(text2);
			CString lstrS;
			lstrS.Format(_T("finished. img scale width: %d, img scale height: %d. img show width: %d, img show height: %d.\r\n>"), m_pImgTemp->GetWidth(), m_pImgTemp->GetHeight(), m_pImgShow->GetWidth(), m_pImgShow->GetHeight());
			text2 += lstrS;
			mEditOutput.SetWindowTextW(text2);

			CTime endTime = CTime::GetTickCount();
			CString text;
			mEditOutput.GetWindowTextW(text);
			text += "进行缩放处理。";
			text += mThreadType.GetCurSel() == 0 ? "采用Windows多线程。" : "采用OpenMP。";
			CString timeStr;
			timeStr.Format(_T("线程：%d个，耗时：%ds。\r\n>"), m_nThreadNum, (endTime - startTime));
			text += timeStr;
			mEditOutput.SetWindowTextW(text);

		}
	}
	return 0;
}

LRESULT CImgProcess1Dlg::OnNoiseThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	static int noiseThreadCount = 0;
	static int tempProcessCount = 0;
	CButton* clb_circulation = ((CButton*)GetDlgItem(IDC_CHECK_CIRCULATION));
	int circulation = clb_circulation->GetCheck() == 0 ? 1 : 10;

	if ((int)wParam == 1) // 0：发送消息，1：接收消息
	{
		if (m_nThreadNum == ++noiseThreadCount) // 每个线程都处理完
		{
			// 为下一次循环初始化
			noiseThreadCount = 0;

			if (++tempProcessCount < circulation)
				addNoise();
			else // 循环结束
			{
				CTime endTime = CTime::GetTickCount();

				// 为下一次处理初始化
				tempProcessCount = 0;
				delete[] m_pThreadParam;
				m_pThreadParam = NULL;
				m_pThreadParam = new ThreadParam[MAX_THREAD];


				CString text;
				mEditOutput.GetWindowTextW(text);
				text += m_pageNoise.mNoiseType.GetCurSel() == 0 ? "进行椒盐噪声处理。" : "进行高斯噪声处理。";
				text += mThreadType.GetCurSel() == 0 ? "采用Windows多线程。" : "采用OpenMP。";
				CString timeStr;
				timeStr.Format(_T("线程：%d个，处理：%d次，耗时：%ds。\r\n>"), m_nThreadNum, circulation, (endTime - startTime));
				text += timeStr;
				mEditOutput.SetWindowTextW(text);
			}
		}
	}
	return 0;
}

LRESULT CImgProcess1Dlg::OnFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	static int filterThreadCount = 0;
	static int tempProcessCount = 0;
	CButton* clb_circulation = ((CButton*)GetDlgItem(IDC_CHECK_CIRCULATION));
	int circulation = clb_circulation->GetCheck() == 0 ? 1 : 10;

	if ((int)wParam == 1) // 0：发送消息，1：接收消息
	{
		if (m_nThreadNum == ++filterThreadCount) // 每个线程都处理完
		{
			// 为下一次循环初始化
			filterThreadCount = 0;

			if (++tempProcessCount < circulation)
				filter();
			else // 循环结束
			{
				CTime endTime = CTime::GetTickCount();

				// 为下一次处理初始化
				tempProcessCount = 0;

				CString text;
				mEditOutput.GetWindowTextW(text);
				switch (m_pageFilter.mFilterType.GetCurSel()) {
				case 0: // 自适应中值滤波
				{
					text += "进行自适应中值滤波。";
					break;
				}
				case 1: // todo:平滑线性滤波
				{
					text += "进行平滑线性滤波。";
					break;
				}
				case 2: // todo:高斯滤波
				{
					text += "进行高斯滤波。";
					break;
				}
				case 3: // todo:维纳滤波
				{
					text += "进行维纳滤波。";
					break;
				}
				}
				text += mThreadType.GetCurSel() == 0 ? "采用Windows多线程。" : "采用OpenMP。";
				CString timeStr;
				timeStr.Format(_T("线程：%d个，处理：%d次，耗时：%ds。\r\n>"), m_nThreadNum, circulation, (endTime - startTime));
				text += timeStr;
				mEditOutput.SetWindowTextW(text);
			}
		}
	}
	return 0;
}


BEGIN_MESSAGE_MAP(CImgProcess1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CImgProcess1Dlg::OnBnClickedButtonOpen)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CImgProcess1Dlg::OnTcnSelchangeTab)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER, &CImgProcess1Dlg::OnNMCustomdrawSlider)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CImgProcess1Dlg::OnBnClickedButtonProcess)

	// 定义线程通信消息函数
	ON_MESSAGE(WM_SCALE, &CImgProcess1Dlg::OnScaleThreadMsgReceived)
	ON_MESSAGE(WM_NOISE, &CImgProcess1Dlg::OnNoiseThreadMsgReceived)
	ON_MESSAGE(WM_FILTER, &CImgProcess1Dlg::OnFilterThreadMsgReceived)
END_MESSAGE_MAP()


// CImgProcess1Dlg 消息处理程序

BOOL CImgProcess1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	mEditInfo.SetWindowTextW(CString("显示图片路径"));
	mEditOutput.SetWindowTextW(CString(">"));

	setTab();

	mThreadType.InsertString(0, _T("Windows多线程"));
	mThreadType.InsertString(1, _T("OpenMP"));
	mThreadType.SetCurSel(0);

	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER);
	slider->SetRange(1, MAX_THREAD, TRUE);
	slider->SetPos(1);

	AfxBeginThread((AFX_THREADPROC)&CImgProcess1Dlg::Update, this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CImgProcess1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CImgProcess1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		Open(this);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImgProcess1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CImgProcess1Dlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码

	TCHAR szFilter[] = _T("JPEG(*jpg)|*.jpg|*.bmp|TIFF(*.tif)|*.tif|All Files （*.*）|*.*||");
	CString filePath("");

	CFileDialog fileOpenDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (fileOpenDialog.DoModal() == IDOK)
	{
		VERIFY(filePath = fileOpenDialog.GetPathName());
		strFilePath = filePath;
		mEditInfo.SetWindowTextW(strFilePath);

		// 清除上一张图片，释放内存
		if (m_pImgSrc != NULL) {
			m_pImgSrc->Destroy();
			delete m_pImgSrc;
			m_pImgSrc = NULL;
		}
		if (m_pImgShow != NULL) {
			m_pImgShow->Destroy();
			delete m_pImgShow;
			m_pImgShow = NULL;
		}
		m_pImgSrc = new CImage();
		m_pImgSrc->Load(strFilePath);
		m_pImgShow = new CImage();
		m_pImgShow->Load(strFilePath);

		Open(this);

		CString text;
		mEditOutput.GetWindowTextW(text);
		text += "成功输入图像。\r\n>";
		mEditOutput.SetWindowTextW(text);

		// Invalidate————使整个窗口客户区无效, 并进行 更新 显示的函数
		this->Invalidate();
	}
}

void CImgProcess1Dlg::OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	pPages[curPage]->ShowWindow(SW_HIDE);
	curPage = mTabControl.GetCurSel();
	pPages[curPage]->ShowWindow(SW_SHOW);
	*pResult = 0;
}

void CImgProcess1Dlg::OnNMCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER);
	CString text("");
	m_nThreadNum = slider->GetPos();
	text.Format(_T("%d"), m_nThreadNum);
	GetDlgItem(IDC_STATIC_THREADNUM)->SetWindowText(text);
	*pResult = 0;
}

void CImgProcess1Dlg::OnBnClickedButtonProcess()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_pImgSrc == NULL) {
		CString text;
		mEditOutput.GetWindowTextW(text);
		text += "你还没有打开图片。\r\n>";
		mEditOutput.SetWindowTextW(text);
	}
	else {
		CString text;
		mEditOutput.GetWindowTextW(text);
		text += "正在处理……\r\n>";
		mEditOutput.SetWindowTextW(text);

		this->Invalidate();
		
		startTime = CTime::GetTickCount();
		switch (curPage) {
		case 0:
			scale();
			break;
		case 1:
			break;
		case 2:
		{
			addNoise();
			break;
		}
		case 3:
		{
			filter();
			break;
		}
		case 4:
			break;
		}
	}
}


/*
void CImgProcess1Dlg::imageScale(float xscale, float yscale)
{
	int MaxColors = m_pImgShow->GetMaxColorTableEntries();
	RGBQUAD* ColorTab;
	ColorTab = new RGBQUAD[MaxColors];

	CDC* pDCsrc, * pDCdrc;

	int alpha = m_pImgShow->GetBPP() == 32 ? 1 : 0;
	bool yes = m_pImgTemp->Create((int)xscale * m_pImgShow->GetWidth(), (int)xscale * m_pImgShow->GetHeight(), m_pImgShow->GetBPP(), alpha);

	if (m_pImgShow->IsIndexed())
	{
		m_pImgShow->GetColorTable(0, MaxColors, ColorTab);
		m_pImgTemp->SetColorTable(0, MaxColors, ColorTab);
	}

	pDCsrc = CDC::FromHandle(m_pImgShow->GetDC());
	pDCdrc = CDC::FromHandle(m_pImgTemp->GetDC());
	pDCdrc->BitBlt(0, 0, m_pImgTemp->GetWidth(), m_pImgTemp->GetHeight(), pDCsrc, 0, 0, SRCCOPY);
	m_pImgShow->ReleaseDC();
	m_pImgTemp->ReleaseDC();
	delete ColorTab;
}


void CImgProcess1Dlg::threadDraw(DrawPara* p)
{
	CRect rect;
	GetClientRect(&rect);
	CDC     memDC;             // 用于缓冲绘图的内存画笔
	CBitmap memBitmap;         // 用于缓冲绘图的内存画布
	memDC.CreateCompatibleDC(p->pDC);  // 创建与原画笔兼容的画笔
	memBitmap.CreateCompatibleBitmap(p->pDC, p->width, p->height);  // 创建与原位图兼容的内存画布
	memDC.SelectObject(&memBitmap);      // 创建画笔与画布的关联
	memDC.FillSolidRect(rect, p->pDC->GetBkColor());
	p->pDC->SetStretchBltMode(HALFTONE);
	// 将pImgSrc的内容缩放画到内存画布中
	p->pImgSrc->StretchBlt(memDC.m_hDC, 0, 0, p->width, p->height);

	// 将已画好的画布复制到真正的缓冲区中
	p->pDC->BitBlt(p->oriX, p->oriY, p->width, p->height, &memDC, 0, 0, SRCCOPY);
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}
*/