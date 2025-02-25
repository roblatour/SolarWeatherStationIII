## Solar Powered Weather Station III

When it comes to solar powered weather stations, the third time was the charm for me!

Here's a little about my first two attempts to put together a Solar Powered Weather Station:

**Attempt I** is described in detail [here](https://hackaday.io/project/179917-open-source-solar-powered-weather-station) and featured: open source software for an ESP32 devkit v1, Real Time Clock, five independent sensors, a  110 mm x 136 mm solar panel, a [DFRobot Solar Power Manager 5V](https://www.dfrobot.com/product-1712.html) solar charger and a 8650 battery which together provided a regulated 5 volts to the solution.  The weather station also had support for Over the Air (OTA) updating and direct weather services publishing.   It ran well in through the summer, but ultimately failed in the fall as it was too power hungry for the short Canadian winter days and long winter nights.

**Attempt II** is described in detail [here](https://github.com/roblatour/WeatherStation), and supplied power in the same way as the first attempt, but carved out some of the non-essential features such as multiple sensors and OTA updates (relying on a single BME680 sensor to report temperature, pressure and humidity), while also off loading the publishing of the weather status to an MQTT connected server inside my home.   Also, this attempt was an earlier adopter of the ESP32-C6 dev board hoping to realize power savings using a Wi-Fi 6 Targeted Wait Times.  When that didn't pan out, I also tried using a [Sparkfun TPL5100](https://www.sparkfun.com/products/15353) to reduce power consumption.  The full solution can be viewed [here]( https://github.com/roblatour/WeatherStation).   However, again, sadly while it lasted longer into the fall, it ultimately failed as the days grew shorter due to lack of power; likely also exasperated by poor battery efficiency in the cold.

... and now on to my latest attempt ... 

**Attempt III** has proven much more successful, and indeed has already survived December 21st (2023) - the shortest day of the year - along with below zero temperatures.  The secret-sauce is in the hardware used to power the solution.  For this the battery and charger from the first two attempts were replaced by a [750F Lithium Ion Capacitor](https://www.aliexpress.com/item/1005004881819434.html) and [one of these boards](https://www.tindie.com/products/jaspersikken/solar-harvesting-into-lithium-ion-capacitor/) supplying a regulated 3.3 volts.  Additionally, using these two components allowed the size of the solar panel to be significantly reduced to 48 mm x 65 mm.   The solution also leveraged the very low deep sleep power consumption of an [Unexpected Maker's FeatherS3](https://unexpectedmaker.com/shop.html#!/FeatherS3/p/577111310/category=154222511) board.  With the FeatherS3 I also used the optional [External u.FL 2.4Ghz Antenna](https://unexpectedmaker.com/shop.html#!/Ext-u-FL-2-4Ghz-Antenna/p/578941059) to help ensure a strong Wi-Fi signal.

Here's the size difference between the solar panel used in Attempts I  & II (first picture below) vs III (second picture below):

![Attempts I & II](https://github.com/roblatour/SolarWeatherStationIII/blob/main/Images/AttemptIandII.jpg)

![Attempt III](https://github.com/roblatour/SolarWeatherStationIII/blob/main/Images/AttemptIII.jpg)

**Plans for a fourth attempt** are on the way.  These include updating the design, and releasing the code, for more or less the same solution with the exception that I would like to try out the [Unexpected Maker's TinyC6](https://unexpectedmaker.com/shop.html#!/TinyC6/p/602208790/category=0) board in order to achieve a WiFi-6 solution.  I do however plan to let Attempt III run a little longer, so that it will have time to deal with much colder outside temperatures, before moving ahead.

So please stay tunned ... 

## Edit: January 2025
The fourth project has been delayed.  Weather Station III succumbed when several insects got into the case this summer and short circuited the board.  After a long delay, I finally got around to replacing the needed components but the replacement solution has been struggling with lack of daylight over the last couple months - something the original did not have issue with last winter.  Perhaps the Lithium Ion Capacitor is getting too old – if so that is truly disappointing because it would only have been running for about a year now.   

## Support

[<img alt="buy me  a coffee" width="200px" src="https://cdn.buymeacoffee.com/buttons/v2/default-blue.png" />](https://www.buymeacoffee.com/roblatour)
