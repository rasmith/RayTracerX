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
	g++ -I"/v/filer4b/v20q001/agrippa/workspace_cdt_8.2.0/Ray/include" -I"/v/filer4b/v20q001/agrippa/workspace_cdt_8.2.0/Ray/tests/gmock/include" -I"/v/filer4b/v20q001/agrippa/workspace_cdt_8.2.0/Ray/tests/gmock/gtest/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


