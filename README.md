---
# 极简音视频组播SDK

[https://github.com/waterfoxfox/MulticastSDK/](https://github.com/waterfoxfox/MulticastSDK/) 

本音视频组播SDK，支持特性：
* 1、	独有的NACK组播重传技术，为每个组播接收端提供个性化重传服务。
* 2、	支持基于RS编码的FEC前向纠错，专为音视频类应用量身定制。
* 3、	支持FEC动态Group分组，避免分组跨帧同时尽可能提升抗连续丢包能力。
* 4、	发送端传入单帧码流，内部自行拆分拼接，实现透传。
* 5、	发送端支持Smooth平滑发送，对视频大I帧场景进行优化。
* 6、	支持接收端自适应JitterBuff缓存技术。
* 7、	两套独立SDK分别适应不同场景，一套仅负责传输层，外层应用负责编解码。一套负责编解码以及网络传输，外层负责采集、渲染。
* 8、	C++开发，性能强劲，无外部库依赖，支持Windows\Android\Linux，支持32位、64位系统。
* 9、	API接口简洁易用、完善的文档与DEMO参考。


---

# 极简音视频组播(内置编解码) WIN SDK

[https://github.com/waterfoxfox/MulticastCodecSDK/](https://github.com/waterfoxfox/MulticastCodecSDK/) 


本音视频组播SDK，支持特性：
* 1、	独有的NACK组播重传技术，为每个组播接收端提供个性化重传服务，网络适应性更强。
* 2、	支持基于RS编码的FEC前向纠错，专为音视频类应用量身定制。
* 3、	支持FEC动态Group分组，避免分组跨帧同时尽可能提升抗连续丢包能力。
* 4、	发送端支持Smooth平滑发送，对视频大I帧场景进行优化。
* 5、	支持接收端自适应JitterBuff缓存技术。
* 6、	SDK支持输入YUV420P、NV12、NV21、RGB24、RGBA32格式原始图像，支持Nvidia、Qsv硬件H264编码以及X264软编码、X265软编码。当系统不支持硬编码时自行切换为软编码。
* 7、   SDK支持ffmpeg H264、HEVC软件解码，输出YUV420P。
* 8、   SDK支持输入输出音频PCM，采用Fdk AAC LC编解码。
* 9、   SDK内置视频缩放、音频重采样能力，支持外层输入可变的不同于编码宽高的采集宽高、支持输出外层指定的采样率、声道数。
* 10、	C++开发，性能强劲，无外部库依赖，支持32位、64位系统。
* 11、	API接口简洁易用、完善的文档与DEMO参考。

---

#  DEMO说明


* MulticastTerminalSdkTest-Video-File-Send：
读取H264文件，并使用《极简音视频组播SDK》进行组播发送，推送使用的参数从TerminalSdkTest.ini中读取。工程使用VS2010开发。

* MulticastTerminalSdkTest-Video-Player：
使用《极简音视频组播(内置编解码)SDK》接收音视频组播流，并解码播放。注意务必在TerminalSdkTest.ini中根据自身情况修改配置参数LocalIp，以保证使用预期的网卡接收组播流。工程使用VS2010开发。

* MulticastScreenShare：
使用《极简音视频组播(内置编解码)SDK》进行屏幕采集、扬声器采集、音视频编码、组播推送。



---

# 相关资源

SDK 商用及定制化、技术支持服务可联系：[http://www.mediapro.cc/](http://www.mediapro.cc/)

