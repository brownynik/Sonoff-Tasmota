/*
  user_config_override.h - user configuration overrides user_config.h for Sonoff-Tasmota

  Copyright (C) 2017  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*****************************************************************************************************\
 * ATTENTION: - Changes to most PARAMETER defines will only override flash settings if you change
 *              define CFG_HOLDER. 
 *            - Expect compiler warnings when no ifdef/undef/endif sequence is used.
 *            - You still need to update user_config.h for major defines MODULE and USE_MQTT_TLS.
 *            - Changing MODULE defines are not being tested for validity as they are in user_config.h.
 *            - Most parameters can be changed online using commands via MQTT, WebConsole or serial.
 *            - So I see no use in this but anyway, your on your own.
\*****************************************************************************************************/

// Examples
//#ifdef CFG_HOLDER
//#undef CFG_HOLDER
//#endif
//#define CFG_HOLDER             0x20161210

//#ifdef STA_SSID1
//#undef STA_SSID1
//#endif
//#define STA_SSID1              "yourssid1"

// -- Wifi -----------------------------------
#define STA_SSID1              "Browny WLAN"      // [Ssid1] Wifi SSID
#define STA_PASS1              "yourpasword"  // [Password1] Wifi password

// -- Wifi ----------------------------------------
// #define WIFI_IP_ADDRESS        "0.0.0.0"         // [IpAddress1] Set to 0.0.0.0 for using DHCP or IP address
#define WIFI_IP_ADDRESS        "192.168.1.151"         // [IpAddress1] Set to 0.0.0.0 for using DHCP or IP address
#define WIFI_GATEWAY           "192.168.1.51"   // {IpAddress2] If not using DHCP set Gateway IP address
#define WIFI_SUBNETMASK        "255.255.255.0"   // [IpAddress3] If not using DHCP set Network mask
#define WIFI_DNS               "192.168.1.51"    // [IpAddress4] If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)

// -- MQTT ----------------------------------------
#define MQTT_HOST            "mosquitto"     // [MqttHost]
#define MQTT_PORT            1883         // [MqttPort] MQTT port (10123 on CloudMQTT)
#define MQTT_USER            "esp8266"  // [MqttUser] Optional user
#define MQTT_PASS            "yourpassword"  // [MqttPassword] Optional password

//#define DOMOTICZ_UPDATE_TIMER  0               // [DomoticzUpdateTimer] Send relay status (0 = disable, 1 - 3600 seconds) (Optional)
//#define TELE_PERIOD            300               // [TelePeriod] Telemetry (0 = disable, 10 - 3600 seconds)
#define DOMOTICZ_UPDATE_TIMER  1               // [DomoticzUpdateTimer] Send relay status (0 = disable, 1 - 3600 seconds) (Optional)
#define TELE_PERIOD            0               // [TelePeriod] Telemetry (0 = disable, 10 - 3600 seconds)

// -- IoT Manager (http://iotmanager.ru/exchange/)
#define USE_IOTMANAGER

//#ifdef USE_DOMOTICZ 
//  #undef USE_DOMOTICZ
//#endif
