## Arduino DS3231 ZS042 AT24C32 IIC Module
This readily available module employs two I2C devices:

- [DS3231](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf)
- [AT24C32](https://ww1.microchip.com/downloads/en/devicedoc/Atmel-8869-SEEPROM-AT24CS32-Datasheet.pdf)

**Both I2C devices function between 3.0V to 5.5V**<br>
### DS3231
For the DS3231 RTC, this part contains 19 registers (see PDF page 11/20)<br>
7-bit I2C address: 0x68

### AT24C32
7-bit I2C address: 0x57


### I2C Bus Interface / Board wiring to NUCLEO-F103RB
NOTE: The I2C pins used are not defaults, and require the pins defined accordingly

Signals Pins Used - Wire Colors - Access:
PB8 I2C1_SCL - Arduino D15 (CN5-10) and Morpho CN10-3 (white)
PB9 I2C1_SCA - Arduino D14 (CN5-9) and Morpho CN10-5 (green)
+3V3 - Arduino CN6-4 and Morpho CN7-16 (orange)
GND  - Arduino CN6-6, CN6-7, and Morpho CN7-19, CN7-20, CN7-22 (grey)
