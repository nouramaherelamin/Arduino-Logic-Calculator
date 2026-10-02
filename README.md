# Logic Calculator

A handheld, Arduino-based multi-mode calculator with a custom 3D-printed enclosure designed in SolidWorks.
Built as a team project for the **Digital Logic Design** course at the Egyptian Chinese University (ECU),
Faculty of Computer & Information Systems.

![Final device](images/final-device.jpg)

## Motivation

Branded scientific calculators in Egypt now cost roughly 1,000-2,000 EGP. The goal of this project was to build a
calculator from simple, low-cost components (target: under 600 EGP) that covers most of the functions of a regular
scientific calculator, plus a few extra ideas of our own (currency converter and a math game).

## Features

| Mode | What it does |
|------|--------------|
| Basic | `+ - x /` with decimal input |
| Scientific | Basic plus a power (`^`) operator in the evaluator (see [Known issues](#known-issues)) |
| Unit converter | kg<->g, km<->m, C<->F |
| Currency converter | USD / EUR / GBP / SAR to EGP (fixed rates) |
| Math game | Random `+ - x /` questions, 3 attempts, score counter |

## Hardware

- Arduino UNO R3
- 16x2 character LCD (parallel interface, no I2C)
- 5x4 membrane keypad (20 keys)
- 10 kOhm potentiometer (LCD contrast, knob on the side of the case)
- 220 Ohm resistor for the LCD backlight
- 2 x 18650 Li-ion cells (3.7 V, 3800 mAh) with holders
- 3D-printed enclosure (PLA)

## Pin mapping

**LCD** (`LiquidCrystal`, 4-bit mode)

| LCD pin | Arduino pin |
|---------|-------------|
| RS | 12 |
| EN | 11 |
| D4 | 5 |
| D5 | 4 |
| D6 | 3 |
| D7 | 2 |

**LCD power and contrast**

| LCD pin | Connection |
|---------|------------|
| VSS, RW, K | GND |
| VDD | 5V |
| VO | Potentiometer middle pin (outer pins to 5V and GND) |
| A | 5V through a 220 Ohm resistor |

**Keypad** (`Keypad`)

| Rows (top to bottom) | Arduino pins |
|----------------------|--------------|
| R1-R5 | 10, 9, 8, 7, 6 |

| Columns (left to right) | Arduino pins |
|-------------------------|--------------|
| C1-C4 | 13, A4 (18), A5 (19), A3 (17) |

**Keypad layout**

```
[F1] [F2] [#]  [*]
[1]  [2]  [3]  [Up]
[4]  [5]  [6]  [Down]
[7]  [8]  [9]  [Esc]
[Left] [0] [Right] [Ent]
```

## Controls

- **Menu:** Up / Down to choose a mode, `Ent` or `#` to confirm.
- **Basic / Scientific:** `F1` = `+`, `F2` = `-`, `*` = multiply, `#` = divide, `Ent` = equals, `Esc` = clear, Left arrow = decimal point.
- **Converters:** Up / Down to change the conversion type, type a number, `Ent` to convert, `Esc` to clear, Left arrow = decimal point.
- **Game:** type the answer, `Ent` to submit, `Esc` to clear.
- **Back to menu:** press `#` twice quickly (inside any mode).

## Software

1. Install the Arduino IDE.
2. Install the **Keypad** library (Library Manager, search "Keypad" by Mark Stanley and Alexander Brevig).
   `LiquidCrystal` ships with the IDE.
3. Open `firmware/Logic_Calculator/Logic_Calculator.ino`.
4. Select **Arduino Uno**, pick the port, and upload.

Serial output at 9600 baud prints debug messages (pressed keys, current expression).

## Mechanical design

The enclosure was modeled in SolidWorks, sized to hold the Arduino, batteries, LCD and keypad,
then printed in PLA on a 3D printer.

| | |
|---|---|
| ![Closed](images/cad-enclosure-closed.jpg) | ![Internal layout](images/cad-internal-layout.jpg) |
| ![Side section](images/cad-side-section.jpg) | ![Close-up](images/cad-arduino-batteries-closeup.jpg) |

## Build

![Wiring and assembly](images/build-wiring.jpg)

## Challenges

Some keys on the 5x4 keypad triggered the wrong functions, which confused the calculator's operations.
This was solved by handling each key explicitly in code with `if` statements, so the same key can have
different jobs depending on the active mode.

## Known issues

- **Power operator is not reachable.** `^` is handled by the evaluator, but `powerMode` is never set to `true`,
  so Scientific mode currently behaves like Basic mode.
- **No operator precedence.** Expressions are evaluated left to right (e.g. `2 + 3 x 4` gives 20).
- **Currency rates are hardcoded** in `convertCurrency()` and need manual updates.
- **Long expressions overflow** the 16-character LCD line.

## Future work

- Voice command input, with accessibility for visually impaired users.
- Cloud sync of history and settings, plus a mobile version with offline currency conversion.
- AI-based math help and a learning mode with personalized quizzes.

## Project structure

```
Logic-Calculator/
|-- firmware/Logic_Calculator/Logic_Calculator.ino
|-- images/
|-- README.md
`-- .gitignore
```

## Team

Team project, Digital Logic Design, Faculty of Computer & Information Systems, Egyptian Chinese University (ECU).
Contributor: Noura Maher Elamin.
