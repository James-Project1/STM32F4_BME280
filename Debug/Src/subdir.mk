################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/bme280_driver.c \
../Src/bme280_processing.c \
../Src/i2c.c \
../Src/main.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/usart.c 

OBJS += \
./Src/bme280_driver.o \
./Src/bme280_processing.o \
./Src/i2c.o \
./Src/main.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/usart.o 

C_DEPS += \
./Src/bme280_driver.d \
./Src/bme280_processing.d \
./Src/i2c.d \
./Src/main.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -c -I../Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/bme280_driver.cyclo ./Src/bme280_driver.d ./Src/bme280_driver.o ./Src/bme280_driver.su ./Src/bme280_processing.cyclo ./Src/bme280_processing.d ./Src/bme280_processing.o ./Src/bme280_processing.su ./Src/i2c.cyclo ./Src/i2c.d ./Src/i2c.o ./Src/i2c.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/usart.cyclo ./Src/usart.d ./Src/usart.o ./Src/usart.su

.PHONY: clean-Src

