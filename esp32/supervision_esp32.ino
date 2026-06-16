#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* SSID         = "INTERN";
const char* PASSWORD     = "CIEL2026*";
const char* MQTT_SERVEUR = "192.168.1.110";
const int   MQTT_PORT    = 1883;

#define DHT_PIN 15

Adafruit_SSD1306 ecran(128, 64, &Wire, -1);
DHTesp           dht;
WiFiClient       client;
PubSubClient     mqtt(client);

float temperature = 0;
float humidite    = 0;
bool  dhtOK       = true;
unsigned long derniereLecture = 0;

void afficherEcran() {
  ecran.clearDisplay();

  ecran.fillRect(0, 0, 128, 13, WHITE);
  ecran.setTextColor(BLACK);
  ecran.setTextSize(1);
  ecran.setCursor(18, 3);
  ecran.print("SUPERVISION ESP32");

  ecran.setTextColor(WHITE);
  ecran.setTextSize(1);

  ecran.setCursor(0, 22);
  ecran.print("Temp     :");
  ecran.setCursor(72, 22);
  if (!dhtOK) ecran.print("ERREUR");
  else { ecran.print(temperature, 1); ecran.print(" C"); }

  ecran.setCursor(0, 38);
  ecran.print("Humidite :");
  ecran.setCursor(72, 38);
  if (!dhtOK) ecran.print("ERREUR");
  else { ecran.print(humidite, 1); ecran.print(" %"); }

  ecran.drawLine(0, 52, 128, 52, WHITE);

  ecran.setCursor(0, 56);
  ecran.print("Abdel Mrad N3");

  ecran.display();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Demarrage ===");

  if (!ecran.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERREUR OLED");
    while (1);
  }
  Serial.println("OLED OK");

  dht.setup(DHT_PIN, DHTesp::DHT22);
  Serial.println("DHT22 OK");

  ecran.clearDisplay();
  ecran.setTextColor(WHITE);
  ecran.setTextSize(1);
  ecran.setCursor(15, 10);
  ecran.print("Portail Captif");
  ecran.setCursor(18, 22);
  ecran.print("Henri Brisson");
  ecran.drawLine(0, 34, 128, 34, WHITE);
  ecran.setCursor(20, 42);
  ecran.print("Demarrage...");
  ecran.setCursor(22, 54);
  ecran.print("Abdel Mrad N3");
  ecran.display();
  delay(3000);

  Serial.print("WiFi...");
  WiFi.begin(SSID, PASSWORD);
  int essais = 0;
  while (WiFi.status() != WL_CONNECTED && essais < 40) {
    delay(500);
    Serial.print(".");
    essais++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" OK — IP : " + WiFi.localIP().toString());
  } else {
    Serial.println(" ECHEC");
  }

  mqtt.setServer(MQTT_SERVEUR, MQTT_PORT);
  Serial.print("MQTT...");
  if (mqtt.connect("ESP32_DHT")) {
    Serial.println(" OK");
  } else {
    Serial.println(" ECHEC code=" + String(mqtt.state()));
  }

  Serial.println("=== Pret ===");
}

void loop() {
  mqtt.loop();

  if (millis() - derniereLecture >= 3000) {
    derniereLecture = millis();
    Serial.println("──────────────────────────────");

    TempAndHumidity data = dht.getTempAndHumidity();
    if (dht.getStatus() == 0) {
      dhtOK       = true;
      temperature = data.temperature;
      humidite    = data.humidity;
      Serial.print("Temperature : "); Serial.print(temperature, 1); Serial.println(" C");
      Serial.print("Humidite    : "); Serial.print(humidite, 1);    Serial.println(" %");
    } else {
      dhtOK = false;
      Serial.println("Temperature : ERREUR DHT22");
      Serial.println("Humidite    : ERREUR DHT22");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi        : OK (" + WiFi.localIP().toString() + ")");
    } else {
      Serial.println("WiFi        : HORS LIGNE — reconnexion...");
      WiFi.begin(SSID, PASSWORD);
    }

    if (mqtt.connected()) {
      Serial.println("MQTT        : OK");
      char buf[16];
      snprintf(buf, sizeof(buf), "%.1f", temperature);
      mqtt.publish("iot/temperature", buf);
      snprintf(buf, sizeof(buf), "%.1f", humidite);
      mqtt.publish("iot/humidite", buf);
      Serial.println("Envoi MQTT  : OK");
    } else {
      Serial.println("MQTT        : HORS LIGNE — reconnexion...");
      mqtt.connect("ESP32_DHT");
    }

    afficherEcran();
  }
}
