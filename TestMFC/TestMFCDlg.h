
// TestMFCDlg.h: 头文件
//

#pragma once


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


// 实现
protected:
	HICON m_hIcon;
	CImage* m_pImgSrc;

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
};
