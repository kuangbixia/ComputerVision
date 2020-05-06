
// RandomFernsDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "RandomFerns.h"
#include "RandomFernsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTime starttime;
CTime endtime;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRandomFernsDlg 对话框



CRandomFernsDlg::CRandomFernsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RANDOMFERNS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRandomFernsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRandomFernsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PROCESS, &CRandomFernsDlg::OnBnClickedProcess)
END_MESSAGE_MAP()


// CRandomFernsDlg 消息处理程序

BOOL CRandomFernsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    IplImage* train_image = cvLoadImage("./model.bmp");
    cvNamedWindow("ferns-model", 1);
    CWnd* pWnd1 = GetDlgItem(IDC_PICTURE_TRAIN);//CWnd是MFC窗口类的基类,提供了微软基础类库中所有窗口类的基本功能。
    CRect rect1;
    pWnd1->GetClientRect(&rect1);//GetClientRect为获得控件相自身的坐标大小
    cvResizeWindow("ferns-model", rect1.Width(), rect1.Height());
    HWND hWndl1 = (HWND)cvGetWindowHandle("ferns-model");//hWnd 表示窗口句柄,获取窗口句柄
    HWND hParent1 = ::GetParent(hWndl1);//GetParent函数一个指定子窗口的父窗口句柄
    ::SetParent(hWndl1, GetDlgItem(IDC_PICTURE_TRAIN)->m_hWnd);
    ::ShowWindow(hParent1, SW_HIDE);
    cvShowImage("ferns-model", train_image);
    cvReleaseImage(&train_image);

    IplImage* detect_image = cvLoadImage("./detect.png");
    cvNamedWindow("ferns-demo", 1);
    CWnd* pWnd2 = GetDlgItem(IDC_PICTURE);//CWnd是MFC窗口类的基类,提供了微软基础类库中所有窗口类的基本功能。
    CRect rect2;
    pWnd2->GetClientRect(&rect2);//GetClientRect为获得控件相自身的坐标大小
    cvResizeWindow("ferns-demo", rect2.Width(), rect2.Height());
    HWND hWndl2 = (HWND)cvGetWindowHandle("ferns-demo");//hWnd 表示窗口句柄,获取窗口句柄
    HWND hParent2 = ::GetParent(hWndl2);//GetParent函数一个指定子窗口的父窗口句柄
    ::SetParent(hWndl2, GetDlgItem(IDC_PICTURE)->m_hWnd);
    ::ShowWindow(hParent2, SW_HIDE);
    cvShowImage("ferns-demo", detect_image);
    cvReleaseImage(&detect_image);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRandomFernsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRandomFernsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRandomFernsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



const int max_filename = 1000;

enum source_type { webcam_source, sequence_source, video_source };

planar_pattern_detector* detector;
template_matching_based_tracker* tracker;

int mode = 2;
bool show_tracked_locations = true;
bool show_keypoints = true;

CvFont font;

void draw_quadrangle(IplImage* frame,
    int u0, int v0,
    int u1, int v1,
    int u2, int v2,
    int u3, int v3,
    CvScalar color, int thickness = 1)
{
    cvLine(frame, cvPoint(u0, v0), cvPoint(u1, v1), color, thickness);
    cvLine(frame, cvPoint(u1, v1), cvPoint(u2, v2), color, thickness);
    cvLine(frame, cvPoint(u2, v2), cvPoint(u3, v3), color, thickness);
    cvLine(frame, cvPoint(u3, v3), cvPoint(u0, v0), color, thickness);
}

void draw_detected_position(IplImage* frame, planar_pattern_detector* detector)
{
    draw_quadrangle(frame,
        detector->detected_u_corner[0], detector->detected_v_corner[0],
        detector->detected_u_corner[1], detector->detected_v_corner[1],
        detector->detected_u_corner[2], detector->detected_v_corner[2],
        detector->detected_u_corner[3], detector->detected_v_corner[3],
        cvScalar(255), 3);
}

void draw_initial_rectangle(IplImage* frame, template_matching_based_tracker* tracker)
{
    draw_quadrangle(frame,
        tracker->u0[0], tracker->u0[1],
        tracker->u0[2], tracker->u0[3],
        tracker->u0[4], tracker->u0[5],
        tracker->u0[6], tracker->u0[7],
        cvScalar(128), 3);
}

void draw_tracked_position(IplImage* frame, template_matching_based_tracker* tracker)
{
    draw_quadrangle(frame,
        tracker->u[0], tracker->u[1],
        tracker->u[2], tracker->u[3],
        tracker->u[4], tracker->u[5],
        tracker->u[6], tracker->u[7],
        cvScalar(255), 3);
}

void draw_tracked_locations(IplImage* frame, template_matching_based_tracker* tracker)
{
    for (int i = 0; i < tracker->nx * tracker->ny; i++) {
        int x1, y1;
        tracker->f.transform_point(tracker->m[2 * i], tracker->m[2 * i + 1], x1, y1);
        cvCircle(frame, cvPoint(x1, y1), 3, cvScalar(255, 255, 255), 1);
    }
}

void draw_detected_keypoints(IplImage* frame, planar_pattern_detector* detector)
{
    for (int i = 0; i < detector->number_of_detected_points; i++)
        cvCircle(frame,
            cvPoint(detector->detected_points[i].fr_u(),
                detector->detected_points[i].fr_v()),
            16 * (1 << int(detector->detected_points[i].scale)),
            cvScalar(100), 1);
}

void draw_recognized_keypoints(IplImage* frame, planar_pattern_detector* detector)
{
    for (int i = 0; i < detector->number_of_model_points; i++)
        if (detector->model_points[i].class_score > 0)
            cvCircle(frame,
                cvPoint(detector->model_points[i].potential_correspondent->fr_u(),
                    detector->model_points[i].potential_correspondent->fr_v()),
                16 * (1 << int(detector->detected_points[i].scale)),
                cvScalar(255, 255, 255), 1);
}


void detect_and_draw(IplImage* frame)
{
    
    static bool last_frame_ok = false;
    
    if (mode == 1 || ((mode == 0) && last_frame_ok)) {
        bool ok = tracker->track(frame);
        last_frame_ok = ok;


        if (!ok) {
            if (mode == 0) return detect_and_draw(frame);
            else {
                draw_initial_rectangle(frame, tracker);
                tracker->initialize();
            }
        }
        else {
            draw_tracked_position(frame, tracker);
            if (show_tracked_locations) draw_tracked_locations(frame, tracker);
        }
        cvPutText(frame, "template-based 3D tracking", cvPoint(10, 30), &font, cvScalar(255, 255, 255));
    }
    else {
        detector->detect(frame);

        if (detector->pattern_is_detected) {
            last_frame_ok = true;

            tracker->initialize(detector->detected_u_corner[0], detector->detected_v_corner[0],
                detector->detected_u_corner[1], detector->detected_v_corner[1],
                detector->detected_u_corner[2], detector->detected_v_corner[2],
                detector->detected_u_corner[3], detector->detected_v_corner[3]);

            if (mode == 3 && tracker->track(frame)) {

                if (show_keypoints) {
                    draw_detected_keypoints(frame, detector);
                    draw_recognized_keypoints(frame, detector);
                }
                draw_tracked_position(frame, tracker);
                if (show_tracked_locations) draw_tracked_locations(frame, tracker);

                cvPutText(frame, "detection+template-based 3D tracking", cvPoint(10, 30), &font, cvScalar(255, 255, 255));
            }
            else {
                if (show_keypoints) {
                    draw_detected_keypoints(frame, detector);
                    draw_recognized_keypoints(frame, detector);
                }
                draw_detected_position(frame, detector);
                cvPutText(frame, "detection", cvPoint(10, 30), &font, cvScalar(255, 255, 255));
            }
        }
        else {
            last_frame_ok = false;
            if (show_keypoints) draw_detected_keypoints(frame, detector);

            if (mode == 3)
                cvPutText(frame, "detection + template-based 3D tracking", cvPoint(10, 30), &font, cvScalar(255, 255, 255));
            else
                cvPutText(frame, "detection", cvPoint(10, 30), &font, cvScalar(255, 255, 255));
        }
    }


    cvShowImage("ferns-demo", frame);
    endtime = CTime::GetTickCount();
    CString timeStr;
    timeStr.Format(_T("耗时：%ds。"), (endtime - starttime));
    AfxMessageBox(timeStr);

    cvWaitKey();
}

void CRandomFernsDlg::doFerns()
{
    starttime = CTime::GetTickCount();

    string model_image = "model.bmp";
    string detect_image = "detect.png";

    affine_transformation_range range;

    detector = planar_pattern_detector_builder::build_with_cache(model_image.c_str(),
        &range,
        400,
        5000,
        0.0,
        32, 7, 4,
        30, 12,
        10000, 200);

    if (!detector) {
        cerr << "Unable to build detector.\n";
        return;
    }

    detector->set_maximum_number_of_points_to_detect(1000);

    tracker = new template_matching_based_tracker();
    string trackerfn = model_image + string(".tracker_data");
    if (!tracker->load(trackerfn.c_str())) {
        cout << "Training template matching..." << endl;
        tracker->learn(detector->model_image,
            5, // number of used matrices (coarse-to-fine)
            40, // max motion in pixel used to train to coarser matrix
            20, 20, // defines a grid. Each cell will have one tracked point.
            detector->u_corner[0], detector->v_corner[1],
            detector->u_corner[2], detector->v_corner[2],
            40, 40, // neighbordhood for local maxima selection
            10000 // number of training samples
        );
        tracker->save(trackerfn.c_str());
    }
    tracker->initialize();

    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX,
        1.0, 1.0, 0.0,
        3, 8);

    IplImage* frame, * gray_frame = NULL;

    frame = cvLoadImage(detect_image.c_str());


    if (gray_frame == NULL)
        gray_frame = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 1);

    cvCvtColor(frame, gray_frame, CV_RGB2GRAY);

    if (frame->origin != IPL_ORIGIN_TL)
        cvFlip(gray_frame, gray_frame, 0);

    
    detect_and_draw(gray_frame);
    

    cvReleaseImage(&frame);


    clog << endl;
    delete detector;
    delete tracker;

    cvReleaseImage(&gray_frame);
    cvDestroyWindow("ferns-demo");
}


void CRandomFernsDlg::OnBnClickedProcess()
{
    // TODO: 在此添加控件通知处理程序代码
    

    doFerns();
}