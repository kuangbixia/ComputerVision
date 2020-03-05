
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
}

void CTestMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INFO, mEditInfo);
	DDX_Control(pDX, IDC_PICTURE, mPictureControl);
}

BEGIN_MESSAGE_MAP(CTestMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTestMFCDlg::OnBnClickedButtonOpen)
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
		if (m_pImgSrc != NULL) {
			int picHeight = m_pImgSrc->GetHeight();
			int picWidth = m_pImgSrc->GetWidth();
			CRect picCtrlRect;
			CRect displayRect;

			// CDC————设备上下文对象
			CDC* pDC = mPictureControl.GetDC();
			// SetStretchBltMode函数，设置指定设备环境中的位图拉伸模式
			/* 
			hDC
			————对象使用的输出设备上下文
			HALFTONE/STRETCH_HALFTONE
			————将源矩形区中的像素映射到目标矩形区的像素块中，
			覆盖目标像素块的一般颜色与源像素的颜色接近。
			在设置完HALFTONE拉伸模之后，
			应用程序必须调用SetBrushOrgEx函数来设置刷子的起始点。
			如果没有成功，那么会出现刷子没对准的情况。
			*/
			SetStretchBltMode(pDC->m_hDC, STRETCH_HALFTONE);

			// 窗口区分为 client 客户区域 和 非客户区
			mPictureControl.GetClientRect(&picCtrlRect);
			if (picHeight <= picCtrlRect.Height() && picWidth <= picCtrlRect.Width()) {
				displayRect = CRect(picCtrlRect.TopLeft(), CSize(picWidth, picHeight));
				// 从源矩形中复制一个位图到目标矩形，必要时按目标设备设置的模式进行图像的拉伸或压缩。
			}
			else {
				float xScale = (float)picCtrlRect.Width() / (float)picWidth;
				float yScale = (float)picCtrlRect.Height() / (float)picHeight;
				float scale = xScale <= yScale ? xScale : yScale;
				displayRect = CRect(picCtrlRect.TopLeft(), CSize((int)picWidth * scale, (int)picHeight * scale));
			}
			m_pImgSrc->StretchBlt(pDC->m_hDC, displayRect, SRCCOPY);
			// CDC消耗很大，要及时释放
			ReleaseDC(pDC);
		}
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



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
