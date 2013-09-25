################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tests/gmock/src/gmock-all.cc \
../tests/gmock/src/gmock-cardinalities.cc \
../tests/gmock/src/gmock-internal-utils.cc \
../tests/gmock/src/gmock-matchers.cc \
../tests/gmock/src/gmock-spec-builders.cc \
../tests/gmock/src/gmock.cc \
../tests/gmock/src/gmock_main.cc 

OBJS += \
./tests/gmock/src/gmock-all.o \
./tests/gmock/src/gmock-cardinalities.o \
./tests/gmock/src/gmock-internal-utils.o \
./tests/gmock/src/gmock-matchers.o \
./tests/gmock/src/gmock-spec-builders.o \
./tests/gmock/src/gmock.o \
./tests/gmock/src/gmock_main.o 

CC_DEPS += \
./tests/gmock/src/gmock-all.d \
./tests/gmock/src/gmock-cardinalities.d \
./tests/gmock/src/gmock-internal-utils.d \
./tests/gmock/src/gmock-matchers.d \
./tests/gmock/src/gmock-spec-builders.d \
./tests/gmock/src/gmock.d \
./tests/gmock/src/gmock_main.d 


# Each subdirectory must supply rules for building sources it contributes
tests/gmock/src/%.o: ../tests/gmock/src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/home/agrippa/sdks/Graphics-Magick_1.3.18/include/GraphicsMagick -I/home/agrippa/sdks/glm_0.9.4.5/include -I"/home/agrippa/workspace_CDT_8.2.0/Ray/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -I"/home/agrippa/workspace_CDT_8.2.0/Ray/tests/gmock/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


