
// ImgProcess1Dlg.h: 头文件
//
#include"CInterpolationDlg.h"
#include"CFourierDlg.h"
#include"CNoiseDlg.h"
#include"CFilterDlg.h"
#include"CBilateralDlg.h"
#include"ImageProcess.h"

#pragma once
#define TABNUM 5
#define MAX_THREAD 8
#define MAX_SPAN 15

// CImgProcess1Dlg 对话框
class CImgProcess1Dlg : public CDialogEx
{
// 构造
public:
	CImgProcess1Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMGPROCESS1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	// 辅助函数
	void setTab();
	static UINT Update(void* p);
	void addNoise();
	void addNoise_WIN();
	void addNoise_OPENMP();
	void filter();
	void filter_WIN();
	void filter_OPENMP();

	// 线程通信消息函数
	afx_msg LRESULT OnSaltNoiseThreadMsgReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMedianFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam);

// 实现
protected:
	HICON m_hIcon;
	CImage* m_pImgSrc;
	CImage* m_pImgCpy;
	int m_nThreadNum;
	ThreadParam* m_pThreadParam;
	CTime startTime;


	int curPage;
	CInterpolationDlg m_pageInterpolation;
	CFourierDlg m_pageFourier;
	CNoiseDlg m_pageNoise;
	CFilterDlg m_pageFilter;
	CBilateralDlg m_pageBilateral;
	
	CDialog* pPages[TABNUM];
	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
	CEdit mEditInfo;
	CTabCtrl mTabControl;
	CStatic mPicCtrlLeft;
	CStatic mPicCtrlRight;
	CEdit mEditOutput;
	afx_msg void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	CComboBox mThreadType;
	afx_msg void OnNMCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonProcess();
};
