// app.cc --- app for generalizing sensor reporting
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

#include <map>
#include "config.hh"
#include "app.hh"

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

App& App::WithSerial(int br) {
  m_baund_rate = br;
  return *this;
}

App& App::WithWifi(String ssid, String password) {
  m_wifi_sid = ssid;
  m_wifi_pwd = password;
  return *this;
}

App& App::WithNTP(String timezone) {
  m_timezone = timezone;
  return *this;
}

App& App::WithInfluxDB2(const char* url, const char* org, const char* bucket, const char* token, const char* ca) {
  m_influxdb_client.setConnectionParams(url, org, bucket, token, ca);
  return *this;
}

App& App::TagPoint(Point& point) {
  point.addTag("app", m_app_name);
  point.addTag("thing", THING);
  point.addTag("device", DEVICE);
  point.addTag("SSID", WiFi.SSID());
  return *this;
}

void App::Setup() {
  Serial.println("setup: init");

  Serial.begin(m_baund_rate);

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(m_wifi_sid.c_str(), m_wifi_pwd.c_str());

  Serial.print("wifi: connecting");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  timeSync(m_timezone.c_str(), "pool.ntp.org", "time.nis.gov");

  if (m_influxdb_client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(m_influxdb_client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(m_influxdb_client.getLastErrorMessage());
  }
}

void App::WritePoint(Point& point) {
  // if no Wifi, try to reconnect or bail
  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
    Serial.println("Wifi connection lost");
    return;
  }

  Serial.print("Writing: ");
  Serial.println(m_influxdb_client.pointToLineProtocol(point));

  if (!m_influxdb_client.writePoint(point)) {
    Serial.print("InfluxDB write failed[");
    Serial.print(m_influxdb_client.getLastStatusCode());
    Serial.print("]: ");
    Serial.println(m_influxdb_client.getLastErrorMessage());
  }
}
