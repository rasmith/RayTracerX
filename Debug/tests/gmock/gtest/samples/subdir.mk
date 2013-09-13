################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tests/gmock/gtest/samples/sample1.cc \
../tests/gmock/gtest/samples/sample10_unittest.cc \
../tests/gmock/gtest/samples/sample1_unittest.cc \
../tests/gmock/gtest/samples/sample2.cc \
../tests/gmock/gtest/samples/sample2_unittest.cc \
../tests/gmock/gtest/samples/sample3_unittest.cc \
../tests/gmock/gtest/samples/sample4.cc \
../tests/gmock/gtest/samples/sample4_unittest.cc \
../tests/gmock/gtest/samples/sample5_unittest.cc \
../tests/gmock/gtest/samples/sample6_unittest.cc \
../tests/gmock/gtest/samples/sample7_unittest.cc \
../tests/gmock/gtest/samples/sample8_unittest.cc \
../tests/gmock/gtest/samples/sample9_unittest.cc 

OBJS += \
./tests/gmock/gtest/samples/sample1.o \
./tests/gmock/gtest/samples/sample10_unittest.o \
./tests/gmock/gtest/samples/sample1_unittest.o \
./tests/gmock/gtest/samples/sample2.o \
./tests/gmock/gtest/samples/sample2_unittest.o \
./tests/gmock/gtest/samples/sample3_unittest.o \
./tests/gmock/gtest/samples/sample4.o \
./tests/gmock/gtest/samples/sample4_unittest.o \
./tests/gmock/gtest/samples/sample5_unittest.o \
./tests/gmock/gtest/samples/sample6_unittest.o \
./tests/gmock/gtest/samples/sample7_unittest.o \
./tests/gmock/gtest/samples/sample8_unittest.o \
./tests/gmock/gtest/samples/sample9_unittest.o 

CC_DEPS += \
./tests/gmock/gtest/samples/sample1.d \
./tests/gmock/gtest/samples/sample10_unittest.d \
./tests/gmock/gtest/samples/sample1_unittest.d \
./tests/gmock/gtest/samples/sample2.d \
./tests/gmock/gtest/samples/sample2_unittest.d \
./tests/gmock/gtest/samples/sample3_unittest.d \
./tests/gmock/gtest/samples/sample4.d \
./tests/gmock/gtest/samples/sample4_unittest.d \
./tests/gmock/gtest/samples/sample5_unittest.d \
./tests/gmock/gtest/samples/sample6_unittest.d \
./tests/gmock/gtest/samples/sample7_unittest.d \
./tests/gmock/gtest/samples/sample8_unittest.d \
./tests/gmock/gtest/samples/sample9_unittest.d 


# Each subdirectory must supply rules for building sources it contributes
tests/gmock/gtest/samples/%.o: ../tests/gmock/gtest/samples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/v/filer4b/v20q001/agrippa/workspace_cdt_8.2.0/Ray/include" -I"/v/filer4b/v20q001/agrippa/workspace_cdt_8.2.0/Ray/tests/gmock/include" -I"/v/filer4b/v20q001/agrippa/workspace_cdt_8.2.0/Ray/tests/gmock/gtest/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


