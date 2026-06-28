#include <WiFi.h>
#include <WebServer.h>

// 硬件定义
#define TOUCH_PIN 4
WebServer server(80);

// WiFi配置，自行修改
const char* ssid = "ddd";
const char* password = "Duan123456";

// 网页仪表盘HTML页面
String dashboardHtml = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>实时触摸传感器仪表盘</title>
    <style>
        *{margin:0;padding:0;box-sizing:border-box;}
        body{background:#121212;color:#fff;text-align:center;padding-top:100px;font-family:Arial;}
        h2{font-size:28px;margin-bottom:60px;color:#42A5F5;}
        .meter-box{
            width: 400px;
            height: 200px;
            margin: 0 auto;
            background:#1e1e1e;
            border:3px solid #2196F3;
            border-radius:16px;
            display:flex;
            align-items:center;
            justify-content:center;
        }
        #sensorVal{
            font-size:80px;
            font-weight:bold;
            color:#00e676;
        }
        .tip{margin-top:40px;font-size:18px;color:#aaa;}
    </style>
</head>
<body>
    <h2>触摸传感器实时数值仪表盘</h2>
    <div class="meter-box">
        <span id="sensorVal">0</span>
    </div>
    <p class="tip">手指靠近引脚 → 数值变小 | 手指离开 → 数值回升</p>

    <script>
        // 定时循环拉取传感器数据（AJAX轮询）
        function updateData(){
            fetch("/data")
                .then(resp => resp.text())
                .then(val => {
                    document.getElementById("sensorVal").innerText = val;
                })
                .catch(err => console.log("获取数据失败"));
        }
        // 每50ms刷新一次，实时流畅
        setInterval(updateData, 50);
    </script>
</body>
</html>
)HTML";

// 路由1：访问首页，返回仪表盘网页
void handleRoot() {
  server.send(200, "text/html", dashboardHtml);
}

// 路由2：数据接口，返回触摸原始模拟量（供前端拉取）
void handleGetData() {
  uint16_t touchRaw = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(touchRaw));
}

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);

  // WiFi连接
  Serial.print("正在连接WiFi：");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！");
  Serial.print("仪表盘访问地址：http://");
  Serial.println(WiFi.localIP());

  // 注册网页与数据接口路由
  server.on("/", handleRoot);
  server.on("/data", handleGetData);
  server.begin();
  Serial.println("Web仪表盘服务已启动");
}

void loop() {
  server.handleClient();
}