#pragma once
class CImgProcess1Dlg;

// CInterpolationDlg 对话框

class CInterpolationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInterpolationDlg)

public:
	CInterpolationDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CInterpolationDlg();

	void scale(void* p);
	void rotate(void* p);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INTERPOLATION };
#endif

protected:
	CImgProcess1Dlg* dlg;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox mScaleOrRotate;
	CEdit mEditXscale;
	CEdit mEditYscale;
	CEdit mEditAngle;
};
