// -*- Mode: C++; -*-

#include <InfluxDbClient.h>
#include "config.hh"
#include "app.hh"

#define TMP36GZ_PIN 33
#define APPLICATION "tmp36gz"

App app(APPLICATION);

Point temperaturePoint("temperature");

Point wifiPoint("wifi");

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
    .TagPoint(wifiPoint);
}

void loop() {
  temperaturePoint.clearFields();
  wifiPoint.clearFields();

  // collect wifi signal quality
  wifiPoint.addField("rssi", WiFi.RSSI());

  // collect temperature sensor value
  int sensorValue = analogRead(TMP36GZ_PIN);
  double volts = sensorValue / 1023.0; // 2^10-=
  double temperature = (volts - 0.5) * 100;
  temperaturePoint.addField("temperature", temperature);

  // write both points to the cloud
  app.WritePoint(wifiPoint);
  app.WritePoint(temperaturePoint);

  // Wait a reasonable interval before next loop
  Serial.println("wait 10s");
  delay(10000);
}
