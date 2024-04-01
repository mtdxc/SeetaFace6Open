// ImageStatic.cpp : 实现文件
//

#include "stdafx.h"

#include "stdafx.h"
#include "CImageStatic.h"

// ImageStatic.cpp
IMPLEMENT_DYNAMIC(CImageStatic, CStatic)
CImageStatic::CImageStatic()
{
}

CImageStatic::~CImageStatic()
{
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
        return 0;
    }
    }
    return CStatic::WindowProc(message, wParam, lParam);
}
