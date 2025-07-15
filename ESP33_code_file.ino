#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUdp.h>
#include <Preferences.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wi-Fi
const char* ssid = "wifi_name";
const char* password = "wifi_pass";

// Web Server
WebServer server(80);
Preferences preferences;

// Relay
const int relayPin = 27;

// System state
String currentMode = "CLASS";
const String correctPassword = "7900";
String customLabel1 = "Class Time";
String customLabel2 = "Exam Mode";
String customLabel3 = "Custom Event";
String customTimes[9];  // 9 bell times
String lastBell = "";

unsigned long lastAdMillis = 0;

// Login Page
String loginPage = R"rawliteral(
<html><body><h2>Enter Password</h2>
<form action='/login' method='POST'>
Password: <input type='password' name='pass'>
<input type='submit' value='Login'>
</form></body></html>
)rawliteral";

// Mode Selection Page
String generateModePage() {
  String page = "<html><body><h2>Select Mode</h2>";
  page += "<form action='/setmode' method='POST'>";
  page += "<select name='mode'>";
  page += "<option value='CLASS'>" + customLabel1 + "</option>";
  page += "<option value='EXAM'>" + customLabel2 + "</option>";
  page += "<option value='CUSTOM'>" + customLabel3 + "</option>";
  page += "</select><br><br>";
  page += "Custom Label 1: <input name='label1' value='" + customLabel1 + "'><br>";
  page += "Custom Label 2: <input name='label2' value='" + customLabel2 + "'><br>";
  page += "Custom Label 3: <input name='label3' value='" + customLabel3 + "'><br><br>";
  for (int i = 0; i < 9; i++) {
    page += "Bell Time " + String(i + 1) + " (HH:MM): <input name='bell" + String(i) + "' value='" + customTimes[i] + "'><br>";
  }
  page += "<br><input type='submit' value='Apply'>";
  page += "</form><br><br>";
  page += "<p>ESP32 Status: ✅ Connected</p></body></html>";
  return page;
}

void showAdScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("SA-TOWER");
  display.setTextSize(1);
  display.setCursor(35, 45);
  display.println("by SANJEY");
  display.display();
  delay(3000);
  lastAdMillis = millis();
}

// ✅ Get formatted time (HH:MM) using internal RTC
String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "00:00";
  char buf[6];
  strftime(buf, sizeof(buf), "%H:%M", &timeinfo);
  return String(buf);
}

// ✅ Ring bell continuously
void ringBell(int durationMs) {
  digitalWrite(relayPin, LOW);  // Relay ON
  delay(durationMs);
  digitalWrite(relayPin, HIGH); // Relay OFF

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(20, 20);
  display.setTextColor(WHITE);
  display.println(" BELL");
  display.display();
  delay(1000);
}

void checkBellSchedule() {
  String currentTime = getFormattedTime();
  String times[9];
  int durations[9];

  if (currentMode == "CLASS") {
    String classTimes[9] = {
      "08:45", "09:35", "10:25", "10:45", "12:25", "13:25", "15:05", "15:25", "16:15"
    };
    int classDurations[9] = {
      3000, 3000, 6000, 3000, 6000, 3000, 6000, 3000, 6000
    };
    memcpy(times, classTimes, sizeof(times));
    memcpy(durations, classDurations, sizeof(durations));
  } else if (currentMode == "EXAM") {
    String examTimes[9] = {
      "09:30", "12:00", "12:30", "13:30", "14:15", "16:15", "00:00", "00:01", "00:02"
    };
    for (int i = 0; i < 9; i++) durations[i] = 3000;
    memcpy(times, examTimes, sizeof(times));
  } else if (currentMode == "CUSTOM") {
    memcpy(times, customTimes, sizeof(times));
    for (int i = 0; i < 9; i++) durations[i] = 3000;
  }

  for (int i = 0; i < 9; i++) {
    if (times[i] == currentTime && lastBell != currentTime) {
      ringBell(durations[i]);
      lastBell = currentTime;
    }
  }
}

void updateDisplay() {
  if (millis() - lastAdMillis >= 10000) showAdScreen();

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(5, 10);
  display.println("  " + getFormattedTime());
  display.setCursor(35, 45);
  display.setTextSize(1);
  display.println(currentMode == "CLASS" ? customLabel1 : currentMode == "EXAM" ? customLabel2 : customLabel3);
  display.display();
}

void applyMode() {
  digitalWrite(relayPin, HIGH);  // Relay OFF default
  updateDisplay();
}

void handleRoot() {
  server.send(200, "text/html", loginPage);
}

void handleLogin() {
  if (server.hasArg("pass") && server.arg("pass") == correctPassword)
    server.send(200, "text/html", generateModePage());
  else
    server.send(200, "text/html", "<h2>Wrong password</h2><a href='/'>Try Again</a>");
}

void handleSetMode() {
  if (server.hasArg("mode")) {
    currentMode = server.arg("mode");
    customLabel1 = server.arg("label1");
    customLabel2 = server.arg("label2");
    customLabel3 = server.arg("label3");

    preferences.begin("settings", false);
    preferences.putString("mode", currentMode);
    preferences.putString("label1", customLabel1);
    preferences.putString("label2", customLabel2);
    preferences.putString("label3", customLabel3);

    for (int i = 0; i < 9; i++) {
      customTimes[i] = server.arg("bell" + String(i));
      preferences.putString(("bell" + String(i)).c_str(), customTimes[i]);
    }

    preferences.end();
    applyMode();
    server.send(200, "text/html", "<h2>Mode set to: " + currentMode + "</h2><a href='/'>Back</a>");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);  // Default OFF

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }
  display.clearDisplay();
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected: " + WiFi.localIP().toString());

  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");  // IST time zone

  preferences.begin("settings", false);
  currentMode = preferences.getString("mode", "CLASS");
  customLabel1 = preferences.getString("label1", "Class Time");
  customLabel2 = preferences.getString("label2", "Exam Mode");
  customLabel3 = preferences.getString("label3", "Custom Event");
  for (int i = 0; i < 9; i++) {
    customTimes[i] = preferences.getString(("bell" + String(i)).c_str(), "");
  }
  preferences.end();

  showAdScreen();
  applyMode();

  server.on("/", handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/setmode", HTTP_POST, handleSetMode);
  server.begin();
}

void loop() {
  server.handleClient();
  checkBellSchedule();
  updateDisplay();
  delay(1000);
}
