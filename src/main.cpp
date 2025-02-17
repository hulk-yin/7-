#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <WiFi.h> 

// 初始化蓝牙串口和 WebSocket 客户端
BluetoothSerial SerialBT;
WebSocketsClient webSocketTTS;
WebSocketsClient webSocketASR;
WebSocketsClient webSocketSpark; 
// 科大讯飞 API 的鉴权信息
const char *appId = "57e5c152";
const char *apiSecret = "2b4d77de806ece14675de7ed359cd995";
const char *apiKey = "e92cfdec778718ef7348c8d4fb303893";
 

// WiFi 配置信息
char ssid[32] = "Xiaomi_73A0_Wi-Fi5";
char password[32] = "03031902";

bool isPaired = false; // 标记是否已配对蓝牙设备
String userInputText;  // 存储用户输入的文本

// TTS WebSocket 事件处理函数
void webSocketTTSEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("TTS 连接断开！");
    break;
  case WStype_CONNECTED:
    Serial.println("TTS 连接成功！");
    break;
  case WStype_TEXT:
    Serial.printf("TTS 接收到文本：%s\n", payload);
    break;
  case WStype_BIN:
    Serial.printf("TTS 接收到二进制数据，长度：%u\n", length);
    break;
  case WStype_ERROR:
    Serial.println("TTS 发生错误！");
    break;
  case WStype_PING:
    Serial.println("TTS 接收到 PING！");
    break;
  case WStype_PONG:
    Serial.println("TTS 接收到 PONG！");
    break;
  default:
    Serial.println("未知的 TTS 事件类型！");
    break;
  }
}

// ASR WebSocket 事件处理函数
void webSocketASREvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("ASR 连接断开！");
    break;
  case WStype_CONNECTED:
    Serial.println("ASR 连接成功！");
    break;
  case WStype_TEXT:
  {
    Serial.printf("ASR 接收到文本：%s\n", payload);
    userInputText = String((char *)payload);
    // 将用户输入文本发送到讯飞 Spark4.0 Ultra 模型进行处理
    String sparkRequestPayload = "{\"text\":\"" + userInputText + "\",\"model\":\"Spark4.0 Ultra\"}";
    webSocketSpark.sendTXT(sparkRequestPayload);
    break;
  }
  case WStype_BIN:
    Serial.printf("ASR 接收到二进制数据，长度：%u\n", length);
    break;
  case WStype_ERROR:
    Serial.println("ASR 发生错误！");
    break;
  case WStype_PING:
    Serial.println("ASR 接收到 PING！");
    break;
  case WStype_PONG:
    Serial.println("ASR 接收到 PONG！");
    break;
  }
}

// Spark WebSocket 事件处理函数
void webSocketSparkEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("Spark 连接断开！");
    break;
  case WStype_CONNECTED:
    Serial.println("Spark 连接成功！");
    break;
  case WStype_TEXT:
  {
    Serial.printf("Spark 接收到文本：%s\n", payload);
    // 将 Spark4.0 Ultra 模型的回答发送到 TTS 服务进行语音合成
    String ttsRequestPayload = "{\"text\":\"" + String((char *)payload) + "\",\"voice\":\"xiaoyan\",\"speed\":50,\"volume\":50,\"pitch\":50,\"engine_type\":\"intp65\"}";
    webSocketTTS.sendTXT(ttsRequestPayload);
    break;
  }
  case WStype_BIN:
    Serial.printf("Spark 接收到二进制数据，长度：%u\n", length);
    break;
  case WStype_ERROR:
    Serial.println("Spark 发生错误！");
    break;
  case WStype_PING:
    Serial.println("Spark 接收到 PING！");
    break;
  case WStype_PONG:
    Serial.println("Spark 接收到 PONG！");
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  // 初始化蓝牙串口并启用主模式
  SerialBT.begin("Bose Mini II SE SoundLink", true); // 蓝牙设备名 
  Serial.println("设备已启动，现在可以配对蓝牙！");

  // 循环链接直到成功配对华为耳机 FreeBuds 5
  while (!isPaired)
  {
    if (SerialBT.connect("HUAWEI FreeBuds 5"))
    {
      Serial.println("成功配对 HUAWEI FreeBuds 5");
      isPaired = true;
    }
    else
    {
      Serial.println("配对 HUAWEI FreeBuds 5 失败，重试中...");
      delay(5000); // 等待 5 秒后重试
    }
  }

  // 连接到 WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("正在连接到 WiFi...");
  }
  Serial.println("WiFi 连接成功！");

  // 初始化 WebSocket 连接
  webSocketTTS.beginSSL("tts-api.xfyun.cn", 443, "/v2/tts");
  webSocketTTS.onEvent(webSocketTTSEvent);

  webSocketASR.beginSSL("iat-api.xfyun.cn", 443, "/v2/iat");
  webSocketASR.onEvent(webSocketASREvent);

  webSocketSpark.beginSSL("spark-api.xf-yun.com", 443, "/v4.0/chat");
  webSocketSpark.onEvent(webSocketSparkEvent);

  // 生成鉴权参数
  String authString = "appid=" + String(appId) + ",apiKey=" + String(apiKey) + ",apiSecret=" + String(apiSecret);
  webSocketTTS.setAuthorization(authString.c_str());
  webSocketASR.setAuthorization(authString.c_str());
  webSocketSpark.setAuthorization(authString.c_str());
 
}

void loop()
{
  if (SerialBT.available())
  {
    uint8_t data = SerialBT.read();
    Serial.write(data);
    // 将蓝牙设备的输入语音发送到科大讯飞语音转文字服务
    webSocketASR.sendBIN(&data, 1);
  }
  webSocketTTS.loop();
  webSocketASR.loop();
  webSocketSpark.loop();

  // 检查是否已配对并播放音频
  if (isPaired)
  {
    // 播放音频逻辑
    // 例如：播放从 WebSocket 接收到的音频数据
  }
}