// app.hh --- app for generalizing sensor reporting
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

#ifndef app_h
#define app_h

#include <InfluxDbClient.h>

class App {

public:
  App(String name): m_app_name(name) {}
  App& WithSerial(int br);
  App& WithWifi(String ssid, String password);
  App& WithNTP(String timezone);
  App& WithInfluxDB2(const char* url,
                     const char* org,
                     const char* bucket,
                     const char* token,
                     const char* cacert);
  App& TagPoint(Point& point);
  void Setup();
  void WritePoint(Point& point);

private:
  String m_app_name;
  int m_baund_rate;
  String m_wifi_sid;
  String m_wifi_pwd;
  String m_timezone;
  InfluxDBClient m_influxdb_client;
};

#endif  // app_h
