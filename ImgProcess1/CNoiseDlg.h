#pragma once
class CImgProcess1Dlg;

// CNoiseDlg 对话框

class CNoiseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNoiseDlg)

public:
	CNoiseDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CNoiseDlg();

	void addNoise(void* p);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NOISE };
#endif

protected:
	CImgProcess1Dlg* dlg;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox mNoiseType;
	CEdit mEditMean;
	CEdit mEditStddev;
};
