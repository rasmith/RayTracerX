################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/image_storage_test.cpp \
../tests/image_test.cpp \
../tests/parse_utils_test.cpp 

OBJS += \
./tests/image_storage_test.o \
./tests/image_test.o \
./tests/parse_utils_test.o 

CPP_DEPS += \
./tests/image_storage_test.d \
./tests/image_test.d \
./tests/parse_utils_test.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/home/agrippa/sdks/Graphics-Magick_1.3.18/include -I/home/agrippa/sdks/glm_0.9.4.5/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


