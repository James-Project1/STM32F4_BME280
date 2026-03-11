# bme280-stm32-bare-metal

Bare-metal BME280 temperature/humidity/pressure sensor driver for STM32F401RETx.
Written in C, no HAL. Communication over I2C1, readings printed via UART2.

## Project Structure

```
Inc/        — Header files
Src/        — Application source files
Drivers/    — CMSIS core and STM32F4xx device headers (not tracked, see below)
startup/    — Reset handler and vector table
linker/     — Flash/RAM memory map
```

## CMSIS Drivers

The `Drivers/` folders are not tracked in this repo. Copy the required headers
from your local STM32CubeF4 package before building.

## Building

Import into STM32CubeIDE via `File → Import → Existing Projects into Workspace`.
