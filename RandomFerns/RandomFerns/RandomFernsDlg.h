
// RandomFernsDlg.h: 头文件
//
#include <cv.h>
#include <highgui.h>

#include <iostream>
#include <string>
using namespace std;

#include "mcv.h"
#include "planar_pattern_detector_builder.h"
#include "template_matching_based_tracker.h"


void draw_quadrangle(IplImage* frame,
	int u0, int v0,
	int u1, int v1,
	int u2, int v2,
	int u3, int v3,
	CvScalar color, int thickness);
void draw_detected_position(IplImage* frame, planar_pattern_detector* detector);
void draw_initial_rectangle(IplImage* frame, template_matching_based_tracker* tracker);
void draw_tracked_position(IplImage* frame, template_matching_based_tracker* tracker);
void draw_tracked_locations(IplImage* frame, template_matching_based_tracker* tracker);
void draw_detected_keypoints(IplImage* frame, planar_pattern_detector* detector);
void draw_recognized_keypoints(IplImage* frame, planar_pattern_detector* detector);
void detect_and_draw(IplImage* frame);

// CRandomFernsDlg 对话框
class CRandomFernsDlg : public CDialogEx
{
// 构造
public:
	CRandomFernsDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RANDOMFERNS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedProcess();
	void doFerns();
};
