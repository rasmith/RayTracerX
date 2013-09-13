################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include tests/gmock/test/subdir.mk
-include tests/gmock/src/subdir.mk
-include tests/gmock/gtest/xcode/Samples/FrameworkSample/subdir.mk
-include tests/gmock/gtest/test/subdir.mk
-include tests/gmock/gtest/src/subdir.mk
-include tests/gmock/gtest/samples/subdir.mk
-include tests/gmock/gtest/fused-src/gtest/subdir.mk
-include tests/gmock/gtest/codegear/subdir.mk
-include tests/gmock/gtest/CMakeFiles/gtest_main.dir/src/subdir.mk
-include tests/gmock/gtest/CMakeFiles/gtest.dir/src/subdir.mk
-include tests/gmock/fused-src/subdir.mk
-include tests/gmock/CMakeFiles/gmock_main.dir/src/subdir.mk
-include tests/gmock/CMakeFiles/gmock.dir/src/subdir.mk
-include tests/CMakeFiles/CompilerIdCXX/subdir.mk
-include tests/CMakeFiles/CompilerIdC/subdir.mk
-include tests/subdir.mk
-include src/subdir.mk
-include CMakeFiles/Ray.dir/src/subdir.mk
-include CMakeFiles/CompilerIdCXX/subdir.mk
-include CMakeFiles/CompilerIdC/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Ray

# Tool invocations
Ray: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	g++  -o "Ray" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) Ray
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
