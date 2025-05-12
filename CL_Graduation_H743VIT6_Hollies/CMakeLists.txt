cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME CL_Graduation_H743VIT6_Hollies)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
    User/Modules/ARM_MATH/Lib
)

# 添加User文件夹下的所有源文件
aux_source_directory(User/Src USER_SOURCES)
aux_source_directory(User/Modules/LetterShell/Src SHELL_SOURCES)
aux_source_directory(User/Modules/OLED/Src OLED_SOURCES)
aux_source_directory(User/Modules/INA228/Src INA228_SOURCES)
aux_source_directory(User/Modules/AD7606/Src AD7606_SOURCES)
aux_source_directory(User/Modules/PID/Src PID_SOURCES)
aux_source_directory(User/Modules/SVPWM/Src SVPWM_SOURCES)
aux_source_directory(User/Modules/SINGLE_PHASE_RECTIFIER/Src SINGLE_PHASE_RECTIFIER_SOURCES)
aux_source_directory(User/Modules/THREE_PHASE_RECTIFIER/Src THREE_PHASE_RECTIFIER_SOURCES)
aux_source_directory(User/Modules/ARM_MATH/Src ARM_MATH_SOURCES)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${USER_SOURCES}
    ${SHELL_SOURCES}
    ${OLED_SOURCES}
    ${INA228_SOURCES}
    ${AD7606_SOURCES}
    ${PID_SOURCES}
    ${SVPWM_SOURCES}
    ${SINGLE_PHASE_RECTIFIER_SOURCES}
    ${THREE_PHASE_RECTIFIER_SOURCES}
    ${ARM_MATH_SOURCES}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    User/Inc
    User/Modules/LetterShell/Inc
    User/Modules/OLED/Inc
    User/Modules/INA228/Inc
    User/Modules/AD7606/Inc
    User/Modules/PID/Inc
    User/Modules/SVPWM/Inc
    User/Modules/SINGLE_PHASE_RECTIFIER/Inc
    User/Modules/THREE_PHASE_RECTIFIER/Inc
    User/Modules/ARM_MATH/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    arm_cortexM7lfdp_math
)

# 添加链接器选项以支持float类型的printf
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -u _printf_float
)
