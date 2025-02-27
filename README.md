# OpenCVwindow


本系统是一个辅助上位机检测程序，可切换不同的模型根据不同的检测来源动态检测目标，例如预设中华稻蝗害虫检测模型，即可对中华稻蝗害虫进行准确识别。该系统会根据所读取输入的模型类别进行读取，并显示于通知栏。每次检测完成后可选保存检测的结果，便于用户快速复现检测结果避免遗漏。视频动态检测便于用户实时检测。支持串口通信与udp&tcp通信，可与单片机进行简易通讯，更好完成下行操作。

开发环境与运行环境.
开发的硬件环境：内存：32G以上；硬盘：1T.
开发软件的操作系统： Windows操作系统.
软件开发工具：QT6.5.
软件运行的操作系统： Windows操作系统.
编程语言：C++.


主要功能：.

①提供数据库系统，加载本地sqlite模型数据库.

![image-1](https://github.com/HEIseYOUmolc/OpenCVwindow/blob/master/resource/p12.png)

②netw模块，支持udp/tcp协议的发送信息配备附带协议的嵌入式设备可以做到通信.

![image-2](https://github.com/HEIseYOUmolc/OpenCVwindow/blob/master/resource/p13.png)

![image-3](https://github.com/HEIseYOUmolc/OpenCVwindow/blob/master/resource/p14.png)

③配备模型、类别、输入源三种选择，并显示选择结果在信息框.



④分别有原始图像和检测图像进行对比，辨别识别力度.

![image-4](https://github.com/HEIseYOUmolc/OpenCVwindow/blob/master/resource/p11.png)

⑤实现源检测，统计串口输入与输出的字节数量（后续会开发tcp与udp字节数量，方便查看通信质量.

⑥可选择是否保存检测后的源，图片保存为图片，摄像头、视频、rtsp、录屏、udp等保存为视频，并统计数量，避免遗漏等情况.

![image-5](https://github.com/HEIseYOUmolc/OpenCVwindow/blob/master/resource/p10.png)

