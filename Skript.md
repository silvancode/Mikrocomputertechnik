# Aufgaben
---
## Übung 4.1 Timing
---
Die Funktion `delay()` pausiert das Programm an dieser Stelle und verwendet Millisekunden als Zeitangabe:
```c
// 1000ms = 1s
delay(1000)
``` 

- `delay()` und `delayMicroseconds()` blockieren den Ablauf, d.h. während der Pause können keine anderen Aufgaben ausgeführt werden.
- Für zeitkritische Anwendungen oder hochpräzise Messungen können die Funktionen aufgrund von Interrupts oder anderen Hintergrundprozessen kleine Abweichungen aufweisen.
- Ein typischer Arduino (z. B. Uno) läuft mit **16 MHz**, d. h. es gibt 16 Millionen Takte pro Sekunde. Das entspricht theoretisch ca. **62,5 ns pro Takt**.
## Übung 4.2 Dynamischer Blinker
---

```c
const int ledPin = 13;
const int cicleTime = 2000;
int change = 100;
int timeOn = 0;
  
void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  timeOn += change; // Schritte
  
  digitalWrite(ledPin, HIGH);
  delay(timeOn); // Zeit eingeschalten
  digitalWrite(ledPin, LOW);
  delay(cicleTime-timeOn); // Zeit ausgeschalten

  if (cicleTime <= timeOn) {
    change = -change; // Für ein wiederkehrendes Muster
  }
}
```
## Übung 4.3 Pull-Up
---
```c

```