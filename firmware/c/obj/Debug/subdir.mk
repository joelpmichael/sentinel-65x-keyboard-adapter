################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Debug/debug.c 

OBJS += \
./Debug/debug.o 

C_DEPS += \
./Debug/debug.d 


# Each subdirectory must supply rules for building sources it contributes
Debug/%.o: ../Debug/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused  -g -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/Debug" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/Core" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/User" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/Peripheral/inc" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/FreeRTOS" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/FreeRTOS/include" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/FreeRTOS/portable" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/FreeRTOS/portable/Common" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/FreeRTOS/portable/GCC/RISC-V" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -I"/home/joel/mrs_community-workspace/CH32V203-FreeRTOS/FreeRTOS/portable/MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


