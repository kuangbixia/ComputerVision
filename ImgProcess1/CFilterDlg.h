#pragma once
class CImgProcess1Dlg;

// CFilterDlg 对话框

class CFilterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFilterDlg)

public:
	CFilterDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFilterDlg();

	void filter(void* p);
	void medianFilter();
	void meanFilter();
	void gaussianFilter();
	void wienerFilter();
	void bilateralFilter();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILTER };
#endif

protected:
	CImgProcess1Dlg* dlg;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox mFilterType;
	CEdit mEditStddev;
	CEdit mEditSigmaD;
	CEdit mEditSigmaR;
};
