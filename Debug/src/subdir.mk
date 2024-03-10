################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/eeprom.c \
../src/lcd.c \
../src/main.c \
../src/serverApi.c \
../src/syscalls.c \
../src/system_stm32f10x.c \
../src/timer.c \
../src/uart.c 

OBJS += \
./src/eeprom.o \
./src/lcd.o \
./src/main.o \
./src/serverApi.o \
./src/syscalls.o \
./src/system_stm32f10x.o \
./src/timer.o \
./src/uart.o 

C_DEPS += \
./src/eeprom.d \
./src/lcd.d \
./src/main.d \
./src/serverApi.d \
./src/syscalls.d \
./src/system_stm32f10x.d \
./src/timer.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -I"C:/Users/Kumar Harsh/Documents/_Documents/NI_TrainingDocuments/Office/OpenSTM_Projects/smartNfcMeteringSystem/StdPeriph_Driver/inc" -I"C:/Users/Kumar Harsh/Documents/_Documents/NI_TrainingDocuments/Office/OpenSTM_Projects/smartNfcMeteringSystem/inc" -I"C:/Users/Kumar Harsh/Documents/_Documents/NI_TrainingDocuments/Office/OpenSTM_Projects/smartNfcMeteringSystem/CMSIS/device" -I"C:/Users/Kumar Harsh/Documents/_Documents/NI_TrainingDocuments/Office/OpenSTM_Projects/smartNfcMeteringSystem/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


