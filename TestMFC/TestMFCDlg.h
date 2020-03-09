
// TestMFCDlg.h: 头文件
//

#pragma once
#include"ImageProcess.h"


#define MAX_THREAD 8
#define MAX_SPAN 15


struct DrawPara {
	CImage* pImgSrc;
	CDC* pDC;
	int oriX;
	int oriY;
	int width;
	int height;
};

// CTestMFCDlg 对话框
class CTestMFCDlg : public CDialogEx
{
// 构造
public:
	CTestMFCDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	CImage* getImage() {
		return m_pImgSrc;
	}

	void AddNoise();
	void MedianFilter();

	void AddNoise_WIN();
	void MedianFilter_WIN();

	void ThreadDraw(DrawPara* p);
	void ImageCopy(CImage* pImgSrc, CImage* pImgDrt);
	static UINT Update(void* p);

	// 线程通信消息函数
	afx_msg LRESULT OnNoiseThreadMsgReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMedianFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam);

// 实现
protected:
	HICON m_hIcon;
	CImage* m_pImgSrc;
	CImage* m_pImgCpy;
	int m_nThreadNum;
	ThreadParam* m_pThreadParam;
	CTime startTime;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
	CEdit mEditInfo;
	CStatic mPictureControl;

	afx_msg void OnNMCustomdrawSliderThreadnum(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonDeal();
};
