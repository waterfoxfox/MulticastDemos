//***************************************************************************//
//* ��Ȩ����  www.mediapro.cc
//*
//* ����ժҪ��SDK������������
//*	
//* ��ǰ�汾��V1.0		
//* ��    �ߣ�mediapro
//* ������ڣ�2020-12-26
//**************************************************************************//

#include "SDCommon.h"
#include "SDLog.h"
#include "SDIniFile.h"
#include "SDClient.h"
#include "SDConsoleIFace.h"
#include "SDTerminalSdk.h"

#include <windows.h>										// We need to include windows.h
#include <stdio.h>											// Include stdio.h for the basics

#define SCREEN_WIDTH  1280									//��ʼ���ڴ�С
#define SCREEN_HEIGHT 720									//

#define SECTION_CONFIG					"Config"
#define KEY_NAME_MULTICAST_IP			"MulticastIp"		//�鲥IP
#define KEY_NAME_MULTICAST_PORT			"MulticastPort"		//�鲥�˿�
#define KEY_NAME_LOCAL_IP				"LocalIp"			//����IP
#define KEY_NAME_ENABLE_NACK			"EnableNack"		//�����鲥NACK
#define KEY_NAME_NACK_PORT				"NackPort"			//NACK�˿�
#define KEY_NAME_AUDIO_SAMP				"AudioSampleRate"	//��Ƶ�ɼ���
#define KEY_NAME_AUDIO_CHANNEL			"AudioChannel"		//��Ƶ������
#define KEY_NAME_JITTER_BUFF_TIME		"JitterBuffTimeMs"	//���ջ���ʱ��ms
#define KEY_NAME_CAP_DOWN_TS			"CaptureDownStreamTs" //��������TS�ļ�
#define KEY_NAME_CAP_DOWN_RAW			"CaptureDownStreamRaw" //��������RAW�ļ�


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
															

	if(MessageBox(NULL, "�Ƿ�ȫ������(Esc�˳�)��", "Options", MB_YESNO | MB_ICONQUESTION) == IDNO)
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
	//��ʼ������
	//********************************************************************************************	
	char strIniFileName[MAX_PATH];
	GetSameExeFile(strIniFileName, ".ini");
	SDLOG_INIT("./log", SD_LOG_LEVEL_INFO, strIniFileName);

	//�ͻ���SDK������ʼ��
	SDTerminal_Enviroment_Init("./log", TERMINAL_LOG_OUTPUT_LEVEL_INFO);
	SDVideoRender_Enviroment_Init("./log", VIDEO_RENDER_LOG_LEVEL_INFO);
	SDAudioRender_Enviroment_Init("./log", AUDIO_RENDER_LOG_LEVEL_INFO);

	//********************************************************************************************	
	//��ȡ����
	//********************************************************************************************	
	CSDIniFile *pIniFile = new CSDIniFile;
	pIniFile->ReadIniFile(strIniFileName);

	//�鲥IP
	char strMulticastIp[64];
	memset(strMulticastIp, 0x0, sizeof(strMulticastIp));
	pIniFile->SDGetProfileString(SECTION_CONFIG, KEY_NAME_MULTICAST_IP, strMulticastIp, 64);

	//�鲥�˿�
	unsigned int unMulticastPort = (unsigned int)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_MULTICAST_PORT, 0);

	//����IP
	char strLocalIp[64];
	memset(strLocalIp, 0x0, sizeof(strLocalIp));
	pIniFile->SDGetProfileString(SECTION_CONFIG, KEY_NAME_LOCAL_IP, strLocalIp, 64);

	//�Ƿ���NACK
	BOOL bEnableNack = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_ENABLE_NACK, 0) == 0 ? FALSE:TRUE;

	//NACK�˿�
	unsigned int unNackPort = (unsigned int)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_NACK_PORT, 0);

	//�û�����
	TERMINAL_USER_TYPE eUserType = MULTICAST_TERMINAL_TYPE_RECV;

	//��Ƶ��Ⱦ������ö�ȡ
	UINT unSampleRate = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_AUDIO_SAMP, 44100);
	UINT unChannels = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_AUDIO_CHANNEL, 2);

	//���ջ���ʱ��
	UINT unJitterBuffTime = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_JITTER_BUFF_TIME, 200);

	//¼�Ʊ�����Ƶ
	g_bSaveDownTs = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_CAP_DOWN_TS, 0) != 0 ? TRUE:FALSE;
	g_bSaveDownRaw = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_CAP_DOWN_RAW, 0) != 0 ? TRUE:FALSE;

	delete pIniFile;


	//********************************************************************************************	
	//������Ч�Լ�У��
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
	//��ҵ�񳡾�������Ϊ2ͨ��
	tEncodeParams.unAudioChannelNum = unChannels;
	//��ҵ�񳡾�������Ϊ44100
	tEncodeParams.unAudioSampleRate = unSampleRate;
	//��ҵ�񳡾���Ϊ�����Ŷˣ�����Ա�����ز�����ʹ������Ĭ��ֵ
	tEncodeParams.unVideoBitrateKbps = 4000;
	tEncodeParams.unVideoFramerate = 25;
	tEncodeParams.unVideoIdrIntervalSecond = 3;
	tEncodeParams.unVideoQualityLevelFrom0To5 = 2;
	tEncodeParams.unVideoTransHeight = 1080;
	tEncodeParams.unVideoTransWidth = 1920;
	tEncodeParams.bEnableHardwareEnc = FALSE;

	TerminalLogonParams tLogonParams;
	memset(&tLogonParams, 0x0, sizeof(TerminalLogonParams));
	//��ҵ�񳡾�������Ϊ���������ͣ�MULTICAST_TERMINAL_TYPE_RECV
	tLogonParams.unUserType = eUserType;
	//�鲥IP
	strcpy(tLogonParams.acMulticastIP, strMulticastIp);
	tLogonParams.unMulticastPort = unMulticastPort;
	//����IP
	strcpy(tLogonParams.acLocalIP, strLocalIp);
	tLogonParams.unNackPort = unNackPort;

	TerminalTransParams tTransParams;
	memset(&tTransParams, 0x0, sizeof(TerminalTransParams));
	//��ҵ�񳡾��¿����鲥NACK
	tTransParams.bEnableNack = bEnableNack;
	//��ҵ�񳡾��¿��������������
	tTransParams.bFreezeFrameWhenLost = TRUE;
	//��ҵ�񳡾���ʹ������FEC GROUP����
	tTransParams.unFecMaxGroupSize = 64;
	tTransParams.unFecMinGroupSize = 28;
	tTransParams.unFecRedunMethod = TERMINAL_FEC_TYPE_FIX;
	tTransParams.unFecRedunRatio = 30;
	//��ҵ�񳡾��¸���ʵ���������JitterBuffʱ�䣬��������Ϊ200ms
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


