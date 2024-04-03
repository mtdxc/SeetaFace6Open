#include "stdafx.h"
#include "DrawVideo.h"
#define DBG_INFO
#define GID_DRAWVDO _T("DrawVdo")

CDrawVideo::CDrawVideo():m_rect(0,0,0,0)
{
	m_hDib =DrawDibOpen();
	if(!m_hDib)
	{
		DBG_INFO(GID_DRAWVDO, "***DrawDibOpen error");
	}
	m_bUseGDI = FALSE;
	Init(NULL);
}

CDrawVideo::~CDrawVideo()
{
	DrawDibClose(m_hDib);
}

void CDrawVideo::Draw(BYTE *pBuffer,int width, int height, BOOL flip, LPCTSTR szText)
{
	if(!IsWindow(m_hWnd))
	{
		DBG_INFO(GID_DRAWVDO, "no window %X, break draw", m_hWnd);
		return;
	}
	BOOL bVisible = (GetWindowLong(m_hWnd, GWL_STYLE) & WS_VISIBLE);
	if(!bVisible){ //IsWindowVisible(m_hWnd)
		DBG_INFO(GID_DRAWVDO, "m_hWnd %X not Visible skip draw", m_hWnd);
		return;
	}
	//DBG_TRACE("DrawVideo", "Draw");
	HDC hDC=::GetDC(m_hWnd);
	if(!hDC){
		DBG_INFO(GID_DRAWVDO, "unable to GetDC %X, break draw", m_hWnd);
		return ;
	}
	CRect destRect;
	if(!GetClientRect(m_hWnd, &destRect))
	{
		DBG_INFO(GID_DRAWVDO, "unable to GetClientRect %X,%d break draw", m_hWnd, GetLastError());
		ReleaseDC(m_hWnd,hDC);
		return;
	}
	if(destRect.IsRectEmpty())
	{
		DBG_INFO(GID_DRAWVDO, "***rect empty, GWL_STYLE: %X", GetWindowLong(m_hWnd, GWL_STYLE));
		ReleaseDC(m_hWnd,hDC);
		return;
	}
	if(m_rect!=destRect)
	{
		DBG_INFO(GID_DRAWVDO, "MoveRect from %d,%d,%d,%d to %d,%d,%d,%d", 
			m_rect.left, m_rect.top, m_rect.right, m_rect.bottom, 
			destRect.left, destRect.top, destRect.right, destRect.bottom);
		m_rect = destRect;
	}
	float xscale = destRect.Width()*1.0 / width;
	float yscale = destRect.Height()*1.0 / height;
	destRect.DeflateRect(1,1);
	if(pBuffer)
	{
		bih.bmiHeader.biWidth = width;
		bih.bmiHeader.biHeight = flip?height:-height;
		bih.bmiHeader.biSizeImage = width * height * 3;
		if (m_bUseGDI || !flip) {
			int oldMode = SetStretchBltMode(hDC, COLORONCOLOR);
			StretchDIBits(hDC, destRect.left, destRect.top, destRect.Width(), destRect.Height(),
				0, 0, width, height, pBuffer, &bih, DIB_RGB_COLORS, SRCCOPY);
			SetStretchBltMode(hDC, oldMode);
		}
		else {
			if (!::DrawDibDraw(m_hDib,
				hDC,
				destRect.left, // dest : left pos
				destRect.top, // dest : top pos
				destRect.Width(), // don't zoom x
				destRect.Height(), // don't zoom y
				&bih.bmiHeader, // bmp header info
				pBuffer,    // bmp data
				0, // dest : left pos
				0, // dest : top pos
				width, // don't zoom x
				height, // don't zoom y
				DDF_NOTKEYFRAME))   // use prev params....//DDF_SAME_DRAW
				DBG_INFO(GID_DRAWVDO, "DrawDibDraw %X error", m_hWnd);
		}
	}
	// m_desRect.top = m_desRect.bottom - 20;
	if(szText && szText[0]) {
		if(1)//GetIniInt("View", "TransText", 0))
		{
			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, 0xffffff);
		}
		// TextOut(hDC, destRect.left+2, destRect.bottom - 20, szText, strlen(szText));
		DrawText(hDC, szText, -1, destRect, DT_BOTTOM|DT_SINGLELINE);
	}
	if (rects.size()) {
		HGDIOBJ bush = GetStockObject(NULL_BRUSH);
		HPEN pen = CreatePen(PS_DASH, 5, RGB(255, 0, 0));
		HGDIOBJ oldPen = ::SelectObject(hDC, pen);
		HGDIOBJ oldBush = ::SelectObject(hDC, bush);
		for (auto rc : rects) {
			//FrameRect(hDC, &rc, bush);
			rc.left *= xscale; rc.right *= xscale;
			rc.top *= yscale; rc.bottom *= yscale;
			if (rc.left < 0) rc.left = 0;
			if (rc.top < 0) rc.top = 0;
			if (rc.right > destRect.right) rc.right = destRect.right;
			if (rc.bottom > destRect.bottom) rc.bottom = destRect.bottom;
			Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
			if (rc.text.length()) {
				DrawText(hDC, rc.text.c_str(), -1, &rc, DT_TOP);
			}
		}
		SelectObject(hDC, oldBush);
		SelectObject(hDC, oldPen);
		DeleteObject(pen);
	}
	if (mapPoints.size()) {
#define PT_SIZE 3
		for (auto iter = mapPoints.begin(); iter!=mapPoints.end(); iter++){
			HGDIOBJ bush = CreateSolidBrush(iter->first);
			HGDIOBJ oldBush = ::SelectObject(hDC, bush);
			for (POINT pt : iter->second) {
				pt.x *= xscale;
				pt.y *= yscale;
				if (pt.x < 0) pt.x = 0; else if (pt.x > destRect.right) pt.x = destRect.right;
				if (pt.y < 0) pt.y = 0; else if (pt.y > destRect.bottom) pt.y = destRect.bottom;
				Ellipse(hDC, pt.x - PT_SIZE, pt.y - PT_SIZE, pt.x + PT_SIZE, pt.y + PT_SIZE);
				//SetPixel(hDC, pt.x, pt.y, RGB(255, 0, 0));
			}
			SelectObject(hDC, oldBush);
			DeleteObject(bush);
		}
	}
	ReleaseDC(m_hWnd,hDC);
}

BOOL CDrawVideo::Init( HWND hWnd )
{
	m_hWnd = hWnd;
	memset( &bih, 0, sizeof( bih ) );
	bih.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bih.bmiHeader.biPlanes = 1;
	bih.bmiHeader.biBitCount = 24;
	return TRUE;
}
