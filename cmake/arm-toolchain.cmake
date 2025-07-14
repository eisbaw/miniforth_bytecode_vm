# ARM toolchain file for CMake cross-compilation
# For building MiniForth VM for ARM Cortex-M microcontrollers

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Cross-compilation tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# Target specific flags
set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_INIT "-mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections")
set(CMAKE_ASM_FLAGS_INIT "-mcpu=cortex-m3 -mthumb")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-mcpu=cortex-m3 -mthumb -Wl,--gc-sections -nostartfiles -nostdlib")

# Size optimization flags
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG -flto -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -flto -fomit-frame-pointer")

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Disable shared libraries (not applicable for embedded)
set(BUILD_SHARED_LIBS OFF)