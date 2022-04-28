//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：windows音频播放类 DLL封装
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2017-11-05
//**************************************************************************//

#ifndef _SD_AUDIO_RENDER_PROCESS_SDK_H_
#define _SD_AUDIO_RENDER_PROCESS_SDK_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined _WIN32 || defined __CYGWIN__
#ifdef DLL_EXPORTS
#ifdef __GNUC__
#define DLLIMPORT_AUDIO_RENDER __attribute__ ((dllexport))
#else
#define DLLIMPORT_AUDIO_RENDER __declspec(dllexport) 
#endif
#else
#ifdef __GNUC__
#define DLLIMPORT_AUDIO_RENDER 
#else
#define DLLIMPORT_AUDIO_RENDER 
#endif
#endif
#else
#if __GNUC__ >= 4
#define DLLIMPORT_AUDIO_RENDER __attribute__ ((visibility ("default")))
#else
#define DLLIMPORT_AUDIO_RENDER
#endif
#endif

#ifdef WIN32
	typedef signed __int64       int64_t;
#endif

#ifdef __APPLE__
#ifndef OBJC_BOOL_DEFINED
	typedef int BOOL;
#endif 
#else
#ifndef BOOL
	typedef int BOOL;
#endif
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


//枚举音频渲染设备的最大数量
#define MAX_SUPPORT_RENDER_DEVICES_NUM				8
//枚举音频渲染设备名称的最大长度																															
#define MAX_SUPPORT_RENDER_DEVICES_NAME_LEN			256

//日志输出的级别
typedef enum AUDIO_RENDER_LOG_OUTPUT_LEVEL
{
	AUDIO_RENDER_LOG_LEVEL_DEBUG = 1,
	AUDIO_RENDER_LOG_LEVEL_INFO,
	AUDIO_RENDER_LOG_LEVEL_WARNING,
	AUDIO_RENDER_LOG_LEVEL_ERROR,
	AUDIO_RENDER_LOG_LEVEL_ALARM,
	AUDIO_RENDER_LOG_LEVEL_FATAL,
	AUDIO_RENDER_LOG_LEVEL_NONE
} AUDIO_RENDER_LOG_OUTPUT_LEVEL;



//////////////////////////////////////////////////////////////////////////
// 音频渲染封装接口


/***
* 枚举当前系统可用的音频渲染设备(获取设备ID和设备名称)
* @param: pnDeviceIds 设备ID存放列表，系统默认音频渲染设备其ID为-1
* @param: pDeviceNames 设备名称存放列表
* @param: nMaxDeviceNum 最大枚举的设备数量
* @param: nMaxDeviceNameLen 最大设备名长度
* @return: 返回设备数量
*/
DLLIMPORT_AUDIO_RENDER int SDAudioRender_GetDeviceList(int *pnDeviceIds, char(*pDeviceNames)[MAX_SUPPORT_RENDER_DEVICES_NAME_LEN], int nMaxDeviceNum, int nMaxDeviceNameLen);


/***
* 环境初始化，系统只需调用一次，主要用于日志模块的初始化
* @param: outputPath表示日志存放路径，支持相对路径和绝对路径，若目录不存在将自动创建
* @param: outputLevel表示日志输出的级别，只有等于或者高于该级别的日志输出到文件，取值范围参考AUDIO_CAP_LOG_OUTPUT_LEVEL
* @return: 
*/
DLLIMPORT_AUDIO_RENDER void  SDAudioRender_Enviroment_Init(const char* outputPath, int outputLevel);

DLLIMPORT_AUDIO_RENDER void  SDAudioRender_Enviroment_Free();


/***
* 创建SDAudioRender对象
* @return: 返回模块指针，为NULL则失败
*/
DLLIMPORT_AUDIO_RENDER void*  SDAudioRender_New();


/***
* 销毁SDAudioRender，使用者应该做好与其他API之间的互斥保护
* @param ppAudioRender: 模块指针指针
* @return:
*/
DLLIMPORT_AUDIO_RENDER void  SDAudioRender_Delete(void** ppAudioRender);



/***
* 开始启动SDAudioRender
* @param pAudioRender: 模块指针
* @param nRenderDeviceID: 音频渲染设备ID，-1为默认设备
* @param nSampleRate: 输入待处理数据采样率
* @param nChannelNum: 输入待处理数据声道数
* @return: TRUE成功，FALSE失败
*/
DLLIMPORT_AUDIO_RENDER BOOL  SDAudioRender_Start(void* pAudioRender, int nRenderDeviceID, int nSampleRate, int nChannelNum);



/***
* 停止SDAudioRender
* @param pAudioRender: 模块指针
* @return:
*/
DLLIMPORT_AUDIO_RENDER void  SDAudioRender_Stop(void* pAudioRender);


/***
* 播放数据，要求采样率、声道数与创建时一致
* @param pAudioRender: 模块指针
* @param pucData: 待播放数据
* @param nLen: 数据大小字节数
* @return: 实际输出数据大小字节数
*/
DLLIMPORT_AUDIO_RENDER int  SDAudioRender_Play(void* pAudioRender, unsigned char *pucData, int nLen);



/***
* 切换音频渲染设备
* @param pAudioRender: 模块指针
* @param nRenderDeviceId: 需要切换到渲染设备ID
* @return: TRUE成功，FALSE失败
*/
DLLIMPORT_AUDIO_RENDER BOOL  SDAudioRender_ChangeRenderDev(void* pAudioRender, int nRenderDeviceId);



#ifdef __cplusplus
}
#endif

#endif // _SD_AUDIO_RENDER_PROCESS_SDK_H_
