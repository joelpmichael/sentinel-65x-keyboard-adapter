################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include FreeRTOS/portable/MemMang/subdir.mk
-include FreeRTOS/portable/GCC/RISC-V/subdir.mk
-include FreeRTOS/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
sentinel-65x-keyboard-adapter.hex \

SECONDARY_LIST += \
sentinel-65x-keyboard-adapter.lst \

SECONDARY_SIZE += \
sentinel-65x-keyboard-adapter.siz \


# All Target
all: sentinel-65x-keyboard-adapter.elf secondary-outputs

# Tool invocations
sentinel-65x-keyboard-adapter.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	riscv-none-elf-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -fno-builtin -flto -Wunused -Wuninitialized -Wall -Wextra  -g3 -T "/home/joel/code/sentinel-65x-keyboard-adapter/firmware/c/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"sentinel-65x-keyboard-adapter.map" --specs=nano.specs --specs=nosys.specs -o "sentinel-65x-keyboard-adapter.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

sentinel-65x-keyboard-adapter.hex: sentinel-65x-keyboard-adapter.elf
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	riscv-none-elf-objcopy -O ihex "sentinel-65x-keyboard-adapter.elf"  "sentinel-65x-keyboard-adapter.hex"
	@echo 'Finished building: $@'
	@echo ' '

sentinel-65x-keyboard-adapter.lst: sentinel-65x-keyboard-adapter.elf
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	riscv-none-elf-objdump --all-headers --demangle --disassemble "sentinel-65x-keyboard-adapter.elf" > "sentinel-65x-keyboard-adapter.lst"
	@echo 'Finished building: $@'
	@echo ' '

sentinel-65x-keyboard-adapter.siz: sentinel-65x-keyboard-adapter.elf
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	riscv-none-elf-size --format=berkeley --totals "sentinel-65x-keyboard-adapter.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) sentinel-65x-keyboard-adapter.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
