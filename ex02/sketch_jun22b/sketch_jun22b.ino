const int ledPin = 2;
unsigned long prevTime = 0;
const long interval = 500; // 500ms翻转，周期1s=1Hz
int ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currTime = millis();
  if (currTime - prevTime >= interval) {
    prevTime = currTime;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}