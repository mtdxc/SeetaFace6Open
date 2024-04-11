
// RealTimeTestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include <string>
#include "CaptureVideo.h"
#include "DrawVideo.h"
#include "ImageStatic.h"
#include "seeta/FaceLandmarker.h"
#include <memory>
class FaceEngine2;
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
	void OnRgbData(std::shared_ptr<seeta::ImageData> rgb);
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
	std::unique_ptr<FaceEngine2> m_engine;
	std::unique_ptr<seeta::FaceLandmarker> m_fd68;
	seeta::ImageData m_crop;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStartPrev();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnButtonCheckBox(UINT nID);
	afx_msg void OnBnClickedButtonAddFace();
	afx_msg void OnBnClickedButtonDelFace();
	afx_msg void OnLbnSelchangeListFace();

	CComboBox m_cbDevices;
	CListBox m_lFaces;
	CString m_szFaceName;
	CImageStatic m_imgFace;

	BOOL m_bAge;
	BOOL m_bSex;
	BOOL m_bEyes;
	BOOL m_bAnti;
	BOOL m_bMask;
};
