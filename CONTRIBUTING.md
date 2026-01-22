# Contributing to STM32 Embedded C++ Learning Space

Thank you for your interest in contributing to this learning space! We welcome contributions that help others understand how to program STM32 microcontrollers using modern C++.

Since this is an educational repository, code readability, documentation, and clean structure are just as important as functionality.

## Getting Started
1.  **Hardware**: The primary target is the **STM32L4xx**. If you are adding examples for other boards, please document the pinout changes clearly.
2.  **IDE / Toolchain**:
    *   We use **VS Code** or **STM32CubeIDE**.
    *   Ensure you have the ARM GCC toolchain installed.
    *   Formatting is handled via `.clang-format` (if available) or manual adherence to the style guide.

## Workflow
1.  **Fork & Clone** the repository.
2.  **Create a Branch** for your feature or fix:
    *   `git checkout -b feature/new-spi-driver`
    *   `git checkout -b fix/uart-buffer-overflow`
    *   `git checkout -b doc/add-pinout-diagram`
3.  **Implement your changes**.
4.  **Test** on actual hardware if possible.
5.  **Submit a Pull Request (PR)** with a clear description of what you added.

## Coding Style & Guidelines
We follow the **[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)**, but with **specific overrides** to fit our embedded context and personal preference.

### 1. Formatting (The "Allman" Exception)
Unlike Google Style (which uses "K&R" or "Attached" braces), we use the **BSD Allman** style. Braces must always be on a new line.

**Right:**
```cpp
void MyFunction(int x)
{
    if (x > 0)
    {
        DoSomething();
    }
    else
    {
        DoError();
    }
}
```

**Wrong:**
```cpp
void MyFunction(int x) {  // Don't do this
    if (x > 0) {
        DoSomething();
    }
}
```

### 2. Embedded C++ Constraints
*   **No Exceptions**: Do not use `try`, `catch`, or `throw`. The project is compiled with `-fno-exceptions`.
*   **Memory Management**:
    *   Dynamic allocation (`new`) is allowed **only during initialization** (e.g., in `setup()`).
    *   Avoid usage of standard containers (`std::vector`, `std::map`) in hot paths or ISRs unless you are certain about the allocator behavior.
    *   Prefer stack allocation or static buffers where possible.
*   **HAL Wrappers**:
    *   Do not expose raw `HAL_` types (like `UART_HandleTypeDef`) in the public application headers (`App.h`).
    *   Wrap low-level HAL calls inside the library classes (`Library/Src`).
*   **Justify Necessary Complexity**: If a complex implementation (e.g., dynamic allocation in a critical path) is unavoidable, clearly explain the reasoning in your Pull Request or commit message. This helps reviewers fact-check the logic and understand the trade-offs.

### 3. Naming Conventions
*   **Classes**: `UpperCamelCase` (e.g., `DigitalOut`, `UpsilonTimer`).
*   **Structs**: `s` prefix and `_t` suffix (e.g., `sConfigModbus_t`).
*   **Enums**: `e` prefix and `_t` suffix (e.g., `eControlModule_t`).
*   **Global Functions**: `UpperCamelCase` (e.g., `InitSystem`, `RunApp`).
*   **Static / Private Functions**: `lowerCamelCase` or prefixed with `_` if private in a class (e.g., `calculateChecksum`, `_internalReset`).
*   **Function Parameters**: `UpperCamelCase` (e.g., `void SetSpeed(int Speed)`).
*   **Local Variables**: `lowerCamelCase` (e.g., `ledCounter`).
*   **Constants/Macros**: `ALL_CAPS` (e.g., `MAX_BUFFER_SIZE`).
*   **Member Variables**: No trailing underscores (unlike Google style). Just usage context is usually enough.
*   **A Note on Style**: `snake_case` is also acceptable for functions and variables, as long as the chosen style is used consistently within a given module. Readability is the priority.

## Adding New Examples

If you are adding a new learning module:

1.  Create a folder in `Examples/` (e.g., `Examples/04_SPI_Display`).
2.  Include a `App.cpp` that contains the full implementation.
3.  Include a `README.md` in that folder explaining:
    *   What the example does.
    *   Wiring (which pins go where).
    *   Expected output.

## Commit Messages

Please write clear commit messages:

*   Usage of present tense ("Add feature" not "Added feature").
*   Prefix the scope if applicable:
    *   `[GPIO] Fix pull-up configuration`
    *   `[Docs] Update contributing guide`
    *   `[Example] Add ADC reading example`

## 🤖 AI Usage Guidelines

We embrace the use of AI tools (Copilot, ChatGPT, Claude, etc.) as assistants, but not as replacements for engineering judgment.

1.  **Verify**: AI often hallucinates HAL functions or ignores real-time constraints. Always verify against the datasheet and test on hardware.
2.  **Understand**: Do not commit code you do not fully understand. You are responsible for the quality and safety of your contribution.
3.  **Format**: AI often ignores project-specific styling (e.g., our **BSD Allman** brace requirement). Please reformat AI-generated code before submitting a PR.
---

Happy Coding!
