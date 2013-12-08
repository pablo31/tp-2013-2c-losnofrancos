################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/anim/anim.c \
../src/anim/i_drawable.c \
../src/anim/screen.c 

OBJS += \
./src/anim/anim.o \
./src/anim/i_drawable.o \
./src/anim/screen.o 

C_DEPS += \
./src/anim/anim.d \
./src/anim/i_drawable.d \
./src/anim/screen.d 


# Each subdirectory must supply rules for building sources it contributes
src/anim/%.o: ../src/anim/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


