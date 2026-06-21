// ESP32板载LED固定GPIO2，替换原来LED_BUILTIN
const int ledPin = 2;
unsigned long prevTime = 0;
const long interval = 500; // 500ms翻转，周期1s=1Hz
int ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200); // 开启串口，波特率115200，和实验文档统一
}

void loop() {
  unsigned long currTime = millis();
  if (currTime - prevTime >= interval) {
    prevTime = currTime;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    
    // 每次LED状态翻转，串口输出提示
    if (ledState == HIGH) {
      Serial.println("LED点亮，单次闪烁完成");
    } else {
      Serial.println("LED熄灭，单次闪烁完成");
    }
  }
}