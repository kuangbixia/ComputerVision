﻿
// ImgProcess1Dlg.h: 头文件
//
#include"CInterpolationDlg.h"
#include"CFourierDlg.h"
#include"CNoiseDlg.h"
#include"CFilterDlg.h"
#include"ImageProcess.h"

#pragma once
#define TABNUM 4
#define MAX_THREAD 4
#define MAX_SPAN 15

struct DrawPara
{
	CImage* pImgSrc;
	CDC* pDC;
	int oriX;
	int oriY;
	int width;
	int height;
};


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
	void Open(void* p);
	static UINT Update(void* p);
	//void imageScale(float xscale, float yscale);
	//void threadDraw(DrawPara* p);

	// 线程通信消息函数
	afx_msg LRESULT OnNoiseThreadMsgReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInterpolationThreadMsgReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFourierThreadMsgReceived(WPARAM wParam, LPARAM lParam);

// 实现
protected:
	HICON m_hIcon;
	

	int curPage;
	CInterpolationDlg m_pageInterpolation;
	CFourierDlg m_pageFourier;
	CNoiseDlg m_pageNoise;
	CFilterDlg m_pageFilter;
	
	CDialog* pPages[TABNUM];
	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTime startTime;

	CImage* m_pImgSrc;
	CImage* m_pImgShow;
	CImage* m_pImgTemp;
	int m_nThreadNum;
	ThreadParam* m_pThreadParam;
	CEdit mEditInfo;
	CString strFilePath;
	CTabCtrl mTabControl;
	CStatic mPicCtrlLeft;
	CStatic mPicCtrlRight;
	CEdit mEditOutput;
	CComboBox mThreadType;

	void printLine(CString text);
	void imageCopy(CImage* pImgSrc, CImage* pImgDrt);


	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonProcess();
};
