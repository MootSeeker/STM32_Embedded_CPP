# GitHub Actions Plan für STM32 Embedded C++

## Was sind GitHub Actions?
GitHub Actions ist eine Plattform für Continuous Integration (CI) und Continuous Delivery (CD), die es ermöglicht, Software-Workflows direkt im Repository zu automatisieren. Für dieses "Learning"-Projekt bedeutet das primär:
- **Automatisierung:** Bei jedem `push` oder `pull request` werden definierte Skripte ausgeführt.
- **Qualitätssicherung:** Sicherstellen, dass neuer Code das Projekt nicht "kaputt" macht (kompiliert noch?).
- **Style Enforcement:** Automatische Überprüfung der strikten Coding-Guidelines (BSD Allman, Naming).

## Warum GitHub Actions für dieses Projekt?
Auch wenn wir keine Hex-Files deployen, helfen Actions dabei:
1.  **Build-Integrität:** Jeder Commit wird in einer sauberen Umgebung kompiliert. "Es funktioniert auf meinem Rechner" wird vermieden.
2.  **Lerneffekt:** Umgang mit CI-Pipelines ist heute Standard in der Embedded-Entwicklung.
3.  **Code-Qualität:** Automatische Checks auf Style-Verstöße oder potenzielle Bugs.

## Ausgewählte Workflows (Priorisiert)

### 1. Host-Build (C++ Logic Check)
Anstatt den STM32-Compiler (Muss installiert werden, kompliziertes Make) zu nutzen, prüfen wir die C++ Logik direkt.
- **Was:** Kompilierung von `App/` und `Library/` mit `g++` (Standard Linux Compiler).
- **Ziel:** Schnelles Finden von Syntaxfehlern in der Anwendungslogik.
- **Voraussetzung:** Hardware-Abhängigkeiten (`Device/`) müssen gemockt oder sauber getrennt sein.

### 2. Code Style Check
Automatische Durchsetzung der Projekt-Richtlinien.
- **Tool:** `clang-format`.
- **Regeln:** BSD Allman Klammern, Naming Conventions (soweit möglich).
- **Aktion:** CI schlägt fehl, wenn Code falsch formatiert ist.

### 3. Statische Code-Analyse
Tiefgehende Prüfung auf potenzielle Bugs und C++ Best Practices.
- **Tool:** `cppcheck`.
- **Ziel:** Erkennen von Speicherlecks, uninitialisierten Variablen und Array-Out-of-Bounds Zugriffen.

### 4. Dokumentation
Automatische Generierung einer Projekt-Webseite aus den Code-Kommentaren.
- **Tool:** Doxygen.
- **Ziel:** Immer aktuelle HTML-Doku, gehostet auf GitHub Pages.

### 5. Unit Tests (Host-Side)
Testen der Logik (Rechentests, State-Machines) auf dem Server.
- **Framework:** GoogleTest oder Catch2.
- **Ablauf:** Die Tests werden im Container kompiliert und ausgeführt. Schlägt ein Test fehl, wird der Commit abgelehnt.

### 6. ToDo Issue Management
Automatische Verwaltung von offenen Aufgaben im Code.
- **Tool:** Todo-to-Issue Action.
- **Funktion:**
    - Findet `// TODO: ...` im Code -> Erstellt ein GitHub Issue.
    - Wird das TODO im Code gelöscht -> Schließt das GitHub Issue automatisch.

## Nächste Schritte zur Umsetzung

Wir beginnen mit Punkt 2 (Style), da dieser die geringsten Abhängigkeiten hat.

1.  **Style:** `.clang-format` Konfigurationsdatei erstellen.
2.  **Basic Build:** `App.cpp` Compile-Test einrichten.
3.  **Analysis:** `cppcheck` integrieren.
4.  **ToDo:** Action aktivieren.
5.  **Tests & Doku:** Als fortgeschrittene Schritte später.
