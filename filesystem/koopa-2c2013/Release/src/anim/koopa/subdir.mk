################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/anim/koopa/koopa.c 

OBJS += \
./src/anim/koopa/koopa.o 

C_DEPS += \
./src/anim/koopa/koopa.d 


# Each subdirectory must supply rules for building sources it contributes
src/anim/koopa/%.o: ../src/anim/koopa/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


