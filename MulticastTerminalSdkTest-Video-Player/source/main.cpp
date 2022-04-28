//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：SDK测试拉流播放
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2020-12-26
//**************************************************************************//

#include "SDCommon.h"
#include "SDLog.h"
#include "SDIniFile.h"
#include "SDClient.h"
#include "SDConsoleIFace.h"
#include "SDTerminalSdk.h"

#include <windows.h>										// We need to include windows.h
#include <stdio.h>											// Include stdio.h for the basics

#define SCREEN_WIDTH  1280									//初始窗口大小
#define SCREEN_HEIGHT 720									//

#define SECTION_CONFIG					"Config"
#define KEY_NAME_MULTICAST_IP			"MulticastIp"		//组播IP
#define KEY_NAME_MULTICAST_PORT			"MulticastPort"		//组播端口
#define KEY_NAME_LOCAL_IP				"LocalIp"			//本地IP
#define KEY_NAME_ENABLE_NACK			"EnableNack"		//开启组播NACK
#define KEY_NAME_NACK_PORT				"NackPort"			//NACK端口
#define KEY_NAME_AUDIO_SAMP				"AudioSampleRate"	//音频采集率
#define KEY_NAME_AUDIO_CHANNEL			"AudioChannel"		//音频声道数
#define KEY_NAME_JITTER_BUFF_TIME		"JitterBuffTimeMs"	//接收缓存时间ms
#define KEY_NAME_CAP_DOWN_TS			"CaptureDownStreamTs" //保存下行TS文件
#define KEY_NAME_CAP_DOWN_RAW			"CaptureDownStreamRaw" //保存下行RAW文件


BOOL g_bFullScreen = FALSE;									// Create a boolean and set it to false.  If we choose full screen, set it to TRUE
BOOL g_bSaveDownTs = FALSE;
BOOL g_bSaveDownRaw = FALSE;
BYTE g_byRecvPosition = 0;

///////////////////////////////// CHANGE TO FULL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This changes the screen to FULL SCREEN
/////
///////////////////////////////// CHANGE TO FULL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ChangeToFullScreen(int width, int height)
{
	DEVMODE dmSettings;									// Device Mode variable - Needed to change modes

	memset(&dmSettings,0,sizeof(dmSettings));			// Makes Sure Memory's Cleared

	// Get the current display settings.  This function fills our the settings.
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		// Display error message if we couldn't get display settings
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= width;					// Set the desired Screen Width
	dmSettings.dmPelsHeight	= height;					// Set the desired Screen Height
	dmSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;	// Set the flags saying we're changing the Screen Width and Height
	
	// This function actually changes the screen to full screen
	// CDS_FULLSCREEN Gets Rid Of Start Bar.
	// We always want to get a result from this function to check if we failed
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	// Check if we didn't receive a good return message From the function
	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		// Display the error message and quit the program
		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
}

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		// Create a function prototype for the wndProc.
															// Here is our "main()" equivalent in windows, WinMain().	
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			
{														
    HWND        hwnd;										// Create a variable that holds a handle to our window.
    MSG         msg;										// Create a variable to hold the message information
    WNDCLASSEX  wndclass;									// This variable will hold all the information about the window (The name, icon, cursor, color, menu bar...)
	DWORD dwStyle;

    wndclass.cbSize        = sizeof (wndclass);				// Here we set the size of the wndclass. 
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		// The style we want is Vertical-Redraw and Horizontal-Redraw
    wndclass.lpfnWndProc   = WndProc;						// Here is where we assing our CALLBACK function. (The function to handle the messages)
    wndclass.cbClsExtra    = 0;								// We want zero extra bytes
    wndclass.cbWndExtra    = 0;								// Init this useless thing to 0
    wndclass.hInstance     = hInstance;						// We assign our hInstance to our window.  
    wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO);	// Load a logo for our window
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);	// Load a arrow cursor for our window.
															// Set the background color for the window
    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wndclass.lpszMenuName  = NULL;							// We pass NULL because we don't want a menu.
    wndclass.lpszClassName = "Remote Video Play";			// Create a name that we identify this window class with.
    wndclass.hIconSm       = LoadIcon (NULL, IDI_WINLOGO);	// Create an icon for the top left of the window.

	RegisterClassEx (&wndclass);							// We need to register the wndclass.
															

	if(MessageBox(NULL, "是否全屏播放(Esc退出)？", "Options", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		dwStyle = WS_OVERLAPPEDWINDOW;						// If we don't want full screen, open a simple window
	} 
	else													// If we chose YES
	{			
		g_bFullScreen = TRUE;									// Set our boolean to TRUE, we wanted full screen

															// This is the style that we need our window to have in order to be windowless full screen
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen(SCREEN_WIDTH, SCREEN_HEIGHT);	// This changes our screen to full screen with our desired resolution
	}

															// Here we create the window returning our handle to the window.
    hwnd = CreateWindow ("Remote Video Play",				// window class name 
						 "Remote Video Play",		  		// window's Title    
						 dwStyle,							// window style		 
						 0,									// initial x position
						 0,									// initial y position
						 SCREEN_WIDTH,						// initial x size - Our resolution width
						 SCREEN_HEIGHT,					    // initial y size - Our resolution height	 
						 NULL,								// Pass NULL for the parent window
						 NULL,								// Pass NULL for a menu
						 hInstance,						    // Pass in our hInstance
						 NULL);								// Pass NULL to the wndProc, we don't want to pass it any variables address's.

    ShowWindow (hwnd, iCmdShow);							// Show the window
    UpdateWindow (hwnd);									// Update our window and paint it to the screen
							
							
	//********************************************************************************************	
	//初始化环境
	//********************************************************************************************	
	char strIniFileName[MAX_PATH];
	GetSameExeFile(strIniFileName, ".ini");
	SDLOG_INIT("./log", SD_LOG_LEVEL_INFO, strIniFileName);

	//客户端SDK环境初始化
	SDTerminal_Enviroment_Init("./log", TERMINAL_LOG_OUTPUT_LEVEL_INFO);
	SDVideoRender_Enviroment_Init("./log", VIDEO_RENDER_LOG_LEVEL_INFO);
	SDAudioRender_Enviroment_Init("./log", AUDIO_RENDER_LOG_LEVEL_INFO);

	//********************************************************************************************	
	//读取配置
	//********************************************************************************************	
	CSDIniFile *pIniFile = new CSDIniFile;
	pIniFile->ReadIniFile(strIniFileName);

	//组播IP
	char strMulticastIp[64];
	memset(strMulticastIp, 0x0, sizeof(strMulticastIp));
	pIniFile->SDGetProfileString(SECTION_CONFIG, KEY_NAME_MULTICAST_IP, strMulticastIp, 64);

	//组播端口
	unsigned int unMulticastPort = (unsigned int)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_MULTICAST_PORT, 0);

	//本地IP
	char strLocalIp[64];
	memset(strLocalIp, 0x0, sizeof(strLocalIp));
	pIniFile->SDGetProfileString(SECTION_CONFIG, KEY_NAME_LOCAL_IP, strLocalIp, 64);

	//是否开启NACK
	BOOL bEnableNack = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_ENABLE_NACK, 0) == 0 ? FALSE:TRUE;

	//NACK端口
	unsigned int unNackPort = (unsigned int)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_NACK_PORT, 0);

	//用户类型
	TERMINAL_USER_TYPE eUserType = MULTICAST_TERMINAL_TYPE_RECV;

	//音频渲染相关设置读取
	UINT unSampleRate = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_AUDIO_SAMP, 44100);
	UINT unChannels = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_AUDIO_CHANNEL, 2);

	//接收缓存时间
	UINT unJitterBuffTime = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_JITTER_BUFF_TIME, 200);

	//录制本地视频
	g_bSaveDownTs = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_CAP_DOWN_TS, 0) != 0 ? TRUE:FALSE;
	g_bSaveDownRaw = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_CAP_DOWN_RAW, 0) != 0 ? TRUE:FALSE;

	delete pIniFile;


	//********************************************************************************************	
	//配置有效性简单校验
	//********************************************************************************************	
	if ((strMulticastIp[0] == 0) || (unMulticastPort == 0))
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "Please setup right multicast ip and port!");
		SDLOG_CLOSE();
		SDTerminal_Enviroment_Free();
		SDVideoRender_Enviroment_Free();
		SDAudioRender_Enviroment_Free();
		return FALSE;
	}

	if ((bEnableNack == TRUE) && (unNackPort == 0))
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "Please setup right nack port!");
		SDLOG_CLOSE();
		SDTerminal_Enviroment_Free();
		SDVideoRender_Enviroment_Free();
		SDAudioRender_Enviroment_Free();
		return FALSE;		
	}


	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "MulticastIp:%s  MulticastPort:%d  LocalIp:%s  Nack:%s  NackPort:%d!", 
				strMulticastIp, unMulticastPort, strLocalIp, bEnableNack == TRUE ? "enable":"disable", unNackPort);

	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "Audio:sample rate:%d  channel:%d  JitterBuff:%d!", 
				unSampleRate, unChannels, unJitterBuffTime);


	TerminalEncodeParams tEncodeParams;
	memset(&tEncodeParams, 0x0, sizeof(TerminalEncodeParams));
	//本业务场景下设置为2通道
	tEncodeParams.unAudioChannelNum = unChannels;
	//本业务场景下设置为44100
	tEncodeParams.unAudioSampleRate = unSampleRate;
	//本业务场景下为纯播放端，会忽略编码相关参数，使用如下默认值
	tEncodeParams.unVideoBitrateKbps = 4000;
	tEncodeParams.unVideoFramerate = 25;
	tEncodeParams.unVideoIdrIntervalSecond = 3;
	tEncodeParams.unVideoQualityLevelFrom0To5 = 2;
	tEncodeParams.unVideoTransHeight = 1080;
	tEncodeParams.unVideoTransWidth = 1920;
	tEncodeParams.bEnableHardwareEnc = FALSE;

	TerminalLogonParams tLogonParams;
	memset(&tLogonParams, 0x0, sizeof(TerminalLogonParams));
	//本业务场景下设置为纯接收类型：MULTICAST_TERMINAL_TYPE_RECV
	tLogonParams.unUserType = eUserType;
	//组播IP
	strcpy(tLogonParams.acMulticastIP, strMulticastIp);
	tLogonParams.unMulticastPort = unMulticastPort;
	//本地IP
	strcpy(tLogonParams.acLocalIP, strLocalIp);
	tLogonParams.unNackPort = unNackPort;

	TerminalTransParams tTransParams;
	memset(&tTransParams, 0x0, sizeof(TerminalTransParams));
	//本业务场景下开启组播NACK
	tTransParams.bEnableNack = bEnableNack;
	//本业务场景下开启丢包冻结机制
	tTransParams.bFreezeFrameWhenLost = TRUE;
	//本业务场景下使用如下FEC GROUP设置
	tTransParams.unFecMaxGroupSize = 64;
	tTransParams.unFecMinGroupSize = 28;
	tTransParams.unFecRedunMethod = TERMINAL_FEC_TYPE_FIX;
	tTransParams.unFecRedunRatio = 30;
	//本业务场景下根据实际情况设置JitterBuff时间，比如设置为200ms
	tTransParams.unJitterBuffDelay = unJitterBuffTime;

	CSDClient client(&tEncodeParams, &tLogonParams, &tTransParams, (void*)hwnd);

	client.Start();
	
	while(1)												// Loop forever
	{	
															
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))		// Get any messages the OS has for us
		{
			
			if(msg.message == WM_QUIT)						// If the message says to quit, quit!
				break;

			TranslateMessage(&msg);							// Translate from virtual keys to character messages						
			DispatchMessage(&msg);							// Send the message to the WndProc
		}							
	}

	UnregisterClass("Full Screen App",hInstance);			// We need to unregister the wndclass.
    
	client.Stop();
	
	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "sdk test over success...");
	SDLOG_CLOSE();
	SDTerminal_Enviroment_Free();
	SDVideoRender_Enviroment_Free();
	SDAudioRender_Enviroment_Free();
	return msg.wParam ;										// Quit the program
}
															// Start our window procedure (wndProc).  This handles the messages
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{	
	switch (iMsg)											// This checks what the message is.  Below is some of the message that windows might return.  There of course is HUNDREDS.
    {
		case WM_KEYDOWN:									// If we pressed a key
			switch (wParam) 								// Check what the wParam holds.  The wParam holds which key we pressed.
			{		
				case VK_ESCAPE:								// If we pressed the ESCAPE key.
					if(g_bFullScreen)							// If we went to full screen mode
					{										// Calling the same function with NULL and 0 reset the screen settings and resolution
						ChangeDisplaySettings(NULL, 0);
					}
					SendMessage(hwnd, WM_CLOSE, 0, 0);		// Close the program
					break;
			}
			break;

		case WM_DESTROY:									// This message is sent when the user closes the window.
			PostQuitMessage(0);								// Post a WM_QUIT to our message loop to quit the application
			break;											// Break from the switch statement
	}													

	return DefWindowProc (hwnd, iMsg, wParam, lParam);		// Return generic information to our message loop
}															// End of the WndProc


