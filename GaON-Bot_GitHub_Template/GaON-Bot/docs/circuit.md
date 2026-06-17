# Circuit Description

가온봇의 회로는 크게 4개 블록으로 구성됩니다.

## 1. 제어부

- Arduino UNO가 전체 주행 제어를 담당합니다.
- 모터 드라이버, LCD, 초음파 센서와 연결됩니다.

## 2. 영상 송출부

- ESP32-CAM이 카메라 영상을 송출합니다.
- 모바일 기기에서 ESP32-CAM의 스트리밍 주소에 접속하여 영상을 확인합니다.

## 3. 구동부

- L911S 모터 드라이버를 사용하여 DC 모터를 제어합니다.
- Arduino UNO의 디지털 출력 핀으로 모터 방향을 제어합니다.

## 4. 전원부

- ESP32-CAM은 보조배터리 5V 출력 사용을 권장합니다.
- 모터 구동부는 별도 전원을 사용하는 것이 안정적입니다.
- 모터와 ESP32-CAM을 같은 전원에 연결하면 전압 강하로 영상이 끊길 수 있습니다.

## 회로도 이미지 넣는 위치

회로도 이미지가 있다면 아래 경로에 넣고 README에 연결하세요.

```text
images/circuit_diagram.png
```

README에 넣는 예시:

```md
![Circuit Diagram](images/circuit_diagram.png)
```

