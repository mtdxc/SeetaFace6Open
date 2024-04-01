
// RealTimeTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RealTimeTest.h"
#include "RealTimeTestDlg.h"
#include "convert.h"
#include "Ini.h"
#include <io.h>
#include <direct.h>
#include <seeta/GenderPredictor.h>
#include <seeta/AgePredictor.h>
#include <seeta/EyeStateDetector.h>
#include <seeta/FaceAntiSpoofing.h>
#ifdef _DEBUG
//release ��,11��
#pragma comment(lib,"SeetaFaceDetector600d.lib") 
#pragma comment(lib,"SeetaFaceLandmarker600d.lib")

#pragma comment(lib,"SeetaFaceRecognizer610d.lib")
#pragma comment(lib,"SeetaGenderPredictor600d.lib") 
#pragma comment(lib,"SeetaAgePredictor600d.lib") 
#pragma comment(lib,"SeetaFaceAntiSpoofingX600d.lib") 
#pragma comment(lib,"SeetaEyeStateDetector200d.lib")

//���ĸ�û�õ�
#pragma comment(lib,"SeetaMaskDetector200d.lib")
#pragma comment(lib,"SeetaFaceTracking600d.lib") 
#pragma comment(lib,"SeetaPoseEstimation600d.lib")
#else
//release ��,11��
#pragma comment(lib,"SeetaFaceDetector600.lib") 
#pragma comment(lib,"SeetaFaceLandmarker600.lib")

#pragma comment(lib,"SeetaFaceRecognizer610.lib")
#pragma comment(lib,"SeetaGenderPredictor600.lib") 
#pragma comment(lib,"SeetaAgePredictor600.lib") 
#pragma comment(lib,"SeetaFaceAntiSpoofingX600.lib") 
#pragma comment(lib,"SeetaEyeStateDetector200.lib")

//���ĸ�û�õ�
#pragma comment(lib,"SeetaMaskDetector200.lib")
#pragma comment(lib,"SeetaFaceTracking600.lib") 
#pragma comment(lib,"SeetaPoseEstimation600.lib")
//#pragma comment(lib,"SeetaQualityAssessor300.lib")
#endif
#define SK_FACEDECT "FaceDetect"
#define SK_FACETRACKER "FaceTracker"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


// CRealTimeTestDlg �Ի���
CRealTimeTestDlg::CRealTimeTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REALTIMETEST_DIALOG, pParent)
	, m_szFaceName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	std::string path = GetIniStr("Model", "Dir", "sf3.0_models");
	try {
	if (access(path.c_str(), 0) == 0) {
		if (*path.rbegin() != '\\')
			path += "\\";
		seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
		int id = 0;
		seeta::ModelSetting FD_model(path + "face_detector.csta", device, id);
		seeta::ModelSetting PD_model(path + "face_landmarker_pts5.csta", device, id);
		seeta::ModelSetting FR_model(path + "face_recognizer.csta", device, id);
		m_engine.reset(new seeta::FaceEngine(FD_model, PD_model, FR_model, 2, 16));
	    /*
		* Set minimum and maximum size of faces to detect (Default: 20, Not Limited)
		- `face_detector.SetMinFaceSize(size);`
		- `face_detector.SetMaxFaceSize(size);`
		* Set step size of sliding window (Default: 4)
		- `face_detector.SetWindowStep(step_x, step_y);`
		* Set scaling factor of image pyramid (0 < `factor` < 1, Default: 0.8)
		- `face_detector.SetImagePyramidScaleFactor(factor);`
		* Set score threshold of detected faces (Default: 2.0)
		- `face_detector.SetScoreThresh(thresh);`
		*/
		if (auto face_detector = &m_engine->FD) {
			face_detector->set(seeta::FaceDetector::PROPERTY_MIN_FACE_SIZE, GetIniInt(SK_FACEDECT, "MinFaceSize", 80));
			//face_detector->set(seeta::FaceDetector::PROPERTY_THRESHOLD, GetIniFloat(SK_FACEDECT, "ScoreThresh", 2.0));
			//face_detector->SetImagePyramidScaleFactor(GetIniFloat(SK_FACEDECT, "ImagePyramidScaleFactor", 0.8));
			//face_detector->SetWindowStep(GetIniInt(SK_FACEDECT, "StepX", 4), GetIniInt(SK_FACEDECT, "StepY", 4));
		}

		seeta::ModelSetting gb_setting(path + "gender_predictor.csta");
		seeta::GenderPredictor GP(gb_setting);

		//5.������ģ�ͳ�ʼ��
		seeta::ModelSetting ap_setting(path + "age_predictor.csta");
		seeta::AgePredictor AP(ap_setting);

		//6.�۾�״̬ģ�ͳ�ʼ��
		seeta::ModelSetting setting;
		setting.append(path + "eye_state.csta");
		seeta::EyeStateDetector EBD(setting);

		//7.������ģ�ͳ�ʼ��
		seeta::ModelSetting anti_setting;
		anti_setting.append(path + "fas_first.csta");
		anti_setting.append(path + "fas_second.csta");
		seeta::FaceAntiSpoofing FAS(anti_setting);
		FAS.SetThreshold(0.3, 0.90);//����Ĭ����ֵ������һ����ֵΪ(0.7, 0.55)
		FAS.SetBoxThresh(0.9);
	}
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
	DDX_Control(pDX, IDC_COMBO_CHECKTYPE, m_cbCheckType);
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


// CRealTimeTestDlg ��Ϣ�������

BOOL CRealTimeTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	if (m_capture.EnumDevices(m_cbDevices.GetSafeHwnd()))
		m_cbDevices.SetCurSel(0);
	m_cbCheckType.AddString(_T("����׷��"));
	m_cbCheckType.AddString(_T("����ʶ��"));
	m_cbCheckType.SetCurSel(0);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRealTimeTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// ���Ҫ���з�ת(����ʶ�𲻶�)��
	RGBtoYUV420P(pRgb, pyuv, width, height, 3, TRUE);

	m_draw.rects.clear();
	m_draw.mapPoints.clear();
	switch (m_cbCheckType.GetCurSel())
	{
	case 0:
	break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
	if (m_engine) {
		seeta::ImageData img_data;
		// yuvͨ������
		img_data.width = width;
		img_data.height = height;
#if 1
		img_data.channels = 3;
		img_data.data = pRgb;// -(height - 1) * width * 3;
#else
		img_data.channels = 1;
		img_data.data = pyuv;
#endif

		auto faces = m_engine->DetectFaces(img_data);
		m_draw.rects.resize(faces.size());
		for (int i = 0; i < faces.size(); i++)
		{
			m_draw.rects[i] = RECT{ faces[i].pos.x, faces[i].pos.y,
				faces[i].pos.x + faces[i].pos.width, faces[i].pos.y + faces[i].pos.height };

			auto points = m_engine->DetectPoints(img_data, faces[i].pos);
			for (int i = 0; i < points.size(); i++) {
				POINT pt{ points[i].x, points[i].y };
				m_draw.mapPoints[RGB(255, 0, 0)].push_back(pt);
			}
			if (i == 0) {
				auto fdb = &m_engine->FDB;
				if (!m_crop) {
					m_crop.reset(new seeta::ImageData(fdb->GetCropFaceWidthV2(), fdb->GetCropFaceHeightV2(), fdb->GetCropFaceChannelsV2()));
				}
				fdb->CropFaceV2(img_data, &points[0], *m_crop);
				m_imgFace.SetRGB(m_crop->data, m_crop->width, m_crop->height);
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

void CRealTimeTestDlg::OnBnClickedButtonAddFace()
{
	UpdateData();
	if (m_szFaceName.GetLength() && m_engine && m_crop) {
		
		int idx = m_lFaces.FindString(0, m_szFaceName);
		if (idx == -1) {
			idx = m_lFaces.AddString(m_szFaceName);
			int64_t data = m_engine->Register(*m_crop);
			m_lFaces.SetItemData(idx, data);
			m_imgFace.SaveImage(GetFaceFile(data).c_str());
		}
		m_lFaces.SetCurSel(idx);
	}
}


void CRealTimeTestDlg::OnBnClickedButtonDelFace()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int sel = m_lFaces.GetCurSel();
	if (sel != -1) {
		int64_t data = m_lFaces.GetItemData(sel);
		if (m_engine)
			m_engine->Delete(data);
		remove(GetFaceFile(data).c_str());
		m_lFaces.DeleteString(sel);
	}
}


void CRealTimeTestDlg::OnLbnSelchangeListFace()
{
	int sel = m_lFaces.GetCurSel();
	if (sel != -1) {
		m_lFaces.GetText(sel, m_szFaceName);
		int64_t data = m_lFaces.GetItemData(sel);
		m_imgFace.SetImage(GetFaceFile(data).c_str());
		UpdateData(FALSE);
	}
}
