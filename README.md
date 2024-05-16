# STM32 Embedded C++ Project 🚀

[![forthebadge](https://forthebadge.com/images/badges/powered-by-coffee.svg)](https://forthebadge.com) 
[![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)](https://forthebadge.com) 
[![forthebadge](https://forthebadge.com/images/badges/built-with-love.svg)](https://forthebadge.com) 
[![forthebadge](https://forthebadge.com/images/badges/for-you.svg)](https://forthebadge.com)

## Overview 🌐

This repository demonstrates the application of C++ in embedded systems, specifically using STM32 microcontrollers. The goal is to provide a learning resource for developers transitioning from general C++ programming to embedded development. The project includes core components, hardware drivers, and practical examples, all aimed at showcasing the capabilities of STM32 microcontrollers and the efficiency of C++ in embedded programming.

### Memory Mapped I/O 🧠

In this project, I aim to implement Memory Mapped I/O (MMIO) for hardware interaction. MMIO maps peripheral control registers to specific memory addresses, allowing direct read/write operations on hardware registers through standard memory access instructions.

Memory Mapped I/O involves assigning a fixed range of memory addresses to hardware peripherals. Each peripheral has a set of registers that control its operation, and these registers are accessed by reading from or writing to the corresponding memory addresses. This method allows software to interact with hardware as if it were interacting with standard memory.

#### Benefits of Memory Mapped I/O:

1. **Efficiency**: Directly accessing hardware registers via memory addresses eliminates the overhead associated with traditional I/O instructions, resulting in faster and more efficient hardware interactions.
   
2. **Simplicity**: Using pointers to access memory-mapped registers simplifies code, making it easier to read, write, and maintain.
   
3. **Flexibility**: MMIO allows for fine-grained control over hardware, enabling precise manipulation of peripheral settings and states.
   
4. **Portability**: Code using MMIO can be easily adapted to different microcontrollers with similar memory-mapped architectures by adjusting base addresses.

By using Memory Mapped I/O, we can leverage the full power of the STM32 microcontroller's hardware capabilities while maintaining efficient and readable code.

## Repository Structure 🗂️

- **Core/**: Core components and configurations.
- **Drivers/**: Hardware drivers and related files.

## Features ✨

- **GPIO Class**: Comprehensive implementation for STM32 microcontrollers.
- **Modern C++**: Utilizes C++11/14/17 features.
- **HAL Integration**: Seamless hardware control via STM32 HAL.

## Getting Started 🚀

### Prerequisites 📋

- STM32CubeIDE or a compatible IDE.
- Basic C++ and embedded systems knowledge.
- STM32 development board (e.g., STM32F4 Discovery, STM32 Nucleo).

### Installation 💻

1. Clone the repository:
    ```sh
    git clone https://github.com/MootSeeker/STM32_Embedded_CPP.git
    cd STM32_Embedded_CPP
    ```
2. Open in STM32CubeIDE.
3. Build and upload to your STM32 board.

## ToDo ✅

- [ ] Complete GPIO class
- [ ] Implement class for Timer
- [ ] Implement class for ADC
- [ ] Implement class for DAC
- [ ] Implement class for CRC
- [ ] Implement class for SPI
- [ ] Implement class for I2C
- [ ] Implement class for CAN
- [ ] Implement class for QSPI
- [ ] Implement class for USART
- [ ] Create examples for all interfaces
- [ ] Transition to Bare Metal, removing HAL dependencies

## Statistics 📊

![Commits](https://img.shields.io/github/commit-activity/m/MootSeeker/STM32_Embedded_CPP)
![Open Issues](https://img.shields.io/github/issues-raw/MootSeeker/STM32_Embedded_CPP)
![Pull Requests](https://img.shields.io/github/issues-pr-raw/MootSeeker/STM32_Embedded_CPP)
![Stars](https://img.shields.io/github/stars/MootSeeker/STM32_Embedded_CPP)
![Forks](https://img.shields.io/github/forks/MootSeeker/STM32_Embedded_CPP)
![Languages](https://img.shields.io/github/languages/top/MootSeeker/STM32_Embedded_CPP)

## Contributing 🤝

Contributions are welcome! Please fork the repository, create a feature branch, commit your changes, and open a pull request.

## License 📜

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact 📧

For questions or feedback, please open an issue on the repository or contact [MootSeeker](https://github.com/MootSeeker).

Happy coding!
