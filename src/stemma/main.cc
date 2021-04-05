// main.cc --- arduino app for reading stemma
//
// Author: Lincoln Clarete <lincoln@clarete.li>
//
// Copyright (C) 2021  Lincoln Clarete
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
