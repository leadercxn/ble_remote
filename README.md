# NRF52 TEMPLATE

## Code Structure

```bash
.
+-- nRF5_SDK_16.0.0_98a08e2
+-- nrf52_template
|   +-- build
|   +-- cmake
|   +-- lib
|   +-- src
|   |   +-- app
|   |   +-- common
|   |   |   +-- CMakeLists.txt
|   |   +-- config
|   |   +-- linker
|   |   +-- CMakeLists.txt
|   +-- CMakeLists.txt
|   +-- readme.md
|   +-- .gitignore
```

## Requirements

* nRF5_SDK_16 has been extracted
* A Linux environment, Ubuntu 18.04
* GCC ARM Embedded 7.2018q2.update
* MDK-ARM version 5.25 (optional)

## How To Use

1. This template project uses GCC ARM Embedded as its compile toolchain.
2. Create 'build' folder under the root of the project folder.
3. Go 'build' folder then 'cmake -GNinja ..'
4. Enter 'ninja' to compile.
5. Enter 'ninja flash' to flash softdevice merged hex file.
6. Enter 'ninja merge' to create a softdevice merged hex file.

## Something attention 
 * According to the actual platform to modify   nrf52_template/cmake/platform.cmake  &&  nrf52_template/src/CMakeLists.txt/ -DBOARD_PCA10056
 * According to the PC compiler path to modify  nrf52_template/cmake/toolchain/armcc.cmake
 * According to the actual platform to add      nrf52_template/src/linker/xxxxxxxxx.sct  
 * Tsailoring                                   nrf52_template/src/common/CMakeLists.txt




