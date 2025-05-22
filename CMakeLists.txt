cmake_minimum_required(VERSION 3.16)
project(STM32_Embedded_CPP C CXX ASM)

# Option to enable building of tests
option(BUILD_TESTING "Enable unit tests" ON)

if(BUILD_TESTING)
    # --- Unit Testing Setup (Host Environment) ---
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
    )

    # Include Directories for Tests
    set(TEST_INCLUDES
        Inc                         # For project headers like gpio.h
        Tests/Inc                   # For catch.hpp, mock_stm32l4xx.h
    )
    
    # Create a native executable for tests (not ARM)
    add_executable(run_tests ${TEST_SOURCES})

    # Define UNIT_TESTING macro for test builds.
    # This is used in headers (e.g., gpio.h) to switch between real and mock peripherals.
    target_compile_definitions(run_tests PRIVATE UNIT_TESTING)

    target_include_directories(run_tests PUBLIC ${TEST_INCLUDES})

    # Set C++ standard to 17
    set_property(TARGET run_tests PROPERTY CXX_STANDARD 17)

    # For Windows/MSVC specifically
    if(MSVC)
        # Set output directory and runtime library
        set_target_properties(run_tests PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}"
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
            
        # Set Runtime Library to Multi-threaded DLL
        set_target_properties(run_tests PROPERTIES 
            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()

    # Enable testing
    include(CTest)
    enable_testing()
    add_test(NAME HostTests 
             COMMAND run_tests 
             WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    message(STATUS "To build tests: build the 'run_tests' target.")
    message(STATUS "To run tests manually: execute 'run_tests' directly from the build directory.")
    message(STATUS "To run tests with CTest: use 'ctest' in your build directory.")
else()
    # --- Firmware build setup ---
    message(STATUS "Firmware build enabled.")
    
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

    # Common flags for Cortex-M4 (STM32L432KC)
    set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

    set(FIRMWARE_C_FLAGS "${CPU_FLAGS} -std=gnu11 -Wall -fdata-sections -ffunction-sections")
    set(FIRMWARE_CXX_FLAGS "${CPU_FLAGS} -std=gnu++17 -Wall -fno-rtti -fno-exceptions -fno-threadsafe-statics -fdata-sections -ffunction-sections")
    set(FIRMWARE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp") # For preprocessing .s files

    set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32L432KCUX_FLASH.ld)
    set(FIRMWARE_LINKER_FLAGS "${CPU_FLAGS} -T ${LINKER_SCRIPT} -Wl,-Map=${PROJECT_NAME}.map,--cref -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs")

    # Firmware source files
    set(FIRMWARE_SOURCES
        Src/main.cpp
        Src/gpio.cpp
        Src/rcc.cpp
        Src/system.cpp
        Src/usart.cpp
        Src/syscalls.c   # System calls for printf, etc.
        Src/sysmem.c     # System memory for malloc, etc.
        Startup/startup_stm32l432kcux.s  # Startup file
    )

    # Firmware include directories 
    set(FIRMWARE_INCLUDES
        Inc
        Drivers/CMSIS/Include
        Drivers/CMSIS/Device/ST/STM32L4xx/Include
    )

    # Create firmware target 
    add_executable(${PROJECT_NAME} ${FIRMWARE_SOURCES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${FIRMWARE_INCLUDES})

    # Set firmware compiler flags and linker script
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${FIRMWARE_C_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>:${FIRMWARE_CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>:${FIRMWARE_ASM_FLAGS}>
    )
    target_link_options(${PROJECT_NAME} PRIVATE ${FIRMWARE_LINKER_FLAGS})

    # Add post-build commands to create binary and hex files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
        COMMENT "Creating binary and hex files..."
    )
endif()
