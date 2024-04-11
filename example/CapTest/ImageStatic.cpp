// ImageStatic.cpp : ʵ���ļ�
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

// CImageStatic ��Ϣ�������
BOOL CImageStatic::SetImage(LPCTSTR szFileName)
{
    BOOL bRet = FALSE;
    m_Image.Destroy(); //���ͷ���ǰ��Դ
    bRet = (m_Image.Load(szFileName) == S_OK); //��������Դ
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
    //������
/* �Լ���������
HFONT hfont = CreateFont(40,     //����߶�
    0,      //������,��������˸߶ȣ���ȸ�0���Զ�ƥ����
    0,      //���ֵ���б�Ƕ� 10�ͱ�ʾ1��
    0,      //���ߵ���б�Ƕ�
    900,    //�ʻ��Ĵ�ϸ��400����Ϊϸ�壬700����Ϊ����
    FALSE,   //б��
    FALSE,   //���»���
    FALSE,   //ˮƽɾ����
    DEFAULT_CHARSET,//GB2312_CHARSET,     //�����ַ���
    OUT_OUTLINE_PRECIS,
    CLIP_DEFAULT_PRECIS,
    CLEARTYPE_QUALITY,
    VARIABLE_PITCH,
    _T("Impact"));   //����������Ҫ���ַ������в����趨
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
    //����������ɫ
    SetTextColor(hDC, RGB(255, 0, 0));
    //���ñ�����ɫ
    SetBkColor(hDC, RGB(255, 255, 0));
    //�������ֱ���ģʽ 
    //OPAQUE��ϵͳĬ�ϣ���������ɫ���������������TRANSPARENTΪ͸��ģʽ
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
    // TODO: �ڴ����ר�ô����/����û���
    switch (message)
    {
    case(WM_ERASEBKGND):
        return 1;
    case(WM_PAINT):
    {
        PAINTSTRUCT ps;
        CDC* pDstDC = BeginPaint(&ps);

        //�ͻ���λ��
        CRect rcClient;
        GetClientRect(&rcClient);

        //˫�����ͼ
        {
            CMemDC memDC(*pDstDC, rcClient);
            CDC* pDC = &memDC.GetDC();

            //��䱳��ɫ
            pDC->FillSolidRect(&rcClient, RGB(100, 100, 100));
            pDC->SetStretchBltMode(HALFTONE);

            if (!m_Image.IsNull())
            {
                //ԭͼ��С
                CRect rcSrc(0, 0, m_Image.GetWidth(), m_Image.GetHeight());
                //����ԭͼ����
                double fZoomRate = min((double)rcClient.Width() / (double)rcSrc.Width(),
                    (double)rcClient.Height() / (double)rcSrc.Height());
                int cx = (int)(rcSrc.Width() * fZoomRate), cy = (int)(rcSrc.Height() * fZoomRate);
                CRect rcDst(0, 0, cx, cy); //Ŀ���С
                rcDst.OffsetRect((rcClient.Width() - cx) / 2, (rcClient.Height() - cy) / 2);//����

                //��ͼ
                m_Image.Draw(pDC->m_hDC, rcDst, rcSrc);
            }
        }

        EndPaint(&ps);
        break;
    }
    }
    return CStatic::WindowProc(message, wParam, lParam);
}
