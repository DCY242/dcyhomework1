#define TOUCH_PIN 4
#define LED_PIN 2

// 触摸阈值
#define TOUCH_THRESHOLD 400
// 防抖时间
const unsigned long DEBOUNCE_DELAY = 50;

// 呼吸参数
int brightness = 0;    // LED亮度PWM值 0~255
int fadeStep;          // 呼吸步进，控制速度
bool fadeUp = true;    // 呼吸增减方向

// 触摸相关
bool lastTouch = false;
unsigned long touchTime = 0;

// 档位变量：1/2/3三档循环
int speedGear = 1;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  touchAttachInterrupt(TOUCH_PIN, touchISR, TOUCH_THRESHOLD);
}

// 中断仅打标记，不做复杂运算
volatile bool touchFlag = false;
void touchISR() {
  touchFlag = true;
}

void loop() {
  unsigned long now = millis();

  // 触摸边沿检测+防抖，切换呼吸档位
  if (touchFlag && now - touchTime > DEBOUNCE_DELAY) {
    touchTime = now;
    touchFlag = false;
    uint16_t touchVal = touchRead(TOUCH_PIN);
    bool curTouch = (touchVal < TOUCH_THRESHOLD);

    // 边缘检测：无触摸→触摸瞬间切换档位
    if (!lastTouch && curTouch) {
      speedGear++;
      if (speedGear > 3) speedGear = 1; // 三档循环
      Serial.print("当前呼吸档位：");
      Serial.println(speedGear);
    }
    lastTouch = curTouch;
  }

  // 根据档位设置呼吸步长
  switch (speedGear) {
    case 1: fadeStep = 1; break;  // 慢速呼吸
    case 2: fadeStep = 3; break;  // 中速呼吸
    case 3: fadeStep = 6; break;  // 快速呼吸
  }

  // PWM呼吸灯主体逻辑
  analogWrite(LED_PIN, brightness);
  if (fadeUp) {
    brightness += fadeStep;
    if (brightness >= 255) fadeUp = false;
  } else {
    brightness -= fadeStep;
    if (brightness <= 0) fadeUp = true;
  }

  delay(10);
}