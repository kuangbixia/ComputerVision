#pragma once


// CBilateralDlg 对话框

class CBilateralDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBilateralDlg)

public:
	CBilateralDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CBilateralDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BILATERAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
