// Low power weather station
//
// Copyright Rob Latour, 2023
// License: MIT
//
#define timeBetweenReportingReadingsInMinutes                              15
#define doubleTimeBetweenReportingReadingsWhenBatteryLevelIsBelow50Percent false

#define outputToSerialMonitor                                              true
#define turnOnBlueLEDWhenPocessing                                         false
#define publishBatteryLevel                                                false
#define publishLastCycleTime                                               true

#define MQTTTopic                                                          "WeatherStationIII"
