#include <WiFi.h>
#include <WebServer.h>

// ========== 配置区 ==========
const char* ssid     = "ddd";
const char* password = "Duan123456";
#define LED_PIN 2    // PWM控制LED引脚
WebServer server(80);
int ledBright = 0;   // 存储0~255亮度值

// HTML网页页面，内置滑动条+JS交互
String htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <title>Web无极调光器</title>
    <style>
        body {text-align:center;margin-top:80px;font-size:22px;}
        .slider{width:80%;height:30px;margin:40px 0;}
        #val{font-size:30px;color:#0066cc;font-weight:bold;}
    </style>
</head>
<body>
    <h2>ESP32 网页无极调光</h2>
    <p>当前亮度：<span id="val">0</span></p>
    <!-- 滑动条 0~255 -->
    <input class="slider" type="range" min="0" max="255" id="brightSlider">

    <script>
        let slider = document.getElementById("brightSlider");
        let showVal = document.getElementById("val");

        // 监听滑块拖动事件
        slider.addEventListener("input", function(){
            let value = this.value;
            showVal.innerText = value;
            // fetch异步发送GET请求到ESP32
            fetch("/set?bri="+value);
        });
    </script>
</body>
</html>
)HTML";

// 首页路由：返回调光网页
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// 亮度设置路由：解析滑块传过来的亮度参数
void handleSetBright() {
  if(server.hasArg("bri")){
    ledBright = server.arg("bri").toInt();
    // 限制0~255区间防止越界
    ledBright = constrain(ledBright,0,255);
    analogWrite(LED_PIN, ledBright);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);

  // 连接WiFi
  Serial.print("连接WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！");
  Serial.print("网页地址：http://");
  Serial.println(WiFi.localIP());

  // 绑定网页路由
  server.on("/", handleRoot);
  server.on("/set", handleSetBright);
  server.begin();
  Serial.println("Web服务器已启动");
}

void loop() {
  server.handleClient();
}