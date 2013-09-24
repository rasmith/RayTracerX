################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tests/gmock/test/gmock-actions_test.cc \
../tests/gmock/test/gmock-cardinalities_test.cc \
../tests/gmock/test/gmock-generated-actions_test.cc \
../tests/gmock/test/gmock-generated-function-mockers_test.cc \
../tests/gmock/test/gmock-generated-internal-utils_test.cc \
../tests/gmock/test/gmock-generated-matchers_test.cc \
../tests/gmock/test/gmock-internal-utils_test.cc \
../tests/gmock/test/gmock-matchers_test.cc \
../tests/gmock/test/gmock-more-actions_test.cc \
../tests/gmock/test/gmock-nice-strict_test.cc \
../tests/gmock/test/gmock-port_test.cc \
../tests/gmock/test/gmock-spec-builders_test.cc \
../tests/gmock/test/gmock_all_test.cc \
../tests/gmock/test/gmock_leak_test_.cc \
../tests/gmock/test/gmock_link2_test.cc \
../tests/gmock/test/gmock_link_test.cc \
../tests/gmock/test/gmock_output_test_.cc \
../tests/gmock/test/gmock_test.cc 

OBJS += \
./tests/gmock/test/gmock-actions_test.o \
./tests/gmock/test/gmock-cardinalities_test.o \
./tests/gmock/test/gmock-generated-actions_test.o \
./tests/gmock/test/gmock-generated-function-mockers_test.o \
./tests/gmock/test/gmock-generated-internal-utils_test.o \
./tests/gmock/test/gmock-generated-matchers_test.o \
./tests/gmock/test/gmock-internal-utils_test.o \
./tests/gmock/test/gmock-matchers_test.o \
./tests/gmock/test/gmock-more-actions_test.o \
./tests/gmock/test/gmock-nice-strict_test.o \
./tests/gmock/test/gmock-port_test.o \
./tests/gmock/test/gmock-spec-builders_test.o \
./tests/gmock/test/gmock_all_test.o \
./tests/gmock/test/gmock_leak_test_.o \
./tests/gmock/test/gmock_link2_test.o \
./tests/gmock/test/gmock_link_test.o \
./tests/gmock/test/gmock_output_test_.o \
./tests/gmock/test/gmock_test.o 

CC_DEPS += \
./tests/gmock/test/gmock-actions_test.d \
./tests/gmock/test/gmock-cardinalities_test.d \
./tests/gmock/test/gmock-generated-actions_test.d \
./tests/gmock/test/gmock-generated-function-mockers_test.d \
./tests/gmock/test/gmock-generated-internal-utils_test.d \
./tests/gmock/test/gmock-generated-matchers_test.d \
./tests/gmock/test/gmock-internal-utils_test.d \
./tests/gmock/test/gmock-matchers_test.d \
./tests/gmock/test/gmock-more-actions_test.d \
./tests/gmock/test/gmock-nice-strict_test.d \
./tests/gmock/test/gmock-port_test.d \
./tests/gmock/test/gmock-spec-builders_test.d \
./tests/gmock/test/gmock_all_test.d \
./tests/gmock/test/gmock_leak_test_.d \
./tests/gmock/test/gmock_link2_test.d \
./tests/gmock/test/gmock_link_test.d \
./tests/gmock/test/gmock_output_test_.d \
./tests/gmock/test/gmock_test.d 


# Each subdirectory must supply rules for building sources it contributes
tests/gmock/test/%.o: ../tests/gmock/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/home/agrippa/sdks/Graphics-Magick_1.3.18/include -I/home/agrippa/sdks/glm_0.9.4.5/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


