# STM32F401RE Bare-Metal BME280 Driver
-  A bare-metal embedded C project for the STM32 Nucleo board. Reads temperature, pressure and humidity from a BME280 sensor over I2C and prints results to a host PC over USART when the onboard button is pressed. 
-  This project was written without using HAL drivers.
-  Example print to the USART terminal :
```
Temp:19.05 C  Press:1016.66 hPa  Hum:57.40%
```
### Hardware 

|   Component    |           Detail           |
| :------------: | :------------------------: |
|      MCU       |  STM32F401RE (Nucleo-64)   |
|     Sensor     |        Bosch BME280        |
|    I2C Bus     | I2C1 : PB8 (SCL), PB9(SDA) |
|     USART      |      USART2 : PA2(TX)      |
|     Button     |         B1 (PC13)          |
| BME280 Address |            0x77            |
|  Clock Speed   |     16MHz HSI, no PLL      |
### Project Structure 
```
Inc/
  bme280.h             BME280 driver and processing API, shared types
  config.h             All peripheral and device configuration macros
  exti.h               EXTI button interrupt API
  i2c.h                I2C1 driver API and context struct
  systick.h            SysTick timebase API
  usart.h              USART2 transmit driver API

Src/
  bme280_driver.c      BME280 I/O — init, calibration, raw read
  bme280_processing.c  Bosch integer compensation formulas
  exti.c               PC13 EXTI configuration and ISR
  i2c.c                Interrupt-driven I2C1 state machine
  main.c               Top-level init and flag-driven main loop
  systick.c            1ms SysTick timebase and delay
  usart.c              Interrupt-driven USART2 TX with ring buffer
```
### Build and Run 
1.  Open STM32CubeIDE
2.  Build and flash to a STM32F401RE nucleo board
3.  Open serial terminal at 115200 baud, 8N1
4.  Press B1 to print the current sensor reading
### Project Goal 
-  Project goal was to remove the abstraction layer that HAL drivers add, to improve fundamental understanding of how peripherals function.
### Project Development
-  This project is the foundation for follow-on projects including the use of an RTOS and multi-device I2C, and an SPI sensor.  
