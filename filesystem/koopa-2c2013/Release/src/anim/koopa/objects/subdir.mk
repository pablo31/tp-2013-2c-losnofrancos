################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/anim/koopa/objects/block.c \
../src/anim/koopa/objects/bowser.c \
../src/anim/koopa/objects/box.c \
../src/anim/koopa/objects/caption_points.c \
../src/anim/koopa/objects/caption_status.c \
../src/anim/koopa/objects/character.c \
../src/anim/koopa/objects/dummy.c \
../src/anim/koopa/objects/goomba.c \
../src/anim/koopa/objects/pipe.c \
../src/anim/koopa/objects/textbox.c 

OBJS += \
./src/anim/koopa/objects/block.o \
./src/anim/koopa/objects/bowser.o \
./src/anim/koopa/objects/box.o \
./src/anim/koopa/objects/caption_points.o \
./src/anim/koopa/objects/caption_status.o \
./src/anim/koopa/objects/character.o \
./src/anim/koopa/objects/dummy.o \
./src/anim/koopa/objects/goomba.o \
./src/anim/koopa/objects/pipe.o \
./src/anim/koopa/objects/textbox.o 

C_DEPS += \
./src/anim/koopa/objects/block.d \
./src/anim/koopa/objects/bowser.d \
./src/anim/koopa/objects/box.d \
./src/anim/koopa/objects/caption_points.d \
./src/anim/koopa/objects/caption_status.d \
./src/anim/koopa/objects/character.d \
./src/anim/koopa/objects/dummy.d \
./src/anim/koopa/objects/goomba.d \
./src/anim/koopa/objects/pipe.d \
./src/anim/koopa/objects/textbox.d 


# Each subdirectory must supply rules for building sources it contributes
src/anim/koopa/objects/%.o: ../src/anim/koopa/objects/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


