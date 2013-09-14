################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/image_utils.cpp \
../src/main.cpp \
../src/parse_utils.cpp 

OBJS += \
./src/image_utils.o \
./src/main.o \
./src/parse_utils.o 

CPP_DEPS += \
./src/image_utils.d \
./src/main.d \
./src/parse_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/home/agrippa/sdks/Graphics-Magick_1.3.18/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/include" -I/home/agrippa/sdks/libpng_1.6.3/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


