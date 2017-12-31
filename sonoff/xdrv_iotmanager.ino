#ifdef USE_IOTMANAGER

void iotmanager_updatePowerState(byte device)
{
    mqtt_publishIoTmanagerPowerState(device);
}

void mqtt_publishIoTmanagerPowerState(byte device, byte state)
{
  char stopic[TOPSZ], svalue[MESSZ], sdevice[20];

  // if ((device < 1) || (device > Maxdevice)) device = 1;
  // #if (MODULE == SONOFF_BASIC)||(MODULE == SONOFF_2)
  #if IOTM_RELAYCOUNT > 0

  snprintf_P(sdevice, sizeof(sdevice), PSTR("%s%d"), IOTM_RELAYNAME, device);
  snprintf_P(svalue, sizeof(svalue), PSTR("{\"status\":%d}"), /*(power & (0x01 << (device -1)))?1:0*/ state);
  snprintf_P(stopic, sizeof(stopic), PSTR(IOTM_STATUS), sysCfg.iotmanager_prefix, MQTTClient, sdevice);
  // yield();
  // mqtt_publish(stopic, svalue, sysCfg.mqtt_power_retain);

  mqttClient.publish(stopic, svalue);
  //mqtt_publish(stopic, svalue);
  
  // yield();
  #endif  
 return;
}

void mqtt_publishIoTmanagerPowerState(byte device)
{
 byte state = (power & (0x01 << (device -1)))?1:0;
 mqtt_publishIoTmanagerPowerState(device, state);
 return;
}

void iotmanager_mqttSubscribe()
{
  // iotmanager_subscribe = true;
  char stopic[TOPSZ], svalue[MESSZ];

  snprintf_P(stopic, sizeof(stopic), PSTR(IOTM_CONTROL), sysCfg.iotmanager_prefix, MQTTClient);
  mqttClient.subscribe(stopic);
  mqttClient.loop();
  snprintf_P(stopic, sizeof(stopic), PSTR(IOTM_REQUEST), sysCfg.iotmanager_prefix);
  mqttClient.subscribe(stopic);
  mqttClient.loop();
  
  //snprintf_P(stopic, sizeof(stopic), PSTR(IOTM_ROOT), sysCfg.iotmanager_prefix);
  //mqttClient.subscribe(stopic);
  //mqttClient.loop();
}


boolean iotmanager_mqttData(char *topicBuf, uint16_t stopicBuf, char *dataBuf, uint16_t sdataBuf)
{
  //char stemp1[10];
  //char scommand[10];
  //unsigned long idx = 0;
  int16_t nvalue;
  int16_t found = 0;
  char *str;
  char svalue[MESSZ];
  char sensorValue[SENSORVALUESZ] = "";
  char log[LOGSZ];
  char stopic[TOPSZ], stemp1[TOPSZ], stemp2[10];
  uint16_t i = 0, index;
  char *p, *mtopic = NULL, *type = NULL, *devc = NULL;

  snprintf_P(stemp1, sizeof(stemp1), PSTR("/%s/"), sysCfg.iotmanager_prefix);

  if (!strncmp(topicBuf, stemp1, strlen(stemp1)))
   {
    i = 0;

    snprintf_P(log, sizeof(log), PSTR("IOTM: IotManager header detected"));
    addLog(LOG_LEVEL_DEBUG_MORE, log);

  
    for (str = strtok_r(topicBuf, "/", &p); str && i < 4; str = strtok_r(NULL, "/", &p)) {
      switch (i++) {
      case 0:  // /IoTmanager/65419d9046c24972/request; /IoTmanager/device; /IoTmanager/ids; /IoTmanager/DVES_E742C1/sensorname/control
        break;
      case 1:  // TopicIndex / Text
      devc = str;
        break;
      case 2:
      type = str;
        break;
      case 3:  // Text
        mtopic = str;
      }
    }
    
    if (!mtopic) mtopic = type;
    if (!mtopic) mtopic = devc;
    for(i = 0; i < strlen(mtopic); i++) mtopic[i] = toupper(mtopic[i]);

    snprintf_P(log, sizeof(log), PSTR("IOTM: IotManager topic: %s"), mtopic);
    addLog(LOG_LEVEL_DEBUG_MORE, log);

    
    if (!strcmp(mtopic,"REQUEST")) {

        StaticJsonBuffer<400> jsonBuf;
        JsonObject& iotmanager = jsonBuf.parseObject(dataBuf);
        if (!iotmanager.success()) return 1;
        //const char iotm_command[20];
        // getKeyCharValue(iotm_command, dataBuf, "\"command\"", sizeof(iotm_command));
        
        //char iotm_command[20];
        //iotm_command = iotmanager["command"];

        const char *iotm_command = iotmanager["command"];

          snprintf_P(log, sizeof(log), PSTR("IOTM: IotManager command: %s"), iotm_command);
          addLog(LOG_LEVEL_DEBUG_MORE, log);
        
        
        if (strlen(iotm_command) > 0) {
          //for(i = 0; i < strlen(iotm_command); i++) iotm_command[i] = toupper(iotm_command[i]);
  
          // payload example for REQUEST topic:
          // {"command":"getPages","param":""}
          // {"command":"getPageById","param":10}
          
          //if (!strcmp(iotm_command,"GETPAGES")) index = 0;
          // strncasecmp
          // strupr
          //if (!strncmp_P(iotm_command,PSTR("GETPAGES"),8)) index = 0;
          if (!strncasecmp_P(iotm_command,PSTR("GETPAGES"),8)) index = 0;
          else
          //if (!strcmp(iotm_command,"GETPAGEBYID")) index = getKeyIntValue(dataBuf,"\"param\"");
          if (!strncasecmp_P(iotm_command,PSTR("GETPAGEBYID"),11)) index = iotmanager["param"];
          else return 1; 

          //snprintf_P(log, sizeof(log), PSTR("IOTM: IotManager command: %s; param = %d"), iotm_command, index);
          //addLog(LOG_LEVEL_DEBUG_MORE, log);

  
          // make topic /IoTManager/DEVICENAME/CONFIG
          snprintf_P(stopic, sizeof(stopic), PSTR(IOTM_CONFIG), sysCfg.iotmanager_prefix, MQTTClient);

          //snprintf_P(log, sizeof(log), PSTR("IOTM: IotManager stopic = %s; relaycount = %d"), stopic, IOTM_RELAYCOUNT);
          //addLog(LOG_LEVEL_DEBUG_MORE, log);
          
          
          // #if (MODULE == SONOFF_BASIC)||(MODULE == SONOFF_2)
          #if IOTM_RELAYCOUNT > 0
          for(int i = 0; i < IOTM_RELAYCOUNT; i++)
            if ((!index)||(sysCfg.iotmRelayWidgets[i].pageId == index)) {
  
  
  
          // snprintf_P(svalue, sizeof(svalue), PSTR("dataBuf = %s, widgetId = %u"), dataBuf, sysCfg.iotmRelayWidgets[i].widgetId);
          // addLog_P(LOG_LEVEL_INFO, svalue);
       
              snprintf_P(svalue, sizeof(svalue), PSTR("{\"id\":%u,\"page\":\"%s\",\"pageId\":%u,\"topic\":\"/%s/%s/%s%u\",%s,\"status\":\"%d\"}"), 
                          sysCfg.iotmRelayWidgets[i].widgetId, 
                          sysCfg.iotmRelayWidgets[i].pageName,
                          sysCfg.iotmRelayWidgets[i].pageId,
                          sysCfg.iotmanager_prefix,
                          MQTTClient,
                          IOTM_RELAYNAME, // "lightswitch" for example
                          //sysCfg.iotmRelayWidgets[i].widgetId,
                          i+1,
                          sysCfg.iotmRelayWidgets[i].jsonBody,
                          (power & (0x01 << i))?1:0
                          );
              mqttClient.publish(stopic, svalue);
         // snprintf_P(log, sizeof(log), PSTR("IOTM: return value (svalue) = %s"), svalue);
         // addLog(LOG_LEVEL_DEBUG_MORE, log);
              
            };
  
          for(int i = 0; i < IOTM_SENSORCOUNT; i++) 
            if ((!index)||(sysCfg.iotmSensorWidgets[i].pageId == index)) {
  
              
              getSensorValueByWidget(sysCfg.iotmSensorWidgets[i], iotmSensorDataBuff[i], sensorValue);
              snprintf_P(svalue, sizeof(svalue), PSTR("{\"id\":%u,\"page\":\"%s\",\"pageId\":%u,\"topic\":\"/%s/%s/%s%u\",%s,\"after\":\"%s\",\"status\":\"%s\"}"), 
              
                          sysCfg.iotmSensorWidgets[i].widgetId, 
                          sysCfg.iotmSensorWidgets[i].pageName,
                          sysCfg.iotmSensorWidgets[i].pageId,
                          sysCfg.iotmanager_prefix,
                          MQTTClient,
                          IOTM_SENSORNAME, // "sensor" for example
                          //sysCfg.iotmRelayWidgets[i].widgetId,
                          i+1,
                          sysCfg.iotmSensorWidgets[i].jsonBody,
                          sysCfg.iotmSensorWidgets[i].unitSuffix,
                          sensorValue  // String(iotmSensorDataBuff[i].cachedValue).c_str()
                          );
                          
              //mqtt_publish(stopic, svalue);
              mqttClient.publish(stopic, svalue);
              
            };
            #endif
              
          };
  
          
    }
    else
    if (!strcmp(mtopic,"CONTROL")) {
  
  //      example of subscribe: /IoTmanager/DVES_E742C1/sensorname<index>/control
  //      devc = DVES_E742C1
  //      type = sensorname<index>, index = 1..max relay count
  //      mtopic = control
  
  //      payload = "1" or "0"
  
    // if (strlen(dataBuf) > 0) do_cmnd_power(atoi(&type[strlen(type)-1]), dataBuf[0]=='1'?1:0);
      if (strlen(dataBuf) > 0) {
          i = atoi(&type[strlen(type)-1]) - 1;
          nvalue = (dataBuf[0]=='1'?1:0);
          if (((power >> i) & 1) == nvalue) return 1;
          snprintf_P(topicBuf, stopicBuf, PSTR("/POWER%s"), (IOTM_RELAYCOUNT > 1) ? &type[strlen(type)-1] : "");
          snprintf_P(dataBuf, sdataBuf, PSTR("%d"), nvalue);
          found = 1;
      };
  
    }

    if (!found) return 1;
    snprintf_P(log, sizeof(log), PSTR("IOTM: Receive topic %s, data %s"), topicBuf, dataBuf);
    addLog(LOG_LEVEL_DEBUG_MORE, log);

    return 0;
    
   }
 
  return 1;
}


#ifdef USE_WEBSERVER

const char HTTP_IOTM_SENSORSUFFIXITEM[] PROGMEM = "sensSuffix[{IOTM_SENSORSUFFIX_INDEX}] = '{IOTM_SENSORSUFFIX_CAPTION}';";
const char HTTP_IOTM_SENSORSUFFIXSCRIPT[] PROGMEM =
"function setSensorSuffix(s,i)"
  "{"
  "var sensSuffix = [];"
  //"sensSuffix[{}] = "";"
  "{IOTM_SENSORSUFFIX_ARRAY}"
  "var inputSuffix = document.getElementById(i);"
  "var n = document.getElementById(s).options.selectedIndex;"
  "if (n > -1) inputSuffix.value = sensSuffix[document.getElementById(s).options[n].value];"
  "}"
  "</script>";

const char HTTP_FORM_IOTMANAGER[] PROGMEM =
  "<fieldset><legend><b>&nbsp;IoTmanager parameters&nbsp;</b></legend><form method='post' action='sv'>"
  "<input id='w' name='w' value='7' hidden><input id='r' name='r' value='1' hidden>"
  "<br/><b>IoTmanager root topic</b> (" IOTM_PREFIX ")<br/><input id='iotm' name='iotm' maxlength=32 placeholder='" IOTM_PREFIX "' value='{im1}'><br/>";
//#if (MODULE == SONOFF_BASIC)||(MODULE == SONOFF_2)
#if IOTM_RELAYCOUNT > 0
const char HTTP_IOTM_RELAYWIDGET[] PROGMEM =
  "<br/><b>Relay widget {WLETTER}</b><br/><textarea style='height:100px;' id='iotmw{WNUMBER}' name='iotmw{WNUMBER}' maxlength=" STR(MESSZ) " placeholder='{json widget style there}'>{RELAYWIDGETBODY}</textarea><br/>"
  "<br/><b>Widget page name</b> (" IOTM_PAGENAME ")<br/><input id='iotmpn{WNUMBER}' name='iotmpn{WNUMBER}' maxlength=20 placeholder='" IOTM_PAGENAME "' value='{WIDGETPAGENAME}'><br/>"
  "<br/><b>Widget page ID</b> (" STR(IOTM_PAGEID) ")<br/>"
  "<select name='iotmpid{WNUMBER}'>{IOTM_PAGEIDSELECTOR}</select><br/>"
  "<br/><b>Widget ID</b>&nbsp;(numeric, between 1 and 225)<br/><input name='iotmwid{WNUMBER}'  type='number' min='1' max='255' value='{WIDGETID}' required><br/><hr>";

const char HTTP_IOTM_WIDGETPAGEID[] PROGMEM = "<option value='{PAGEID}' {iotm_pid_selected{PAGEID}}>{PAGEID}</option>";

const char HTTP_IOTM_SENSORTYPE[] PROGMEM = "<option value='{SENSORTYPE}' {iotm_tid_selected{SENSORTYPE}}>{SENSORTYPEDESCRIPT}</option>";

const char HTTP_IOTM_SENSORWIDGET[] PROGMEM =
  "<br/><b>Sensor widget {WLETTER}</b><br/><textarea style='height:100px;' id='iotmsw{WNUMBER}' name='iotmsw{WNUMBER}' maxlength=" STR(MESSZ) " placeholder='{json widget style there}'>{SENSORWIDGETBODY}</textarea><br/>"
  "<br/><b>Widget page name</b> (" IOTM_PAGENAME ")<br/><input id='iotmspn{WNUMBER}' name='iotmspn{WNUMBER}' maxlength=20 placeholder='" IOTM_PAGENAME "' value='{WIDGETPAGENAME}'><br/>"
  "<br/><b>Widget page ID</b> (" STR(IOTM_PAGEID) ")<br/>"
  "<select name='iotmspid{WNUMBER}'>{IOTM_PAGEIDSELECTOR}</select><br/>"
  "<br/><b>Widget ID</b>&nbsp;(numeric, between 1 and 225)<br/><input name='iotmswid{WNUMBER}'  type='number' min='1' max='255' value='{WIDGETID}' required><br/>"
  "<br/><b>Sensor type</b><br/>"
//  "<select id='iotmtid{WNUMBER}' name='iotmtid{WNUMBER}'>{IOTM_SENSORTYPESELECTOR}</select><br/>"
  "<select id=\"iotmtid{WNUMBER}\" name=\"iotmtid{WNUMBER}\" OnChange=\"setSensorSuffix('iotmtid{WNUMBER}','iotmsuffix{WNUMBER}')\">{IOTM_SENSORTYPESELECTOR}</select><br/>"
  "<br/><b>Sensor Suffix</b><br/><input id='iotmsuffix{WNUMBER}' name='iotmsuffix{WNUMBER}' maxlength=" STR(IOTM_SNSUFFIXZ) " placeholder='{WIDGETSENSORTYPEDESCRIPT}' value='{WIDGETSENSORTYPEDESCRIPT}'><br/>"
  "<br/><b>Publication interval (sec)</b>&nbsp;(numeric, between 1 and " STR(IOTM_SENSORPUBINTERVAL_MAX) ")<br/><input name='iotmpub{WNUMBER}'  type='number' min='1' max='" STR(IOTM_SENSORPUBINTERVAL_MAX) "' value='{SENSORPUBINTERVAL}' required><br/><hr>";
#endif // IOTM_RELAYCOUNT > 0 -- MODULE == SONOFF

void handleIoTmanager()
{
  if (_httpflag == HTTP_USER) {
    handleRoot();
    return;
  }
  addLog_P(LOG_LEVEL_DEBUG, PSTR("HTTP: Handle IoTmanager config"));

  String page = FPSTR(HTTP_HEAD);
  String script = FPSTR(HTTP_IOTM_SENSORSUFFIXSCRIPT);
  String pageidselector = "";
  
  for(int j = stEmpty; j < IOTM_SNTYPECOUNT; j++) {
      pageidselector += FPSTR(HTTP_IOTM_SENSORSUFFIXITEM);
      pageidselector.replace("{IOTM_SENSORSUFFIX_INDEX}", String(j));
      pageidselector.replace("{IOTM_SENSORSUFFIX_CAPTION}",iotmSensorUnitSuffix[j]);
    };
  script.replace("{IOTM_SENSORSUFFIX_ARRAY}", pageidselector);
  page.replace("</script>", script);
  
  page.replace("{v}", "Configure IoTmanager");
  page += FPSTR(HTTP_FORM_IOTMANAGER);
  page.replace("{im1}", String(sysCfg.iotmanager_prefix));

// #if (MODULE == SONOFF_BASIC)||(MODULE == SONOFF_2)
#if IOTM_RELAYCOUNT > 0

  for(int i=0;i<IOTM_RELAYCOUNT;i++) {
    page += FPSTR(HTTP_IOTM_RELAYWIDGET);
    page.replace("{WLETTER}", String((char)('A'+i)));
    // page.replace("{WLETTERLCASE}", String((char)('a'+i)));
    page.replace("{WNUMBER}", String((int)(i+1)));
    page.replace("{RELAYWIDGETBODY}", String(sysCfg.iotmRelayWidgets[i].jsonBody));

    page.replace("{WIDGETPAGENAME}", String(sysCfg.iotmRelayWidgets[i].pageName));

    pageidselector = "";
    for(int j=0;j<IOTM_PAGECOUNT;j++) {
      pageidselector += FPSTR(HTTP_IOTM_WIDGETPAGEID);
      pageidselector.replace("{PAGEID}", String((j+1)*10));
      pageidselector.replace("{iotm_pid_selected" + String((j+1)*10) + "}",(sysCfg.iotmRelayWidgets[i].pageId == ((j+1)*10))?"selected":"");
    };
    page.replace("{IOTM_PAGEIDSELECTOR}", pageidselector);
    page.replace("{WIDGETID}", String(sysCfg.iotmRelayWidgets[i].widgetId));
    
  };

  for(int i=0;i<IOTM_SENSORCOUNT;i++) {
    page += FPSTR(HTTP_IOTM_SENSORWIDGET);
    page.replace("{WLETTER}", String((char)('A'+i)));
    // page.replace("{WLETTERLCASE}", String((char)('a'+i)));
    page.replace("{WNUMBER}", String((int)(i+1)));
    page.replace("{SENSORWIDGETBODY}", String(sysCfg.iotmSensorWidgets[i].jsonBody));
    page.replace("{WIDGETPAGENAME}", String(sysCfg.iotmSensorWidgets[i].pageName));

    pageidselector = "";
    for(int j=0;j<IOTM_PAGECOUNT;j++) {
      pageidselector += FPSTR(HTTP_IOTM_WIDGETPAGEID);
      pageidselector.replace("{PAGEID}", String((j+1)*10));
      pageidselector.replace("{iotm_pid_selected" + String((j+1)*10) + "}",(sysCfg.iotmSensorWidgets[i].pageId == ((j+1)*10))?"selected":"");
    };
    page.replace("{IOTM_PAGEIDSELECTOR}", pageidselector);
    page.replace("{WIDGETID}", String(sysCfg.iotmSensorWidgets[i].widgetId));
    page.replace("{WIDGETSENSORTYPEDESCRIPT}", sysCfg.iotmSensorWidgets[i].unitSuffix);
    page.replace("{SENSORPUBINTERVAL}", String(sysCfg.iotmSensorWidgets[i].pubInterval));

    pageidselector = "";
    for(int j = stEmpty; j < IOTM_SNTYPECOUNT; j++) {
      pageidselector += FPSTR(HTTP_IOTM_SENSORTYPE);
      pageidselector.replace("{SENSORTYPE}", String(j));
      pageidselector.replace("{iotm_tid_selected" + String(j) + "}",(sysCfg.iotmSensorWidgets[i].sensorType == j)?"selected":"");
      pageidselector.replace("{SENSORTYPEDESCRIPT}",iotmSensorTypeDescript[j]);
    };
    page.replace("{IOTM_SENSORTYPESELECTOR}", pageidselector);
  };
  
#endif
  
  page += FPSTR(HTTP_FORM_END);
  page += FPSTR(HTTP_BTN_CONF);
  showPage(page);
}

void iotmanager_saveSettings()
{
    char log[LOGSZ]; //, stemp[20];

    strlcpy(sysCfg.iotmanager_prefix, (!strlen(webServer->arg("iotm").c_str())) ? IOTM_PREFIX : webServer->arg("iotm").c_str(), sizeof(sysCfg.iotmanager_prefix));
// #if (MODULE == SONOFF_BASIC)||(MODULE == SONOFF_2)
#if IOTM_RELAYCOUNT > 0
    for(int i=0;i<IOTM_RELAYCOUNT;i++) {

      // Widget body (JSON-based configuration)
      char  jsonBody[MESSZ];
      String wtag = "iotmw";
      wtag.concat(i+1);
      snprintf_P(jsonBody, sizeof(jsonBody), PSTR(IOTM_RELAYWIDGET), 'A'+i);
      strlcpy(sysCfg.iotmRelayWidgets[i].jsonBody, (!strlen(webServer->arg(wtag).c_str())) ? jsonBody : webServer->arg(wtag).c_str(), sizeof(sysCfg.iotmRelayWidgets[i].jsonBody));

      // snprintf_P(log, sizeof(log), PSTR("HTTP: IoTmanager jSon body value = %s"), IOTM_RELAYWIDGET);
      // addLog(LOG_LEVEL_DEBUG, log);

      // Page name
      wtag = "iotmpn";
      wtag.concat(i+1);      
      strlcpy(sysCfg.iotmRelayWidgets[i].pageName, (!strlen(webServer->arg(wtag).c_str())) ? IOTM_PAGENAME : webServer->arg(wtag).c_str(), sizeof(sysCfg.iotmRelayWidgets[i].pageName));
      // Page ID
      wtag = "iotmpid";
      wtag.concat(i+1);      
      sysCfg.iotmRelayWidgets[i].pageId = (!strlen(webServer->arg(wtag).c_str())) ? IOTM_PAGEID : webServer->arg(wtag).toInt();
      // Widget ID
      wtag = "iotmwid";
      wtag.concat(i+1);      
      sysCfg.iotmRelayWidgets[i].widgetId = (!strlen(webServer->arg(wtag).c_str())) ? i+1 : webServer->arg(wtag).toInt();
      
    };

    for(int i=0;i<IOTM_SENSORCOUNT;i++) {
      // Widget body (JSON-based configuration)
      char  jsonBody[MESSZ];
      String wtag = "iotmsw";
      wtag.concat(i+1);
      snprintf_P(jsonBody, sizeof(jsonBody), PSTR(IOTM_SENSORWIDGET), 'A'+i);
      strlcpy(sysCfg.iotmSensorWidgets[i].jsonBody, (!strlen(webServer->arg(wtag).c_str())) ? jsonBody : webServer->arg(wtag).c_str(), sizeof(sysCfg.iotmSensorWidgets[i].jsonBody));
      // Page name
      wtag = "iotmspn";
      wtag.concat(i+1);      
      strlcpy(sysCfg.iotmSensorWidgets[i].pageName, (!strlen(webServer->arg(wtag).c_str())) ? IOTM_PAGENAME : webServer->arg(wtag).c_str(), sizeof(sysCfg.iotmSensorWidgets[i].pageName));
      // Page ID
      wtag = "iotmspid";
      wtag.concat(i+1);      
      sysCfg.iotmSensorWidgets[i].pageId = (!strlen(webServer->arg(wtag).c_str())) ? IOTM_PAGEID : webServer->arg(wtag).toInt();
      // Widget ID
      wtag = "iotmswid";
      wtag.concat(i+1);      
      sysCfg.iotmSensorWidgets[i].widgetId = (!strlen(webServer->arg(wtag).c_str())) ? IOTM_RELAYCOUNT+i+1 : webServer->arg(wtag).toInt();
      // Sensor Type
      wtag = "iotmtid";
      wtag.concat(i+1);
      sysCfg.iotmSensorWidgets[i].sensorType = (!strlen(webServer->arg(wtag).c_str())) ? getSensorTypeByModel(i) : (sensor_t)webServer->arg(wtag).toInt();
      // Sensor Suffix
      wtag = "iotmsuffix";
      wtag.concat(i+1);
      if (!strlen(webServer->arg(wtag).c_str())) {
        getSensorSuffixByType(sysCfg.iotmSensorWidgets[i].unitSuffix, sysCfg.iotmSensorWidgets[i].sensorType);    
      } else strlcpy(sysCfg.iotmSensorWidgets[i].unitSuffix, webServer->arg(wtag).c_str(), sizeof(sysCfg.iotmSensorWidgets[i].unitSuffix));
      // Publication interval (sec)
      wtag = "iotmpub";
      wtag.concat(i+1);
      //iotmSensorDataBuff[i].pubInterval = (!strlen(webServer->arg(wtag).c_str())) ? IOTM_SENSORPUBINTERVAL_DEF : webServer->arg(wtag).toInt();
      sysCfg.iotmSensorWidgets[i].pubInterval = (!strlen(webServer->arg(wtag).c_str())) ? IOTM_SENSORPUBINTERVAL_DEF : webServer->arg(wtag).toInt();
      iotmSensorDataBuff[i].isUpdated = sufNotUpdated;
      iotmSensorDataBuff[i].cachedValue = 0;
      //iotmSensorDataBuff[i].pubCurrentTime = iotmSensorDataBuff[i].pubInterval;
      iotmSensorDataBuff[i].pubCurrentTime = sysCfg.iotmSensorWidgets[i].pubInterval;
    };
    
#endif // MODULE == SONOFF
    snprintf_P(log, sizeof(log), PSTR("HTTP: IoTmanager prefix %s"), sysCfg.iotmanager_prefix);
  
    addLog(LOG_LEVEL_INFO, log);
}


#endif

#endif
