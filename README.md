# STM32 Embedded C++ Project

Welcome to the STM32 Embedded C++ project repository! This project showcases how to use C++ for embedded programming on STM32 microcontrollers. It is designed as a learning resource for those who are transitioning from general C++ programming to embedded systems development.

## Repository Structure

- **src/**: Contains the source code files for the project.
- **include/**: Contains the header files.
- **examples/**: Contains example applications demonstrating various features of the GPIO class and other functionalities.
- **docs/**: Contains documentation files and additional resources.
- **tests/**: Contains unit tests for the codebase.

## Features

- **GPIO Class**: A comprehensive implementation of a General-Purpose Input/Output (GPIO) class for STM32 microcontrollers.
- **C++11/14/17 Compatibility**: The codebase is written using modern C++ standards to leverage features like auto, constexpr, and template programming.
- **HAL Integration**: Integration with the STM32 Hardware Abstraction Layer (HAL) for seamless hardware control.
- **Examples and Demos**: Practical examples demonstrating how to use the GPIO class and other components in real-world scenarios.

## Getting Started

### Prerequisites

- STM32CubeIDE or any compatible IDE for STM32 development.
- Basic knowledge of C++ and embedded systems.
- STM32 development board (e.g., STM32F4 Discovery, STM32 Nucleo).

### Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/MootSeeker/STM32_Embedded_CPP.git
    cd STM32_Embedded_CPP
    ```

2. Open the project in STM32CubeIDE or your preferred development environment.

3. Build and upload the project to your STM32 board.

### Running Examples

Navigate to the `examples/` directory and choose an example to run. Each example has its own README file with instructions on how to build and execute it.

### Documentation

Detailed documentation for the project can be found in the `docs/` directory. This includes:

- GPIO Class Documentation
- How to add new peripherals
- Troubleshooting guide

## Contributing

Contributions are welcome! Please fork the repository and use a feature branch. Pull requests are accepted.

1. Fork the repository.
2. Create a new feature branch (`git checkout -b feature/YourFeature`).
3. Commit your changes (`git commit -am 'Add some feature'`).
4. Push to the branch (`git push origin feature/YourFeature`).
5. Create a new Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

- Special thanks to the open-source community for providing the tools and resources that made this project possible.
- Thanks to the STM32 team for their excellent HAL and development tools.

## Contact

For questions or feedback, please open an issue on the repository or contact [MootSeeker](https://github.com/MootSeeker).

Happy coding!
