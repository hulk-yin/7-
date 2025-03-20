# ESP32智能环境监控与语音助手

## 项目概述

该项目使用ESP32开发板创建一个智能环境监控系统，配备OLED显示、麦克风输入和扬声器输出功能。

## 当前进度
- ✅ 已完成OLED显示屏初始化和基本显示功能
- ⬜ 麦克风模块(INMP441)功能待实现
- ⬜ 扬声器模块(MAX98357A)功能待实现

## 硬件连接
已按照接线说明文档完成OLED显示屏连接：
- OLED VCC -> ESP32 3.3V
- OLED GND -> ESP32 GND  
- OLED SCL -> ESP32 GPIO22
- OLED SDA -> ESP32 GPIO21

## 构建与上传
使用PlatformIO构建并上传到ESP32开发板：
1. 打开PlatformIO IDE
2. 打开本项目
3. 点击"Build"编译项目
4. 点击"Upload"上传到开发板

## 项目结构

```
/e:/PlatformIO/Projects/7-/
├── include/                # 头文件目录
│   ├── config.h            # 配置参数
│   ├── utils.h             # 工具函数声明
│   ├── WebSocketHandlers.h # WebSocket处理
│   ├── AudioProcessor.h    # 音频处理
│   ├── ASRClient.h         # 语音识别客户端
│   ├── TTSClient.h         # 语音合成客户端
│   └── SparkClient.h       # AI大模型客户端
├── src/                    # 源代码目录
│   ├── main.cpp            # 主程序
│   ├── utils.cpp           # 工具函数实现
│   ├── WebSocketHandlers.cpp # WebSocket实现
│   ├── AudioProcessor.cpp    # 音频处理实现
│   ├── ASRClient.cpp         # 语音识别实现
│   ├── TTSClient.cpp         # 语音合成实现
│   └── SparkClient.cpp       # AI大模型实现
├── docs/                   # 文档目录
│   └── 功能说明.md         # 产品功能详细说明
└── test/                   # 测试目录
    └── test_main.cpp       # 主要功能测试
```

## 开发环境

- PlatformIO
- 目标平台: ESP32
- 依赖库: 
  - Arduino (^2.0.0)
  - ArduinoJson (^6.19.4)
  - WebSockets (^2.3.5)
  - NimBLE-Arduino (^1.4.0)
  - DHT sensor library (^1.4.4)
  - Crypto

## 主要功能

1. **环境监测功能**
   - 实时温湿度监测与显示
   - 温湿度异常预警
   - 历史数据记录与分析

2. **语音交互功能**
   - 本地语音唤醒
   - 语音识别(ASR)
   - 语音合成(TTS)
   - 语音控制设备功能

3. **AI助手功能**
   - 接入讯飞星火大模型
   - 智能对话与问答
   - 个性化服务推荐

4. **联网功能**
   - WiFi连接配置
   - WebSocket通信
   - 云端数据同步
   - 远程控制与监控

5. **蓝牙功能**
   - BLE数据广播
   - 手机APP连接配对
   - 近场控制与配置

6. **低功耗管理**
   - 智能休眠策略
   - 电池电量监测
   - 低功耗模式切换

## 功能说明
当前版本仅实现OLED显示功能，将显示系统运行时间。

## 使用说明

1. 使用PlatformIO打开项目
2. 根据需要修改config.h中的配置参数
3. 编译并上传到ESP32设备
4. 通过手机APP或Web界面连接设备进行配置
5. 使用"你好ESP32"唤醒语音助手

## 贡献指南

1. Fork本项目到您的GitHub账户
2. 创建您的特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交您的更改 (`git commit -m '添加了一些很棒的功能'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建一个Pull Request

## 许可证

本项目采用MIT许可证 - 详见 LICENSE 文件
