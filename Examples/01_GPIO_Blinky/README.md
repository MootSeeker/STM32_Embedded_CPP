# GPIO Blinky Example

This example demonstrates the basic usage of the `GPIO` library to control a digital output (LED).

## Description
The application initializes a GPIO pin as an output and toggles it in an infinite loop, creating a blinking effect. It demonstrates the separation of initialization (`App_Init`) and the main loop (`App_Run`).

## Wiring
*   **LED Pin:** `PB11` (Port B, Pin 11).
    *   *Note:* Adjust the pin definition in `App.cpp` to match your specific hardware (e.g., `PA5` for Nucleo LD2).

## Code Structure
*   **App_Init()**: Allocates and configures the `GPIOOutput` object.
*   **App_Run()**: Contains the main `while(1)` loop that toggles the LED.

## Usage
1.  Copy the content of `App.cpp` from this folder to `App/Src/App.cpp`.
2.  Build and Run.
