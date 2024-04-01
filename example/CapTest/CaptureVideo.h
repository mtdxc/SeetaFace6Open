// CCaptureVideo视频捕捉类头文件
/////////////////////////////////////////////////////////////////////
#if !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
#define AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_
/////////////////////////////////////////////////////////////////////
// CaptureVideo.h : header file
/////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <atlbase.h>
#include <windows.h>
#include <dshow.h>
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "Qedit.h" 

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x ) \
if ( NULL != x ) \
{ \
  x->Release( ); \
  x = NULL; \
}
#endif

typedef void (*DataCallBack)(BYTE* data, int width, int height, void* user);
class CSampleGrabberCB : public ISampleGrabberCB 
{
public:
	friend class CCaptureVideo;
	CSampleGrabberCB(); 
	~CSampleGrabberCB(){}

	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );
	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize );

protected:
	/// 采集宽度
	int width;
	/// 采集宽度
	int height;
	UINT fmtTime;
	/// 回调
	DataCallBack onData;
	/// 回调参数
	void* usdata;
};

class CCaptureVideo //: public CWnd 
{
public:
	void SetSize(int width, int height, UINT timeFmt=DT_BOTTOM|DT_SINGLELINE);
	void SetCallBack(DataCallBack cb, void* user);
    /*
    初始化视频捕捉
    @param iDeviceID 设备ID 
    @param hWnd 本地预览窗口,如为NULL则隐藏本地预览窗口
    @return HRESULT 
    */
    HRESULT Start(int iDeviceID, HWND hWnd);
	// 停止视频捕捉
	void Stop();
  bool started() { return m_pGB != NULL; }
	typedef std::vector<CString> DevList;
	/// 把视频设备枚举到列表控件中
    static int EnumDevices(HWND hList);
	static int EnumDevices(DevList& devList);

    CCaptureVideo();
    virtual ~CCaptureVideo();

private:
	CSampleGrabberCB sgCB;
    IGraphBuilder *m_pGB;
    ICaptureGraphBuilder2* m_pCapture;
    IBaseFilter* m_pBF;
    IMediaControl* m_pMC;
    IVideoWindow* m_pVW;
    CComPtr<ISampleGrabber> m_pGrabber;
protected:
    HRESULT SetVideoSize(int width, int height);
    void FreeMediaType(AM_MEDIA_TYPE& mt);
	void DeleteMediaType(AM_MEDIA_TYPE* pmt);
    bool BindFilter(int deviceId, IBaseFilter **pFilter);
    void ResizeVideoWindow(HWND hWnd);
    HRESULT SetupVideoWindow(HWND hWnd);
    HRESULT InitCaptureGraphBuilder();
};
#endif // !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
