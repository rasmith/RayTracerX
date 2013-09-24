################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tests/gmock/gtest/fused-src/gtest/gtest-all.cc \
../tests/gmock/gtest/fused-src/gtest/gtest_main.cc 

OBJS += \
./tests/gmock/gtest/fused-src/gtest/gtest-all.o \
./tests/gmock/gtest/fused-src/gtest/gtest_main.o 

CC_DEPS += \
./tests/gmock/gtest/fused-src/gtest/gtest-all.d \
./tests/gmock/gtest/fused-src/gtest/gtest_main.d 


# Each subdirectory must supply rules for building sources it contributes
tests/gmock/gtest/fused-src/gtest/%.o: ../tests/gmock/gtest/fused-src/gtest/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/home/agrippa/sdks/Graphics-Magick_1.3.18/include -I/home/agrippa/sdks/glm_0.9.4.5/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


