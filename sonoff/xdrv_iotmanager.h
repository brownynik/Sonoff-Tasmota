#ifdef USE_IOTMANAGER
#ifndef IOTMANAGER_H
#define IOTMANAGER_H

/********************************************** WEB LANGUAGE () ************************************************************************/
#define IOTM_LANG_EN            1
#define IOTM_LANG_RU            2
#define IOTM_LANG               IOTM_LANG_EN

/* разводка по моделям
#define SONOFF_BASIC              1
#define SONOFF_RF                 2
#define SONOFF_SV                 3
#define SONOFF_4CH                4
*/

#define IOTM_PREFIX            "IoTmanager"                  // IoT manager prefix
#define IOTM_ROOT              "/%s"                         // IoT manager root topic
#define IOTM_CONTROL           "/%s/%s/+/control"
#define IOTM_REQUEST           "/%s/+/request"
#define IOTM_RESPONSE          "/%s/%s/response"
#define IOTM_CONFIG            "/%s/%s/config"
#define IOTM_STATUS            "/%s/%s/%s/status"

#define IOTM_RELAYWIDGET      "\"widget\":\"toggle\",\"descr\":\"Relay %c\",\"descrClass\":\"balanced-900 text-center\",\"descrStyle\":\"background-color:#0F0;border:solid 2px #F00;font-size:20px;font-weight:bold;\",\"descrStyleOff\":\"background-color:#0F0;border:solid 2px #000;font-size:20px;font-weight:bold;\",\"iconClass\":\"ion-lightbulb\",\"iconStyle\":\"color:red;font-size:25px;\",\"iconStyleOff\":\"color:black;font-size:25px;\",\"trackStyle\":\"background-color:red;\",\"trackStyleOff\":\"background-color:white;\",\"handleStyle\":\"background-color:yellow;\",\"handleStyleOff\":\"background-color:green;\""
#define IOTM_PAGENAME         "LivingRoom"
#define IOTM_PAGEID           10
#define IOTM_PAGECOUNT        4
#define IOTM_RELAYNAME        "relay" // relay1 - relay4, lightswitch1 - lightswitch4

/************************** SENSOR'S ********************************************************************************************************************************/
#define IOTM_SENSORNAME       "sensor"
#define IOTM_SENSORWIDGET     "\"widget\":\"anydata\",\"class1\":\"item no-border\",\"style2\":\"font-size:18px;float:left\",\"descr\":\"Sensor %c\",\"class3\":\"balanced\",\"style3\":\"font-size:18px;font-weight:bold;float:right\""

#define  SENSORVALUESZ  50

#define IOTM_SENSORPUBINTERVAL_MAX  3600 // интервал отправки сообщения от датчика - максимальный
#define IOTM_SENSORPUBINTERVAL_DEF  10    // интервал отправки сообщения от датчика - по умолчанию


#if MODULE == SONOFF_BASIC 
#define  IOTM_RELAYCOUNT      1
#define  IOTM_SENSORCOUNT     0 // развести потом это значение по моделям
#endif

#if MODULE == SONOFF_DUAL 
#define  IOTM_RELAYCOUNT      2
#define  IOTM_SENSORCOUNT     0
#endif

#if MODULE == SONOFF_4CH 
#define  IOTM_RELAYCOUNT      4
#define  IOTM_SENSORCOUNT     0
#endif

#if MODULE == SONOFF_DEV
#define  IOTM_RELAYCOUNT      0
#define  IOTM_SENSORCOUNT     4
#endif





#ifndef IOTM_RELAYCOUNT
#define IOTM_RELAYCOUNT       0
#endif

#ifndef IOTM_SENSORCOUNT
#define IOTM_SENSORCOUNT      0
#endif

//#else
//  #define  IOTM_RELAYCOUNT       0
//  #define  IOTM_SENSORCOUNT      0 // развести потом это значение по моделям
//#endif

#define IOTM_SNTYPECOUNT     10 // must be equal by sensor_t range
#define IOTM_SNSUFFIXZ        5 // "Wt", "A", "V", "dB", etc.
#define IOTM_SNSUFFIXTITLEZ  50 // "Temperature", "Humidity", "V", "dB", etc.

// типы сенсоров: отсутствует, температура, влажность, давление, сила тока, напряжение, мощность тока, уровень шума, уровень CO2, освещённость
enum sensor_t   {stEmpty = 0, stTemperature, stHumidity, stPressure, stCurrent, stVoltage, stElectricPower, stNoiseLevel, stCO2Level, stLuminanceLevel};
enum sensorupdateflag_t  {sufNotUpdated = 0, sufUpdated, sufSkipUpdate};

const char iotmSensorUnitSuffix[IOTM_SNTYPECOUNT][IOTM_SNSUFFIXZ] /*PROGMEM*/ = {
#if IOTM_LANG == IOTM_LANG_RU
  {""},
  #if TEMP_CONVERSION == 0
  {"\u00b0C"},
  #else
  {"\u00b0F"},
  #endif
  {"%"},
  {"Па"},
  {"A"},
  {"В"},
  {"Вт"},
  {"dB"},
  {"ppm"}, //CO2: ppm
  {"лк"} // люкс
  #else
  {""},
  #if TEMP_CONVERSION == 0
  {"\u00b0C"},
  #else
  {"\u00b0F"},
  #endif
  {"%"},
  {"Pa"},
  {"A"},
  {"V"},
  {"Wt"},
  {"dB"},
  {"ppm"},
  {"Lux"}
  #endif
};

const char iotmSensorTypeDescript[IOTM_SNTYPECOUNT][IOTM_SNSUFFIXTITLEZ] /*PROGMEM*/ = {
  #if IOTM_LANG == IOTM_LANG_RU
  {"Нет сенсора"},      
  #if TEMP_CONVERSION == 0
  {"Температура (\u00b0С)"},
  #else
  {"Температура (\u00b0F)"},
  #endif
  {"Влажность (%)"},
  {"Давление (Па)"},
  {"Сила тока (A)"},
  {"Напряжение (В)"},
  {"Мощность (Вт)"},
  {"Уровень шума (dB)"},
  {"Уровень СО2 (ppm)"}, //CO2: ppm
  {"Освещённость (люкс)"} // люкс
  #else
  {"Empty sensor"},      
  #if TEMP_CONVERSION == 0
  {"Temperature (\u00b0С)"},
  #else
  {"Temperature (\u00b0F)"},
  #endif
  {"Humidity (%)"},
  {"Pressure (Pa)"},
  {"Current (A)"},
  {"Voltage (V)"},
  {"Electrical Power (Wt)"},
  {"Noise level (dB)"},
  {"CO2 Level (ppm)"},
  {"LuminanceLevel"}
  #endif
};

struct iotmRelayWidget {
  int   widgetId;  
  int   pageId;
  char  pageName[20];
  char  jsonBody[MESSZ];
};

struct iotmSensorWidget {
  int       widgetId;  
  int       pageId;
  char      pageName[20];
  char      jsonBody[MESSZ];

  char      unitSuffix[IOTM_SNSUFFIXZ]; // V, A, Wt, C, %, etc. // почему тут храним суффикс, а не подставляем его автоматически? Потому, что с одного и того же сенсора давления один виджет заберёт Па, а другой гПа (разумеется, пересчитав коэффициент)
  sensor_t  sensorType;
  uint16_t  pubInterval; // 1 sec .. IOTM_SENSORPUBINTERVAL_MAX sec  
};

struct iotmSensorData {
  float     cachedValue;
  uint16_t  pubCurrentTime; // counter: from pubInterval to 0 sec
  uint8_t   isUpdated; // флаг обновления значения
};

sensor_t getSensorTypeByModel(int sensorIndex) 
{


    // отдаёт тип сенсора в зависимости от модели Sonoff и индекса сенсора.
    // В данном случае, порядок следования сенсоров носит условный характер, т.е. какой сенсор будет первым, а какой вторым, решает исключительно сборщик прошивки.
    sensor_t t = stEmpty;
    if (sensorIndex < IOTM_SENSORCOUNT) 
      switch (sensorIndex) {
        case 0: t = stTemperature;
                break;
        case 1: t = stHumidity;
                break;
        default: stEmpty;
      };
    return t;
}

void getSensorSuffixByType(char *output, sensor_t sensorType)
{
  output[0] = '\0';
  if ((sensorType!= stEmpty) && (sensorType < IOTM_SNTYPECOUNT)) strlcpy(output, iotmSensorUnitSuffix[sensorType], IOTM_SNSUFFIXZ);
  //else strlcpy(output,"\0",1);
  
  
  return;
}

// char *getSensorValueByWidget(iotmSensorWidget &widget, iotmSensorData &data) 
void getSensorValueByWidget(iotmSensorWidget &widget, iotmSensorData &data, char *sv) 
{
  //static char sv[SENSORVALUESZ] = "";
  sv[0] = '\0';
  char stemp1[10];

  switch (widget.sensorType)
  {
    case stTemperature:   dtostrf(data.cachedValue, 1, TEMP_RESOLUTION &3, sv /*stemp1*/);
                          // snprintf_P(sv, sizeof(sv), PSTR("%s%s"), stemp1, widget.unitSuffix);
                          //snprintf_P(sv, sizeof(sv), PSTR("%s"), stemp1);
                          break;
    case stHumidity:      dtostrf(data.cachedValue, 1, HUMIDITY_RESOLUTION &3, sv /*stemp1*/);
                          //snprintf_P(sv, sizeof(sv), PSTR("%s%s"), stemp1, widget.unitSuffix);
                          //snprintf_P(sv, sizeof(sv), PSTR("%s"), stemp1);
                          break;
    default:              snprintf_P(sv, sizeof(sv), PSTR("%s"), data.cachedValue);
  };

  return;
  //return sv;
}

unsigned long getKeyIntValue(const char *json, const char *key)
{
  char *p, *b;
  int i;

  // search key
  p = strstr(json, key);
  if (!p) return 0;
  // search following separator :
  b = strchr(p + strlen(key), ':');
  if (!b) return 0;
  // Only the following chars are allowed between key and separator :
  for(i = b - json + strlen(key); i < p-json; i++) {
    switch (json[i]) {
    case ' ':
    case '\n':
    case '\t':
    case '\r':
      continue;
    default:
      return 0;
    }
  }
  b++;
  // Allow integers as string too (used in "svalue" : "9")
  while ((b[0] == ' ') || (b[0] == '"')) b++;
  // Convert to integer
  return atoi(b);
}

void getKeyCharValue(char* output, const char *json, const char *key, byte size)
{
 // {"command":"getPageById","param":0}
  output[0] = '\0';
  char *p, *b;
  int i;

  // search key
  p = strstr(json, key);
  if (!p) return;
  // search following separator :
  b = strchr(p + strlen(key), ':');
  if (!b) return;
  // Only the following chars are allowed between key and separator :
  for(i = b - json + strlen(key); i < p-json; i++) {
    switch (json[i]) {
    case ' ':
    case '\n':
    case '\t':
    case '\r':
      continue;
    default:
      return;
    }
  }
  
  // search ',' or '}'
  p = strchr(b +1,',');
  if (!p) p = strchr(b +1,'}');
  if (!p) return;


  b++;
  p--;  
  while (((b[0] == ' ')||(b[0] == '\"')||(b[0] == '\t'))&&(b < p)) b++;
  while (((p[0] == ' ')||(p[0] == '\"')||(p[0] == '\t'))&&(p > p)) p--;
  strlcpy(output, b, size < (p-b+1)?size:(p-b+1));
  
  return;
}

iotmSensorData iotmSensorDataBuff[IOTM_SENSORCOUNT];

#endif
#endif


