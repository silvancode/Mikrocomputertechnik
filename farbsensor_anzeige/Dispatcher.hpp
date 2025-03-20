#ifndef _DISPATCHER_HPP
#define _DISPATCHER_HPP

#include "Arduino.h"

// Mit der Dispatcher-Klasse ist es einfacher, bestimmte Funktionen
// in einem festen Intervall (bspw. alle 30ms, etc.) aufzurufen.
// Alle Funktionen, die vom Dispatcher aufgerufen werden sollen, muessen
// folgendes Profil haben:
//
typedef void (*TaskFunc)();

//----------------------------------------------------------------------------
//
// Task (class) --
//
// Die Klasse Task wird fuer eine durch den Dispatcher aufrufbare
// Aktivitaet verwendet. Nur Tasks koennen dem Dispatcher zur Ausfuehrung
// uebergeben werden.
//
class Task {
friend class Dispatcher;
private:
  Task *next;
  TaskFunc func;
  unsigned long execTime, interval;
  unsigned long numCalls, lag;
  void Start(unsigned long t0);
public:
  Task(TaskFunc f, unsigned long interval);
  virtual void Run();
  unsigned long Interval();
  void SetInterval(unsigned long interval);
  unsigned long Calls();
  unsigned long Lag();
  unsigned long Duration();
};

Task::Task(TaskFunc f, unsigned long i) {
  next = NULL;
  func = f;
  interval = i;
  execTime = 0;
  lag = 0;
  numCalls = 0;
}

void Task::Start(unsigned long t0) {
  numCalls += 1;
  lag += (t0 - execTime);
  Run();
  execTime = t0 + interval;
}

void Task::Run() {
  func();
}

unsigned long Task::Interval() {
  return interval;
}

void Task::SetInterval(unsigned long i) {
  interval = i;
}

unsigned long Task::Calls() {
  return numCalls;
}

unsigned long Task::Lag() {
  return lag;
}

unsigned long Task::Duration() {
  return execTime;
}

//----------------------------------------------------------------------------
//
// Dispatcher (class) --
//
class Dispatcher {
private:
  // Die Tasks sind nach ihrer Ausfuehrungszeit (execTime) sortiert
  // als einfach verkettete Liste organisiert. readyList zeigt dabei auf
  // den ersten Task.
  Task *readyList;

  // Diese beiden Methoden dienen dem effizienten Entfernen
  // und sortierten Einfuegen von Tasks in die readyList. Fuer
  // eine einfachere Fehleranalyse habe ich alle Elemente dieser
  // Liste public deklariert.
  //
  // Mit Pop wird der erste Task der Liste retorniert, dessen
  // Ausfuehrungszeit kleiner oder gleich t ist. Die Ausfuehrungszeit
  // wird dabei in Millisekunden seit Start des Programmes gemessen, also
  // genau das, was millis() als Rueckgabewert liefert.
  Task *Pop(unsigned long t);

  // Mit Insert wird der Task in die Liste der bereitstehenden Tasks
  // eingereiht. Das Feld execTime von task muss vorgaengig korrekt
  // initiiert werden.
  void Insert(Task *task);

public:

  Dispatcher();
  // Diese Methode muss innerhalb der loop() Funktion so oft wie moeglich
  // aufgerufen werden. Falls die gesamte Applikationslogik in Dispatcher-
  // Funktionen steckt, sieht loop() wie folgt aus:
  //
  //   void loop() {
  //       disp.Tick()
  //  }
  //
  void Tick();
  // Fuegt dem Dispatcher einen neuen Task hinzu.
  void AddTask(Task &task);
  // Loescht eine Funktion aus dem Dispatcher.
  void DelTask(Task &task);
};

//----------------------------------------------------------------------------

Dispatcher::Dispatcher() {
  readyList = NULL;
}

void Dispatcher::AddTask(Task &task) {
  unsigned long currentTime = millis();

  if (task.execTime < currentTime) {
    task.execTime = currentTime + task.interval;
  }
  Insert(&task);
}

void Dispatcher::DelTask(Task &t) {
  Task *task, *ptr;

  task = &t;
  if (readyList == NULL) {
    return;
  }
  if (readyList == task) {
    readyList = task->next;
    task->next = NULL;
    return;
  }
  ptr = readyList;
  while (ptr->next != NULL) {
    if (ptr->next == task) {
      ptr->next = task->next;
      task->next = NULL;
      return;
    }
  }
}

void Dispatcher::Tick() {
  Task *task;
  // In currentTime wird die Zeit (in ms seit Programmstart) gespeichert.
  // Damit wird einerseits der naechste ausfuehrbare Task ermittelt und
  // auch die Start()-Methode des Tasks aufgerufen. Bin noch unsicher, ob
  // currentTime pro Aufruf von Tick() befuellt werden soll (wie aktuell)
  // oder vor jedem Pop() innerhalb der Schleife.
  unsigned long currentTime = millis();

  while (1) {
    task = Pop(currentTime);
    if (task == NULL) {
      return;
    }
    task->Start(currentTime);
    if (task->interval > 0) {
      Insert(task);
    }
  }
}

Task *Dispatcher::Pop(unsigned long t) {
  Task *ptr;

  if (readyList == NULL) {
    return NULL;
  }
  ptr = readyList;
  if (ptr->execTime > t) {
    return NULL;
  }
  readyList = ptr->next;
  ptr->next = NULL;
  return ptr;
}

void Dispatcher::Insert(Task *task) {
  Task *ptr;

  ptr = readyList;
  if (ptr == NULL) {
    readyList = task;
  } else if (task->execTime <= ptr->execTime) {
    task->next = ptr;
    readyList = task;
  } else {
    while (ptr->next != NULL) {
      if (task->execTime <= ptr->next->execTime) {
        break;
      }
      ptr = ptr->next;
    }
    task->next = ptr->next;
    ptr->next = task;
  }
}

#endif
