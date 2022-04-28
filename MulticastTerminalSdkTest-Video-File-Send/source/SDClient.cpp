////////////////////////////////////////////////////////////////////////////////
//  File Name: CSDClient.cpp
//
//  Functions:
//      客户端对象类.
//
//  History:
//  Date        Modified_By  Reason  Description	
//
////////////////////////////////////////////////////////////////////////////////

#include "SDClient.h"


extern char		g_acH264FileUrl[1024];
extern int		g_nH264Fps;
extern UINT		g_unRedunRatio;
extern UINT		g_unFecMinGroupSize;
extern UINT		g_unFecMaxGroupSize;
extern BOOL		g_bEnableNack;
extern unsigned short  g_usNackPort;

#define MAX_PAYLOAD_LEN_AUDIO	2048

CSDClient::CSDClient()
{
	m_pSendThread = NULL;
	//客户端SDK环境初始化
	SDTerminalMulticast_Enviroment_Init("./log", MULTICAST_LOG_OUTPUT_LEVEL_INFO);
	m_pTerminal = SDTerminalMulticast_Create();
	m_bClosed = TRUE;
}

CSDClient::~CSDClient()
{
	SDTerminalMulticast_Delete(&m_pTerminal);
	SDTerminalMulticast_Enviroment_Free();
}


BOOL CSDClient::Start(char* strLocalIp, char *strMulticastIp, unsigned short usMulticastPort)
{
	if (g_acH264FileUrl[0])
	{
		m_H264File.Start(g_acH264FileUrl, g_nH264Fps, TRUE);
	}
	else
	{
		SDLOG_PRINTF(m_strTag, SD_LOG_LEVEL_INFO, "File Video test is disable!");	
	}

	m_bClosed = FALSE;

	//设置传输相关参数
	SDTerminalMulticast_SetVideoFrameRateForSmoother(m_pTerminal, g_nH264Fps);

	UINT unJitterBuffDelay = 200;
	SDTerminalMulticast_SetTransParams(m_pTerminal, unJitterBuffDelay, MULTICAST_FEC_FIX_REDUN_TYPE, g_unRedunRatio, g_unFecMinGroupSize, g_unFecMaxGroupSize, 
										g_bEnableNack, g_usNackPort);


	//发送文件时，使用外部提供时间戳（来自文件本身），避免时间戳受读文件开销影响
	SDTerminalMulticast_SetUseInternalTimeStamp(m_pTerminal, FALSE);


	//登录服务器
	int nRet = SDTerminalMulticast_Online(m_pTerminal, MULTICAST_TERMINAL_TYPE_SEND_ONLY, strLocalIp, strMulticastIp, usMulticastPort);
	if (nRet < 0)
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "SDTerminalMulticast_Online Failed return:%d!", nRet);
		m_bClosed = TRUE;
		m_H264File.Stop();
		return FALSE;
	}

	//创建发送线程
	char acThreadName[128];
	sprintf(acThreadName, "SendThread");
	m_pSendThread = new CSDThread(acThreadName);

	if (!m_pSendThread->CreateThread(SendThread, SendThreadClose, (void*)this))
	{
		SDLOG_PRINTF(m_strTag, SD_LOG_LEVEL_ERROR, "CreateThread Failed for Send Thread!");
		m_bClosed = TRUE;
		m_H264File.Stop();
		SDTerminalMulticast_Offline(m_pTerminal);
		delete m_pSendThread;
		m_pSendThread = NULL;
		return FALSE;
	}
	return TRUE;
}


void CSDClient::Close()
{
	m_bClosed = TRUE;
	if (m_pSendThread)
	{
		m_pSendThread->CloseThread();
		delete m_pSendThread;
		m_pSendThread = NULL;
	}

	SDTerminalMulticast_Offline(m_pTerminal);
	m_H264File.Stop();
}


int CSDClient::SendThreadClose(void* pParam1)
{
	CSDClient* pMain = (CSDClient*) pParam1;

	return 0;
}

#define MAX_FRAME_SIZE	(1920*1080)

int CSDClient::SendThread(void *pObject1)
{
	CSDClient* pClient = (CSDClient*)pObject1;

	BYTE* pFrame = (BYTE*)calloc(MAX_FRAME_SIZE, sizeof(BYTE));
	if (pFrame == NULL)
	{
		SDLOG_PRINTF(m_strTag, SD_LOG_LEVEL_ERROR, "malloc frame buff failed!");
		return 0;
	}

	int nFrameInterval = 1000 / g_nH264Fps;
	UINT unFrameNo = 0;
	long nPrevSendTime = 0;

	while(pClient->m_bClosed == FALSE)
	{
		long nStartTime = SD_GetTickCount();


		UINT unDts,unPts;
		UINT unFrameLen = pClient->m_H264File.ReadH264RawFrame(pFrame, MAX_FRAME_SIZE, &unDts, &unPts);
		if (unFrameLen)
		{
			//SDLOG_PRINTF(m_strTag, SD_LOG_LEVEL_INFO, "Read a frme:%d!", unFrameLen);
			SDTerminalMulticast_SendVideoData(pClient->m_pTerminal, pFrame, unFrameLen, unDts, FALSE);
		}

		unFrameNo++;

		long nCurrTime = SD_GetTickCount();
		long nUsedTime = nCurrTime - nStartTime;

		long nSleepTime = nFrameInterval - nUsedTime;
		if (nSleepTime < 0)
		{
			nSleepTime = 0;
		}
		
		//SDLOG_PRINTF(m_strTag, SD_LOG_LEVEL_INFO, "Send a frme:%d!  send interval:%d", unFrameNo, nCurrTime - nPrevSendTime);
		nPrevSendTime = nCurrTime;
		SD_Sleep(nSleepTime);
	}

	if (pFrame)
	{
		free(pFrame);
	}

	return 0;
}



