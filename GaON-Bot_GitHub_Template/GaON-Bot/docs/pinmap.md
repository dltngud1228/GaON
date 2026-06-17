# GaON Bot Pin Map

> 이 문서는 가온봇의 핀맵을 정리하기 위한 파일입니다.  
> 실제 최종 배선과 다르면 반드시 수정하세요.

## Arduino UNO

| 기능 | 핀 | 비고 |
|---|---:|---|
| ESP32-CAM 통신 RX | A0 | ESP32-CAM GPIO13 → UNO A0 |
| ESP32-CAM 통신 TX | A1 | 필요 시 사용 |
| LCD SDA | A4 | I2C |
| LCD SCL | A5 | I2C |
| Front Left Motor IA | D2 | 확인 필요 |
| Front Left Motor IB | D3 | 확인 필요 |
| Front Right Motor IA | D4 | 확인 필요 |
| Front Right Motor IB | D5 | 확인 필요 |
| Rear Left Motor IA | D6 | 확인 필요 |
| Rear Left Motor IB | D7 | 확인 필요 |
| Ultrasonic TRIG | D8 | 확인 필요 |
| Ultrasonic ECHO | D9 | 확인 필요 |

## ESP32-CAM

| 기능 | 핀 | 비고 |
|---|---:|---|
| Arduino UNO로 신호 전송 | GPIO13 | UNO A0 연결 |
| 5V | 5V | 보조배터리 5V 권장 |
| GND | GND | 공통 GND 필요 |
| Camera Pins | 기본 AI Thinker 설정 | 코드 참고 |

## 주의사항

- ESP32-CAM에는 9V를 직접 연결하지 않습니다.
- ESP32-CAM은 안정적인 5V 전원을 사용하는 것이 좋습니다.
- 모터 전원과 ESP32-CAM 전원을 분리하면 영상 끊김 문제를 줄일 수 있습니다.
- LCD는 Arduino UNO의 5V/GND와 I2C 핀을 사용합니다.

