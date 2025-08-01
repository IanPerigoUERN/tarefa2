# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Tarefa_1 C CXX ASM)

# Pull in FreeRTOS

include($ENV{FREERTOS_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1


add_executable(Tarefa_1
    Tarefa_1.c
    inc_joystick/joystick_lib.c
    inc_OLED/ssd1306.c
    Temperature/temp.c
    Blink/new_blink.c
    Conections/mqtt_conn.c
    Conections/wifi_conn.c
    )


pico_enable_stdio_usb(Tarefa_1 1)
pico_enable_stdio_uart(Tarefa_1 0)

target_include_directories(Tarefa_1 PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(Tarefa_1 
    pico_stdlib 
    FreeRTOS-Kernel 
    FreeRTOS-Kernel-Heap4
    hardware_adc
    hardware_i2c
    # Oferece suporte Wi-Fi (CYW43) com LwIP (pilha TCP/IP) em um modo thread-safe em segundo plano.
    pico_cyw43_arch_lwip_threadsafe_background
    # Adiciona suporte a cliente MQTT sobre o LwIP.
    pico_lwip_mqtt
    # A pilha Lightweight IP (LwIP), que fornece funcionalidades TCP/IP (HTTP, MQTT, UDP, DHCP, etc.).
    pico_lwip
    # O driver do chip Wi-Fi CYW43 (usado no Pico W).
    pico_cyw43_driver
    )





# create map/bin/hex file etc.
pico_add_extra_outputs(Tarefa_1)

# add url via pico_set_program_url