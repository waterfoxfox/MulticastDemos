//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：客户端对象类.
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2018-6-20
//**************************************************************************//

#if !defined(_SDCLIENT_H)
#define _SDCLIENT_H

#include "SDCommon.h"
#include "SDTerminalSdk.h"
#include "SDAudioRenderSdk.h"
#include "SDVideoRenderSdk.h"

class CSDClient
{
public:
	CSDClient(TerminalEncodeParams *ptEncParams, TerminalLogonParams *ptLogonParams, TerminalTransParams *ptTransParams, void* pWindowHandle);
	virtual ~CSDClient();

public:
	BOOL Start();
	void Stop();


private:
	// SDK回调接口实现
	//输出接收并解码后的YUV数据
	static void RemoteVideoYuvData(unsigned char* data, unsigned int unWidth, unsigned int unHeight, unsigned int unPts, void *pObject);

	//输出接收并解码后的PCM数据
	static void RemoteAudioPcmData(unsigned char* data, unsigned int unLen, unsigned int unPts, void *pObject);



private:
	TerminalEncodeParams	m_tEncParams;
	TerminalLogonParams		m_tLogonParams;
	TerminalTransParams		m_tTransParams;

	void*					m_pTerminal;
	void*					m_pAudioRender;
	void*					m_pVideoRender;
	BOOL					m_bClosed;

};

#endif // !defined(_SDCLIENT_H)
