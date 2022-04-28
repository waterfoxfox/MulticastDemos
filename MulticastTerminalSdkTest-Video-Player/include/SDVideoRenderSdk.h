//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：基于webrtc的视频渲染封装
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2021-09-20
//**************************************************************************//

#ifndef __SD_VIDEO_RENDER_SDK_H_
#define __SD_VIDEO_RENDER_SDK_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined _WIN32 || defined __CYGWIN__
  #ifdef DLL_EXPORTS
    #ifdef __GNUC__
      #define DLLIMPORT_VIDEO_RENDER __attribute__ ((dllexport))
    #else
      #define DLLIMPORT_VIDEO_RENDER __declspec(dllexport) 
    #endif
  #else
    #ifdef __GNUC__
      #define DLLIMPORT_VIDEO_RENDER 
    #else
      #define DLLIMPORT_VIDEO_RENDER 
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define DLLIMPORT_VIDEO_RENDER __attribute__ ((visibility ("default")))
  #else
    #define DLLIMPORT_VIDEO_RENDER
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



//单个窗口内最大允许的视频流数目
#define MAX_SUPPORT_STREAM_NUM_PER_WINDOW		24



//日志输出的级别
typedef enum VIDEO_RENDER_LOG_OUTPUT_LEVEL
{
    VIDEO_RENDER_LOG_LEVEL_DEBUG = 1,
    VIDEO_RENDER_LOG_LEVEL_INFO,
    VIDEO_RENDER_LOG_LEVEL_WARNING,
    VIDEO_RENDER_LOG_LEVEL_ERROR,
    VIDEO_RENDER_LOG_LEVEL_ALARM,
    VIDEO_RENDER_LOG_LEVEL_FATAL,
    VIDEO_RENDER_LOG_LEVEL_NONE
} VIDEO_RENDER_LOG_OUTPUT_LEVEL;




//////////////////////////////////////////////////////////////////////////
// 视频渲染封装接口

/***
* 环境初始化，系统只需调用一次，主要用于日志模块的初始化
* @param: outputPath表示日志存放路径，支持相对路径和绝对路径，若目录不存在将自动创建
* @param: outputLevel表示日志输出的级别，只有等于或者高于该级别的日志输出到文件，取值范围参考TERMINAL_LOG_OUTPUT_LEVEL
* @return:
*/
DLLIMPORT_VIDEO_RENDER void  SDVideoRender_Enviroment_Init(const char* outputPath, int outputLevel);

DLLIMPORT_VIDEO_RENDER void  SDVideoRender_Enviroment_Free();


/***
* 创建SDVideoRender对象
* @param: pWindowHandle 待渲染的窗口句柄
* @param: bKeepRatio 是否通过填黑边方式保持宽高比渲染
* @return: 返回模块指针，为NULL则失败
*/
DLLIMPORT_VIDEO_RENDER void*  SDVideoRender_New(void* pWindowHandle, BOOL bKeepRatio);


/***
* 销毁SDVideoRender对象，使用者应该做好与其他API之间的互斥保护
* @param ppVideoRender: 模块指针指针
* @return:
*/
DLLIMPORT_VIDEO_RENDER void  SDVideoRender_Delete(void** ppVideoRender);



/***
* 添加待渲染的视频流
* @param pVideoRender: 模块指针
* @param: unStreamId 流ID，最大不超过MAX_SUPPORT_STREAM_NUM_PER_WINDOW
* @param: unZOrder 流的相对渲染层级，0为最上层
* @param: fLeft 流在画面中的相对位置，左，范围[0~1]
* @param: fTop 流在画面中的相对位置，上，范围[0~1]
* @param: fRight 流在画面中的相对位置，右，范围[0~1]
* @param: fBottom 流在画面中的相对位置，下，范围[0~1]
* @return: TRUE成功，FALSE失败
*/
DLLIMPORT_VIDEO_RENDER BOOL SDVideoRender_AddStream(void* pVideoRender, unsigned int unStreamId, unsigned int unZOrder, float fLeft, float fTop, float fRight, float fBottom);



/***
* 启动视频渲染器
* @param pVideoRender: 模块指针
* @return: TRUE成功，FALSE失败
*/
DLLIMPORT_VIDEO_RENDER BOOL SDVideoRender_StartRender(void* pVideoRender);



/***
* 停止视频渲染器
* @param pVideoRender: 模块指针
* @return:
*/
DLLIMPORT_VIDEO_RENDER void SDVideoRender_StopRender(void* pVideoRender);



/***
* 在指定流上渲染一帧画面
* @param pVideoRender: 模块指针
* @param: unStreamId 目标流ID，最大不超过MAX_SUPPORT_STREAM_NUM
* @param: data 待渲染的YUV420数据
* @param: unWidth YUV420数据宽
* @param: unHeight YUV420数据高
* @return: TRUE成功，FALSE失败
*/
DLLIMPORT_VIDEO_RENDER BOOL SDVideoRender_RenderYuv420Frame(void* pVideoRender, unsigned int unStreamId, unsigned char* data, unsigned int unWidth, unsigned int unHeight);


#ifdef __cplusplus
}
#endif

#endif // _SD_DESKTOP_CAPTURE_SDK_H_
