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
protected:
	CImage m_Image;
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};