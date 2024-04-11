// ImageStatic.cpp : 实现文件
//

#include "stdafx.h"

#include "stdafx.h"
#include "ImageStatic.h"

// ImageStatic.cpp
IMPLEMENT_DYNAMIC(CImageStatic, CStatic)
CImageStatic::CImageStatic()
{
    m_hFont = nullptr;
}

CImageStatic::~CImageStatic()
{
    if (m_hFont) {
        DeleteObject(m_hFont);
        m_hFont = nullptr;
    }
}


BEGIN_MESSAGE_MAP(CImageStatic, CStatic)
END_MESSAGE_MAP()

// CImageStatic 消息处理程序
BOOL CImageStatic::SetImage(LPCTSTR szFileName)
{
    BOOL bRet = FALSE;
    m_Image.Destroy(); //先释放以前资源
    bRet = (m_Image.Load(szFileName) == S_OK); //加载新资源
    if (m_hWnd)
        Invalidate(FALSE);
    return bRet;
}

BOOL CImageStatic::SaveImage(LPCTSTR szFileName)
{
    if (m_Image.IsNull())
        return FALSE;
    return m_Image.Save(szFileName) == S_OK;
}

BOOL CImageStatic::SetRGB(void* rgb, int width, int height)
{
    if (m_Image.IsNull() || m_Image.GetWidth() != width || m_Image.GetHeight() != height) {
        m_Image.Destroy();
        m_Image.Create(width, height, 24);
    }
    if (height > 0) {
        memcpy(m_Image.GetPixelAddress(0, height - 1), rgb, width * height * 3);
    }
    else {
        memcpy(m_Image.GetBits(), rgb, width * -height * 3);
    }
    if (m_hWnd)
        Invalidate(FALSE);
    return 0;
}


int CImageStatic::setFont(LPCTSTR family, int size,
    bool bItalic, bool bUnderline, bool bStrikeOut)
{
    //字体句柄
/* 自己创建字体
HFONT hfont = CreateFont(40,     //字体高度
    0,      //字体宽度,如果给出了高度，宽度给0，自动匹配宽高
    0,      //文字的倾斜角度 10就表示1度
    0,      //基线的倾斜角度
    900,    //笔画的粗细，400以下为细体，700以上为粗体
    FALSE,   //斜体
    FALSE,   //有下划线
    FALSE,   //水平删除线
    DEFAULT_CHARSET,//GB2312_CHARSET,     //设置字符集
    OUT_OUTLINE_PRECIS,
    CLIP_DEFAULT_PRECIS,
    CLEARTYPE_QUALITY,
    VARIABLE_PITCH,
    _T("Impact"));   //字样名，需要在字符集中有才能设定
*/
    if (m_hFont) {
        DeleteObject(m_hFont);
        m_hFont = nullptr;
    }
    m_hFont = CreateFont(size,
        0, 0, 0, FW_DONTCARE, bItalic, bUnderline, bStrikeOut, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, family);
    return m_hFont != nullptr;
}

int CImageStatic::putText(LPCTSTR text, CRect rc, int flag)
{
    if (m_Image.IsNull())
        return -1;
    HDC hDC = m_Image.GetDC();
    //设置文字颜色
    SetTextColor(hDC, RGB(255, 0, 0));
    //设置背景颜色
    SetBkColor(hDC, RGB(255, 255, 0));
    //设置文字背景模式 
    //OPAQUE，系统默认，用自身背景色来填充整个背景，TRANSPARENT为透明模式
    SetBkMode(hDC, TRANSPARENT);

    int ret = 0;
    if (m_hFont) {
        HGDIOBJ hOld = ::SelectObject(hDC, m_hFont);
        ret = DrawText(hDC, text, -1, rc, flag);
        ::SelectObject(hDC, hOld);
    }
    else {
        ret = DrawText(hDC, text, -1, rc, flag);
        // TextOut(hDC, rc.left, rc.top, text, -1);
    }
    m_Image.ReleaseDC();
    Invalidate();
    return ret;
}

LRESULT CImageStatic::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    switch (message)
    {
    case(WM_ERASEBKGND):
        return 1;
    case(WM_PAINT):
    {
        PAINTSTRUCT ps;
        CDC* pDstDC = BeginPaint(&ps);

        //客户区位置
        CRect rcClient;
        GetClientRect(&rcClient);

        //双缓冲绘图
        {
            CMemDC memDC(*pDstDC, rcClient);
            CDC* pDC = &memDC.GetDC();

            //填充背景色
            pDC->FillSolidRect(&rcClient, RGB(100, 100, 100));
            pDC->SetStretchBltMode(HALFTONE);

            if (!m_Image.IsNull())
            {
                //原图大小
                CRect rcSrc(0, 0, m_Image.GetWidth(), m_Image.GetHeight());
                //锁定原图比例
                double fZoomRate = min((double)rcClient.Width() / (double)rcSrc.Width(),
                    (double)rcClient.Height() / (double)rcSrc.Height());
                int cx = (int)(rcSrc.Width() * fZoomRate), cy = (int)(rcSrc.Height() * fZoomRate);
                CRect rcDst(0, 0, cx, cy); //目标大小
                rcDst.OffsetRect((rcClient.Width() - cx) / 2, (rcClient.Height() - cy) / 2);//居中

                //绘图
                m_Image.Draw(pDC->m_hDC, rcDst, rcSrc);
            }
        }

        EndPaint(&ps);
        break;
    }
    }
    return CStatic::WindowProc(message, wParam, lParam);
}
