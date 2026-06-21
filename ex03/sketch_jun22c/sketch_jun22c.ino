const int ledPin = 2;
// 时序定义
const unsigned long dot = 200;    // 短闪时长
const unsigned long dash = 600;   // 长闪时长
const unsigned long charGap = 200;// 单次闪烁间隔
const unsigned long sosGap = 1500;// 整套SOS结束长停顿

unsigned long prevT = 0;
int stage = 0; // 0=等待新一轮SOS 1=3短S 2=3长O 3=3短S
int cnt = 0;
bool ledOn = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long now = millis();
  unsigned long delta = now - prevT;

  if(stage == 0){
    if(delta >= sosGap){
      prevT = now;
      stage = 1;
      cnt = 0;
    }
  }else if(stage == 1){
    if(!ledOn){
      if(delta >= charGap){
        prevT = now;
        ledOn = true;
        digitalWrite(ledPin, HIGH);
      }
    }else{
      if(delta >= dot){
        prevT = now;
        ledOn = false;
        digitalWrite(ledPin, LOW);
        cnt++;
        if(cnt >=3){stage=2;cnt=0;}
      }
    }
  }else if(stage == 2){
    if(!ledOn){
      if(delta >= charGap){
        prevT = now;
        ledOn = true;
        digitalWrite(ledPin, HIGH);
      }
    }else{
      if(delta >= dash){
        prevT = now;
        ledOn = false;
        digitalWrite(ledPin, LOW);
        cnt++;
        if(cnt >=3){stage=3;cnt=0;}
      }
    }
  }else if(stage == 3){
    if(!ledOn){
      if(delta >= charGap){
        prevT = now;
        ledOn = true;
        digitalWrite(ledPin, HIGH);
      }
    }else{
      if(delta >= dot){
        prevT = now;
        ledOn = false;
        digitalWrite(ledPin, LOW);
        cnt++;
        if(cnt >=3){stage=0;cnt=0;}
      }
    }
  }
}