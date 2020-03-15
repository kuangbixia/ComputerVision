## MFC
- MFC(Microsoft Foundation Classes)，是微软公司提供的一个类库（class libraries），以C++类的形式封装了Windows的API，并且包含一个应用程序框架，以减少应用程序开发人员的工作量。其中包含的类包含大量Windows句柄封装类和很多Windows的内建控件和组件的封装类。
- 简而言之，MFC是WinAPI与C++的结合。
- 要明白，MFC不只是一个功能单纯的界面开发系统，最应该花费时间的是**消息和设备环境**。
- [数据类型](https://baike.baidu.com/item/MFC/2236974#7)
#### 基于对话框的MFC
##### 设计窗口
资源视图->dialog文件下->打开工具箱->添加控件->设置控件ID（以便在cpp文件中调用内置函数获取）->必要时修改Caption（在控件上显示自定义内容）
##### cpp文件
一般要包含以下自动生成的头文件：
- pch.h
- framework.h
- cpp对应的头文件
#### 根cpp的结构
##### CAboutDlg 对话框
用于应用程序“关于”菜单项。
##### C(projectNAME)Dlg 对话框
实际上就是对应头文件的成员函数的实现。
###### 构造函数
一般用于初始化类的成员变量。
``` C++
CTestMFCDlg::CTestMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// 初始化变量
	m_pImgSrc = NULL;
	m_pImgCpy = NULL;
	m_nThreadNum = 1;
	m_pThreadParam = new ThreadParam[MAX_THREAD];
}
```
###### DoDataExchange()函数
一般用于绑定控件的成员变量。
``` C++
void CTestMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
    // 绑定控件变量
	DDX_Control(pDX, IDC_EDIT_INFO, mEditInfo);
	DDX_Control(pDX, IDC_PICTURE, mPictureControl);
}
```
###### 消息函数映射
- 类似于事件处理
- 自定义消息函数时要注意：**要在framework.h中定义比WM_USER大的message**，作为ON_MESSAGE()的第一个参数。
``` C++
#ifndef WM_MEDIAN_FILTER
#define WM_MEDIAN_FILTER WM_USER+1
#endif

#ifndef WM_NOISE
#define WM_NOISE WM_USER+2
#endif
```
``` C++
BEGIN_MESSAGE_MAP(CTestMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	// 控件处理消息函数
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTestMFCDlg::OnBnClickedButtonOpen)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THREADNUM, &CTestMFCDlg::OnNMCustomdrawSliderThreadnum)
	ON_BN_CLICKED(IDC_BUTTON_DEAL, &CTestMFCDlg::OnBnClickedButtonDeal)

	// 定义线程通信消息函数
	ON_MESSAGE(WM_NOISE, &CTestMFCDlg::OnNoiseThreadMsgReceived)
	ON_MESSAGE(WM_MEDIAN_FILTER, &CTestMFCDlg::OnMedianFilterThreadMsgReceived)
END_MESSAGE_MAP()
```
###### 消息处理程序
实际上是消息函数映射里函数的实现。
- OnInitDialog()
    - 用于初始化窗口
    - 一般对控件进行初始设置
    ``` C++
        // 1.通过控件绑定的变量调用接口
        mEditInfo.SetWindowTextW(CString("显示图片路径"));
        // 2.通过控件ID获取控件来调用接口
	    CComboBox* cmb_function = (CComboBox*)GetDlgItem(IDC_COMBO_FUNCTION);
	    cmb_function->AddString(_T("椒盐噪声"));
	    cmb_function->AddString(_T("自适应中值滤波"));
	    cmb_function->SetCurSel(0);
    ```
- OnSysCommand()
    - 暂时不理
- OnPaint()
    - 窗口处于最小化状态
        - 暂时不理
        - 有调用SendMessage()消息函数
    - 窗口不处于最小化状态
        - 必要时绘制
- OnQueryDragIcon()
    - 暂时不理
- 控件处理消息函数
    - 点击/滑动/选择控件后调用的函数
- 自定义的线程通信消息**接收**函数
    - 传入参数WPARAM，为0则是发送消息，为1则是接收消息。
###### 自定义函数的实现
一般是为了重用。

#### 自定义消息函数
- 要先定义消息
	- 要比WM_USER大
	- 通过这个变量来绑定消息的发送与接收
		- 发送消息时会传入它作为参数，以便接收函数识别
		``` C++
			::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MEDIAN_FILTER, 1, NULL);
		```
		- 接收消息函数在定义时，把它作为参数传入绑定发送消息和相应处理的函数
		``` C++
			ON_MESSAGE(WM_MEDIAN_FILTER, &CTestMFCDlg::OnMedianFilterThreadMsgReceived)
		```
``` C++
#ifndef WM_MEDIAN_FILTER
#define WM_MEDIAN_FILTER WM_USER+1
#endif
```
- 发送消息
``` C++
	// 传递消息
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MEDIAN_FILTER, 1, NULL);
```
- 接收消息，要自己定义一个接收消息函数
``` C++
	// 消息接收处理函数
	LRESULT CImgProcess1Dlg::OnMedianFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	if ((int)wParam == 1) // 0：发送消息，1：接收消息
	{
		...
	}
	return 0;
}
```

#### 分页
- 使用Tab Control控件。
- 新建对话框，并对它创建对应的类。注意：在新建的对话框点击鼠标右键->添加类，会省很多麻烦事。
- 在根对话框类头文件中，include新对话框类，使用它声明变量，以便在根cpp中引用新对话框的控件变量。
	- 可以在根cpp中直接使用新对话框变量对新对话框的控件进行设置。
	- 调用新对话框的create()函数与tab control控件关联。
- 将新对话框移动到tab control控件的空白位置，作为tab的一个页面。
- 使用辅助变量，跟踪tab的当前位置，将其对应的页面（即新对话框）设置为显示(SW_SHOW)，其余的页面则设置为不显示(SW_HIDE)。

#### Edit Control编辑框文本追加及换行
##### 文本追加
- GetWindowTextW()获取当前的text内容
``` C++
	CString text;
	mEditOutput.GetWindowTextW(text);
```
- +=延长字符串
- SetWindowTextW()设置新的text内容
``` C++
	text += "成功输入图像。\r\n>";
	mEditOutput.SetWindowTextW(text);
```
##### 换行
- 要在dialog中edit control控件设置属性
	- Mutilines->true
	- Want return->true
	- Horizontal Scroll->TRUE
	- Vertical Scroll->TRUE
- 字符串中的换行符不是"\n"，而是"\r\n"

#### CString转float
```C++
	CString str("0.5");
	float f=_ttof(str);
```

#### CImage写入访问权限冲突
- 使用m_pImgTemp作为第二张图象暂时存储处理结果（如：缩放，加权均值滤波等）时，要注意在创建多线程的部分，不要将m_pImgTemp的创建放入线程中，否则会导致后面线程处理时可能会同时写入同一个位置，导致写入访问冲突。