//***************************************************************************//
//* ��Ȩ����  www.mediapro.cc
//*
//* ����ժҪ���ͻ��˶�����.
//*	
//* ��ǰ�汾��V1.0		
//* ��    �ߣ�mediapro
//* ������ڣ�2018-6-20
//**************************************************************************//
#include "SDClient.h"
#include <time.h>

extern BOOL g_bSaveDownTs;
extern BOOL g_bSaveDownRaw;
extern BYTE g_byRecvPosition;

CSDClient::CSDClient(TerminalEncodeParams *ptEncParams, TerminalLogonParams *ptLogonParams, TerminalTransParams *ptTransParams, void* pWindowHandle)
{
	m_bClosed = TRUE;

	memcpy(&m_tEncParams, ptEncParams, sizeof(TerminalEncodeParams));
	memcpy(&m_tLogonParams, ptLogonParams, sizeof(TerminalLogonParams));
	memcpy(&m_tTransParams, ptTransParams, sizeof(TerminalTransParams));

	//SDT-SDK
	m_pTerminal = SDTerminal_New(&m_tEncParams, RemoteVideoYuvData, RemoteAudioPcmData, this);

	//��Ƶ��Ⱦ
	m_pAudioRender = SDAudioRender_New();

	//��Ƶ��Ⱦ
	m_pVideoRender = SDVideoRender_New(pWindowHandle, TRUE);
}

CSDClient::~CSDClient()
{
	SDTerminal_Delete(&m_pTerminal);
	SDAudioRender_Delete(&m_pAudioRender);
	SDVideoRender_Delete(&m_pVideoRender);
}


BOOL CSDClient::Start()
{
	if (m_pTerminal == NULL)
	{
		return FALSE;
	}

	//��ʼ��Ƶ��Ⱦ
	BOOL bRet = SDAudioRender_Start(m_pAudioRender, -1, m_tEncParams.unAudioSampleRate, m_tEncParams.unAudioChannelNum);
	if (bRet == FALSE)
	{
		return FALSE;
	}

	//��ʼ��Ƶ��Ⱦ
	//���ŵ�0��λ������
	unsigned int unStreamId = 0;
	bRet = SDVideoRender_AddStream(m_pVideoRender, 0, 0, 0.0, 0.0, 1.0, 1.0);
	if (bRet == FALSE)
	{
		SDAudioRender_Stop(m_pAudioRender);
		return FALSE;
	}
	SDVideoRender_StartRender(m_pVideoRender);
	if (bRet == FALSE)
	{
		SDAudioRender_Stop(m_pAudioRender);
		return FALSE;
	}


	//��������Ƶ�������Դ����¼������
	bRet = SDTerminal_Online(m_pTerminal, &m_tLogonParams, &m_tTransParams);
	if (bRet == FALSE)
	{
		SDAudioRender_Stop(m_pAudioRender);
		SDVideoRender_StopRender(m_pVideoRender);
		return FALSE;
	}

	if (g_bSaveDownTs == TRUE)
	{

		//¼�Ƶ�����ts�ļ��У���ǰ�����������
		char acSaveFileDir[512];
		GetExeFilePath(acSaveFileDir);
		strcat(acSaveFileDir, "ts/");
		SD_CreateDir(acSaveFileDir);

		UINT tv_sec = 0;
		UINT tv_usec = 0;
		SD_GetCurrentTime(tv_sec, tv_usec);

		struct tm *p;
		time_t timeData = tv_sec;
		p = localtime(&timeData);
		p->tm_year = p->tm_year + 1900;
		p->tm_mon = p->tm_mon + 1;

		char acSaveFileName[128];
		sprintf(acSaveFileName, "%04d-%02d-%02d_%02d-%02d-%02d.ts", p->tm_year, p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
		strcat(acSaveFileDir, acSaveFileName);

		SDTerminal_StartCapDownStream(m_pTerminal, acSaveFileDir);
	}

	if (g_bSaveDownRaw == TRUE)
	{

		//¼�Ƶ�����ts�ļ��У���ǰ�����������
		char acSaveFileDir[512];
		GetExeFilePath(acSaveFileDir);
		strcat(acSaveFileDir, "raw/");
		SD_CreateDir(acSaveFileDir);

		UINT tv_sec = 0;
		UINT tv_usec = 0;
		SD_GetCurrentTime(tv_sec, tv_usec);

		struct tm *p;
		time_t timeData = tv_sec;
		p = localtime(&timeData);
		p->tm_year = p->tm_year + 1900;
		p->tm_mon = p->tm_mon + 1;

		char acSaveFileName[128];
		sprintf(acSaveFileName, "%04d-%02d-%02d_%02d-%02d-%02d.h264", p->tm_year, p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
		strcat(acSaveFileDir, acSaveFileName);

		SDTerminal_StartCapDownRawStream(m_pTerminal, acSaveFileDir);
	}

	m_bClosed = FALSE;
	return TRUE;
}


void CSDClient::Stop()
{
	m_bClosed = TRUE;

	if (g_bSaveDownTs == TRUE)
	{
		SDTerminal_StopCapDownStream(m_pTerminal);
	}

	if (g_bSaveDownRaw == TRUE)
	{
		SDTerminal_StopCapDownRawStream(m_pTerminal);
	}

	if (m_pTerminal)
	{
		SDTerminal_Offline(m_pTerminal);
	}

	if (m_pAudioRender)
	{
		SDAudioRender_Stop(m_pAudioRender);
	}

	if (m_pVideoRender)
	{
		SDVideoRender_StopRender(m_pVideoRender);
	}
}


// SDK�ص��ӿ�ʵ��
//������ղ�������YUV����
void CSDClient::RemoteVideoYuvData(unsigned char* data, unsigned int unWidth, unsigned int unHeight, unsigned int unPts, void *pObject)
{
	CSDClient* pClient = (CSDClient*)pObject;

	SDVideoRender_RenderYuv420Frame(pClient->m_pVideoRender, 0, data, unWidth, unHeight);

}

//������ղ�������PCM����
void CSDClient::RemoteAudioPcmData(unsigned char* data, unsigned int unLen, unsigned int unPts, void *pObject)
{
	CSDClient* pClient = (CSDClient*)pObject;
	SDAudioRender_Play(pClient->m_pAudioRender, data, (int)unLen);
}




