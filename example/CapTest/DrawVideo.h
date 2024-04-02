//绘制图像类
//JC 2013-07-31
#ifndef AVLIB_DRAWVIDEO_H_
#define AVLIB_DRAWVIDEO_H_
#include <Vfw.h>
#include <vector>
#include <string>
#include <map>
struct RectInfo : public RECT {
	std::string text;
};
class CDrawVideo
{
public:
	std::vector<RectInfo> rects;
	typedef std::vector<POINT> POINTS;
	std::map<COLORREF, POINTS> mapPoints;
	//Attributes
protected:
	HWND m_hWnd;
	HDRAWDIB m_hDib;
	BITMAPINFO bih;
	CRect m_rect;
	BOOL m_bUseGDI;
public:
	CDrawVideo();
	~CDrawVideo();
	BOOL Init(HWND hWnd);
	void Release() {}
	//绘制RGB数据  数据长度为width*height*3
	void Draw(BYTE* pBuf, int width, int height, LPCTSTR szText = NULL);
};

#endif //AVLIB_DRAWVIDEO_H_