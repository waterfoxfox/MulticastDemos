//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：Windows音频采集对外DLL封装接口（支持扬声器、麦克风、二者混音的采集，支持resample到
//*			  用户指定的采样率、声道数）
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2020-8-6
//**************************************************************************//

#ifndef _SD_AUDIO_CAPTURE_SDK_H_
#define _SD_AUDIO_CAPTURE_SDK_H_

#ifdef __cplusplus
extern "C" {
#endif


#if defined _WIN32 || defined __CYGWIN__
#ifdef DLL_EXPORTS
#ifdef __GNUC__
#define DLLIMPORT_AUDIO_CAP __attribute__ ((dllexport))
#else
#define DLLIMPORT_AUDIO_CAP __declspec(dllexport) 
#endif
#else
#ifdef __GNUC__
#define DLLIMPORT_AUDIO_CAP 
#else
#define DLLIMPORT_AUDIO_CAP 
#endif
#endif
#else
#if __GNUC__ >= 4
#define DLLIMPORT_AUDIO_CAP __attribute__ ((visibility ("default")))
#else
#define DLLIMPORT_AUDIO_CAP
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


#define MAX_OUTPUT_DATA_SIZE		(10000)

//日志输出的级别
typedef enum AUDIO_CAP_LOG_OUTPUT_LEVEL
{
	AUDIO_CAP_LOG_OUTPUT_LEVEL_DEBUG = 1,
	AUDIO_CAP_LOG_OUTPUT_LEVEL_INFO,
	AUDIO_CAP_LOG_OUTPUT_LEVEL_WARNING,
	AUDIO_CAP_LOG_OUTPUT_LEVEL_ERROR,
	AUDIO_CAP_LOG_OUTPUT_LEVEL_ALARM,
	AUDIO_CAP_LOG_OUTPUT_LEVEL_FATAL,
	AUDIO_CAP_LOG_OUTPUT_LEVEL_NONE
} AUDIO_CAP_LOG_OUTPUT_LEVEL;

typedef enum
{
	//当前系统默认MIC的采集
	AUDIO_SRC_MIC		= 0, 
	//当前系统默认扬声器的采集
	AUDIO_SRC_RENDER    = 1, 
	//MIC和扬声器的混音
	AUDIO_SRC_MIX		= 2, 
} AUDIO_CAPTURE_SRC_TYPE;


typedef enum AUDIO_CAPTURE_STATUS
{
	//初始状态
	AUDIO_CAPTURE_STATUS_INIT = 0,
	//采集启动失败
	AUDIO_CAPTURE_STATUS_FAILED = 1,
	//正常采集到数据
	AUDIO_CAPTURE_STATUS_NOMAL = 2,
	//未采集到有效数据
	AUDIO_CAPTURE_STATUS_NODATA = 3
} AUDIO_CAPTURE_STATUS;


//采集数据输出回调
typedef void (*OutputCapturedData)(unsigned char *data, unsigned int len, void *pObject);

//采集状态输出回调
typedef void (*OutputCapturedStatus)(int eStatus, void *pObject);


//////////////////////////////////////////////////////////////////////////
// 音频采集封装接口

/***
* 环境初始化，系统只需调用一次，主要用于日志模块的初始化
* @param: outputPath表示日志存放路径，支持相对路径和绝对路径，若目录不存在将自动创建
* @param: outputLevel表示日志输出的级别，只有等于或者高于该级别的日志输出到文件，取值范围参考AUDIO_CAP_LOG_OUTPUT_LEVEL
* @return: 
*/
DLLIMPORT_AUDIO_CAP void  SDAudioCapture_Enviroment_Init(const char* outputPath, int outputLevel);

DLLIMPORT_AUDIO_CAP void  SDAudioCapture_Enviroment_Free();


/***
* 创建SDAudioCapture，仅支持16bit
* @return: 返回模块指针，为NULL则失败
*/
DLLIMPORT_AUDIO_CAP void*  SDAudioCapture_New();


/***
* 销毁SDAudioCapture，使用者应该做好与其他API之间的互斥保护
* @param ppAudioCapture: 模块指针指针
* @return:
*/
DLLIMPORT_AUDIO_CAP void  SDAudioCapture_Delete(void** ppAudioCapture);


/***
* 开始采集
* @param pAudioCapture: 模块指针
* @param nSampleRate: 采集输出的采样率
* @param nChannelNum: 采集输出的声道数
* @param nBitsPerSample: 采集输出的样点比特数
* @param nOutputByteNum: 采集单次输出的样点字节数，不超过MAX_OUTPUT_DATA_SIZE
* @param eCapType: 采集的类型，麦克风或扬声器
* @param pfOutputCaptureData: 采集音频数据输出回调接口
* @param pfOutputCapStatus: 采集状态输出回调接口
* @param pObject: 上述输出回调接口的透传指针，将通过回调函数形参方式透传外层
* @return:
*/
DLLIMPORT_AUDIO_CAP BOOL  SDAudioCapture_Start(void* pAudioCapture, unsigned int nSampleRate, unsigned int nChannelNum, unsigned int nBitsPerSample, unsigned int nOutputByteNum, 
									AUDIO_CAPTURE_SRC_TYPE eCapType, OutputCapturedData pfOutputCaptureData, OutputCapturedStatus pfOutputCapStatus, void* pObject);


/***
* 停止采集
* @param pAudioCapture: 模块指针
* @return:
*/
DLLIMPORT_AUDIO_CAP BOOL  SDAudioCapture_Stop(void* pAudioCapture);


#ifdef __cplusplus
}
#endif

#endif // _SD_AUDIO_CAPTURE_SDK_H_
