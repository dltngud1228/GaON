/*
  GaON Bot - Arduino UNO Code

  역할:
  - 모터 제어
  - LCD 상태 표시
  - 초음파 센서 거리 측정
  - ESP32-CAM 또는 외부 모듈과의 시리얼 통신

  사용 방법:
  1. 이 파일의 예시 핀맵을 실제 가온봇 핀맵에 맞게 수정합니다.
  2. 기존에 사용했던 최종 Arduino UNO 코드를 이 파일에 붙여넣어도 됩니다.
  3. Wi-Fi 비밀번호, 개인정보 등은 절대 이 파일에 넣지 마세요.

  TODO:
  - 실제 최종 핀맵 확인 후 수정
  - 최종 주행 함수 붙여넣기
  - 초음파 센서 핀 확인
*/

#include <SoftwareSerial.h>

// ESP32-CAM GPIO13 -> Arduino UNO A0
// Arduino UNO A1 -> 필요 시 ESP32-CAM RX 또는 예비 통신 핀
SoftwareSerial espSerial(A0, A1);

// =======================
// Motor Pin Example
// =======================
// 아래 핀은 예시입니다. 실제 가온봇 최종 배선에 맞게 수정하세요.

const int FL_IA = 2;  // Front Left Motor IA
const int FL_IB = 3;  // Front Left Motor IB

const int FR_IA = 4;  // Front Right Motor IA
const int FR_IB = 5;  // Front Right Motor IB

const int RL_IA = 6;  // Rear Left Motor IA
const int RL_IB = 7;  // Rear Left Motor IB

// 초음파 센서 핀은 실제 배선 확인 후 수정
const int TRIG_PIN = 8;
const int ECHO_PIN = 9;

// =======================
// Basic Motor Functions
// =======================

void stopMotors() {
  digitalWrite(FL_IA, LOW);
  digitalWrite(FL_IB, LOW);
  digitalWrite(FR_IA, LOW);
  digitalWrite(FR_IB, LOW);
  digitalWrite(RL_IA, LOW);
  digitalWrite(RL_IB, LOW);
}

void moveForward() {
  // TODO: 실제 바퀴 방향에 맞게 HIGH/LOW 수정
  digitalWrite(FL_IA, HIGH);
  digitalWrite(FL_IB, LOW);
  digitalWrite(FR_IA, HIGH);
  digitalWrite(FR_IB, LOW);
  digitalWrite(RL_IA, HIGH);
  digitalWrite(RL_IB, LOW);
}

void moveBackward() {
  // TODO: 실제 바퀴 방향에 맞게 HIGH/LOW 수정
  digitalWrite(FL_IA, LOW);
  digitalWrite(FL_IB, HIGH);
  digitalWrite(FR_IA, LOW);
  digitalWrite(FR_IB, HIGH);
  digitalWrite(RL_IA, LOW);
  digitalWrite(RL_IB, HIGH);
}

void turnLeft() {
  // TODO: 실제 좌회전 방향에 맞게 수정
  digitalWrite(FL_IA, LOW);
  digitalWrite(FL_IB, HIGH);
  digitalWrite(FR_IA, HIGH);
  digitalWrite(FR_IB, LOW);
  digitalWrite(RL_IA, LOW);
  digitalWrite(RL_IB, HIGH);
}

void turnRight() {
  // TODO: 실제 우회전 방향에 맞게 수정
  digitalWrite(FL_IA, HIGH);
  digitalWrite(FL_IB, LOW);
  digitalWrite(FR_IA, LOW);
  digitalWrite(FR_IB, HIGH);
  digitalWrite(RL_IA, HIGH);
  digitalWrite(RL_IB, LOW);
}

// =======================
// Ultrasonic Sensor
// =======================

long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distance = duration * 0.034 / 2;

  return distance;
}

// =======================
// Setup & Loop
// =======================

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  pinMode(FL_IA, OUTPUT);
  pinMode(FL_IB, OUTPUT);
  pinMode(FR_IA, OUTPUT);
  pinMode(FR_IB, OUTPUT);
  pinMode(RL_IA, OUTPUT);
  pinMode(RL_IB, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  stopMotors();

  Serial.println("GaON Bot UNO Ready");
}

void loop() {
  if (espSerial.available()) {
    char command = espSerial.read();

    switch (command) {
      case 'F':
        moveForward();
        break;

      case 'B':
        moveBackward();
        break;

      case 'L':
        turnLeft();
        break;

      case 'R':
        turnRight();
        break;

      case 'S':
        stopMotors();
        break;

      default:
        stopMotors();
        break;
    }
  }

  // 거리 확인 예시
  long distance = readDistanceCM();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(100);
}
