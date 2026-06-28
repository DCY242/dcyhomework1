#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 400

bool ledState = false;
// 触摸状态缓存：true=当前有触摸，false=无触摸
bool lastTouch = false;
// 防抖时间戳
unsigned long touchDebounceTime = 0;
const unsigned long debounceDelay = 50; // 50ms防抖

// 中断仅设置标记，禁止串口、复杂操作
volatile bool touchTriggerFlag = false;

void gotTouch() {
  touchTriggerFlag = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  touchAttachInterrupt(TOUCH_PIN, gotTouch, THRESHOLD);
}

void loop() {
  unsigned long now = millis();
  // 检测中断标记 + 防抖冷却时间到
  if (touchTriggerFlag && (now - touchDebounceTime > debounceDelay)) {
    touchDebounceTime = now;
    touchTriggerFlag = false;

    // 读取当前触摸原始值
    uint16_t touchVal = touchRead(TOUCH_PIN);
    bool curTouch = (touchVal < THRESHOLD);

    // 边沿检测：上一次无触摸，本次刚触摸（按下瞬间）
    if (!lastTouch && curTouch) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      Serial.print("触摸触发，LED状态：");
      Serial.println(ledState ? "亮" : "灭");
    }
    // 更新历史触摸状态
    lastTouch = curTouch;
  }
  delay(20);
}