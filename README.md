# STM32 Embedded C++ Project 🚀

[![forthebadge](https://forthebadge.com/images/badges/powered-by-coffee.svg)](https://forthebadge.com) 
[![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)](https://forthebadge.com) 
[![forthebadge](https://forthebadge.com/images/badges/built-with-love.svg)](https://forthebadge.com) 
[![forthebadge](https://forthebadge.com/images/badges/for-you.svg)](https://forthebadge.com)

## Overview

This repository demonstrates the application of C++ in embedded systems, specifically using STM32 microcontrollers. The goal is to provide a learning resource for developers transitioning from general C++ programming to embedded development. The project includes core components, hardware drivers, and practical examples, all aimed at showcasing the capabilities of STM32 microcontrollers and the efficiency of C++ in embedded programming.

## Recent changes (Sept 17, 2025)
This project received a focused refactor of the GPIO/EXTI subsystem and an example application demonstrating interrupt-driven button handling. The project is now structured as a learning space (`Library` vs `Examples`).

## Build & run notes
- Open in STM32CubeIDE or VS Code and build as usual.
- Ensure `stm32l4xx_ll_*` headers are available and CubeMX config enables the required peripherals (GPIO, SYSCFG, NVIC lines for EXTI).

## Repository Structure 
- **Core/**: Core components and configurations.
- **App/**: Example applications demonstrating usage.
- **Inc/**: Header files for classes and interfaces.
- **Src/**: Source files for implementations.
- **Drivers/**: Hardware drivers and related files.

## Features 
- **GPIO Class**: Comprehensive implementation for STM32 microcontrollers.
- **Modern C++**: Utilizes C++11/14/17 features.
- **HAL Integration**: Seamless hardware control via STM32 HAL.

## Getting Started 

### Prerequisites 

- STM32CubeIDE or a compatible IDE.
- Basic C++ and embedded systems knowledge.
- STM32 development board (e.g., STM32F4 Discovery, STM32 Nucleo).

### Installation 

1. Clone the repository:
    ```sh
    git clone https://github.com/MootSeeker/STM32_Embedded_CPP.git
    cd STM32_Embedded_CPP
    ```
2. Open in STM32CubeIDE.
3. Build and upload to your STM32 board.

## ToDo ✅

- [x] Complete GPIO class
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

### Languages Used in This Repository
![Top Langs](https://img.shields.io/github/languages/top/MootSeeker/STM32_Embedded_CPP)
![Languages](https://img.shields.io/github/languages/count/MootSeeker/STM32_Embedded_CPP)

## Contributing 

Contributions are welcome! Please fork the repository, create a feature branch, commit your changes, and open a pull request.

## License 

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact 

For questions or feedback, please open an issue on the repository or contact [MootSeeker](https://github.com/MootSeeker).

Happy coding!
