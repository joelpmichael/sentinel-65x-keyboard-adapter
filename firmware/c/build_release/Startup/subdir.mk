################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Startup/startup_ch32v20x_D6.S 

OBJS += \
./Startup/startup_ch32v20x_D6.o 

S_UPPER_DEPS += \
./Startup/startup_ch32v20x_D6.d 


# Each subdirectory must supply rules for building sources it contributes
Startup/%.o: ../Startup/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross Assembler'
	riscv-none-elf-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -flto -finline-functions -Wunused -Wuninitialized -Wall -Wextra -Winline  -g3 -x assembler-with-cpp -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/include" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/Common" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/MemMang" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/GCC/RISC-V" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -I"/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Startup" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


