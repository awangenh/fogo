################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Source/App.cpp 

OBJS += \
./Source/App.o 

CPP_DEPS += \
./Source/App.d 


# Each subdirectory must supply rules for building sources it contributes
Source/%.o: ../Source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_GNUWIN32_ -D_WXMSW_ -DWXUSINGDLL -I"E:\Eclipse_workspaces\CSC_new\Include" -I"E:\Eclipse_workspaces\libutils\Include" -I"C:/wxWidgets-2.8.0/include" -I"C:/wxWidgets-2.8.0/contrib/include" -I"C:/wxWidgets-2.8.0/lib/gcc_dll/msw" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

