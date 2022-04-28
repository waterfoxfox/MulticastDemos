////////////////////////////////////////////////////////////////////////////////
//  File Name: CSDClient.h
//
//  Functions:
//      客户端对象类.
//
//  History:
//  Date        Modified_By  Reason  Description	
//
////////////////////////////////////////////////////////////////////////////////

#if !defined(_SDCLIENT_H)
#define _SDCLIENT_H

#include "SDCommon.h"
#include "SDThread.h"
#include "SDTerminalMulticastSdk.h"
#include "SDH264FileParse.h"

#define H264_FILE_FPS_DEF		25


class CSDClient
{
public:
	CSDClient();
	virtual ~CSDClient();

public:
	BOOL Start(char* strLocalIp, char *strMulticastIp, unsigned short usMulticastPort);
	void Close();
	
private:
	static int SendThread(void *pParam1);
	static int SendThreadClose(void *pParam1);


private:
	void*				 m_pTerminal;

	CSDThread*			 m_pSendThread;
	BOOL				 m_bClosed;
	CSDH264FilePase		 m_H264File;	

};

#endif // !defined(_SDCLIENT_H)
