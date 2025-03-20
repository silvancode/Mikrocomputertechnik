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
const int taster = 2;
const int led = 5;

void setup() {
  pinMode(taster, INPUT_PULLUP); // Pull-Up Widerstand definieren
  pinMode(led, OUTPUT);
}

void loop() {
  button = digitalRead(taster);
  // Umkehren der Logik
  if (button) {
    digitalWrite(led, LOW);
  }else{
    digitalWrite(led, HIGH);
  }
}
```
## Übung 4.4 Widerstände (Pullup, Pulldown)
---
Es sind alle Inputs der UNO-Reihe als Pull-Up definierbar, man kann immer einen internen Widerstand einschalten.
Pull-Down Widerstände gibt es nicht, das wäre einfach ein Vorwiderstand einer LED.
## Übung 4.5 Lichtschalter
---
```C
const int taster = 2;
const int led = 5;

bool ledState = LOW;
bool lastButtonState = HIGH; // Invertierte Logik für Pull-Up
int lastDebounceTime = 0; // Zeitstempel der letzten Änderung
const int debounceDelay = 50; // Entprellzeit (ms)

void setup() {
  pinMode(taster, INPUT_PULLUP);  // Pull-Up Widerstand definieren
  pinMode(led, OUTPUT);
}

void loop() {
  bool currentButtonState = digitalRead(taster);

  // Prüfen, ob sich der Tasterzustand geändert hat
  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();  // Zeitpunkt der letzten Änderung merken
  }

  // Wenn der Tasterzustand länger als debounceDelay stabil ist, verarbeiten
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Bei einem Wechsel von HIGH zu LOW (Taster wurde gedrückt)
    if (lastButtonState == HIGH && currentButtonState == LOW) {
      ledState = !ledState;  // LED-Zustand umschalten
      digitalWrite(ledPin, ledState ? HIGH : LOW);
    }
  }

  // Aktuellen Zustand als letzten Zustand speichern
  lastButtonState = currentButtonState;
}
```