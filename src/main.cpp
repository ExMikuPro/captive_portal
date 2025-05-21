#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>

#define EEPROM_SIZE 96
#define SSID_ADDR 0
#define PASS_ADDR 32

ESP8266WebServer server(80);
DNSServer dnsServer;

// 生成 WiFi 列表选项
String generateWiFiOptions() {
  int n = WiFi.scanNetworks();
  String options = "";
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    options += "<option value='" + ssid + "' onclick=\"document.getElementById('ssidInput').value='" + ssid + "'\">" + ssid + "</option>";
  }
  return options;
}

// 生成 HTML 页面
String generateConfigPage() {
  String options = generateWiFiOptions();
  return "<!DOCTYPE html><html lang='zh-CN'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
         "<title>WiFi 配网</title><style>body{display:flex;justify-content:center;align-items:center;flex-direction:column;min-height:100vh;margin:0;font-family:sans-serif;background-color:#f4f4f4}.container{background:#fff;padding:20px;border-radius:12px;box-shadow:0 2px 10px rgba(0,0,0,0.1);width:90%;max-width:400px;text-align:center}select,input[type='text'],input[type='password'],input[type='submit']{width:100%;padding:10px;margin-top:10px;border-radius:6px;border:1px solid #ccc;box-sizing:border-box;font-size:16px;outline:none;transition:border-color 0.3s,box-shadow 0.3s}input[type='password']:focus,input[type='text']:focus,select:focus{border-color:#4CAF50;box-shadow:0 0 4px rgba(76,175,80,0.5)}input[type='submit']{background-color:#4CAF50;color:white;border:none;cursor:pointer}input[type='submit']:hover{background-color:#45a049}</style></head><body><div class='container'><h2>WiFi 配置</h2><form method='POST' action='/connect'><select onchange=\"document.getElementById('ssidInput').value=this.value\">" + options + "</select><input type='text' id='ssidInput' name='ssid' placeholder='或手动输入WiFi名称'><input type='password' name='password' placeholder='密码'><input type='submit' value='保存并连接'></form></div></body></html>";
}

// 保存 WiFi 配置并重启
void handleConnect() {
  if (!server.hasArg("ssid") || !server.hasArg("password")) {
    server.send(400, "text/html", "<meta charset='UTF-8'><b>缺少参数</b>");
    return;
  }

  String ssid = server.arg("ssid");
  String pass = server.arg("password");

  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; i++) {
    EEPROM.write(SSID_ADDR + i, i < ssid.length() ? ssid[i] : 0);
    EEPROM.write(PASS_ADDR + i, i < pass.length() ? pass[i] : 0);
  }
  EEPROM.commit();

  server.send(200, "text/html", "<meta charset='UTF-8'><b>WiFi信息已保存，正在重启连接...</b>");
  delay(1500);
  ESP.restart();
}

// 从 EEPROM 读取 WiFi 配置
void readWiFiConfig(String &ssid, String &pass) {
  char buf[33];
  for (int i = 0; i < 32; i++) buf[i] = EEPROM.read(SSID_ADDR + i);
  buf[32] = 0;
  ssid = String(buf);
  for (int i = 0; i < 32; i++) buf[i] = EEPROM.read(PASS_ADDR + i);
  buf[32] = 0;
  pass = String(buf);
}

// 清除 WiFi 配置
void clearWiFiConfig() {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 64; i++) EEPROM.write(i, 0);
  EEPROM.commit();
  Serial.println("WiFi 配置已清除");
}

// 启动强制门户模式
void startCaptivePortal() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("ESP-Setup", "12345678");
  dnsServer.start(53, "*", WiFi.softAPIP());

  server.on("/", HTTP_GET, []() {
      server.send(200, "text/html", generateConfigPage());
  });

  server.on("/connect", HTTP_POST, handleConnect);

  // Captive Portal 探测路径兼容
  server.on("/generate_204", []() {
      server.sendHeader("Location", "/", true);
      server.send(302, "text/plain", "");
  });
  server.on("/hotspot-detect.html", []() {
      server.send(200, "text/html", generateConfigPage());
  });
  server.on("/ncsi.txt", []() {
      server.send(200, "text/plain", "Microsoft NCSI");
  });

  server.begin();
}

// 尝试连接 WiFi
void tryConnectWiFi() {
  String ssid, pass;
  readWiFiConfig(ssid, pass);

  if (ssid.length()) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    startCaptivePortal();
  } else {
    Serial.print("连接成功，IP地址: ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "clear") {
      clearWiFiConfig();
    }
  }

  tryConnectWiFi();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "clear") {
      clearWiFiConfig();
      ESP.restart();
    }
  }
}
