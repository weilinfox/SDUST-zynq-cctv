# youmu-zynq-cctv

毕业设计部分测试工程

## 视频分辨率转换 IP

[hls_resize](hls_resize) 用 HLS 编写的，用于将 1920x1080@30fps 的视频转换为 800x480@30fps 。

## 视频压缩

[lz4_c_test](lz4_c_test) 相较于专用的压缩算法， lz4 同时具有较好的通用性、极高的压缩速度和可观的压缩率，适合在 PS 端运行。

## AX7020 Demo 工程

[hdmi_ov5640_single.zip](hdmi_ov5640_single.zip) 一个 demo 工程，摄像头使用 AN5642 ， HDMI 输出为 800x480@60fps ，只使用了其中一个摄像头。

没有网络支持没有双缓冲。

<!-- ## HDMI 输出时钟计算

使用 [Video Timings Calculator](https://tomverbeure.github.io/video_timings_calculator) ，由于我得屏幕分辨率为 800x480@60Hz ，输入后得到 CVT 的 Pixel Clock 为 29.5MHz 。
-->
