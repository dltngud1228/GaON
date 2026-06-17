/*
  GaON Bot - ESP32-CAM Code

  역할:
  - ESP32-CAM 영상 스트리밍
  - Wi-Fi 접속 또는 자체 AP 구성
  - 필요 시 Arduino UNO로 명령 전송

  중요:
  - GitHub에 올릴 때 실제 Wi-Fi 이름/비밀번호를 절대 넣지 마세요.
  - 아래 YOUR_WIFI_SSID, YOUR_WIFI_PASSWORD 부분을 실제 사용 시에만 수정하세요.
  - 공개 저장소에는 개인정보와 비밀번호를 올리지 마세요.

  TODO:
  - 실제 사용했던 ESP32-CAM 최종 코드 붙여넣기
  - 카메라 모델 설정 확인
  - 자체 Wi-Fi AP 방식인지, 공유기 연결 방식인지 선택
*/

#include "esp_camera.h"
#include <WiFi.h>

// =======================
// Wi-Fi Settings
// =======================
// GitHub 공개용 파일에는 실제 비밀번호를 적지 마세요.

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Arduino UNO로 명령을 보낼 때 사용할 핀 예시
// 실제 가온봇에서는 ESP32-CAM GPIO13 -> UNO A0로 연결했던 구성을 사용
const int UNO_TX_PIN = 13;

// =======================
// Camera Pin Settings
// AI Thinker ESP32-CAM 기준 예시
// =======================

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// 카메라 서버 함수는 기존 ESP32-CAM CameraWebServer 예제에서 사용
void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  pinMode(UNO_TX_PIN, OUTPUT);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;

  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  delay(10000);
}

/*
  참고:
  Arduino IDE의 예제:
  File > Examples > ESP32 > Camera > CameraWebServer

  위 예제의 app_httpd.cpp, camera_index.h 등이 함께 필요할 수 있습니다.
  이 저장소에는 기본 구조만 넣어두었습니다.
*/
