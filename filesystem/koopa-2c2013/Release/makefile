################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/validate/subdir.mk
-include src/magic/subdir.mk
-include src/files/subdir.mk
-include src/anim/koopa/objects/subdir.mk
-include src/anim/koopa/subdir.mk
-include src/anim/commons/collections/subdir.mk
-include src/anim/commons/subdir.mk
-include src/anim/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: koopa

# Tool invocations
koopa: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "koopa" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) koopa
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
