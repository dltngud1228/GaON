# GaON Bot

GaON Bot은 **ESP32-CAM 영상 송출**과 **Arduino UNO 기반 주행 제어**를 결합한 이동형 로봇 프로젝트입니다.  
사용자는 모바일 기기를 통해 영상을 확인하고, 로봇을 원격으로 조종할 수 있습니다.

> 이 저장소는 가온봇 프로젝트의 코드, 핀맵, 회로 설명, 제작 과정, 시연 자료를 정리하기 위한 GitHub 포트폴리오용 저장소입니다.

---

## 프로젝트 개요

- 프로젝트명: **GaON Bot**
- 개발 목적: 카메라 기반 원격 주행 로봇 제작
- 주요 기능:
  - ESP32-CAM 기반 실시간 영상 송출
  - Arduino UNO 기반 모터 제어
  - L911S 모터 드라이버를 이용한 DC 모터 제어
  - LCD를 활용한 상태 표시
  - 초음파 센서를 활용한 거리 감지
  - 모바일 기기를 이용한 원격 조종

---

## 사용 부품

| 구분 | 부품 |
|---|---|
| 메인 제어 | Arduino UNO |
| 영상 송출 | ESP32-CAM |
| 모터 제어 | L911S 모터 드라이버 |
| 구동부 | DC 모터, 바퀴 |
| 표시 장치 | I2C LCD |
| 센서 | 초음파 센서 |
| 전원 | 보조배터리, 9V 배터리, 토글 스위치 |

---

## 시스템 구조

```text
[Mobile Device]
      |
      | Wi-Fi / Web Streaming
      v
[ESP32-CAM] ---- Serial ---- [Arduino UNO]
                                |
                                | Motor Control
                                v
                         [L911S Motor Driver]
                                |
                                v
                            [DC Motors]

[Ultrasonic Sensor] ---> [Arduino UNO] ---> [LCD Display]
```

---

## 폴더 구조

```text
GaON-Bot/
├─ README.md
├─ .gitignore
├─ arduino_uno/
│  ├─ gaonbot_uno.ino
│  └─ README.md
├─ esp32_cam/
│  ├─ gaonbot_esp32cam.ino
│  └─ README.md
├─ docs/
│  ├─ pinmap.md
│  ├─ circuit.md
│  ├─ troubleshooting.md
│  └─ github_upload_guide.md
├─ demo/
│  └─ demo_link.md
├─ hardware/
│  └─ parts_list.md
└─ images/
   └─ put_images_here.txt
```

---

## 주요 역할

### Arduino UNO

- 모터 제어
- LCD 상태 표시
- 초음파 센서 거리 측정
- ESP32-CAM 또는 외부 제어 신호 처리

### ESP32-CAM

- 실시간 영상 스트리밍
- 자체 Wi-Fi 또는 기존 Wi-Fi 연결
- 카메라 서버 실행

---

## 시연 결과

- 모바일 기기 기반 주행 조종 성공
- ESP32-CAM 영상 송출 성공
- LCD 상태 표시 성공
- 모터 주행 기능 구현 성공
- 교내 프로젝트 발표에서 **장려상 수상**

---

## 문제 해결 과정

자세한 내용은 [`docs/troubleshooting.md`](docs/troubleshooting.md)를 참고하세요.

대표적인 문제는 다음과 같습니다.

1. ESP32-CAM 영상 끊김 문제
2. 모터 구동 시 전압 강하 문제
3. 배터리 전원 분리 문제
4. 회로 배선 안정성 문제
5. LCD 표시 불안정 문제

---

## 개선 예정

- 회로도 이미지 정리
- 핀맵 최종 검증
- 전원 스위치 접근성 개선
- 영상 송출 안정성 향상
- 배터리 전원 구조 개선
- 외관 및 내부 배선 정리

---

## 주의사항

이 저장소에 업로드하기 전, 아래 정보는 반드시 제거해야 합니다.

- Wi-Fi 이름과 비밀번호
- 개인 전화번호
- 학번, 이름 등 공개하고 싶지 않은 개인정보
- 학교 제출용 원본 파일 중 개인정보가 포함된 자료
- 용량이 큰 원본 영상 파일

