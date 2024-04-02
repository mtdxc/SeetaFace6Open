
// RealTimeTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RealTimeTest.h"
#include "RealTimeTestDlg.h"
#include "convert.h"
#include "Ini.h"
#include <io.h>
#include <direct.h>
#include "../opencv/FaceEngine2.h"

#define SK_FACEDECT "FaceDetect"
#define SK_FACETRACKER "FaceTracker"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


// CRealTimeTestDlg 对话框
CRealTimeTestDlg::CRealTimeTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REALTIMETEST_DIALOG, pParent)
	, m_szFaceName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	std::string path = GetIniStr("Model", "Dir", "sf3.0_models");
	try {
		m_engine.reset(new FaceEngine2);
		m_engine->init(path.c_str(), "face_recognizer.csta");
		seeta::ModelSetting model2(path + "/face_landmarker_pts68.csta");
		m_fd68.reset(new seeta::FaceLandmarker(model2));
	}
	catch (std::exception e) {
		AfxMessageBox(e.what());
	}

}

CRealTimeTestDlg::~CRealTimeTestDlg()
{
	if (m_capture.started()) {
		m_capture.Stop();
	}
}

void CRealTimeTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICES, m_cbDevices);
	DDX_Control(pDX, IDC_LIST_FACE, m_lFaces);
	DDX_Text(pDX, IDC_EDIT_FACE_NAME, m_szFaceName);
	DDX_Control(pDX, IDC_STATIC_FACE, m_imgFace);
}

BEGIN_MESSAGE_MAP(CRealTimeTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_PREV, &CRealTimeTestDlg::OnBnClickedButtonStartPrev)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CRealTimeTestDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_ADD_FACE, &CRealTimeTestDlg::OnBnClickedButtonAddFace)
	ON_BN_CLICKED(IDC_BUTTON_DEL_FACE, &CRealTimeTestDlg::OnBnClickedButtonDelFace)
	ON_LBN_SELCHANGE(IDC_LIST_FACE, &CRealTimeTestDlg::OnLbnSelchangeListFace)
END_MESSAGE_MAP()


// CRealTimeTestDlg 消息处理程序

BOOL CRealTimeTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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
	if (m_capture.EnumDevices(m_cbDevices.GetSafeHwnd()))
		m_cbDevices.SetCurSel(0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRealTimeTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRealTimeTestDlg::OnPaint()
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
HCURSOR CRealTimeTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void VideCallBack(BYTE* data, int width, int height, void* user) {
	CRealTimeTestDlg* pDlg = (CRealTimeTestDlg*)user;
	if (pDlg) pDlg->OnRgbData(data, width, height);
}

void CRealTimeTestDlg::OnRgbData(BYTE* pRgb, int width, int height)
{
	int nyuv = width * height * 3 / 2;
	if (yuv.size() < nyuv)
		yuv.resize(nyuv);
	BYTE* pyuv = (BYTE*)yuv.data();
	// 这边要进行翻转(否则识别不对)！
	// RGBtoYUV420P(pRgb, pyuv, width, height, 3, TRUE);

	m_draw.rects.clear();
	m_draw.mapPoints.clear();
	if (m_engine) {
		/*
		seeta::ImageData rgb_data,yuv_data;
		// yuv通道数据
		rgb_data.width = width;
		rgb_data.height = height;
		yuv_data.width = width;
		yuv_data.height = height;
		rgb_data.channels = 3;
		rgb_data.data = pRgb;// -(height - 1) * width * 3;
		yuv_data.channels = 1;
		yuv_data.data = pyuv;
		seeta::ImageData& img_data = rgb_data;
		*/
		seeta::ImageData img_data(width, height, 3);
		byte* pdst = img_data.data;
		int stride = width * 3;
		for (size_t y = 0; y < height; y++)
		{
			byte* psrc = pRgb + (height - 1 - y) * stride;
			memcpy(pdst, psrc, stride);
			pdst += stride;
		}
		// YUV420PtoRGB(pyuv, img_data.data, width, height, FALSE);
		m_engine->updateRgb(img_data);
		int nFace = m_engine->face_size();
		m_draw.rects.resize(nFace);
		for (int i = 0; i < nFace; i++)
		{
			auto face = m_engine->get(i);
			m_draw.rects[i].left = face->pos.x;
			m_draw.rects[i].top = face->pos.y;
			m_draw.rects[i].right = face->pos.x + face->pos.width;
			m_draw.rects[i].bottom = face->pos.y + face->pos.height;
			
			m_draw.rects[i].text.clear();
			
			if (face->db.score > 0.5) {
				CString text;
				text.Format("%s@%.2f", face->db.name.c_str(), face->db.score);
				m_draw.rects[i].text = text;
			}
			// 增加检测信息
			AntiSpoofing as;
			if (IsDlgButtonChecked(IDC_CHECK_ANTI) && m_engine->getAnti(i, as)) {
				if (m_draw.rects[i].text.length()) m_draw.rects[i].text.push_back(' ');
				m_draw.rects[i].text += m_engine->get_fas_status(as.status);
			}
			int val1, val2;
			if (IsDlgButtonChecked(IDC_CHECK_GENDER) && m_engine->getGender(i, val1)) {
				if (m_draw.rects[i].text.length()) m_draw.rects[i].text.push_back(' ');
				m_draw.rects[i].text += val1 ? "女" : "男";
			}
			if (IsDlgButtonChecked(IDC_CHECK_AGE) && m_engine->getAge(i, val1)) {
				if (m_draw.rects[i].text.length()) m_draw.rects[i].text.push_back(' ');
				m_draw.rects[i].text += std::to_string(val1) + "岁";
			}
			float fMask;
			if (IsDlgButtonChecked(IDC_CHECK_MASK) && m_engine->getMask(i, fMask) && fMask > 0.5) {
				if (m_draw.rects[i].text.length()) m_draw.rects[i].text.push_back(' ');
				m_draw.rects[i].text += "戴口罩";
			}
			if (IsDlgButtonChecked(IDC_CHECK_EYES) && m_engine->getEyeStat(i, val1, val2)) {
				if (m_draw.rects[i].text.length()) m_draw.rects[i].text.push_back('\n');
				m_draw.rects[i].text += std::string("左眼:") + m_engine->get_eye_status(val1);
				m_draw.rects[i].text += std::string(",右眼:") + m_engine->get_eye_status(val2);
			}
			// 保存截图
			if (i == 0 && face->crop.count()) {
				m_crop = face->crop;
				m_imgFace.SetRGB(m_crop.data, m_crop.width, -m_crop.height);
			}
			auto points = face->points;
			if (m_fd68) {
				points = m_fd68->mark(img_data, face->pos);
			}
			for (int i = 0; i < points.size(); i++) {
				POINT pt{ points[i].x, points[i].y };
				m_draw.mapPoints[RGB(255, 0, 0)].push_back(pt);
			}
		}
	}

	m_nFrame++;
	if (GetTickCount() - m_tick > 1000) {
		_stprintf(strfps, _T("FPS:%d"), m_nFrame);
		m_nFrame = 0;
		m_tick = GetTickCount();
	}
	m_draw.Draw(pRgb, width, height, strfps);
}


void CRealTimeTestDlg::OnBnClickedButtonStartPrev()
{
	CWnd* btn = GetDlgItem(IDC_BUTTON_START_PREV);
	// TODO: 在此添加控件通知处理程序代码
	if (m_capture.started()) {
		m_capture.Stop();
		btn->SetWindowText(_T("Start Test"));
	}
	else {
		m_capture.SetSize(640, 480);
		m_capture.SetCallBack(&VideCallBack, this);
		HWND hVideo = NULL;
		GetDlgItem(IDC_VIDEO, &hVideo);
		m_draw.Init(hVideo);

		m_capture.Start(m_cbDevices.GetCurSel(), NULL);// hVideo);
		m_nFrame = 0;
		strfps[0] = 0;
		m_tick = GetTickCount();

		btn->SetWindowText(_T("Stop Test"));
	}
}

void CRealTimeTestDlg::OnBnClickedButtonReset() {
	// TODO: 在此添加控件通知处理程序代码
}

#ifndef S_ISDIR
#define S_ISDIR(X) ((X) & _S_IFDIR)
#endif

std::string GetFaceFile(int64_t data) {
	std::string sRet = "faces/";
	struct stat fileStat;
	if ((stat(sRet.c_str(), &fileStat) == 0) && S_ISDIR(fileStat.st_mode))
	{
		// exist = true;
	}
	else {
		mkdir(sRet.c_str());
	}
	return sRet + std::to_string(data) + ".bmp";
}

std::string GetFaceFile(const char* data) {
	std::string sRet = "faces/";
	struct stat fileStat;
	if ((stat(sRet.c_str(), &fileStat) == 0) && S_ISDIR(fileStat.st_mode))
	{
		// exist = true;
	}
	else {
		mkdir(sRet.c_str());
	}
	return sRet + data + ".bmp";
}

void CRealTimeTestDlg::OnBnClickedButtonAddFace()
{
	UpdateData();
	if (m_szFaceName.GetLength() && m_engine && m_crop.width) {
		
		int idx = m_lFaces.FindString(0, m_szFaceName);
		if (idx == -1) {
			idx = m_lFaces.AddString(m_szFaceName);
			m_engine->addFaceDb(m_szFaceName, m_crop);
			m_imgFace.SaveImage(GetFaceFile(m_szFaceName).c_str());
		}
		m_lFaces.SetCurSel(idx);
	}
}


void CRealTimeTestDlg::OnBnClickedButtonDelFace()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = m_lFaces.GetCurSel();
	if (sel != -1) {
		CString text;
		m_lFaces.GetText(sel, text);
		if (m_engine)
			m_engine->delFaceDb(text);
		remove(GetFaceFile(m_szFaceName).c_str());
		m_lFaces.DeleteString(sel);
	}
}


void CRealTimeTestDlg::OnLbnSelchangeListFace()
{
	int sel = m_lFaces.GetCurSel();
	if (sel != -1) {
		m_lFaces.GetText(sel, m_szFaceName);
		m_imgFace.SetImage(GetFaceFile(m_szFaceName).c_str());
		UpdateData(FALSE);
	}
}
