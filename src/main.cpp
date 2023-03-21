#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <WebServices.h>
// #include <ostream>
#include <iostream>
#include <config.h>
#include <pinman.h>
#include <Wire.h>
void scanner();

AsyncWebServer server(80);
bool WebSerialActive = false;
PinManager pinMan;
String cmd;
// typedef std::function<void(uint8_t *data, size_t len)> RecvMsgHandler;
#define ENABLE_SPI_SDCARD
#ifdef ENABLE_SPI_SDCARD

#include "FS.h"
#include "SD.h"
#include <SPI.h>

void setupSDCard()
{
  SPIClass SPI2(HSPI);
  SPI2.setHwCs(true);

  SPI2.begin(pinSdSclk, pinSdMiso, pinSdMosi, pinDownload);

  // Assuming use of SPI SD card
  if (!SD.begin(pinDownload, SPI2))
  {
    Serial.println("Card Mount Failed");
  }
  else
  {
    Serial.println("SDCard Mount PASS");
    String size = String((uint32_t)(SD.cardSize() / 1024 / 1024)) + "MB";
    Serial.println(size);
    File mapFile = SD.open("/www/index.html", FILE_READ);
    if (!mapFile)
    {
      Serial.println("ERROR opening file for reading");
    }
    else
    {
      while (mapFile.available())
      {
        Serial.write(mapFile.read());
      }
      mapFile.close();
    }
  }
}
#else
#define setupSDCard()
#endif

void task_WebConsoleInputHandler(void *args)
{
  Serial.println(cmd);
  delay(1000);
  int pin = 99;

  pin = atoi(cmd.c_str());
  Serial.println(pin);
  delay(1000);
  // we gaan de pinnen testen volgens de defines, zodat we direct weten of het end to end goed zit.
  switch (pin)
  {

  case pinMotorLeftPWM:
    printf("Test pinMotorLeftPWM O PWM\n");
    pinMan.pinModeX(pinMotorLeftPWM, OUTPUT);
    pinMan.digitalWriteX(pinMotorLeftPWM, 1);
    delay(10000);
    pinMan.digitalWriteX(pinMotorLeftPWM, 0);
    break;
  case pinBatterySwitch:
    printf("Test pinBatterySwitch O D\n");
    pinMan.pinModeX(pinBatterySwitch, OUTPUT);
    pinMan.digitalWriteX(pinBatterySwitch, 1);
    delay(10000);
    pinMan.digitalWriteX(pinBatterySwitch, 0);
    break;
  case pinGsmReset:
    printf("Test pinGsmReset O D\n");
    pinMan.pinModeX(pinGsmReset, OUTPUT);
    pinMan.digitalWriteX(pinGsmReset, 1);
    delay(10000);
    pinMan.digitalWriteX(pinGsmReset, 0);
    break;
  case pinMotorRightPWM:
    printf("Test pinMotorRightPWM O PWM\n");
    pinMan.pinModeX(pinMotorRightPWM, OUTPUT);
    pinMan.digitalWriteX(pinMotorRightPWM, 1);
    delay(10000);
    pinMan.digitalWriteX(pinMotorRightPWM, 0);
    break;
  case pinGsmRx:
    printf("Test pinGsmRx I D\n");
    pinMan.pinModeX(pinGsmRx, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinGsmRx));
      delay(1000);
    }
    break;
  case pinGsmTx:
    printf("Test pinGsmTx O D\n");
    pinMan.pinModeX(pinGsmTx, OUTPUT);
    pinMan.digitalWriteX(pinGsmTx, 1);
    delay(10000);
    pinMan.digitalWriteX(pinGsmTx, 0);
    break;
  case pinMotorMowPWM:
    printf("Test pinMotorMowPWM O PWM\n");
    pinMan.pinModeX(pinMotorMowPWM, OUTPUT);
    pinMan.digitalWriteX(pinMotorMowPWM, 1);
    delay(10000);
    pinMan.digitalWriteX(pinMotorMowPWM, 0);
    break;
  case pinSdSclk:
    printf("Test pinSdSclk O D\n");
    pinMan.pinModeX(pinSdSclk, OUTPUT);
    pinMan.digitalWriteX(pinSdSclk, 1);
    delay(10000);
    pinMan.digitalWriteX(pinSdSclk, 0);
    break;
  case pinSdMiso:
    printf("Test pinSdMiso I D\n");
    pinMan.pinModeX(pinSdMiso, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinSdMiso));
      delay(1000);
    }
    break;
  case pinSdMosi:
    printf("Test pinSdMosi O D\n");
    pinMan.pinModeX(pinSdMosi, OUTPUT);
    pinMan.digitalWriteX(pinSdMosi, 1);
    delay(10000);
    pinMan.digitalWriteX(pinSdMosi, 0);
    break;
  case pinGpsTx:
    printf("Test pinGpsTx O D\n");
    pinMan.pinModeX(pinGpsTx, OUTPUT);
    pinMan.digitalWriteX(pinGpsTx, 1);
    delay(10000);
    pinMan.digitalWriteX(pinGpsTx, 0);
    break;
  case pinGpsRx:
    printf("Test pinGpsRx I D\n");
    pinMan.pinModeX(pinGpsRx, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinGpsRx));
      delay(1000);
    }
    break;
  case pinSonarRightEcho:
    printf("Test pinSonarRightEcho I D\n");
    pinMan.pinModeX(pinSonarRightEcho, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinSonarRightEcho));
      delay(1000);
    }
    break;
  case pinBuzzer:
    printf("Test pinBuzzer O D\n");
    pinMan.pinModeX(pinBuzzer, OUTPUT);
    pinMan.digitalWriteX(pinBuzzer, 1);
    delay(10000);
    pinMan.digitalWriteX(pinBuzzer, 0);
    break;
  case pinIrqExp:
    printf("Test pinIrqExp IU D\n");
    pinMan.pinModeX(pinIrqExp, INPUT_PULLUP);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinIrqExp));
      delay(1000);
    }
    break;
  case pinSonarLeftEcho:
    printf("Test pinSonarLeftEcho I D\n");
    pinMan.pinModeX(pinSonarLeftEcho, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinSonarLeftEcho));
      delay(1000);
    }
    break;
  case pinSonarCenterEcho:
    printf("Test pinSonarCenterEcho I D\n");
    pinMan.pinModeX(pinSonarCenterEcho, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinSonarCenterEcho));
      delay(1000);
    }
    break;
  case pinMotorEnable:
    printf("Test pinMotorEnable O D\n");
    pinMan.pinModeX(pinMotorEnable, OUTPUT);
    pinMan.digitalWriteX(pinMotorEnable, 1);
    delay(10000);
    pinMan.digitalWriteX(pinMotorEnable, 0);
    break;
  case pinSonarLeftTrigger:
    printf("Test pinSonarLeftTrigger O D\n");
    pinMan.pinModeX(pinSonarLeftTrigger, OUTPUT);
    pinMan.digitalWriteX(pinSonarLeftTrigger, 1);
    delay(10000);
    pinMan.digitalWriteX(pinSonarLeftTrigger, 0);
    break;
  case pinSonarCenterTrigger:
    printf("Test pinSonarCenterTrigger O D\n");
    pinMan.pinModeX(pinSonarCenterTrigger, OUTPUT);
    pinMan.digitalWriteX(pinSonarCenterTrigger, 1);
    delay(10000);
    pinMan.digitalWriteX(pinSonarCenterTrigger, 0);
    break;
  case pinMotorMowEnable:
    printf("Test pinMotorMowEnable O D\n");
    pinMan.pinModeX(pinMotorMowEnable, OUTPUT);
    pinMan.digitalWriteX(pinMotorMowEnable, 1);
    delay(10000);
    pinMan.digitalWriteX(pinMotorMowEnable, 0);
    break;
  case pinMotorMowDir:
    printf("Test pinMotorMowDir O D\n");
    pinMan.pinModeX(pinMotorMowDir, OUTPUT);
    pinMan.digitalWriteX(pinMotorMowDir, 1);
    delay(10000);
    pinMan.digitalWriteX(pinMotorMowDir, 0);
    break;
  case pinSonarRightTrigger:
    printf("Test pinSonarRightTrigger O D\n");
    pinMan.pinModeX(pinSonarRightTrigger, OUTPUT);
    pinMan.digitalWriteX(pinSonarRightTrigger, 1);
    delay(10000);
    pinMan.digitalWriteX(pinSonarRightTrigger, 0);
    break;
  case pinMotorLeftDir:
    printf("Test pinMotorLeftDir O D\n");
    pinMan.pinModeX(pinMotorLeftDir, OUTPUT);
    pinMan.digitalWriteX(pinMotorLeftDir, 1);
    delay(10000);
    pinMan.digitalWriteX(pinMotorLeftDir, 0);
    break;
  case pinMotorRightDir:
    printf("Test pinMotorRightDir O D\n");
    pinMan.pinModeX(pinMotorRightDir, OUTPUT);
    pinMan.digitalWriteX(pinMotorRightDir, 1);
    delay(10000);
    pinMan.digitalWriteX(pinMotorRightDir, 0);
    break;
  case pinMotorRightFault:
    printf("Test pinMotorRightFault I D\n");
    pinMan.pinModeX(pinMotorRightFault, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinMotorRightFault));
      delay(1000);
    }
    break;
  case pinMotorMowFault:
    printf("Test pinMotorMowFault I D\n");
    pinMan.pinModeX(pinMotorMowFault, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinMotorMowFault));
      delay(1000);
    }
    break;
  case pinMotorLeftFault:
    printf("Test pinMotorLeftFault I D\n");
    pinMan.pinModeX(pinMotorLeftFault, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinMotorLeftFault));
      delay(1000);
    }
    break;
  case pinBumperRight:
    printf("Test pinBumperRight IU D\n");
    pinMan.pinModeX(pinBumperRight, INPUT_PULLUP);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinBumperRight));
      delay(1000);
    }
    break;
  case pinBumperLeft:
    printf("Test pinBumperLeft IU D\n");
    pinMan.pinModeX(pinBumperLeft, INPUT_PULLUP);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinBumperLeft));
      delay(1000);
    }
    break;
  case pinOdometryRight:
    printf("Test pinOdometryRight IU D\n");
    pinMan.pinModeX(pinOdometryRight, INPUT_PULLUP);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinOdometryRight));
      delay(1000);
    }
    break;
  case pinOdometryLeft:
    printf("Test pinOdometryLeft IU D\n");
    pinMan.pinModeX(pinOdometryLeft, INPUT_PULLUP);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.digitalReadX(pinOdometryLeft));
      delay(1000);
    }
    break;
  case pinMotorRightSense:
    printf("Test pinMotorRightSense I ADC\n");
    pinMan.pinModeX(pinMotorRightSense, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.analogReadX(pinMotorRightSense));
      delay(1000);
    }
    break;
  case pinMotorLeftSense:
    printf("Test pinMotorLeftSense I ADC\n");
    pinMan.pinModeX(pinMotorLeftSense, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.analogReadX(pinMotorLeftSense));
      delay(1000);
    }
    break;
  case pinBatteryVoltage:
    printf("Test pinBatteryVoltage I ADC\n");
    pinMan.pinModeX(pinBatteryVoltage, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.analogReadX(pinBatteryVoltage));
      delay(1000);
    }
    break;
  case pinMotorMowSense:
    printf("Test pinMotorMowSense I ADC\n");
    pinMan.pinModeX(pinMotorMowSense, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.analogReadX(pinMotorMowSense));
      delay(1000);
    }
    break;
  case pinChargeVoltage:
    printf("Test pinChargeVoltage I ADC\n");
    pinMan.pinModeX(pinChargeVoltage, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.analogReadX(pinChargeVoltage));
      delay(1000);
    }
    break;
  case pinChargeCurrent:
    printf("Test pinChargeCurrent I ADC\n");
    pinMan.pinModeX(pinChargeCurrent, INPUT);
    for (int i = 0; i < 10; i++)
    {
      printf("gelezen waarde %i\n", pinMan.analogReadX(pinChargeCurrent));
      delay(1000);
    }
    break;
  case pinChargeRelay:
    printf("Test pinChargeRelay O D*\n");
    pinMan.pinModeX(pinChargeRelay, OUTPUT);
    pinMan.analogWriteX(pinChargeRelay, 160);
    delay(10000);
    pinMan.analogWriteX(pinChargeRelay, 0);
    break;

  default:
    printf("There is no pin defined as %s\n", cmd.c_str());
    break;
  }
  printf("einde test");
  vTaskDelete(NULL);
}

void WebConsoleInputHandler(uint8_t *data, size_t len)
{
  char cmdstr[len + 1];
  memcpy(cmdstr, data, len);
  cmdstr[len] = 0;
  cmd = cmdstr;
  Serial.println(cmd);
  delay(1000);
  xTaskCreate(task_WebConsoleInputHandler, "task_webconsoleinput", 4096, NULL, tskIDLE_PRIORITY, NULL);
};

void setup()
{
  // standard IO streams are inherited when a task is created, so this needs to be done before creating other tasks:
  // _GLOBAL_REENT->_stdout = fwopen(NULL, multi_writefn);
  // _GLOBAL_REENT->_stderr = fwopen(NULL, &multi_writefn);
  // currently bug in fwopen, this is a workaround
  _GLOBAL_REENT->_stdout = funopen(NULL, NULL, &multi_writefn, NULL, NULL);
  _GLOBAL_REENT->_stderr = funopen(NULL, NULL, &multi_writefn, NULL, NULL);

  // enable line buffering for this stream (to be similar to the regular UART-based output)
  // static char stdout_buf[256];
  // static char stderr_buf[256];
  // setvbuf(_GLOBAL_REENT->_stdout, stdout_buf, _IOLBF, sizeof(stdout_buf));
  // setvbuf(_GLOBAL_REENT->_stderr, stderr_buf, _IOLBF, sizeof(stderr_buf));
  setbuf(_GLOBAL_REENT->_stdout, NULL);
  setbuf(_GLOBAL_REENT->_stderr, NULL);
  // Also redirect stdout/stderr of main task
  stdout = _GLOBAL_REENT->_stdout;
  stderr = _GLOBAL_REENT->_stderr;

  Serial.begin(115200);

  /**
   * Connect to Wifi
   */
  WiFi.begin("Duffelstraat 2.4", "SJiADgXg5Z");
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.println("Wifi connecting...");
    notConnectedCounter++;
    if (notConnectedCounter > 150)
    { // Reset board if not connected after 5s
      Serial.println("Resetting due to Wifi not connecting...");
      ESP.restart();
    }
  }
  Serial.print("Wifi connected, IP address: ");
  Serial.println(WiFi.localIP());
  /**
   * Enable OTA update
   */
  ArduinoOTA.begin();

  WebSerial.msgCallback(WebConsoleInputHandler);
  // WebSerial.begin(&server);
  WebServices webServices;
  webServices.begin(&server);
  server.begin();
  WebSerialActive = false;
  Wire.begin();
  scanner();
  pinMan.begin();
  setupSDCard();
}
int i;
void loop()
{
  // Check for over the air update request and (if present) flash it
  ArduinoOTA.handle();

  i++;
  if (i > 20)
  {
    //    WebSerial.println("Hello World");
    Serial.println("Hello World");
    i = 0;
    // std::cout<<"redirected from std::cout"<<std::endl;
    // printf("en vanuit printf %i",i);
  }
  delay(1000);
}
void scanner()
{
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
  {
    Serial.println("No I2C devices found\n");
  }
  else
  {
    Serial.println("done\n");
  }
}