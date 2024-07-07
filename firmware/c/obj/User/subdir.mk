################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v20x_it.c \
../User/main.c \
../User/s65x_controller.c \
../User/s65x_controller_keyboard.c \
../User/s65x_controller_mouse.c \
../User/s65x_controller_pad.c \
../User/system_ch32v20x.c \
../User/usb_device.c \
../User/usb_device_cdc-acm.c \
../User/usb_host.c \
../User/usb_host_keyboard.c \
../User/usb_host_mouse.c 

OBJS += \
./User/ch32v20x_it.o \
./User/main.o \
./User/s65x_controller.o \
./User/s65x_controller_keyboard.o \
./User/s65x_controller_mouse.o \
./User/s65x_controller_pad.o \
./User/system_ch32v20x.o \
./User/usb_device.o \
./User/usb_device_cdc-acm.o \
./User/usb_host.o \
./User/usb_host_keyboard.o \
./User/usb_host_mouse.o 

C_DEPS += \
./User/ch32v20x_it.d \
./User/main.d \
./User/s65x_controller.d \
./User/s65x_controller_keyboard.d \
./User/s65x_controller_mouse.d \
./User/s65x_controller_pad.d \
./User/system_ch32v20x.d \
./User/usb_device.d \
./User/usb_device_cdc-acm.d \
./User/usb_host.d \
./User/usb_host_keyboard.d \
./User/usb_host_mouse.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused  -g -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Debug" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Core" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/User" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Peripheral/inc" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/include" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/Common" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/GCC/RISC-V" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


