#pragma once

// ImageStatic.h

#include <atlimage.h>
#include <afxglobals.h>
class CImageStatic : public CStatic
{
	DECLARE_DYNAMIC(CImageStatic)

public:
	CImageStatic();
	virtual ~CImageStatic();

	BOOL SetImage(LPCTSTR szFileName);
	BOOL SaveImage(LPCTSTR szFileName);
	BOOL SetRGB(void* rgb, int width, int height);
	int putText(LPCTSTR text, CRect rc, int flag);
	int getWidth() const { return m_Image.GetWidth(); }
	int getHeight() const { return m_Image.GetHeight(); }
	int setFont(LPCTSTR family, int size,
		bool bItalic = false,
		bool bUnderline = false, 
		bool bStrikeOut = false);
protected:
	HFONT m_hFont = nullptr;
	CImage m_Image;
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};