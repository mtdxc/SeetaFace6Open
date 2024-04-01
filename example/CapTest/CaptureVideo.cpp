//-------------------------------------------------------------------
// CCaptureVideo视频捕捉类实现文件CaptureVideo.cpp
//-------------------------------------------------------------------
// CaptureVideo.cpp: implementation of the CCaptureVideo class.
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CaptureVideo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCaptureVideo::CCaptureVideo()
{
  //COM Library Intialization
  if(FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
  {
    AfxMessageBox(_T("CoInitialize Failed!"));
    return;
  }
  m_pVW = NULL;
  m_pMC = NULL;
  m_pGB = NULL;
  m_pBF = NULL; 
  m_pCapture = NULL; 
}

CCaptureVideo::~CCaptureVideo()
{
	Stop();
    //CoUninitialize( );
}

int CCaptureVideo::EnumDevices(HWND hList)
{
  if (!hList)
    return -1;
  std::vector<CString> devList;
  int id = EnumDevices(devList);
  for (int i=0;i<id;i++)
  {
	  ::SendMessage(hList, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)devList[i]);
  }
  return id;
}

int CCaptureVideo::EnumDevices( std::vector<CString>& devList )
{
	int id = 0;

	//枚举视频扑捉设备
	ICreateDevEnum *pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);

	if (hr != NOERROR)return -1;
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);

	if (hr != NOERROR)return -1;
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) 
			{
				id++;
#ifdef _UNICODE
        devList.push_back(var.bstrVal);
#else
        char str[2048];
				WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, str, 2048, NULL, NULL);
				devList.push_back(str);
#endif
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
	}
	return id;
}

HRESULT CCaptureVideo::Start(int iDeviceID, HWND hWnd)
{
  HRESULT hr;
  hr = InitCaptureGraphBuilder();
  if (FAILED(hr)){
    AfxMessageBox(_T("Failed to get video interfaces!"));
    return hr;
  }
  // Bind Device Filter. We know the device because the id was passed in
  if(!BindFilter(iDeviceID, &m_pBF))return S_FALSE;
  hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
  // hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, 
  // m_pBF, NULL, NULL);
  // create a sample grabber
  hr = m_pGrabber.CoCreateInstance( CLSID_SampleGrabber );
  if( !m_pGrabber ){
    AfxMessageBox(_T("Fail to create SampleGrabber, maybe qedit.dll is not registered?"));
    return hr;
  }
  CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabBase( m_pGrabber );

  AM_MEDIA_TYPE mt; 
  ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
  mt.majortype = MEDIATYPE_Video;
  mt.subtype = MEDIASUBTYPE_RGB24; //MEDIASUBTYPE_YV12;
  hr = m_pGrabber->SetMediaType(&mt);
  if( FAILED( hr ) ){
    AfxMessageBox(_T("Fail to set media type!"));
    return hr;
  }
  hr = m_pGB->AddFilter( pGrabBase, L"Grabber" );
  if( FAILED( hr ) ){
    AfxMessageBox(_T("Fail to put sample grabber in graph"));
    return hr;
  }
  if(sgCB.width && sgCB.height)
	SetVideoSize(sgCB.width, sgCB.height);
  // try to render preview/capture pin
  hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);
  if( FAILED( hr ) )
    hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);

    if( FAILED( hr ) ){
      AfxMessageBox(_T("Can’t build the graph"));
      return hr;
    }
  
  hr = m_pGrabber->GetConnectedMediaType( &mt );
  if ( FAILED( hr) ){
    AfxMessageBox(_T("Failt to read the connected media type"));
    return hr;
  }
  
  VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mt.pbFormat;
  sgCB.width = vih->bmiHeader.biWidth;
  sgCB.height = vih->bmiHeader.biHeight;
  FreeMediaType(mt);

  hr = m_pGrabber->SetBufferSamples( FALSE );
  hr = m_pGrabber->SetOneShot( FALSE );
  hr = m_pGrabber->SetCallback( &sgCB, 1 );

  //设置视频捕捉窗口
  if(hWnd)
	SetupVideoWindow(hWnd);
  else
  {// 隐藏视频窗口
	  m_pVW->put_AutoShow(OAFALSE);
	  m_pVW->put_Visible(OAFALSE);
  }
  hr = m_pMC->Run();//开始视频捕捉
  if (FAILED(hr)){ 
    AfxMessageBox(_T("Couldn’t run the graph!")); 
    return hr; 
  }
  return S_OK;
}

bool CCaptureVideo::BindFilter(int deviceId, IBaseFilter **pFilter)
{
  if (deviceId < 0)
  return false;

  // enumerate all video capture devices
  CComPtr<ICreateDevEnum> pCreateDevEnum;
  HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
IID_ICreateDevEnum, (void**)&pCreateDevEnum);
  if (hr != NOERROR)
  {
    return false;
  }
  CComPtr<IEnumMoniker> pEm;
  hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
  if (hr != NOERROR) 
  {
    return false;
  }
  pEm->Reset();
  ULONG cFetched;
  IMoniker *pM;
  int index = 0;
  while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK, index <= deviceId)
  {
    IPropertyBag *pBag;
    hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
    if(SUCCEEDED(hr)) 
    {
      VARIANT var;
      var.vt = VT_BSTR;
      hr = pBag->Read(L"FriendlyName", &var, NULL);
      if (hr == NOERROR) 
      {
        if (index == deviceId)
        {
          pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
        }
        SysFreeString(var.bstrVal);
      }
      pBag->Release();
    }
    pM->Release();
    index++;
  }
  return true;
}

HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
  HRESULT hr;

  // 创建IGraphBuilder接口
  hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
  // 创建ICaptureGraphBuilder2接口
  hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
IID_ICaptureGraphBuilder2, (void **) &m_pCapture);
  if (FAILED(hr))return hr;
  m_pCapture->SetFiltergraph(m_pGB);
  hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
  if (FAILED(hr))return hr;
  hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVW);
  if (FAILED(hr))return hr;
  return hr;
}

HRESULT CCaptureVideo::SetupVideoWindow(HWND hWnd)
{
  HRESULT hr;
  hr = m_pVW->put_Owner((OAHWND)hWnd);
  if (FAILED(hr))return hr;
  hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
  if (FAILED(hr))return hr;
  ResizeVideoWindow(hWnd);
  hr = m_pVW->put_Visible(OATRUE);
  return hr;
}

void CCaptureVideo::ResizeVideoWindow(HWND hWnd)
{
  if (m_pVW){
    //让图像充满整个窗口
    CRect rc;
    ::GetClientRect(hWnd,&rc);
    m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
  } 
}

void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE& mt)
{
  if (mt.cbFormat != 0) {
    CoTaskMemFree((PVOID)mt.pbFormat);
    // Strictly unnecessary but tidier
    mt.cbFormat = 0;
    mt.pbFormat = NULL;
  }
  if (mt.pUnk != NULL) {
    mt.pUnk->Release();
    mt.pUnk = NULL;
  }
}

void CCaptureVideo::DeleteMediaType( AM_MEDIA_TYPE* pmt )
{
	if(pmt){
		FreeMediaType(*pmt);
		CoTaskMemFree(pmt);
	}
}
void CCaptureVideo::Stop()
{
	// Stop media playback
	if(m_pMC)m_pMC->Stop();
	if(m_pVW){
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
		SAFE_RELEASE(m_pVW);
	}
	SAFE_RELEASE(m_pCapture);
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pBF);
	m_pGrabber.Release();
}

HRESULT CCaptureVideo::SetVideoSize(int width, int height)
{
	//* 设置视频格式
	CComPtr<IAMStreamConfig> pConfig;
	HRESULT hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Interleaved,
		m_pBF,IID_IAMStreamConfig,(void **)&pConfig);
	// 这pBuild是一个ICaptureGraphBuilder2 Interface.
	if( hr != NOERROR )
	{
		hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,
			m_pBF,IID_IAMStreamConfig,(void **)&pConfig);
		if(hr == NOERROR){
			AM_MEDIA_TYPE* pmt;
			pConfig->GetFormat(&pmt);
			if(pmt->formattype == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER * vvih = (VIDEOINFOHEADER*) pmt->pbFormat;
				vvih->bmiHeader.biHeight = height;  //修改采集视频的高为288
				vvih->bmiHeader.biWidth = width;   //修改采集视频的宽为352
				pConfig->SetFormat(pmt);   //重新设置参数
			}
			DeleteMediaType(pmt); 
		}
		else
		{
      AfxMessageBox(_T("load config filter failed!"));
		}
	}
	return hr;
}

void CCaptureVideo::SetSize( int width, int height, UINT timeFmt)
{
	sgCB.width = width;
	sgCB.height = height;
	sgCB.fmtTime = timeFmt;
}

void CCaptureVideo::SetCallBack( DataCallBack cb, void* user )
{
	sgCB.onData = cb;
	sgCB.usdata = user;
}

STDMETHODIMP CSampleGrabberCB::QueryInterface( REFIID riid, void ** ppv )
{
	if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ){ 
		*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
		return NOERROR;
	} 
	return E_NOINTERFACE;
}

STDMETHODIMP CSampleGrabberCB::BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
{
	if(onData){
		// if(fmtTime) DrawRgbText(pBuffer, width, height, TimeStr(), fmtTime);
		(*onData)(pBuffer, width, height, usdata);
	}
	return 0;
}

STDMETHODIMP CSampleGrabberCB::SampleCB( double SampleTime, IMediaSample * pSample )
{
	return 0;
}

CSampleGrabberCB::CSampleGrabberCB()
{
	width =  352;
	height = 288;
	fmtTime = 0;
	onData = NULL;
	usdata = NULL;
}