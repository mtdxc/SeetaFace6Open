
// RealTimeTestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include <string>
#include "CaptureVideo.h"
#include "DrawVideo.h"
#include "FaceEngine.h"
#include "CImageStatic.h"
#include <memory>
// CRealTimeTestDlg �Ի���
class CRealTimeTestDlg : public CDialogEx
{
	// ����
public:
	CRealTimeTestDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CRealTimeTestDlg();
	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REALTIMETEST_DIALOG };
#endif
	void OnRgbData(BYTE* pRgb, int with, int heght);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	TCHAR strfps[32];
	DWORD m_tick;
	int m_nFrame;
	HICON m_hIcon;
	CCaptureVideo m_capture;
	CDrawVideo m_draw;
	std::unique_ptr<seeta::FaceEngine> m_engine;
	std::unique_ptr<seeta::FaceLandmarker> m_fd68;
	std::unique_ptr<seeta::ImageData> m_crop;
	std::string yuv;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStartPrev();
	afx_msg void OnBnClickedButtonReset();

	CComboBox m_cbDevices;
	CComboBox m_cbCheckType;
	CListBox m_lFaces;
	CString m_szFaceName;
	CImageStatic m_imgFace;

	afx_msg void OnBnClickedButtonAddFace();
	afx_msg void OnBnClickedButtonDelFace();
	afx_msg void OnLbnSelchangeListFace();
};
