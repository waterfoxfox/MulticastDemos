//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：windows 组播 SDT SDK，内置音视频编解码
//*			  
//*
//*	说    明：
//*			  
//*				
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2020-8-6
//**************************************************************************//

#ifndef _SD_MEDIAPRO_TERMINAL_SDK_H_
#define _SD_MEDIAPRO_TERMINAL_SDK_H_


#ifdef __cplusplus
extern "C" {
#endif

#if defined _WIN32 || defined __CYGWIN__
#ifdef DLL_EXPORTS
#ifdef __GNUC__
#define DLLIMPORT_MULTICAST_SDK __attribute__ ((dllexport))
#else
#define DLLIMPORT_MULTICAST_SDK __declspec(dllexport) 
#endif
#else
#ifdef __GNUC__
#define DLLIMPORT_MULTICAST_SDK 
#else
#define DLLIMPORT_MULTICAST_SDK 
#endif
#endif
#else
#if __GNUC__ >= 4
#define DLLIMPORT_MULTICAST_SDK __attribute__ ((visibility ("default")))
#else
#define DLLIMPORT_MULTICAST_SDK
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



//FEC GROUP的最小值、最大值
#define TERMINAL_FEC_GROUP_MIN			8
#define TERMINAL_FEC_GROUP_MAX			72


//FEC 固定冗余度时可选冗余度的最小值、最大值
#define TERMINAL_FEC_REDUN_RATIO_MIN	0
#define TERMINAL_FEC_REDUN_RATIO_MAX	100	

//最大Jitter Buff缓存时长,ms
#define TERMINAL_JITTER_BUFF_MAX		600

//编码码率最小值、最大值,kbps
#define TERMINAL_ENC_BITRATE_MIN		50
#define TERMINAL_ENC_BITRATE_MAX		10000

//编码帧率最小值、最大值
#define TERMINAL_ENC_FRAMERATE_MIN		1
#define TERMINAL_ENC_FRAMERATE_MAX		30

//编码分辨率最小值、最大值
#define	TERMINAL_ENC_WIDTH_MIN		    176
#define	TERMINAL_ENC_HEIGHT_MIN			144
#define	TERMINAL_ENC_WIDTH_MAX		    1920
#define	TERMINAL_ENC_HEIGHT_MAX			1080

//IDR间隔的最小值、最大值
#define	TERMINAL_ENC_IDR_INTERVAL_MIN	1
#define	TERMINAL_ENC_IDR_INTERVAL_MAX	10

//音频采样率支持的最小值、最大值
#define	TERMINAL_SAMPLE_RATE_MIN		8000
#define	TERMINAL_SAMPLE_RATE_MAX		48000

//音频声道数支持的最小值、最大值
#define	TERMINAL_AUDIO_CHANNELS_MIN		1
#define	TERMINAL_AUDIO_CHANNELS_MAX		2



//日志输出的级别
typedef enum TERMINAL_LOG_OUTPUT_LEVEL
{
	TERMINAL_LOG_OUTPUT_LEVEL_DEBUG		= 1,
	TERMINAL_LOG_OUTPUT_LEVEL_INFO		= 2,
	TERMINAL_LOG_OUTPUT_LEVEL_WARNING	= 3,	
	TERMINAL_LOG_OUTPUT_LEVEL_ERROR		= 4,
	TERMINAL_LOG_OUTPUT_LEVEL_ALARM		= 5,
	TERMINAL_LOG_OUTPUT_LEVEL_FATAL		= 6,
	TERMINAL_LOG_OUTPUT_LEVEL_NONE		= 7
} TERMINAL_LOG_OUTPUT_LEVEL;

//用户类型
typedef enum TERMINAL_USER_TYPE
{
	MULTICAST_TERMINAL_TYPE_RECV = 0,
	MULTICAST_TERMINAL_TYPE_SEND = 1
} TERMINAL_USER_TYPE;


//FEC 冗余方法
typedef enum TERMINAL_FEC_METHOD_TYPE
{
	//自动冗余度(目前组播不支持)
	TERMINAL_FEC_TYPE_AUTO				= 0,
	//固定冗余度
	TERMINAL_FEC_TYPE_FIX				= 1,
} TERMINAL_FEC_METHOD_TYPE;


//视频编码图像质量档次，对应X264 X265 preset
typedef enum TERMINAL_ENCODE_QUALITY
{
	//"ultrafast" - baseline
	TERMINAL_ENC_QUALITY_0				= 0,
	//"superfast" - baseline
	TERMINAL_ENC_QUALITY_1				= 1,
	//"veryfast"  - baseline
	TERMINAL_ENC_QUALITY_2				= 2,
	//"faster"    - baseline
	TERMINAL_ENC_QUALITY_3				= 3,
	//"fast"      - baseline
	TERMINAL_ENC_QUALITY_4				= 4,
	//"medium" - high profile
	TERMINAL_ENC_QUALITY_5				= 5,
} TERMINAL_ENCODE_QUALITY;


//输入视频格式
typedef enum TERMINAL_VIDEO_INPUT_FORMAT
{
	TERMINAL_VIDEO_FORMAT_INVALID = -1, 
	TERMINAL_VIDEO_FORMAT_YUV420P,       
	TERMINAL_VIDEO_FORMAT_NV12,             
	TERMINAL_VIDEO_FORMAT_NV21,        
	TERMINAL_VIDEO_FORMAT_RGB24,
	TERMINAL_VIDEO_FORMAT_RGBA32
} TERMINAL_VIDEO_INPUT_FORMAT;



//音视频编码相关参数
typedef struct TerminalEncodeParams
{
	//编码时采用的压缩标准：0-H264  1-H265
	unsigned int unVideoCodecType;
	//编码宽度
	unsigned int unVideoTransWidth;
	//编码高度
	unsigned int unVideoTransHeight;
	//编码帧率
	unsigned int unVideoFramerate;
	//编码码率
	unsigned int unVideoBitrateKbps;
	//编码质量，越大计算复杂度越高，画质相对更好，取值见：TERMINAL_ENCODE_QUALITY
	unsigned int unVideoQualityLevelFrom0To5;
	//编码IDR帧间隔秒数
	unsigned int unVideoIdrIntervalSecond;
	//是否启用硬编码，硬编码不支持则自动切回软编码
	unsigned int bEnableHardwareEnc;
	//音频编码发送、解码输出的采样率
	unsigned int unAudioSampleRate;
	//音频编码发送、解码输出的声道数
	unsigned int unAudioChannelNum;
}TerminalEncodeParams;


//登录相关参数
typedef struct TerminalLogonParams
{
	//本客户端类型：
	unsigned int unUserType;
	//组播端口，将使用本端口发送音频数据  使用本端口+2发送视频数据
	unsigned int unMulticastPort;
	//当开启NACK功能时，组播发送端用于NACK信令、媒体传输的单播端口，收发双方保持设置一致时生效。
	unsigned int unNackPort;
	//绑定的本地IP地址，当设置为空时，内部将使用INADDR_ANY，交由操作系统选择一个网卡IP。建议指定IP以防多IP时系统选择错误而无法正常收发组播流
	char acLocalIP[128];
	//组播地址
	char acMulticastIP[128];
}TerminalLogonParams;


//传输相关参数
typedef struct TerminalTransParams
{
	//发送使用的FEC方法，取值见：TERMINAL_FEC_METHOD_TYPE
	unsigned int unFecRedunMethod;
	//发送使用的FEC冗余度
	unsigned int unFecRedunRatio;
	//发送使用的FEC min group size
	unsigned int unFecMinGroupSize;
	//发送使用的FEC max group size
	unsigned int unFecMaxGroupSize;
	//是否启用组播NACK功能，当设置为TRUE时开启，收发双方保持设置一致时生效。
	unsigned int bEnableNack;
	//接收JitterBuff缓存时间，单位毫秒，设置为0时可获得最低延时
	unsigned int unJitterBuffDelay;
	//是否启用接收丢包冻结机制，启用后丢包可冻结画面避免用户感知花屏
	unsigned int bFreezeFrameWhenLost;
}TerminalTransParams;


//输出接收并解码后的YUV数据
typedef void (*RemoteVideoYuvDataCallback)(unsigned char* data, unsigned int unWidth, unsigned int unHeight, unsigned int unPts, void *pObject);

//输出接收并解码后的PCM数据
typedef void (*RemoteAudioPcmDataCallback)(unsigned char* data, unsigned int unLen, unsigned int unPts, void *pObject);



//////////////////////////////////////////////////////////////////////////
// 客户端类封装接口

/***
* 环境初始化，系统只需调用一次，主要用于日志模块的初始化
* @param: outputPath表示日志存放路径，支持相对路径和绝对路径，若目录不存在将自动创建
* @param: outputLevel表示日志输出的级别，只有等于或者高于该级别的日志输出到文件，取值范围参考TERMINAL_LOG_OUTPUT_LEVEL
* @return: 
*/
DLLIMPORT_MULTICAST_SDK void  SDTerminal_Enviroment_Init(const char* outputPath, int outputLevel);

DLLIMPORT_MULTICAST_SDK void  SDTerminal_Enviroment_Free();


/***
* 创建SDTerminal
* @param ptEncodeParams: 编码相关参数
* @param pfOutputVideo: 解码后视频输出回调接口
* @param pfOutputAudio: 解码后音频输出回调接口
* @param pObject: 上述输出回调接口的透传指针，将通过回调函数形参方式透传外层
* @return: 返回模块指针，为NULL则失败
*/
DLLIMPORT_MULTICAST_SDK void*  SDTerminal_New(TerminalEncodeParams* ptEncodeParams, RemoteVideoYuvDataCallback pfOutputVideo, 
								RemoteAudioPcmDataCallback pfOutputAudio, void* pObject);


/***
* 从服务器下线并销毁SDTerminal，使用者应该做好与其他API之间的互斥保护
* 互斥保护方法1：业务保证其他API接口停止调用后，才会调用本接口
* 互斥保护方法2：创建两把锁，分别用于音频发送和视频发送等API与本接口的互斥
*				 一定要对音视频分别使用不同的锁，这样才不会变成二者串行
* @param ppTerminal: 模块指针指针
* @return:
*/
DLLIMPORT_MULTICAST_SDK void  SDTerminal_Delete(void** ppTerminal);


/***
* 准备推送和接收音视频环境
* @param pTerminal: 模块指针
* @param ptLogonParams: 登录相关参数
* @param ptTransParams: 传输相关参数
* @return: 接口调用是否成功
*/
DLLIMPORT_MULTICAST_SDK BOOL  SDTerminal_Online(void* pTerminal, TerminalLogonParams* ptLogonParams, TerminalTransParams* ptTransParams);


/***
* 停止推送和接收音视频
* @param pTerminal: 模块指针
* @return:
*/
DLLIMPORT_MULTICAST_SDK void  SDTerminal_Offline(void* pTerminal);


/***
* 编码发送音频
* @param pTerminal: 模块指针
* @param buf: 输入音频PCM存放区
* @param unLen: 输入音频PCM字节数
* @return:
*/
DLLIMPORT_MULTICAST_SDK void  SDTerminal_SendAudioData(void* pTerminal, unsigned char* buf, unsigned int unLen);


/***
* 编码发送视频
* @param pTerminal: 模块指针
* @param buf: 待编码发送的视频数据存放区
* @param unLen: 视频数据存放区字节数
* @param unWidth: 视频宽度
* @param unHeight: 视频高度
* @param eFormat: 视频格式
* @return:
*/
DLLIMPORT_MULTICAST_SDK void  SDTerminal_SendVideoData(void* pTerminal, unsigned char* buf, unsigned int unWidth, unsigned int unHeight, TERMINAL_VIDEO_INPUT_FORMAT eFormat);



/***
* 获得硬编码支持情况,Online之后调用生效
* @param pTerminal: 模块指针
* @param pbWillUseHwEnc: 返回当前是否采用硬编码
* @param peHwRecomInputFormat: 若采用硬编码，硬编码推荐的输入格式
* @return: 接口调用是否成功
*/
DLLIMPORT_MULTICAST_SDK BOOL  SDTerminal_GetHardwareEncInfo(void* pTerminal, BOOL* pbWillUseHwEnc, TERMINAL_VIDEO_INPUT_FORMAT *peHwRecomInputFormat);



/***
* 获取SDK版本信息
* @param pTerminal: 模块指针
* @return: SDK版本号
*/
DLLIMPORT_MULTICAST_SDK unsigned int  SDTerminal_GetVersion(void* pTerminal);



/***
* 启动上行码流的TS文件录制,Online之后调用生效
* @param pTerminal: 模块指针
* @param strTsFileDir: TS文件URL
* @return: 接口调用是否成功
*/
DLLIMPORT_MULTICAST_SDK BOOL  SDTerminal_StartCapUpStream(void* pTerminal, const char* strTsFileUrl);


/***
* 停止上行码流的TS文件录制
* @param pTerminal: 模块指针
* @return: 接口调用是否成功
*/
DLLIMPORT_MULTICAST_SDK void  SDTerminal_StopCapUpStream(void* pTerminal);


/***
* 启动下行码流的TS文件录制,Online之后调用生效
* @param pTerminal: 模块指针
* @param strTsFileDir: TS文件URL
* @return: 接口调用是否成功
*/
DLLIMPORT_MULTICAST_SDK BOOL  SDTerminal_StartCapDownStream(void* pTerminal, const char* strTsFileUrl);


/***
* 停止下行码流的TS文件录制
* @param pTerminal: 模块指针
* @return: 
*/
DLLIMPORT_MULTICAST_SDK void  SDTerminal_StopCapDownStream(void* pTerminal);


/***
* 启动下行码流的RAW文件录制,Online之后调用生效
* @param pTerminal: 模块指针
* @param strRawFileUrl: RAW文件URL
* @return: 接口调用是否成功
*/
DLLIMPORT_MULTICAST_SDK BOOL  SDTerminal_StartCapDownRawStream(void* pTerminal, const char* strRawFileUrl);


/***
* 停止下行码流的RAW文件录制
* @param pTerminal: 模块指针
* @return: 
*/
DLLIMPORT_MULTICAST_SDK void  SDTerminal_StopCapDownRawStream(void* pTerminal);


/***
* 获取当前时刻音视频上下行丢包率情况，内部已经乘100.0转换为百分比
* @param pTerminal: 模块指针
* @param pfVideoUpLostRatio: 视频上行丢包率
* @param pfVideoDownLostRatio: 视频下行丢包率
* @param pfAudioUpLostRatio: 音频上行丢包率
* @param pfAudioDownLostRatio: 音频下行丢包率
* @return: 
*/
DLLIMPORT_MULTICAST_SDK void  SDTerminal_GetVideoAudioUpDownLostRatio(void* pTerminal, float *pfVideoUpLostRatio, float *pfVideoDownLostRatio, 
														float *pfAudioUpLostRatio, float *pfAudioDownLostRatio);




#ifdef __cplusplus
}
#endif

#endif // _SD_MEDIAPRO_TERMINAL_SDK_H_
