#include <WiFi.h>
#include <WebServer.h>

// -------------------硬件配置-------------------
#define TOUCH_PIN 4
#define ALARM_LED 2
#define TOUCH_THRESHOLD 400
WebServer server(80);

// -------------------WiFi配置 自行修改-------------------
const char* WIFI_SSID = "ddd";
const char* WIFI_PWD  = "Duan123456";

// -------------------全局安防状态变量-------------------
// 0=未布防  1=已布防(待机监控)  2=报警锁定
int systemState = 0;
bool lastTouchState = false;
unsigned long debounceTime = 0;
const unsigned long debounceDelay = 50;

// 网页HTML界面：布防/撤防双按钮
String htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>物联网安防报警器</title>
    <style>
        body{text-align:center;margin-top:60px;font-size:24px;}
        button{font-size:22px;padding:15px 40px;margin:20px 10px;border:none;border-radius:8px;cursor:pointer;}
        #armBtn{background:#c82423;color:white;}
        #disarmBtn{background:#248836;color:white;}
        #statusText{margin-top:30px;font-weight:bold;}
    </style>
</head>
<body>
    <h2>ESP32 安防报警主机</h2>
    <button id="armBtn">布防 Arm</button>
    <button id="disarmBtn">撤防 Disarm</button>
    <div id="statusText">当前状态：未布防</div>

    <script>
        const armBtn = document.getElementById("armBtn");
        const disarmBtn = document.getElementById("disarmBtn");
        const statusText = document.getElementById("statusText");

        // 布防请求
        armBtn.onclick = function(){
            fetch("/cmd?state=1")
                .then(res=>res.text())
                .then(data=>{ statusText.innerText = "当前状态：已布防，监控中"; });
        }
        // 撤防请求
        disarmBtn.onclick = function(){
            fetch("/cmd?state=0")
                .then(res=>res.text())
                .then(data=>{ statusText.innerText = "当前状态：未布防"; });
        }
    </script>
</body>
)HTML";

// 首页路由，返回控制网页
void handleIndex(){
    server.send(200, "text/html", htmlPage);
}

// 接收网页布撤防指令
void handleCommand(){
    if(server.hasArg("state")){
        int recvState = server.arg("state").toInt();
        // 网页只能切换 0未布防 / 1布防；报警2只能撤防清除
        if(recvState == 0){
            systemState = 0;
            digitalWrite(ALARM_LED, LOW);
        }else if(recvState == 1 && systemState != 2){
            systemState = 1;
        }
    }
    server.send(200, "text/plain", "OK");
}

void setup() {
    Serial.begin(115200);
    pinMode(ALARM_LED, OUTPUT);
    digitalWrite(ALARM_LED, LOW);

    // 连接WiFi
    Serial.print("连接WiFi：");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi连接成功！");
    Serial.print("安防主机地址：http://");
    Serial.println(WiFi.localIP());

    // 绑定网页路由
    server.on("/", handleIndex);
    server.on("/cmd", handleCommand);
    server.begin();
    Serial.println("Web安防服务器启动完成");
}

void loop() {
    server.handleClient();
    unsigned long now = millis();
    uint16_t touchValue = touchRead(TOUCH_PIN);
    bool currentTouch = (touchValue < TOUCH_THRESHOLD);

    // 软件防抖+触摸边沿检测
    if(now - debounceTime > debounceDelay){
        if(currentTouch != lastTouchState){
            debounceTime = now;
            // 边沿触发：刚触摸瞬间
            if(currentTouch){
                // 仅系统处于【已布防】时才触发报警锁定
                if(systemState == 1){
                    systemState = 2;
                    Serial.println("检测到入侵，进入报警锁定！");
                }
            }
            lastTouchState = currentTouch;
        }
    }

    // 报警锁定状态：LED高频闪烁
    if(systemState == 2){
        digitalWrite(ALARM_LED, HIGH);
        delay(80);
        digitalWrite(ALARM_LED, LOW);
        delay(80);
    }
}