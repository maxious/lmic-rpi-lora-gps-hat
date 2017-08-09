Rasberry Pi Zero with RF96 and Adafruit Bonnet


|function|Wiring Pi | BCM|
|-----|-----|----|
|NSS|22|6|
|RESET|21|5|
|DIO0|24|19|
|DIO1|28|20|
|DIO2|29|21|
|MOSI||10|
|MISO||9|
|SCLK||11|
|Ground|||
|3.3V|||





```c
// Note: WiringPi Pin Numbering Schema
const int WIRING_PI_PIN_NSS = 22;
const int WIRING_PI_PIN_RST = 21;
const int WIRING_PI_PIN_DIO[3] = { 24, 28, 29 };

// Note: BCM Pin Numbering Schema
const int BCM_PIN_DIO[3] = { 19, 20, 21 };
```