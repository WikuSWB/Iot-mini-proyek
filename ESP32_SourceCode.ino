#include <MFRC522.h>
#include <Arduino.h>

String uid;

#define SS_PIN 5     // Change these pins as needed
#define RST_PIN 27    // Change these pins as needed
MFRC522 rfid(SS_PIN, RST_PIN);

// Buzzer setup
#define BUZZER_PIN 26  // Change this to the pin where your buzzer is connected

//---- WiFi ----
#include <WiFi.h>
#include <HTTPClient.h>

HTTPClient http;
String Link, LinkTiming, Mode, Status, payload;
int httpCode;

const char *ssid = "Area 1";
const char *password = "1234abcd";
const char* host = "192.168.43.55";

byte statusKoneksi;

void connectToWiFi() {
  int attempts = 0;
  const int maxAttempts = 5;

  // Attempt to connect to WiFi
  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    delay(5000); // Wait for the connection
    attempts++;
  }

  // Check the connection result
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
  } else {
    Serial.println("WiFi failed to connect after several tries");
  }
}

void checkUID() {
  String startCheck = "Start";

  Link = "http://" + String(host) + "/aplikasilayar/checkUID.php?State=" + startCheck + "&UID=" + uid;

  httpGet();
}

void httpGet()
{
  http.begin(Link);
  httpCode = http.GET();
  payload = http.getString();
  Serial.println("HTTP GET response: " + String(httpCode));
  Serial.println(payload);
  http.end();
  // Sound the buzzer after making the HTTP request
  soundBuzzer();
}

void soundBuzzer() {
  // Sound the buzzer for a short duration
  tone(BUZZER_PIN, 700); // Adjust the frequency as needed
  delay(100); // Adjust the duration as needed
  noTone(BUZZER_PIN);
}

void readRFID() {
  // Look for new cards
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // Print UID to serial monitor
    Serial.print("UID Value: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      uid += String(rfid.uid.uidByte[i]);
    }

    Serial.println("UID is: ");
    Serial.print(uid);

    checkUID();

    // Construct the UID string
    uid = "";
  }
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  // Buzzer setup
  pinMode(BUZZER_PIN, OUTPUT);

  connectToWiFi();
}

void loop() {
  readRFID();
}
