# Multipurpose fermentation chamber
Arduino project for Multipurpose Fermenter Chamber

# Arduino
This code runs on an Arduino UNO

# Component List
- 1x Arduino NANO: https://www.ebay.com.au/itm/264896399554
- 1x Expansion Shield for Arduino Nano: https://www.ebay.com.au/itm/393364710321
- 1x LCD1602 16x2 Blue LCD Keypad Shield: https://www.ebay.com.au/itm/303878820588
- 1x BTS7960B H-Bridge: https://www.ebay.com.au/itm/225436991283
- 1x MOFSET Module: https://www.ebay.com.au/itm/404283060944
- 2x DHT Temperature Sensor: https://www.ebay.com.au/itm/175729001188
- 1x 12v Peltier Cooling System Kit: https://www.ebay.com.au/itm/404077418614
- 1x 12v 6A 75w switching power supply: https://www.ebay.com.au/itm/402600818586

  # Connection diagram
This diagram is for illustration purposes only and is not an exact representation of the final result.
  ![image](https://github.com/uron83/fermenter/assets/13381582/39977fab-4b40-4d80-8e1b-e50b70dbffa9)
  
  [Made with Wowki](https://wokwi.com/projects/377011821229324289)
  

  # Pinout
| Component | Pin  | Arduino | Other  |
|:---:|:---:|:---:|:---:|
| LCD1602 | A0  | A0  |   |
|   | RS  | D8  |   |
|   | EN  | D9  |   |
|   | D4  | D4  |   |
|   | D5  | D5  |   |
|   | D6  | D6  |   |
|   | D7  | D7  |   |
|   | BL  | D10  |   |
|   | GND  | GND  |   |
|   | 5V  | 5V  |   |
| BTS7960B | VCC  | 5V  |   |
|  | R_IS  | GND  |   |
|  | R_EN  | 5V  |   |
|  | R_PWM  | D3  |   |
|  | GND  | GND  |   |
|  | L_IS  | GND  |   |
|  | L_EN  |  5V |   |
|  | L_PWM  |  D2 |   |
|  | M+ |  | Peltier +  |
|  | M- |  | Peltier -  |
|  | B+ |  | PS 12v  |
|  | B- |  | PS GND  |
| DHT (int)  | VCC  |  5V |   |
|  | GND  |  GND |   |
|  | SDA  |  D16 (A2) |   |
| DHT (ext)  | VCC  |  5V |   |
|  | GND  |  GND |   |
|  | SDA  |  D15 (A1) |   |
| MOFSET  | VCC  |  D2 |   |
|  | GND  |  D2 |   |
|  | SIG  |  D2 |   |
|  | V+  |   | Coolers + (red)  |
|  | V-  |   | Coolers - (black) |
|  | VIN  |   | PS 12v  |
|  | GND  |   | PS GND  |
