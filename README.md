# bme280-stm32-bare-metal

Bare-metal BME280 temperature/humidity/pressure sensor driver for STM32F401RETx.
Written in C, no HAL. Fully interrupt/event-driven — no polling or blocking calls.
Communication over I2C1, readings printed via UART2.

## Project Structure

```
Inc/        — Header files
Src/        — Application source files
Drivers/    — CMSIS core and STM32F4xx device headers (not tracked, see below)
startup/    — Reset handler and vector table
linker/     — Flash/RAM memory map
```

---

## Module Reference

### `config.h`
Central configuration for the entire project. No `.c` file.
- System clock frequency (`SYS_CLOCK_HZ`)
- APB1/APB2 bus frequencies
- UART baud rate
- I2C speed
- BME280 I2C address
- Interrupt priority levels for all peripherals (SysTick > I2C > UART)
- Any project-wide `#define` constants

---

### `rcc` — Reset and Clock Control
Centralised clock configuration. Every peripheral depends on this running first.
- Enable HSE/PLL and configure system clock
- Set AHB, APB1, APB2 bus prescalers
- Expose clock enable helpers for GPIO, UART, I2C peripherals
- All other `_Init()` functions call into this rather than writing to `RCC` registers directly

---

### `nvic` — Nested Vector Interrupt Controller
Interrupt priority and enable/disable management. Likely header-only.
- Set interrupt priorities for each peripheral (defined in `config.h`)
- Enable/disable specific IRQ lines
- Global interrupt enable/disable helpers (`__enable_irq` / `__disable_irq` wrappers)
- All priority assignments documented in one place

---

### `systick` — System Tick Timer
Core interrupt timebase for the whole project.
- Initialise SysTick to fire every 1 ms
- Maintain a `volatile uint32_t` millisecond tick counter incremented in the ISR
- Provide `SysTick_GetTick()` for timestamping
- Provide non-blocking `SysTick_DelayMs()` using tick comparison rather than spin-loops

---

### `gpio` — General Purpose I/O
Thin abstraction over GPIO pin configuration and control.
- Pin mode configuration (input, output, alternate function, analog)
- Output type (push-pull vs open-drain)
- Speed and pull-up/pull-down settings
- `GPIO_SetPin()`, `GPIO_ResetPin()`, `GPIO_TogglePin()`
- Used by status LEDs or any manual pin toggling

---

### `uart` — UART2 Driver (PA2 TX / PA3 RX)
Interrupt-driven transmit and receive over UART2.
- Initialise UART2 at baud rate defined in `config.h`
- TX: circular buffer populated in application code, drained in `USART2_IRQHandler`
- RX: received bytes written to a buffer in `USART2_IRQHandler`, read by application
- `UART_SendString()` and `UART_Printf()` as the public API
- No blocking waits on `TXE` or `RXNE` flags

---

### `i2c` — I2C1 Driver (PB6 SCL / PB7 SDA)
Interrupt-driven I2C1 master for sensor communication.
- Initialise I2C1 in fast mode (400 kHz)
- State-machine driven transfers handled in `I2C1_EV_IRQHandler` and `I2C1_ER_IRQHandler`
- `I2C_WriteReg()` and `I2C_ReadRegs()` as the public API
- Error handling for bus busy, NACK, and arbitration loss
- Transfer complete signalled via a `volatile` flag polled non-blocking by callers

---

### `bme280` — BME280 Sensor Driver
Sensor initialisation, configuration and compensated data output.
- Verify chip ID on startup
- Soft reset and read factory calibration data (temperature, pressure, humidity blocks)
- Configure oversampling and trigger forced-mode measurements
- Compensation math from BME280 datasheet (integer/float) applied to raw ADC values
- Exposes `BME280_ReadData()` returning a struct with `temperature`, `pressure`, `humidity`
- Relies entirely on `i2c` module — no direct register access

---

## Interrupt Priority Scheme

| Peripheral  | Priority    | Rationale                          |
|-------------|-------------|------------------------------------|
| SysTick     | 0 (highest) | Core timebase, must not be starved |
| I2C1 Event  | 1           | Sensor data transfer               |
| I2C1 Error  | 1           | Bus error handling                 |
| USART2      | 2           | Lower urgency output               |

---

## CMSIS Drivers

The `Drivers/` folders are not tracked in this repo. Copy the required headers
from your local STM32CubeF4 package before building.

## Building

Import into STM32CubeIDE via `File → Import → Existing Projects into Workspace`.
