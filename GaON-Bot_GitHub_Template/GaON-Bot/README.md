GaON Bot | 고독사 방지 원격 소통 로봇
ESP32-CAM 실시간 영상 송출과 Arduino UNO 기반 주행·비접촉 제스처 응답을 결합한 이동형 로봇 프로젝트

[프로젝트 상세 기술 문서](GaON-Bot_GitHub_Template/GaON-Bot/README.md) · [Arduino 제어 코드](GaON-Bot_GitHub_Template/GaON-Bot/arduino_uno/gaonbot_uno.ino) · [ESP32-CAM 코드](GaON-Bot_GitHub_Template/GaON-Bot/esp32_cam/gaonbot_esp32cam.ino) · 시연 영상
프로젝트 소개
GaON Bot은 혼자 생활하는 사용자의 상태를 멀리서 확인하고 간단한 질문에 응답받는 상호작용을 구현하기 위해 제작한 로봇입니다. ESP32-CAM이 자체 Wi-Fi와 카메라 스트리밍 서버를 제공하고, 스마트폰 등 접속 기기에서 주행·질문 명령을 전송합니다. Arduino UNO는 4개의 구동 모터, 16×2 LCD 및 2개의 초음파 센서를 제어합니다.

이 프로젝트는 자율주행 시스템이 아닌 원격 주행·상태 확인·제스처 응답 기능을 구현한 시제품입니다. 사용자 안전성이나 실제 돌봄 서비스 효과가 검증된 제품은 아닙니다.
구현 기능
기능	구현 방식
실시간 영상 확인	ESP32-CAM JPEG 스트리밍 (:81/stream)
원격 주행	HTTP 명령 → ESP32-CAM → UART → Arduino UNO → DC 모터
질문 및 상태 표시	3개의 사전 정의 질문, 16×2 I2C LCD
비접촉 응답	초음파 센서 2개로 YES / NO / NO_RESPONSE 판단
응답 조회	Arduino 결과를 ESP32-CAM이 수신하고 /status로 제공


시스템 구조
[스마트폰 / 웹 클라이언트]
      │ Wi-Fi (ESP32-CAM 자체 AP)
      │ HTTP GET / 영상 스트리밍
      ▼
[ESP32-CAM] ── UART 4,800bps ── [Arduino UNO]
      │                            ├─ DC 모터 4개 / 모터 드라이버
      │                            ├─ I2C LCD 16×2
      │                            └─ 초음파 센서 2개
      └─ /status : YES / NO / NO_RESPONSE
사용 기술
Hardware: ESP32-CAM, Arduino UNO, DC 모터 4개, L9110S 계열 모터 드라이버, 초음파 센서 2개, I2C LCD

Software / Protocol: Arduino C/C++, ESP32 Camera API, HTTP, Wi-Fi AP, UART, I2C
구현 결과 및 의미
기존 프로젝트 기록에 따르면 원격 주행, 영상 송출, LCD 상태 표시 및 모터 주행을 시연했고 교내 프로젝트 발표에서 장려상을 수상했습니다. 핵심 구현은 영상과 제어 기능을 서로 다른 보드로 분리하고, 두 센서의 손동작을 시간 조건에 따라 구분해 결과를 다시 전송한 점입니다.
 
[제작 사진 추가 보기](GaON-Bot_GitHub_Template/GaON-Bot/images/README.md) · [시연 영상 및 검증 범위](GaON-Bot_GitHub_Template/GaON-Bot/demo/README.md)
프로젝트 문서
문서	내용
[상세 README](GaON-Bot_GitHub_Template/GaON-Bot/README.md)	개발 배경, 동작 원리, 전체 파일 구성
[Arduino UNO](GaON-Bot_GitHub_Template/GaON-Bot/arduino_uno/README.md)	모터·LCD·초음파 제스처 제어 및 핀 배치
[ESP32-CAM](GaON-Bot_GitHub_Template/GaON-Bot/esp32_cam/README.md)	AP, 스트리밍, HTTP 명령, UART 프로토콜
[설계 문서](GaON-Bot_GitHub_Template/GaON-Bot/docs/README.md)	회로·핀맵·트러블슈팅 자료 안내
[하드웨어](GaON-Bot_GitHub_Template/GaON-Bot/hardware/README.md)	부품 및 전원 구성
[시연](GaON-Bot_GitHub_Template/GaON-Bot/demo/README.md)	공개 시연 링크 및 확인 항목


한계 및 향후 개선점
- 제스처 인식 중에는 블로킹 루프가 실행되어 명령에 즉각 반응하지 못할 수 있습니다.
- 모터 제어와 통신은 안전 검증이 완료된 상용 시스템이 아닙니다.
- 기존 핀맵 문서와 실제 소스코드의 일부 핀 지정이 다르므로 재현 시 소스코드 기준으로 검토 후 실물 배선을 확인해야 합니다.
- 영상 안정성, 전원 구성 및 제스처 인식 정확도에 대한 정량 측정 자료는 별도로 확보되지 않았습니다.
