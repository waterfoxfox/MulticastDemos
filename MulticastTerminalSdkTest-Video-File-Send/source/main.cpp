////////////////////////////////////////////////////////////////////////////////
//  File Name: main.cpp
//
//  Functions:
//      SDK测试DEMO.
//
//  History:
//  Date        Modified_By  Reason  Description	
//
////////////////////////////////////////////////////////////////////////////////
#include "SDCommon.h"
#include "SDLog.h"
#include "SDIniFile.h"
#include "SDClient.h"
#include "SDConsoleIFace.h"
#include "SDTerminalMulticastSdk.h"

#if defined(WIN32) && defined(_DEBUG_)
#include <afx.h>
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#endif



#define SECTION_CONFIG					"Config"
#define KEY_NAME_LOCAL_IP				"LocalIp"				//本地IP
#define KEY_NAME_MULTICAST_IP			"MulticastIp"			//组播IP
#define KEY_NAME_MULTICAST_PORT			"MulticastPort"			//组播端口
#define KEY_NAME_NACK_PORT				"NackPort"				//NACK端口

#define KEY_NAME_H264_FILE				"H264FileUrl"			//充当测试数据到H264文件URL	
#define KEY_NAME_H264_FPS				"H264FileFps"			//充当测试数据到H264文件帧率

#define KEY_NAME_FEC_REDUN_RATIO		"FecRedunRatio"			//FEC冗余度比例
#define KEY_NAME_FEC_MIN_GROUP_SIZE		"FecMinGroupSize"		//FEC Min Group size
#define KEY_NAME_FEC_MAX_GROUP_SIZE		"FecMaxGroupSize"		//FEC Max Group size
#define KEY_NAME_FEC_ENABLE_NACK		"FecEnableNack"			//是否支持组播NACK功能



CSDClient	g_Client;
char		g_acH264FileUrl[1024];
int			g_nH264Fps					= H264_FILE_FPS_DEF;

UINT		g_unRedunRatio				= 30;
UINT		g_unFecMinGroupSize			= 16;
UINT		g_unFecMaxGroupSize			= 64;
BOOL		g_bEnableNack				= TRUE;
unsigned short g_usNackPort				= 20000;

BOOL		g_bSaveRecvData				= TRUE;


//主函数
int main(int argc, char *argv[])
{
	int nRet = 0;

	//初始化DEMO日志模块
	char strIniFileName[MAX_PATH];
	GetSameExeFile(strIniFileName, ".ini");
	SDLOG_INIT("./log", SD_LOG_LEVEL_INFO, strIniFileName);


	//读取配置
	CSDIniFile *pIniFile = new CSDIniFile;
	pIniFile->ReadIniFile(strIniFileName);

	//本地IP
	char strLocalIp[64];
	memset(strLocalIp, 0x0, sizeof(strLocalIp));
	pIniFile->SDGetProfileString(SECTION_CONFIG, KEY_NAME_LOCAL_IP, strLocalIp, 64);

	//组播IP
	char strMulticastIp[64];
	memset(strMulticastIp, 0x0, sizeof(strMulticastIp));
	pIniFile->SDGetProfileString(SECTION_CONFIG, KEY_NAME_MULTICAST_IP, strMulticastIp, 64);

	//组播端口
	unsigned short usMulticastPort = (unsigned short)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_MULTICAST_PORT, 0);


	//充当测试数据的H264文件路径
	memset(g_acH264FileUrl, 0x0, sizeof(g_acH264FileUrl));
	pIniFile->SDGetProfileString(SECTION_CONFIG, KEY_NAME_H264_FILE, g_acH264FileUrl, sizeof(g_acH264FileUrl));
	g_nH264Fps = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_H264_FPS, H264_FILE_FPS_DEF);
	if ((g_nH264Fps <= 0) || (g_nH264Fps > 60))
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_WARNING, "Please setup right h264 fps(%d), will use default:%d!", g_nH264Fps, H264_FILE_FPS_DEF);
		g_nH264Fps = H264_FILE_FPS_DEF;
	}

	//传输FEC参数读取
	//FEC冗余比率
	UINT unFecRedundancy = (UINT)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_FEC_REDUN_RATIO, 30);
	if (unFecRedundancy > 100)
	{
		unFecRedundancy = 100;
	}
	g_unRedunRatio = unFecRedundancy;

	//FEC Min Group大小 
	UINT unFecMinGroupSize = (UINT)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_FEC_MIN_GROUP_SIZE, 16);
	if (unFecMinGroupSize > 64)
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_WARNING, "Fec Group min size:%d invalid [8, 64]!", unFecMinGroupSize);
		unFecMinGroupSize = 64;
	}

	if (unFecMinGroupSize < 8)
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_WARNING, "Fec Group min size:%d invalid [8, 64]!", unFecMinGroupSize);
		unFecMinGroupSize = 8;
	}
	g_unFecMinGroupSize = unFecMinGroupSize;


	//FEC Max Group大小 
	UINT unFecMaxGroupSize = (UINT)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_FEC_MAX_GROUP_SIZE, 64);
	if (unFecMaxGroupSize > 64)
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_WARNING, "Fec Group max size:%d invalid [8, 64]!", unFecMaxGroupSize);
		unFecMaxGroupSize = 64;
	}

	if (unFecMaxGroupSize < 8)
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_WARNING, "Fec Group max size:%d invalid [8, 64]!", unFecMaxGroupSize);
		unFecMaxGroupSize = 8;
	}
	g_unFecMaxGroupSize = unFecMaxGroupSize;

	if (g_unFecMaxGroupSize < g_unFecMinGroupSize)
	{
		delete pIniFile;
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "Fec Group min size:%d  max size:%d  invalid!", g_unFecMinGroupSize, g_unFecMaxGroupSize);
		SDLOG_CLOSE();
		return FALSE;
	}

	//NACK设置
	UINT unEnableNack = (UINT)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_FEC_ENABLE_NACK, TRUE);
	if (unEnableNack != 0)
	{
		g_bEnableNack = TRUE;
	}
	else
	{
		g_bEnableNack = FALSE;
	}
	//NACK端口
	g_usNackPort = (unsigned short)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_NACK_PORT, 20000);

	SDLOG_PRINTF("CAVProcess", SD_LOG_LEVEL_INFO, "Fec RedunRatio:%d  MinGroup:%d  MaxGroup:%d  NackEnable:%s! NackPort:%d",  
				g_unRedunRatio, g_unFecMinGroupSize, g_unFecMaxGroupSize, g_bEnableNack == TRUE ? "Y":"N", g_usNackPort);

	delete pIniFile;
	
	//配置有效性校验
	if ((strMulticastIp[0] == 0) || (usMulticastPort == 0))
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "Please setup right multicast ip and port!");
		SDLOG_CLOSE();
		return FALSE;
	}

	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "LocalIp:%s  MulticastIp:%s port:%d!", strLocalIp[0] == 0 ? "ANY":strLocalIp, strMulticastIp, usMulticastPort);
	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "H264 File: %s  Fps:%d!", g_acH264FileUrl, g_nH264Fps);


	//启动测试服务
	if (!g_Client.Start(strLocalIp, strMulticastIp, usMulticastPort))
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "Test start fail...");
		SDLOG_CLOSE();
		return FALSE;
	}
	else
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "sdk test start success...");
	}

	//循环
	CSDConsleIFace::RunCommandLine("sdk_test");

	g_Client.Close();
	

	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "sdk test over success...");
	SDLOG_CLOSE();

	return TRUE;
}

