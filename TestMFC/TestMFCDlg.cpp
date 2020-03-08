
// TestMFCDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "TestMFC.h"
#include "TestMFCDlg.h"
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


// CTestMFCDlg 对话框

CTestMFCDlg::CTestMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// 初始化变量
	m_pImgSrc = NULL;
	m_pImgCpy = NULL;
	m_nThreadNum = 1;
	m_pThreadParam = new ThreadParam[MAX_THREAD];
}

void CTestMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INFO, mEditInfo);
	DDX_Control(pDX, IDC_PICTURE, mPictureControl);
}

void CTestMFCDlg::AddNoise()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	CButton* clb_circulation = ((CButton*)GetDlgItem(IDC_CHECK_CIRCULATION));
	// 作用？？？
	int circulation = clb_circulation->GetCheck() == 0 ? 1 : 100;
	startTime = CTime::GetTickCount();
	switch (thread) {
	case 0: // WIN多线程
	{
		AddNoise_WIN();
		break;
	}
	case 1: // OpenMP
	{
		// ???
		int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;

		#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; i++) {
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].startIndex = i * subLength;
			if (i != m_nThreadNum - 1) {
				m_pThreadParam[i].endIndex = (i + 1) * subLength - 1;
			}
			else {
				m_pThreadParam[i].endIndex = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
			}
			ImageProcess::addNoise(&m_pThreadParam[i]);
		}
		break;
	}
	case 2: // OpenCL
	{
		// TODO
		break;
	}
	}
}

void CTestMFCDlg::MedianFilter()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	CButton* clb_circulation = ((CButton*)GetDlgItem(IDC_CHECK_CIRCULATION));
	// ？？？why 4？
	int circulation = clb_circulation->GetCheck() == 0 ? 1 : 4;
	startTime = CTime::GetTickCount();
	switch (thread) {
	case 0: // WIN多线程
	{
		MedianFilter_WIN();
		break;
	}
	case 1: // OpenMP
	{
		int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;

		#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; ++i)
		{
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
			m_pThreadParam[i].maxSpan = MAX_SPAN;
			m_pThreadParam[i].src = m_pImgSrc;
			ImageProcess::medianFilter(&m_pThreadParam[i]);
		}
		break;
	}
	case 2: // OpenCL
	{
		break;
	}
	}

}

void CTestMFCDlg::AddNoise_WIN() // 跟OpenMP的区别？？
{
	int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i)
	{
		m_pThreadParam[i].src = m_pImgSrc;
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::addNoise, &m_pThreadParam[i]);
	}
}

void CTestMFCDlg::MedianFilter_WIN()
{
	int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;
	int h = m_pImgSrc->GetHeight() / m_nThreadNum;
	int w = m_pImgSrc->GetWidth() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i)
	{
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
		m_pThreadParam[i].maxSpan = MAX_SPAN;
		m_pThreadParam[i].src = m_pImgSrc;
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::medianFilter, &m_pThreadParam[i]);
	}
}

void CTestMFCDlg::ThreadDraw(DrawPara* p)
{
}

void CTestMFCDlg::ImageCopy(CImage* pImgSrc, CImage* pImgDrt)
{
}

UINT CTestMFCDlg::Update(void* p)
{
	while (1)
	{
		Sleep(200);
		CTestMFCDlg* dlg = (CTestMFCDlg*)p;
		if (dlg->m_pImgSrc != NULL) {
			int picHeight = dlg->m_pImgSrc->GetHeight();
			int picWidth = dlg->m_pImgSrc->GetWidth();
			CRect picCtrlRect;
			CRect displayRect;

			// CDC————设备上下文对象
			CDC* pDC = dlg->mPictureControl.GetDC();
			// SetStretchBltMode函数，设置指定设备环境中的位图拉伸模式
			SetStretchBltMode(pDC->m_hDC, STRETCH_HALFTONE);

			// 窗口区分为 client 客户区域 和 非客户区
			dlg->mPictureControl.GetClientRect(&picCtrlRect);
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
	}
	return 0;
}

LRESULT CTestMFCDlg::OnNoiseThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}

LRESULT CTestMFCDlg::OnMedianFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}

BEGIN_MESSAGE_MAP(CTestMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	// 控件处理消息函数
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTestMFCDlg::OnBnClickedButtonOpen)
	ON_CBN_SELCHANGE(IDC_COMBO_FUNCTION, &CTestMFCDlg::OnCbnSelchangeComboFunction)
	ON_CBN_SELCHANGE(IDC_COMBO_THREAD, &CTestMFCDlg::OnCbnSelchangeComboThread)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THREADNUM, &CTestMFCDlg::OnNMCustomdrawSliderThreadnum)
	ON_BN_CLICKED(IDC_BUTTON_DEAL, &CTestMFCDlg::OnBnClickedButtonDeal)

	// 定义线程通信消息函数
	ON_MESSAGE(WM_NOISE, &CTestMFCDlg::OnNoiseThreadMsgReceived)
	ON_MESSAGE(WM_MEDIAN_FILTER, &CTestMFCDlg::OnMedianFilterThreadMsgReceived)
END_MESSAGE_MAP()


// CTestMFCDlg 消息处理程序

BOOL CTestMFCDlg::OnInitDialog()
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

	CComboBox* cmb_function = (CComboBox*)GetDlgItem(IDC_COMBO_FUNCTION);
	cmb_function->AddString(_T("椒盐噪声"));
	cmb_function->AddString(_T("自适应中值滤波"));
	cmb_function->SetCurSel(0);

	CComboBox* cmb_thread = (CComboBox*)GetDlgItem(IDC_COMBO_THREAD);
	cmb_thread->InsertString(0, _T("WIN多线程"));
	cmb_thread->InsertString(1, _T("OpenMP"));
	cmb_thread->InsertString(2, _T("OpenCL"));
	cmb_thread->SetCurSel(0);

	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_THREADNUM);
	slider->SetRange(1, MAX_THREAD, TRUE);
	slider->SetPos(1);

	AfxBeginThread((AFX_THREADPROC)&CTestMFCDlg::Update, this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestMFCDlg::OnPaint()
{
	// 判断窗口是否在最小化状态？？
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
		/*
		if (m_pImgSrc != NULL) {
			int picHeight = m_pImgSrc->GetHeight();
			int picWidth = m_pImgSrc->GetWidth();
			CRect picCtrlRect;
			CRect displayRect;

			// CDC————设备上下文对象
			CDC* pDC = mPictureControl.GetDC();
			// SetStretchBltMode函数，设置指定设备环境中的位图拉伸模式
			
			//hDC————对象使用的输出设备上下文
			//HALFTONE/STRETCH_HALFTONE————将源矩形区中的像素映射到目标矩形区的像素块中，
			//覆盖目标像素块的一般颜色与源像素的颜色接近。
			//在设置完HALFTONE拉伸模之后，应用程序必须调用SetBrushOrgEx函数来设置刷子的起始点。
			//如果没有成功，那么会出现刷子没对准的情况。
			SetStretchBltMode(pDC->m_hDC, STRETCH_HALFTONE);

			// 窗口区分为 client 客户区域 和 非客户区
			mPictureControl.GetClientRect(&picCtrlRect);
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
			m_pImgSrc->StretchBlt(pDC->m_hDC, displayRect, SRCCOPY);
			// CDC消耗很大，要及时释放
			ReleaseDC(pDC);
		}
		*/
		
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 控件处理函数的实现

void CTestMFCDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("JPEG(*jpg)|*.jpg|*.bmp|TIFF(*.tif)|*.tif|All Files （*.*）|*.*||");
	CString filePath("");

	CFileDialog fileOpenDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (fileOpenDialog.DoModal() == IDOK)
	{
		VERIFY(filePath = fileOpenDialog.GetPathName());
		CString strFilePath(filePath);
		mEditInfo.SetWindowTextW(strFilePath);

		/* 将图片作为成员写进头文件供多个函数调用
		CImage m_pImgSrc;
		m_pImgSrc.Load(strFilePath);
		*/
		
		// 清除上一张图片，释放内存
		if (m_pImgSrc != NULL) {
			m_pImgSrc->Destroy();
			delete m_pImgSrc;
		}
		m_pImgSrc = new CImage();
		m_pImgSrc->Load(strFilePath);
		// Invalidate————使整个窗口客户区无效, 并进行 更新 显示的函数
		this->Invalidate();
	}
}

void CTestMFCDlg::OnCbnSelchangeComboFunction()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CTestMFCDlg::OnCbnSelchangeComboThread()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CTestMFCDlg::OnNMCustomdrawSliderThreadnum(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_THREADNUM);
	CString text("");
	m_nThreadNum = slider->GetPos();
	text.Format(_T("%d"), m_nThreadNum);
	GetDlgItem(IDC_STATIC_THREADNUM)->SetWindowText(text);


	*pResult = 0;
}

void CTestMFCDlg::OnBnClickedButtonDeal()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* cmb_function = ((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTION));
	int func = cmb_function->GetCurSel();
	if (m_pImgSrc != NULL) { // 避免没添加图片时出错
		switch (func)
		{
		case 0:  //椒盐噪声
			AddNoise();
			break;
		case 1: //自适应中值滤波
			MedianFilter();
			break;
		default:
			break;
		}
	}
}
