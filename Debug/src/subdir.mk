################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/keypad.c \
../src/main.c \
../src/pwm.c \
../src/syscalls.c \
../src/system_stm32f0xx.c 

S_UPPER_SRCS += \
../src/io.S \
../src/nano_wait.S 

OBJS += \
./src/io.o \
./src/keypad.o \
./src/main.o \
./src/nano_wait.o \
./src/pwm.o \
./src/syscalls.o \
./src/system_stm32f0xx.o 

S_UPPER_DEPS += \
./src/io.d \
./src/nano_wait.d 

C_DEPS += \
./src/keypad.d \
./src/main.d \
./src/pwm.d \
./src/syscalls.d \
./src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F051R8Tx -DSTM32F0DISCOVERY -DDEBUG -DSTM32F051 -DUSE_STDPERIPH_DRIVER -I"C:/Users/FIYI/Desktop/hw10/Utilities" -I"C:/Users/FIYI/Desktop/hw10/StdPeriph_Driver/inc" -I"C:/Users/FIYI/Desktop/hw10/inc" -I"C:/Users/FIYI/Desktop/hw10/CMSIS/device" -I"C:/Users/FIYI/Desktop/hw10/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F051R8Tx -DSTM32F0DISCOVERY -DDEBUG -DSTM32F051 -DUSE_STDPERIPH_DRIVER -I"C:/Users/FIYI/Desktop/hw10/Utilities" -I"C:/Users/FIYI/Desktop/hw10/StdPeriph_Driver/inc" -I"C:/Users/FIYI/Desktop/hw10/inc" -I"C:/Users/FIYI/Desktop/hw10/CMSIS/device" -I"C:/Users/FIYI/Desktop/hw10/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


