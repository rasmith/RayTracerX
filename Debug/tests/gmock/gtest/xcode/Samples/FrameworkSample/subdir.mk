################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tests/gmock/gtest/xcode/Samples/FrameworkSample/widget.cc \
../tests/gmock/gtest/xcode/Samples/FrameworkSample/widget_test.cc 

OBJS += \
./tests/gmock/gtest/xcode/Samples/FrameworkSample/widget.o \
./tests/gmock/gtest/xcode/Samples/FrameworkSample/widget_test.o 

CC_DEPS += \
./tests/gmock/gtest/xcode/Samples/FrameworkSample/widget.d \
./tests/gmock/gtest/xcode/Samples/FrameworkSample/widget_test.d 


# Each subdirectory must supply rules for building sources it contributes
tests/gmock/gtest/xcode/Samples/FrameworkSample/%.o: ../tests/gmock/gtest/xcode/Samples/FrameworkSample/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/home/agrippa/sdks/Graphics-Magick_1.3.18/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/include" -I/home/agrippa/sdks/libpng_1.6.3/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


