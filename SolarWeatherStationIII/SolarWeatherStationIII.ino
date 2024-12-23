// Solar Weather Station III
//
// Copyright Rob Latour, 2024
// License: MIT
//
// https://github.com/roblatour/SolarWeatherStationIII
//
// Compile and upload using Arduino IDE (2.2.2 or greater)
//
// Physical board:                        Feather S3
//
// Board in Arduino board manager:        UM FeatherS3
//
// Arduino Tools settings:
//
// USB CDC On Boot:                       "Enabled"
// CPU Frequency:                         "240MHz (WiFi)"
// Core Debug Level:                      "None"
// USB DFU On Boot:                       "Disabled"
// Erase All Flash Before Sketch Upload:  "Disabled"
// Events Run On:                         "Core 1"
// Flash Mode:                            "QI0"
// Arduino Runs On:                       "Core 1"
// USB Firmware MSCOn Boot:               "Disabled"
// Partition Scheme:                      "Default (6.25MB APP/3.43MB SPIFFS)"
// PSRAM:                                 "Enabled"
// Upload Mode:                           "USB-OTG CDC (TinyUSB)"
// Upload Speed:                          "921600"
// USB Mode:                              "Hardware CDC and JTAG"
//
// ----------------------------------------------------------------
// Programmer                             ESPTool
// ----------------------------------------------------------------

#include "secrets.h"  // contains the MQTT and Wi-Fi credentials used in this sketch

#include "general.h"

// Feather S3 board
#include <UMS3.h>
UMS3 ums3;


#include <EspMQTTClient.h>  // https://github.com/plapointe6/EspMQTTClient
                            // please also ensure this patch is applied: https://github.com/plapointe6/EspMQTTClient/pull/138/files

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"  // https://github.com/adafruit/Adafruit_BME680

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme;

#include <string.h>

#define ProgramName "SolarWeatherStationIII"

EspMQTTClient client(
  WiFiSSID,
  WIFIPassword,
  MQTTBroker,
  MQTTUser,
  MQTTPassword,
  MQTTClientName,
  MQTTPort);

void serialPrintHMSFromMs(int32_t timeInMs) {

  unsigned long seconds = timeInMs / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  seconds %= 60;
  minutes %= 60;
  hours %= 24;

  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
}

void publishStatus(String subtopic, String message) {

  String fullTopic;
  fullTopic = MQTTTopic;
  fullTopic.concat("/");
  fullTopic.concat(subtopic);

  if (outputToSerialMonitor)
    Serial.println("Publishing " + fullTopic + " " + message);

  client.publish(fullTopic, message);
};

void publishReadings(String subtopic, float value) {

  String fullTopic;
  fullTopic = MQTTTopic;
  fullTopic.concat("/");
  fullTopic.concat(subtopic);

  String valueAsAString = String(value, 2);  // 2 decimal places

  if (outputToSerialMonitor)
    Serial.println("Publishing " + fullTopic + " " + valueAsAString);

  client.publish(fullTopic, valueAsAString);
};

void getAndPublishReadings() {

  // Power up the BME680 sensor
  ums3.setLDO2Power(true);
  delay(250);

  // Initialize the BME680 sensor
  if (bme.begin()) {

    float temperature = 0;
    float pressure = 0;
    float humidity = 0;

    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setHumidityOversampling(BME680_OS_2X);

    bool readingsPerformed = bme.performReading();

    if (readingsPerformed) {

      temperature = bme.temperature;
      if ((temperature >= -60.0) && (temperature <= 140.0))
        publishReadings("temperature", temperature);
      else
        publishReadings("error_temperature", temperature);

      pressure = bme.pressure / 100.0;
      if ((pressure >= 870.0) && (pressure <= 1090.0))
        publishReadings("pressure", pressure);
      else
        publishReadings("error_pressure", pressure);

      humidity = bme.humidity;
      if ((humidity >= 0.0) && (humidity <= 100.0))
        publishReadings("humidity", humidity);
      else
        publishReadings("error_humidity", humidity);

    } else {

      String errorMessage = "Failed to get a reading";

      if (outputToSerialMonitor)
        Serial.println(errorMessage);

      publishStatus("error_message", errorMessage);
    };
	
	ums3.setLDO2Power(false);

  } else {

    ums3.setLDO2Power(false);

    String errorMessage = "Could not find a valid BME680 sensor, check wiring!";

    if (outputToSerialMonitor)
      Serial.println(errorMessage);

    publishStatus("error_message", errorMessage);
  };
};


void getAndPublishRunTime() {

  if (publishLastCycleTime) {
    float lastCycleTime = ((float)millis() + 520.0) / 1000.0;  // the 520 ms are added to account for the delays and processing below
    publishReadings("last_cycle_time", lastCycleTime);
  };
};

void goToSleep() {

  // Goto sleep
  if (outputToSerialMonitor)
    Serial.println("Going to sleep ...");

  // Calculate Time to Sleep (in uS) = minutes between readings (converted to uS)
  int32_t TimeToSleep = timeBetweenReportingReadingsInMinutes * 60 * 1000000;

  // Reduce the Time to Sleep by the time spent in processing through to this point
  // This adjust the timeing between publications to more accurately be the desired minutes between reported readings

  int32_t cycleTime = millis();

  TimeToSleep -= cycleTime * 1000;

  if (outputToSerialMonitor) {
    Serial.print("Cycle time: ");
    serialPrintHMSFromMs(cycleTime);
    Serial.println("");

    Serial.print("Sleep time: ");
    serialPrintHMSFromMs(TimeToSleep / 1000);
    Serial.println("");

    delay(20);  // allow time to finishing outputting to serial monitor
  };

  if (turnOnBlueLEDWhenPocessing)
    ums3.setBlueLED(false);

  esp_sleep_enable_timer_wakeup(TimeToSleep);
  esp_deep_sleep_start();
};

void onConnectionEstablished() {

  if (outputToSerialMonitor)
    Serial.println("Connection established");

  getAndPublishReadings();

  getAndPublishRunTime();

  delay(500);  // allow time for any remaining publishing to complete

  goToSleep();
}

void setup() {

  if (outputToSerialMonitor) {
    Serial.begin(115200);
    Serial.println("************************************************");
    Serial.println("Starting " + String(ProgramName));
  };

  // Initializes UM Feather S3 board peripherals
  ums3.begin();
  ums3.setLDO2Power(false);

  // re-initial EspMQTTClient (this is required to reset things after awaking from deep sleep)
  EspMQTTClient(
    WiFiSSID,
    WIFIPassword,
    MQTTBroker,
    MQTTUser,
    MQTTPassword,
    MQTTClientName,
    MQTTPort);

  client.enableDebuggingMessages(outputToSerialMonitor);

  if (turnOnBlueLEDWhenPocessing)
    ums3.setBlueLED(true);
}

void loop() {

  static bool reportWiFiConnectOnce = true;
  static bool reportMQTTConnectOnce = true;
  static wl_status_t lastWiFiStatus = WL_NO_SHIELD;

  client.loop();

  if (reportWiFiConnectOnce)
    if (client.isWifiConnected()) {
      reportWiFiConnectOnce = false;
      Serial.print("WiFi connected");
    };

  if (reportMQTTConnectOnce)
    if (client.isMqttConnected()) {
      reportMQTTConnectOnce = false;
      Serial.print("MQTT connected");
    };

  // if after a minute mqtt is still not connected then turn off the WiFi and go into deep sleep so as to try again in next cycle

  if ((millis() > 60000) && (!client.isMqttConnected())) {

    if (outputToSerialMonitor)
      Serial.println("Saftey sleep");

    goToSleep();
  };

  if (WiFi.status() != lastWiFiStatus) {
    lastWiFiStatus = WiFi.status();
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
  };

  delay(25);
}
