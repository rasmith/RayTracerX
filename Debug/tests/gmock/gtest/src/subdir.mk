################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tests/gmock/gtest/src/gtest-all.cc \
../tests/gmock/gtest/src/gtest-death-test.cc \
../tests/gmock/gtest/src/gtest-filepath.cc \
../tests/gmock/gtest/src/gtest-port.cc \
../tests/gmock/gtest/src/gtest-printers.cc \
../tests/gmock/gtest/src/gtest-test-part.cc \
../tests/gmock/gtest/src/gtest-typed-test.cc \
../tests/gmock/gtest/src/gtest.cc \
../tests/gmock/gtest/src/gtest_main.cc 

OBJS += \
./tests/gmock/gtest/src/gtest-all.o \
./tests/gmock/gtest/src/gtest-death-test.o \
./tests/gmock/gtest/src/gtest-filepath.o \
./tests/gmock/gtest/src/gtest-port.o \
./tests/gmock/gtest/src/gtest-printers.o \
./tests/gmock/gtest/src/gtest-test-part.o \
./tests/gmock/gtest/src/gtest-typed-test.o \
./tests/gmock/gtest/src/gtest.o \
./tests/gmock/gtest/src/gtest_main.o 

CC_DEPS += \
./tests/gmock/gtest/src/gtest-all.d \
./tests/gmock/gtest/src/gtest-death-test.d \
./tests/gmock/gtest/src/gtest-filepath.d \
./tests/gmock/gtest/src/gtest-port.d \
./tests/gmock/gtest/src/gtest-printers.d \
./tests/gmock/gtest/src/gtest-test-part.d \
./tests/gmock/gtest/src/gtest-typed-test.d \
./tests/gmock/gtest/src/gtest.d \
./tests/gmock/gtest/src/gtest_main.d 


# Each subdirectory must supply rules for building sources it contributes
tests/gmock/gtest/src/%.o: ../tests/gmock/gtest/src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/home/agrippa/sdks/Graphics-Magick_1.3.18/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/include" -I/home/agrippa/sdks/libpng_1.6.3/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


