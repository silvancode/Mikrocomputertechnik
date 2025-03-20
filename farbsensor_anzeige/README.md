# Arduino Projekt Farbsensor mit LED-Matrix

## Projektbeschreibung
In diesem Projekt habe ich einen Arduino genutzt, um eine LED-Matrix mit einem Farbsensor anzusteuern. Die LED-Matrix hat drei Reihen à 8 LEDs (RGB-Farben).

## Benötigte Materialien
- Arduino Board (Arduino Mega 2560 Rev3)
- USB-Kabel
- Breadboard
- Steckkabel
- 24 LEDs (Rot: 8, Grün: 8, Blau: 8)
- 3 Widerstände den LEDs angepasst
- Farbsensor Adafruit TCS34725

## Schaltplan
Die Bauteile werden wie folgt verbunden:
- An den 8 normalen Outputs wird je ein LED jeder Farbe angeschlossen (PWM Pin: 2 - 9)
- Der Minuspol der LEDs wird pro Farbe zusammengeführt und mit einem Widerstand auf drei Open-Collector Pins angeschlossen (Pin: 22, 23, 24)
- TCS34725 über VIN und GND mit 5V speisen, für den Datenbus SDA und SCL mit den selben Pins auf dem Arduino verbinden
