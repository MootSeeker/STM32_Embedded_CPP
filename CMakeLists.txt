cmake_minimum_required(VERSION 3.16)
project(STM32_Embedded_CPP C CXX ASM)

# Option to enable building of tests
option(BUILD_TESTING "Enable unit tests" ON)

# Only set up ARM toolchain if we're not building tests
if(NOT BUILD_TESTING)
    # --- Toolchain and Target Setup for ARM Firmware ---
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR ARM) # Or your specific Cortex-M processor

    # Adjust TOOLCHAIN_PREFIX to your arm-none-eabi toolchain location/name if not in PATH
    set(TOOLCHAIN_PREFIX arm-none-eabi-)
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc) # Using gcc for .s files
    set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
    set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)
endif()

# Common flags for Cortex-M4 (adjust for your specific MCU, e.g., STM32L432KC)
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

set(FIRMWARE_C_FLAGS "${CPU_FLAGS} -std=gnu11 -Wall -fdata-sections -ffunction-sections")
set(FIRMWARE_CXX_FLAGS "${CPU_FLAGS} -std=gnu++17 -Wall -fno-rtti -fno-exceptions -fno-threadsafe-statics -fdata-sections -ffunction-sections")
set(FIRMWARE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp") # For preprocessing .s files

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32L432KCUX_FLASH.ld) # Ensure this path is correct
set(FIRMWARE_LINKER_FLAGS "${CPU_FLAGS} -T ${LINKER_SCRIPT} -Wl,-Map=${PROJECT_NAME}.map,--cref -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs")

# --- Firmware Target ---
# Source Files
set(FIRMWARE_SOURCES
    Src/main.cpp
    Src/gpio.cpp
    Src/rcc.cpp
    Src/system.cpp
    Src/usart.cpp
    Src/syscalls.c  # System calls for printf, etc.
    Src/sysmem.c    # System memory for malloc, etc.
    Startup/startup_stm32l432kcux.s # Startup file
)

# Include Directories for Firmware
set(FIRMWARE_INCLUDES
    Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32L4xx/Include # Path to stm32l4xx.h and system_stm32l4xx.h
)

add_executable(${PROJECT_NAME}.elf ${FIRMWARE_SOURCES})
target_include_directories(${PROJECT_NAME}.elf PUBLIC ${FIRMWARE_INCLUDES})

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${FIRMWARE_C_FLAGS}>
    $<$<COMPILE_LANGUAGE:CXX>:${FIRMWARE_CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${FIRMWARE_ASM_FLAGS}>
)
target_link_options(${PROJECT_NAME}.elf PRIVATE ${FIRMWARE_LINKER_FLAGS})

# Create .hex and .bin files post-build
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating .hex, .bin files and printing size"
)

# --- Unit Testing Setup (Host Environment) ---
# (BUILD_TESTING is already defined at top)

if(BUILD_TESTING)
    message(STATUS "Unit testing enabled.")
    
    # For Windows builds with MSVC
    if(MSVC)
        # Disable specific warnings for test builds
        add_compile_options(/wd4996) # Disable deprecated function warnings
        
        # Set runtime library for consistent linking
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    endif()

    # Test Source Files
    set(TEST_SOURCES
        Tests/Runner/test_runner.cpp    # Catch2 main runner
        Tests/Src/mock_stm32l4xx.cpp    # Mock implementations
        Tests/Src/test_gpio.cpp         # GPIO tests
        # Add other test files: Tests/Src/test_xxx.cpp

        # Source files under test (compiled for host)
        Src/gpio.cpp
        # Src/rcc.cpp # Add if rcc is a dependency of gpio and needs to be linked or mocked further
        # Add other source files from Src/ that are being tested or are dependencies
    )

    # Include Directories for Tests
    set(TEST_INCLUDES
        Inc                         # For project headers like gpio.h
        Tests/Inc                   # For catch.hpp, mock_stm32l4xx.h
        # Add other necessary include paths for host compilation if any
    )

    # Create a native executable for tests (not ARM)
    add_executable(run_tests ${TEST_SOURCES})

    # Define UNIT_TESTING macro for test builds.
    # This is used in headers (e.g., gpio.h) to switch between real and mock peripherals.
    target_compile_definitions(run_tests PRIVATE UNIT_TESTING)

    target_include_directories(run_tests PUBLIC ${TEST_INCLUDES})

    # For Windows/MSVC specifically
    if(MSVC)
        # Set output directory to match what CTest expects
        set_target_properties(run_tests PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
    endif()

    # Enable testing but don't automatically run tests after build
    enable_testing()
    add_test(NAME HostTests COMMAND run_tests)

    message(STATUS "To build tests: build the 'run_tests' target.")
    message(STATUS "To run tests manually: execute 'run_tests' directly from the build directory.")
    message(STATUS "To run tests with CTest: use 'ctest' in your build directory.")
    
else()
    message(STATUS "Unit testing disabled. Set BUILD_TESTING=ON to enable.")
endif()
