#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


const int FL_IA = 2;
const int FL_IB = 3;

const int FR_IA = 4;
const int FR_IB = 5;

const int RL_IA = 6;
const int RL_IB = 7;

const int RR_IA = 8;
const int RR_IB = 9;

// =====================
// 초음파 센서 핀
// =====================
const int LEFT_TRIG = 10;
const int LEFT_ECHO = 11;

const int RIGHT_TRIG = 12;
const int RIGHT_ECHO = 13;

// =====================
// 제스처 설정
// =====================
const int HAND_DISTANCE = 7;
const unsigned long GESTURE_TIME = 8000;
const unsigned long YES_HOLD_TIME = 1500;
const int NO_ALTERNATE_COUNT = 3;

void setup() {
  // UNO 기본 시리얼 D0/D1 사용
  // ESP32-CAM과 통신 속도 4800
  Serial.begin(4800);

  pinMode(FL_IA, OUTPUT);
  pinMode(FL_IB, OUTPUT);

  pinMode(FR_IA, OUTPUT);
  pinMode(FR_IB, OUTPUT);

  pinMode(RL_IA, OUTPUT);
  pinMode(RL_IB, OUTPUT);

  pinMode(RR_IA, OUTPUT);
  pinMode(RR_IB, OUTPUT);

  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);

  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);

  stopMotor();

  lcd.init();
  lcd.backlight();

  showHappyFace();
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd != '\n' && cmd != '\r') {
      runCommand(cmd);
    }
  }
}

void runCommand(char cmd) {
  if (cmd == 'F' || cmd == 'f') {
    forward();
  }
  else if (cmd == 'B' || cmd == 'b') {
    backward();
  }
  else if (cmd == 'L' || cmd == 'l') {
    left();
  }
  else if (cmd == 'R' || cmd == 'r') {
    right();
  }
  else if (cmd == 'S' || cmd == 's') {
    stopMotor();
  }
  else if (cmd == '1') {
    stopMotor();
    showMessageAndGesture("Did you eat?");
  }
  else if (cmd == '2') {
    stopMotor();
    showMessageAndGesture("Take medicine?");
  }
  else if (cmd == '3') {
    stopMotor();
    showMessageAndGesture("Are you okay?");
  }
  else if (cmd == 'T' || cmd == 't') {
    sendYesToESP();
    showYesFace();
    delay(1500);
    showHappyFace();
  }
}

// =====================
// LCD 얼굴
// =====================
void showHappyFace() {
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("^");

  lcd.setCursor(12, 0);
  lcd.print("^");

  lcd.setCursor(5, 1);
  lcd.print("\\____/");
}

void showYesFace() {
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("^");

  lcd.setCursor(12, 0);
  lcd.print("^");

  lcd.setCursor(5, 1);
  lcd.print("\\____/");
}

void showNoFace() {
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("T");

  lcd.setCursor(12, 0);
  lcd.print("T");

  lcd.setCursor(5, 1);
  lcd.print("/----\\");
}

void showNoResponseFace() {
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("-");

  lcd.setCursor(12, 0);
  lcd.print("-");

  lcd.setCursor(5, 1);
  lcd.print("------");
}

// =====================
// ESP32-CAM으로 결과 전송
// =====================
void sendYesToESP() {
  Serial.write('Y');
}

void sendNoToESP() {
  Serial.write('N');
}

void sendNoResponseToESP() {
  Serial.write('X');
}

// =====================
// 메시지 + 제스처
// =====================
void showMessageAndGesture(String msg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
  lcd.setCursor(0, 1);
  lcd.print("Gesture Ready");

  delay(1000);

  waitGesture();

  delay(1500);

  showHappyFace();
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  long distance = duration * 0.034 / 2;
  return distance;
}

void waitGesture() {
  unsigned long startTime = millis();

  unsigned long rightHoldStart = 0;
  bool rightHolding = false;

  int lastSide = 0;
  int alternateCount = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gesture Ready");
  lcd.setCursor(0, 1);
  lcd.print("R=YES LR=NO");

  while (millis() - startTime < GESTURE_TIME) {
    long leftDistance = getDistance(LEFT_TRIG, LEFT_ECHO);
    delay(40);

    long rightDistance = getDistance(RIGHT_TRIG, RIGHT_ECHO);
    delay(40);

    bool leftClose = (leftDistance > 0 && leftDistance <= HAND_DISTANCE);
    bool rightClose = (rightDistance > 0 && rightDistance <= HAND_DISTANCE);

    // YES: 오른쪽 센서 1.5초 유지
    if (rightClose && !leftClose) {
      if (rightHolding == false) {
        rightHolding = true;
        rightHoldStart = millis();
      }

      if (millis() - rightHoldStart >= YES_HOLD_TIME) {
        sendYesToESP();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Answer: YES");

        delay(800);
        showYesFace();

        return;
      }
    }
    else {
      rightHolding = false;
      rightHoldStart = 0;
    }

    // NO: 좌우 번갈아 감지
    int currentSide = 0;

    if (leftClose && !rightClose) {
      currentSide = 1;
    }
    else if (rightClose && !leftClose) {
      currentSide = 2;
    }

    if (currentSide != 0) {
      if (lastSide == 0) {
        lastSide = currentSide;
      }
      else if (currentSide != lastSide) {
        alternateCount++;
        lastSide = currentSide;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("NO Gesture");
        lcd.setCursor(0, 1);
        lcd.print("Count: ");
        lcd.print(alternateCount);
      }
    }

    if (alternateCount >= NO_ALTERNATE_COUNT) {
      sendNoToESP();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Answer: NO");

      delay(800);
      showNoFace();

      return;
    }
  }

  // 시간 초과
  sendNoResponseToESP();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("No Response");

  delay(800);
  showNoResponseFace();
}

// =====================
// 모터 제어
// =====================
void forward() {
  digitalWrite(FL_IA, HIGH);
  digitalWrite(FL_IB, LOW);

  digitalWrite(FR_IA, HIGH);
  digitalWrite(FR_IB, LOW);

  digitalWrite(RL_IA, HIGH);
  digitalWrite(RL_IB, LOW);

  digitalWrite(RR_IA, HIGH);
  digitalWrite(RR_IB, LOW);
}

void backward() {
  digitalWrite(FL_IA, LOW);
  digitalWrite(FL_IB, HIGH);

  digitalWrite(FR_IA, LOW);
  digitalWrite(FR_IB, HIGH);

  digitalWrite(RL_IA, LOW);
  digitalWrite(RL_IB, HIGH);

  digitalWrite(RR_IA, LOW);
  digitalWrite(RR_IB, HIGH);
}

void left() {
  digitalWrite(FL_IA, LOW);
  digitalWrite(FL_IB, HIGH);

  digitalWrite(RL_IA, LOW);
  digitalWrite(RL_IB, HIGH);

  digitalWrite(FR_IA, HIGH);
  digitalWrite(FR_IB, LOW);

  digitalWrite(RR_IA, HIGH);
  digitalWrite(RR_IB, LOW);
}

void right() {
  digitalWrite(FL_IA, HIGH);
  digitalWrite(FL_IB, LOW);

  digitalWrite(RL_IA, HIGH);
  digitalWrite(RL_IB, LOW);

  digitalWrite(FR_IA, LOW);
  digitalWrite(FR_IB, HIGH);

  digitalWrite(RR_IA, LOW);
  digitalWrite(RR_IB, HIGH);
}

void stopMotor() {
  digitalWrite(FL_IA, LOW);
  digitalWrite(FL_IB, LOW);

  digitalWrite(FR_IA, LOW);
  digitalWrite(FR_IB, LOW);

  digitalWrite(RL_IA, LOW);
  digitalWrite(RL_IB, LOW);

  digitalWrite(RR_IA, LOW);
  digitalWrite(RR_IB, LOW);
}
