#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"

// =====================
// ESP32-CAM 핀
// =====================
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

// =====================
// 자체 Wi-Fi
// =====================
const char* ssid = "GAONBOT";
const char* password = "12345678";

// =====================
// UNO 통신 핀
// ESP32 IO13 → UNO D0
// UNO D1 → ESP32 IO14
// =====================
#define UNO_RX_PIN 14
#define UNO_TX_PIN 13

HardwareSerial UnoSerial(2);

// 앱인벤터에서 읽을 결과값
String lastResult = "NONE";

// 서버 핸들
httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

// 스트리밍 설정
#define PART_BOUNDARY "123456789000000000000987654321"

static const char* _STREAM_CONTENT_TYPE =
  "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;

static const char* _STREAM_BOUNDARY =
  "\r\n--" PART_BOUNDARY "\r\n";

static const char* _STREAM_PART =
  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// =====================
// MOTOR, 메시지 제어
// =====================
void sendCommandToUno(String cmd) {
  cmd.toUpperCase();

  char sendChar = ' ';

  if (cmd == "F") sendChar = 'F';
  else if (cmd == "B") sendChar = 'B';
  else if (cmd == "L") sendChar = 'L';
  else if (cmd == "R") sendChar = 'R';
  else if (cmd == "S" || cmd == "STOP") sendChar = 'S';
  else if (cmd == "M1" || cmd == "1") sendChar = '1';
  else if (cmd == "M2" || cmd == "2") sendChar = '2';
  else if (cmd == "M3" || cmd == "3") sendChar = '3';
  else if (cmd == "T") sendChar = 'T';

  if (sendChar != ' ') {
    UnoSerial.write(sendChar);

    Serial.print("Send to UNO: ");
    Serial.println(sendChar);
  }
}

// =====================
// 기본 페이지
// =====================
static esp_err_t index_handler(httpd_req_t *req) {
  String html = "";
  html += "<html><body>";
  html += "<h2>GAONBOT Server</h2>";
  html += "<p>Stream: <a href='http://192.168.4.1:81/stream'>Open Stream</a></p>";
  html += "<p>Status: <a href='/status'>/status</a></p>";
  html += "<p>Commands: /F /B /L /R /S /M1 /M2 /M3</p>";
  html += "</body></html>";

  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, html.c_str(), html.length());
}

// =====================
// 앱인벤터 결과 확인 주소
// =====================
static esp_err_t status_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/plain");
  return httpd_resp_send(req, lastResult.c_str(), lastResult.length());
}

// =====================
// 명령 처리
// =====================
static esp_err_t command_handler(httpd_req_t *req) {
  String uri = String(req->uri);
  uri.replace("/", "");
  uri.toUpperCase();

  Serial.print("HTTP Command: ");
  Serial.println(uri);

  // 새 질문 보내면 이전 결과 초기화
  if (uri == "M1" || uri == "M2" || uri == "M3" ||
      uri == "1" || uri == "2" || uri == "3") {
    lastResult = "NONE";
  }

  sendCommandToUno(uri);

  httpd_resp_set_type(req, "text/plain");
  return httpd_resp_send(req, "OK", 2);
}

// =====================
// 카메라 스트리밍
// =====================
static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  char part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);

  if (res != ESP_OK) {
    return res;
  }

  while (true) {
    fb = esp_camera_fb_get();

    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    }
    else {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));

      if (res == ESP_OK) {
        size_t hlen = snprintf(part_buf, 64, _STREAM_PART, fb->len);
        res = httpd_resp_send_chunk(req, part_buf, hlen);
      }

      if (res == ESP_OK) {
        res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
      }

      esp_camera_fb_return(fb);
      fb = NULL;
    }

    if (res != ESP_OK) {
      break;
    }
  }

  return res;
}

// =====================
// 서버 시작
// =====================
void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  config.max_uri_handlers = 20;

  httpd_uri_t index_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_handler,
    .user_ctx = NULL
  };

  httpd_uri_t status_uri = {
    .uri = "/status",
    .method = HTTP_GET,
    .handler = status_handler,
    .user_ctx = NULL
  };

  httpd_uri_t f_uri = { .uri="/F", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };
  httpd_uri_t b_uri = { .uri="/B", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };
  httpd_uri_t l_uri = { .uri="/L", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };
  httpd_uri_t r_uri = { .uri="/R", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };
  httpd_uri_t s_uri = { .uri="/S", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };
  httpd_uri_t stop_uri = { .uri="/STOP", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };

  httpd_uri_t m1_uri = { .uri="/M1", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };
  httpd_uri_t m2_uri = { .uri="/M2", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };
  httpd_uri_t m3_uri = { .uri="/M3", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };

  httpd_uri_t one_uri = { .uri="/1", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };
  httpd_uri_t two_uri = { .uri="/2", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };
  httpd_uri_t three_uri = { .uri="/3", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };

  httpd_uri_t t_uri = { .uri="/T", .method=HTTP_GET, .handler=command_handler, .user_ctx=NULL };

  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &status_uri);

    httpd_register_uri_handler(camera_httpd, &f_uri);
    httpd_register_uri_handler(camera_httpd, &b_uri);
    httpd_register_uri_handler(camera_httpd, &l_uri);
    httpd_register_uri_handler(camera_httpd, &r_uri);
    httpd_register_uri_handler(camera_httpd, &s_uri);
    httpd_register_uri_handler(camera_httpd, &stop_uri);

    httpd_register_uri_handler(camera_httpd, &m1_uri);
    httpd_register_uri_handler(camera_httpd, &m2_uri);
    httpd_register_uri_handler(camera_httpd, &m3_uri);

    httpd_register_uri_handler(camera_httpd, &one_uri);
    httpd_register_uri_handler(camera_httpd, &two_uri);
    httpd_register_uri_handler(camera_httpd, &three_uri);

    httpd_register_uri_handler(camera_httpd, &t_uri);
  }

  httpd_config_t stream_config = HTTPD_DEFAULT_CONFIG();
  stream_config.server_port = 81;
  stream_config.ctrl_port = 32769;

  httpd_uri_t stream_uri = {
    .uri = "/stream",
    .method = HTTP_GET,
    .handler = stream_handler,
    .user_ctx = NULL
  };

  if (httpd_start(&stream_httpd, &stream_config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

// =====================
// SETUP
// =====================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // RX=14, TX=13
  UnoSerial.begin(4800, SERIAL_8N1, UNO_RX_PIN, UNO_TX_PIN);

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

  // 보드 코어 버전에 따라 아래 2줄에서 오류 나면
  // pin_sscb_sda → pin_sccb_sda
  // pin_sscb_scl → pin_sccb_scl 로 바꾸면 됨
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
  }
  else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 15;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  // 카메라 상하 반전
  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  s->set_hmirror(s, 0);

  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("GAONBOT WiFi Started");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("PASSWORD: ");
  Serial.println(password);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  startCameraServer();

  Serial.println("Server started");
  Serial.println("Main: http://192.168.4.1/");
  Serial.println("Stream: http://192.168.4.1:81/stream");
  Serial.println("Status: http://192.168.4.1/status");
}

// =====================
// LOOP
// UNO에서 Y/N/X 받기
// =====================
void loop() {
  if (UnoSerial.available()) {
    char result = UnoSerial.read();

    Serial.print("From UNO: ");
    Serial.println(result);

    if (result == 'Y') {
      lastResult = "YES";
      Serial.println("Gesture Result: YES");
    }
    else if (result == 'N') {
      lastResult = "NO";
      Serial.println("Gesture Result: NO");
    }
    else if (result == 'X') {
      lastResult = "NO_RESPONSE";
      Serial.println("Gesture Result: NO_RESPONSE");
    }
  }

  delay(10);
}

  위 예제의 app_httpd.cpp, camera_index.h 등이 함께 필요할 수 있습니다.
  이 저장소에는 기본 구조만 넣어두었습니다.
*/
