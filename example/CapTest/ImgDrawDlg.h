#pragma once

#include "ImageStatic.h"
// CImgDrawDlg 对话框

class CImgDrawDlg : public CDialog
{
	DECLARE_DYNAMIC(CImgDrawDlg)

public:
	CImgDrawDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CImgDrawDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMGDRAW_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedSelectFile();
	afx_msg void OnBnClickedSaveFile();
	afx_msg void OnBnClickedAdd();

	CImageStatic m_cImage;
	CEdit m_cLine;
	int ypos;
};
