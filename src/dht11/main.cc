// -*- Mode: C++; -*-

#include "config.hh"
#include "app.hh"
#include "DHT.h"

#define DHT11_PIN 23
#define APPLICATION "dht11"

App app(APPLICATION);
DHT dht(DHT11_PIN, DHT11);

Point temperaturePoint("temperature");
Point humidityPoint("humidity");
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

  app
    .TagPoint(temperaturePoint)
    .TagPoint(humidityPoint)
    .TagPoint(wifiPoint);

  dht.begin();
}

void loop() {
  temperaturePoint.clearFields();
  humidityPoint.clearFields();
  wifiPoint.clearFields();

  // collect wifi signal quality
  wifiPoint.addField("rssi", WiFi.RSSI());

  // collect temperature & humidity from sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("ºC ");
  Serial.print("Humidity: ");
  Serial.println(humidity);
  temperaturePoint.addField("temperature", temperature);
  humidityPoint.addField("humidity", humidity);

  // write all points to the cloud
  app.WritePoint(wifiPoint);
  app.WritePoint(temperaturePoint);
  app.WritePoint(humidityPoint);

  // Wait 10s
  Serial.println("Wait 10s");
  delay(10000);
}
