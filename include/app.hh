#ifndef app_h
#define app_h

#include <map>
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
