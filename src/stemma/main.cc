// -*- Mode: C++; -*-

#include <InfluxDbClient.h>
#include <Adafruit_seesaw.h>
#include "config.hh"
#include "app.hh"

#define I2C_ADDR    0x36
#define APPLICATION "stemma"

App app(APPLICATION);

Point temperaturePoint("temperature");

Point moisturePoint("moisture");

Point wifiPoint("wifi");

Adafruit_seesaw ss;

void setup() {
  app
    .WithSerial(115200)
    .WithWifi(WIFI_SSID, WIFI_PASSWORD)
    .WithNTP(TZ_INFO)
    .WithInfluxDB2(INFLUXDB_URL,
                   INFLUXDB_ORG,
                   INFLUXDB_BUCKET,
                   INFLUXDB_TOKEN,
                   CERT_R3)
    .Setup();

  // must happen after setup
  app
    .TagPoint(temperaturePoint)
    .TagPoint(moisturePoint)
    .TagPoint(wifiPoint);

  // initialize driver
  if (!ss.begin(I2C_ADDR)) {
    Serial.println("seesaw not found, hanging.");
    while(1);
  } else {
    Serial.print("seesaw started, version: ");
    Serial.println(ss.getVersion(), HEX);
  }
}

void loop() {
  temperaturePoint.clearFields();
  moisturePoint.clearFields();
  wifiPoint.clearFields();

  // collect wifi signal quality
  wifiPoint.addField("rssi", WiFi.RSSI());

  float temperature = ss.getTemp();
  uint16_t capacitiveReading = ss.touchRead(0);
  temperaturePoint.addField("temperature", temperature);
  moisturePoint.addField("moisture", capacitiveReading);

  // write both points to the cloud
  app.WritePoint(wifiPoint);
  app.WritePoint(temperaturePoint);
  app.WritePoint(moisturePoint);

  // Wait a reasonable interval before next loop
  Serial.println("wait 10s");
  delay(10000);
}
