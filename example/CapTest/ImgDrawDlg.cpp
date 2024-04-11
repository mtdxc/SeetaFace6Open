// ImgDrawDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "ImgDrawDlg.h"


// CImgDrawDlg 对话框

IMPLEMENT_DYNAMIC(CImgDrawDlg, CDialog)

CImgDrawDlg::CImgDrawDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_IMGDRAW_DIALOG, pParent)
{

}

CImgDrawDlg::~CImgDrawDlg()
{
}

void CImgDrawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMG, m_cImage);
	DDX_Control(pDX, IDC_EDIT_LINE, m_cLine);
}


BEGIN_MESSAGE_MAP(CImgDrawDlg, CDialog)
	ON_BN_CLICKED(IDC_SELECT_FILE, &CImgDrawDlg::OnBnClickedSelectFile)
	ON_BN_CLICKED(IDB_ADD, &CImgDrawDlg::OnBnClickedAdd)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SAVE_FILE, &CImgDrawDlg::OnBnClickedSaveFile)
END_MESSAGE_MAP()

BOOL CImgDrawDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// CImgDrawDlg 消息处理程序

void CImgDrawDlg::OnBnClickedSelectFile()
{
	CFileDialog dlg(TRUE);
	dlg.GetOFN().lpstrFilter = _T("Image Files |*.png,*.jpg,*.jpeg|All Files (*.*)|*.*||");
	if (dlg.DoModal() == IDOK) {
		m_cImage.SetImage(dlg.GetPathName());
		ypos = 0;
	}
}

void CImgDrawDlg::OnBnClickedSaveFile()
{
	CFileDialog dlg(FALSE, _T(".png"), NULL, 6, "Image Files |*.png");
	if (dlg.DoModal() == IDOK) {
		m_cImage.SaveImage(dlg.GetPathName());
	}
}

void CImgDrawDlg::OnBnClickedAdd()
{
	CString line;
	m_cLine.GetWindowText(line);
	if (line) {
		static int fontSize = 10;
		static int fontIdx = 0;
		CString fonts[] = { "Arial", "Impact", "楷体", "宋体" };
		fontSize += 5;
		m_cImage.setFont(fonts[fontIdx++ % 4], fontSize);
		CRect rc(0, ypos, m_cImage.getWidth(), m_cImage.getHeight());
		ypos += m_cImage.putText(line, rc, DT_SINGLELINE|DT_END_ELLIPSIS);
	}
}


void CImgDrawDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_cLine.m_hWnd) {
		CRect rc1;
		m_cLine.GetWindowRect(rc1);
		ScreenToClient(rc1);
		rc1.right = cx - 10;
		m_cLine.MoveWindow(rc1);
	}
	if (m_cImage.m_hWnd) {
		CRect rc(10, 50, cx - 10, cy - 10);
		m_cImage.MoveWindow(rc, FALSE);
		m_cImage.Invalidate();
	}
}


