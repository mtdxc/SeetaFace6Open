////////////////////////////////////////////////////////////////////////////
//
//
//    Project     : VideoNet version 1.1.
//    Description : Peer to Peer Video Conferencing over the LAN.
//	  Author      :	Nagareshwar Y Talekar ( nsry2002@yahoo.co.in)
//    Date        : 15-6-2004.
//
//
//    File description : 
//    Name    : convert.h
//    Details : Conversion routine from RGB24 to YUV420 & YUV420 to RGB24.
//
/////////////////////////////////////////////////////////////////////////////


#if !defined _CONVERT_H
#define _CONVERT_H

#include <stdio.h>

void RGBtoYUV420P(const BYTE * rgb,
	BYTE * yuv,
	int width,
	int height,
	unsigned rgbIncrement, // 3 RGB24 or 4 RGB32
	BOOL flip);

void YUV420PtoRGB(const BYTE * srcFrameBuffer,
	BYTE * dstFrameBuffer,
	int width,
	int height,
	BOOL flipVertical) ;

void YUV420PtoRGB(
  const BYTE * yplane, int stride_y,
  const BYTE * uplane, int stride_u,
  const BYTE * vplane, int stride_v,
  BYTE * rgb, int width, int height,
  BOOL flipVertical);
#endif