#include <Arduino.h>
#include <ArduinoOTA.h>
#include <RoamingWifi.h>
#include <confnvs.h>
#include <iostream>
#include <sstream>
#include <config.h>
#include <settings.h>

#include <Wire.h>
unsigned int crc16(unsigned int crc, unsigned char *buf, int len);
bool decodeLine(String line);
void setupDataReadout();
double getValue(String buffer, char startchar, char endchar);

void task_ota(void *args)
{
  ArduinoOTA.begin();
  while (1)
  {
    // Check for over the air update request and (if present) flash it
    ArduinoOTA.handle();
    delay(5000);
  }
};

void setup()
{
  Serial.begin(115200);
  ConfNvs confNvs;
  confNvs.begin();
  /**
   * Connect to Wifi
   */
  RoamingWiFi.start();
  xTaskCreate(task_ota, "task_ArduinOTA", 4096, NULL, tskIDLE_PRIORITY, NULL);

  Serial1.setRxBufferSize(4096);
  Serial1.begin(115200, SERIAL_8N1, -1, -1, true);

  Serial.println("version 8");
  delay(2000);

  setupDataReadout();
}
int i;
char charRead;
String lineRead = "";
unsigned int CRC = 0x0000;
unsigned int finalCRC = 0x0000;
#define DEBUGPRINT 0
#define DEBUGPRINTHEX 1
void loop()
{

  while (Serial1.available())
  {
    charRead = Serial1.read();
    lineRead += charRead;
#if DEBUGPRINTHEX
    Serial.print(" ");
    Serial.print(int(charRead));
    Serial.print("=");
    if( int(charRead) == 10  || int(charRead) == 13) {
      // Deze niet printen op het scerm 
    } else {
      Serial.print(charRead);
    }
        if( int(charRead) == 10 ) {
     Serial.println();
    }
#endif
#if DEBUGPRINT
    Serial.print(charRead);
#endif

    if (charRead == '/')
    {
      CRC = 0;
    }
    CRC = crc16(CRC, (unsigned char *)&charRead, 1);
    if (charRead == '!')
    {
      finalCRC = CRC;
      Serial.printf("%X=", CRC);
    }

    if ( int(charRead) == 10)
    {
#if !DEBUGPRINT && !DEBUGPRINTHEX
      Serial.println();
      Serial.println("lineRead");
      for (int i = 0; i < lineRead.length(); i++)
      {
        Serial.print(int(lineRead.substring(i, i + 1).c_str()));
        Serial.print(" ");
        Serial.print(lineRead.substring(i, i + 1).c_str());
      }
      Serial.println(lineRead);
#endif
#if DEBUGPRINTHEX
      // Serial.printf("Gelezen lengte: %i \n", lineRead.length());
#endif
      // if (lineRead.startsWith("!"))
      {
        if (decodeLine(lineRead))
        {
          // this means that the telegram is processed completely and valid (crc)
          Serial.println();
          Serial.println("totaal verbruik, L1,L2,L3");
          Serial.printf("%f %f %f %f\n", telegramObjects[4].value, telegramObjects[6].value, telegramObjects[7].value, telegramObjects[8].value);
        }
      }
      lineRead = "";
      
    } // end of processing end of line (10 or 13)
  }

  delay(1);
}

unsigned int crc16(unsigned int crc, unsigned char *buf, int len)
{
  for (int pos = 0; pos < len; pos++)
  {
    crc ^= (unsigned int)buf[pos];

    for (int i = 8; i != 0; i--)
    {
      if ((crc & 0x0001) != 0)
      {
        crc >>= 1;
        crc ^= 0xA001;
      }
      else
      {
        crc >>= 1;
      }
    }
  }
  return crc;
}

/**
 * decodeline will interpret every line in the telegram
 * returns true if it is the crc line and the crc is correct
 * for the other lines this function will just store the read values, if any
 */
bool decodeLine(String line)
{
  // int dummy= int(line.substring(0,1).c_str());
  // Serial.println();
  //  Serial.print(line);

  //  Serial.println();
  // return false;
  if (line.startsWith("!"))
  {
    // Serial.printf("\n%i %i %i %i\n", telegramObjects[4].value, telegramObjects[6].value, telegramObjects[7].value, telegramObjects[8].value);

    // Serial.print("!!!!!!!!!!!!!!!!!!!!!!!!");
    unsigned long challengeCRC = std::stoul(("0x" + line.substring(1, 5)).c_str(), nullptr, 16);
    Serial.print(challengeCRC);
    Serial.print("  ");
    Serial.println(finalCRC);

    if (challengeCRC == finalCRC)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  // all the other lines of the telegram
  // Loops throug all the telegramObjects to find the code in the telegram line
  // If it finds the code the value will be stored in the object so it can later be send to the mqtt broker
  for (int i = 0; i < NUMBER_OF_READOUTS; i++)
  {
    if (line.startsWith(telegramObjects[i].code))
    {
      double newValue = getValue(line, telegramObjects[i].startChar, telegramObjects[i].endChar);
      if (newValue != telegramObjects[i].value && newValue >= 0)
      {
        telegramObjects[i].value = newValue;
        telegramObjects[i].sendData = true;
      }

#ifdef DEBUG
      Serial.println((String) "Found a Telegram object: " + telegramObjects[i].name + " value: " + telegramObjects[i].value);
#endif
      break;
    }
  }
  return false;
}

double getValue(String buffer, char startchar, char endchar)
{
  int start = buffer.lastIndexOf(startchar);
  if (start < 0)
  {
    return -1;
  }
  int end = buffer.lastIndexOf(endchar);
  if (end < start)
  {
    return -1;
  }
  String valueStr = buffer.substring(start + 1, end);
  Serial.println(valueStr);
  char *e;
  errno = 0;
  double x = std::strtod(valueStr.c_str(), &e);

  if (*e != '\0' || // error, we didn't consume the entire string
      errno != 0)   // error, overflow or underflow
  {
    return -1;
  }
  return x;
}

/**
   setupDataReadout()

   This method can be used to create more data readout to mqtt topic.
   Use the name for the mqtt topic.
   The code for finding this in the telegram see
    https://www.netbeheernederland.nl/_upload/Files/Slimme_meter_15_a727fce1f1.pdf for the dutch codes pag. 19 -23
   Use startChar and endChar for setting the boundies where the value is in between.
   Default startChar and endChar is '(' and ')'
   Note: Make sure when you add or remove telegramObject to update the NUMBER_OF_READOUTS accordingly.
*/
void setupDataReadout()
{
  // 1-0:1.8.1(000992.992*kWh)
  // 1-0:1.8.1 = Elektra verbruik laag tarief (DSMR v5.0)
  telegramObjects[0].name = "consumption_low_tarif";
  strcpy(telegramObjects[0].code, "1-0:1.8.1");

  // 1-0:1.8.2(000560.157*kWh)
  // 1-0:1.8.2 = Elektra verbruik hoog tarief (DSMR v5.0)
  telegramObjects[1].name = "consumption_high_tarif";
  strcpy(telegramObjects[1].code, "1-0:1.8.2");

  telegramObjects[2].name = "return_low_tarif";
  strcpy(telegramObjects[2].code, "1-0:2.8.1");

  // 1-0:1.8.2(000560.157*kWh)
  // 1-0:1.8.2 = Elektra verbruik hoog tarief (DSMR v5.0)
  telegramObjects[3].name = "return_high_tarif";
  strcpy(telegramObjects[3].code, "1-0:2.8.2");

  // 1-0:1.7.0(00.424*kW) Actueel verbruik
  // 1-0:1.7.x = Electricity consumption actual usage (DSMR v5.0)
  telegramObjects[4].name = "actual_consumption";
  strcpy(telegramObjects[4].code, "1-0:1.7.0");

  // 1-0:2.7.0(00.000*kW) Actuele teruglevering (-P) in 1 Watt resolution
  telegramObjects[5].name = "actual_return";
  strcpy(telegramObjects[5].code, "1-0:2.7.0");

  // 1-0:21.7.0(00.378*kW)
  // 1-0:21.7.0 = Instantaan vermogen Elektriciteit levering L1
  telegramObjects[6].name = "actual_consumption_l1";
  strcpy(telegramObjects[6].code, "1-0:21.7.0");

  // 1-0:41.7.0(00.378*kW)
  // 1-0:41.7.0 = Instantaan vermogen Elektriciteit levering L2
  telegramObjects[7].name = "actual_consumption_l2";
  strcpy(telegramObjects[7].code, "1-0:41.7.0");

  // 1-0:61.7.0(00.378*kW)
  // 1-0:61.7.0 = Instantaan vermogen Elektriciteit levering L3
  telegramObjects[8].name = "actual_consumption_l3";
  strcpy(telegramObjects[8].code, "1-0:61.7.0");

  // 1-0:22.7.0(00.378*kW)
  // 1-0:22.7.0 = Instantaan vermogen Elektriciteit teruglevering L1
  telegramObjects[9].name = "actual_return_l1";
  strcpy(telegramObjects[9].code, "1-0:22.7.0");

  // 1-0:42.7.0(00.378*kW)
  // 1-0:42.7.0 = Instantaan vermogen Elektriciteit teruglevering L2
  telegramObjects[10].name = "actual_return_l2";
  strcpy(telegramObjects[10].code, "1-0:42.7.0");

  // 1-0:62.7.0(00.378*kW)
  // 1-0:62.7.0 = Instantaan vermogen Elektriciteit teruglevering L3
  telegramObjects[11].name = "actual_return_l3";
  strcpy(telegramObjects[11].code, "1-0:62.7.0");

  // 1-0:31.7.0(002*A)
  // 1-0:31.7.0 = Instantane stroom Elektriciteit L1
  telegramObjects[12].name = "actual_current_l1";
  strcpy(telegramObjects[12].code, "1-0:31.7.0");

  // 1-0:51.7.0(002*A)
  // 1-0:51.7.0 = Instantane stroom Elektriciteit L2
  telegramObjects[13].name = "actual_current_l2";
  strcpy(telegramObjects[13].code, "1-0:51.7.0");

  // 1-0:71.7.0(002*A)
  // 1-0:71.7.0 = Instantane stroom Elektriciteit L3
  telegramObjects[14].name = "actual_current_l3";
  strcpy(telegramObjects[14].code, "1-0:71.7.0");

  // 1-0:32.7.0(232.0*V)
  // 1-0:32.7.0 = Voltage L1
  telegramObjects[15].name = "actual_voltage_l1";
  strcpy(telegramObjects[15].code, "1-0:32.7.0");

  // 1-0:52.7.0(232.0*V)
  // 1-0:52.7.0 = Voltage L2
  telegramObjects[16].name = "actual_voltage_l2";
  strcpy(telegramObjects[16].code, "1-0:52.7.0");

  // 1-0:72.7.0(232.0*V)
  // 1-0:72.7.0 = Voltage L3
  telegramObjects[17].name = "actual_voltage_l3";
  strcpy(telegramObjects[17].code, "1-0:72.7.0");

  // 0-0:96.14.0(0001)
  // 0-0:96.14.0 = Actual Tarif
  telegramObjects[18].name = "actual_tarif_group";
  strcpy(telegramObjects[18].code, "0-0:96.14.0");
  telegramObjects[18].endChar = ')';

  // 0-1:24.2.3(150531200000S)(00811.923*m3)
  // 0-1:24.2.3 = Gas (DSMR v5.0) on Belgian meters
  telegramObjects[19].name = "gas_meter_m3";
  strcpy(telegramObjects[19].code, "0-1:24.2.3");

#ifdef DEBUG
  Serial.println("MQTT Topics initialized:");
  for (int i = 0; i < NUMBER_OF_READOUTS; i++)
  {
    Serial.println(String(MQTT_ROOT_TOPIC) + "/" + telegramObjects[i].name);
  }
#endif
}