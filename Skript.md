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

bool ledState = false;
bool lastButtonState = true; // Invertierte Logik für Pull-Up
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
    // Bei einem Wechsel von true zu false (Taster wurde gedrückt)
    if (lastButtonState == true && currentButtonState == false) {
      ledState = !ledState;  // LED-Zustand umschalten
      digitalWrite(ledPin, ledState ? HIGH : LOW);
    }
  }

  // Aktuellen Zustand als letzten Zustand speichern
  lastButtonState = currentButtonState;
}
```
## Übung 4.6 Fade-in Fade-Out
---
```C
const int taster = 2;
const int led = 5;

bool ledState = false;
bool lastButtonState = true; // Invertierte Logik für Pull-Up
int lastDebounceTime = 0; // Zeitstempel der letzten Änderung
const int debounceDelay = 50; // Entprellzeit (ms)
const int fadeTime = 200; // Fade-Zeit (ms)

void setup() {
  pinMode(taster, INPUT_PULLUP);  // Pull-Up Widerstand definieren
  pinMode(led, OUTPUT);
}

void loop() {
  bool currentButtonState = digitalRead(taster);

  // Prüfen, ob sich der Tasterzustand geändert hat
  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();  // Zeitpunkt der letzten Änderung merken
  }

  // Wenn der Tasterzustand länger als debounceDelay stabil ist, verarbeiten
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Bei einem Wechsel von true zu false (Taster wurde gedrückt)
    if (lastButtonState == true && currentButtonState == false) {
      ledState = !ledState;  // LED-Zustand umschalten
      for (int dim = fadeTime; dim > 0; dim--) {
        // Durch invertieren des Zahlbereichs in Map Fade-In oder -Out
        if (ledState) {
            map(dim, 0, fadeTime, 0, 255);
            analogWrite(led, dim);
        }else{
            map(dim, 0, fadeTime, 255, 0);
            analogWrite(led, dim);
        }
        delay(1)
      }
    }
  }

  // Aktuellen Zustand als letzten Zustand speichern
  lastButtonState = currentButtonState;
}
```
## Übung 4.7 Interrupt
---
```C
const int ledPin = 5;
const int buttonPin = 2;

volatile bool buttonPressed = false; // fallende Flanke
bool ledState = false; // LED-Status

const int fadeTime = 200; // Fade-Vorgangs in ms

void IRAM_ATTR InterruptHandler() {
  buttonPressed = true; // Flanke setzen
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), InterruptHandler, FALLING);
}

void fadeLED(bool fadeIn) {
  unsigned long interval = fadeTime / 255;  // Zeit pro Schritt berechnen

  if (fadeIn) {
    // LED langsam einschalten
    for (int brightness = 0; brightness <= 255; brightness++) {
      analogWrite(ledPin, brightness);
      delay(interval);
    }
  } else {
    // LED langsam ausschalten
    for (int brightness = 255; brightness >= 0; brightness--) {
      analogWrite(ledPin, brightness);
      delay(interval);
    }
  }
}

void loop() {
  if (buttonPressed) {
    buttonPressed = false; // Zurücksetzen der Flanke
    ledState = !ledState; // Zustand umschalten
    fadeLED(ledState); // LED faden (ein- oder aus)
  }
}
```
## Übung 4.8 Fade kombiniert
---
```C
const int ledPin = 5;
const int buttonPin = 2;

// Einstellungen (frei wählbar)
const unsigned int fadeTime = 500;      // Dauer des Fade (ms)
const unsigned int debounceDelay = 50;  // Entprellzeit (ms)

volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0;

bool ledState = false;

void IRAM_ATTR InterruptHandler() {
  unsigned long interruptTime = millis();

  // Software-Entprellen direkt im Interrupt (zeitbasiert)
  if (interruptTime - lastInterruptTime > debounceDelay) {
    buttonPressed = true;
    lastInterruptTime = interruptTime;
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), InterruptHandler, FALLING);
}

void fadeLED(bool fadeIn) {
  unsigned int interval = fadeTime / 255;

  if (fadeIn) {
    for (int brightness = 0; brightness <= 255; brightness++) {
      analogWrite(ledPin, brightness);
      delay(interval);
    }
  } else {
    for (int brightness = 255; brightness >= 0; brightness--) {
      analogWrite(ledPin, brightness);
      delay(interval);
    }
  }
}

void loop() {
  if (buttonPressed) {
    buttonPressed = false;
    ledState = !ledState;
    fadeLED(ledState);
  }
}
```
