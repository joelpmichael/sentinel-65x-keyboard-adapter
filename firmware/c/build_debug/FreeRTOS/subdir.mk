################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS/croutine.c \
../FreeRTOS/event_groups.c \
../FreeRTOS/list.c \
../FreeRTOS/queue.c \
../FreeRTOS/stream_buffer.c \
../FreeRTOS/tasks.c \
../FreeRTOS/timers.c 

OBJS += \
./FreeRTOS/croutine.o \
./FreeRTOS/event_groups.o \
./FreeRTOS/list.o \
./FreeRTOS/queue.o \
./FreeRTOS/stream_buffer.o \
./FreeRTOS/tasks.o \
./FreeRTOS/timers.o 

C_DEPS += \
./FreeRTOS/croutine.d \
./FreeRTOS/event_groups.d \
./FreeRTOS/list.d \
./FreeRTOS/queue.d \
./FreeRTOS/stream_buffer.d \
./FreeRTOS/tasks.d \
./FreeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/%.o: ../FreeRTOS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-elf-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -flto -Wunused -Wuninitialized -Wall -Wextra  -g3 -DHAS_CONTROLLER_KEYBOARD -DHAS_PS2_KEYBOARD -DHAS_USB_KEYBOARD -DDEBUG -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Debug" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Core" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/User" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Peripheral/inc" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/include" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/Common" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/GCC/RISC-V" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/MemMang" -std=gnu17 -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


