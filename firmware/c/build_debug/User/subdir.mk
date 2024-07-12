################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v20x_it.c \
../User/main.c \
../User/ps2_controller.c \
../User/ps2_controller_keyboard.c \
../User/ps2_controller_mouse.c \
../User/s65x_controller.c \
../User/s65x_controller_keyboard.c \
../User/s65x_controller_mouse.c \
../User/s65x_controller_pad.c \
../User/system_ch32v20x.c \
../User/usb_device.c \
../User/usb_device_cdc-acm.c \
../User/usb_host.c \
../User/usb_host_hub.c \
../User/usb_host_keyboard.c \
../User/usb_host_mouse.c \
../User/usb_host_pad.c 

OBJS += \
./User/ch32v20x_it.o \
./User/main.o \
./User/ps2_controller.o \
./User/ps2_controller_keyboard.o \
./User/ps2_controller_mouse.o \
./User/s65x_controller.o \
./User/s65x_controller_keyboard.o \
./User/s65x_controller_mouse.o \
./User/s65x_controller_pad.o \
./User/system_ch32v20x.o \
./User/usb_device.o \
./User/usb_device_cdc-acm.o \
./User/usb_host.o \
./User/usb_host_hub.o \
./User/usb_host_keyboard.o \
./User/usb_host_mouse.o \
./User/usb_host_pad.o 

C_DEPS += \
./User/ch32v20x_it.d \
./User/main.d \
./User/ps2_controller.d \
./User/ps2_controller_keyboard.d \
./User/ps2_controller_mouse.d \
./User/s65x_controller.d \
./User/s65x_controller_keyboard.d \
./User/s65x_controller_mouse.d \
./User/s65x_controller_pad.d \
./User/system_ch32v20x.d \
./User/usb_device.d \
./User/usb_device_cdc-acm.d \
./User/usb_host.d \
./User/usb_host_hub.d \
./User/usb_host_keyboard.d \
./User/usb_host_mouse.d \
./User/usb_host_pad.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-elf-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -flto -Wunused -Wuninitialized -Wall -Wextra  -g3 -DHAS_CONTROLLER_KEYBOARD -DHAS_PS2_KEYBOARD -DHAS_USB_KEYBOARD -DDEBUG -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Debug" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Core" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/User" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Peripheral/inc" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/include" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/Common" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/GCC/RISC-V" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/MemMang" -std=gnu17 -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


