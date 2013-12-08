################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/anim/commons/collections/list.c 

OBJS += \
./src/anim/commons/collections/list.o 

C_DEPS += \
./src/anim/commons/collections/list.d 


# Each subdirectory must supply rules for building sources it contributes
src/anim/commons/collections/%.o: ../src/anim/commons/collections/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


