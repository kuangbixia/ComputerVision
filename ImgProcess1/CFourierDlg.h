#pragma once
class CImgProcess1Dlg;


// CFourierDlg 对话框

class CFourierDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFourierDlg)

public:
	CFourierDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFourierDlg();

	void fourier(void* p);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FOURIER };
#endif

protected:
	CImgProcess1Dlg* dlg;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
