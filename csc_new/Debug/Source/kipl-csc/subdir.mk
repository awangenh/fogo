################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Source/kipl-csc/islhierarchy.cpp \
../Source/kipl-csc/trafficsigncolsim.cpp \
../Source/kipl-csc/trafficsignlinkcontrol.cpp 

OBJS += \
./Source/kipl-csc/islhierarchy.o \
./Source/kipl-csc/trafficsigncolsim.o \
./Source/kipl-csc/trafficsignlinkcontrol.o 

CPP_DEPS += \
./Source/kipl-csc/islhierarchy.d \
./Source/kipl-csc/trafficsigncolsim.d \
./Source/kipl-csc/trafficsignlinkcontrol.d 


# Each subdirectory must supply rules for building sources it contributes
Source/kipl-csc/%.o: ../Source/kipl-csc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_GNUWIN32_ -D_WXMSW_ -DWXUSINGDLL -I"E:\Eclipse_workspaces\CSC_new\Include" -I"E:\Eclipse_workspaces\libutils\Include" -I"C:/wxWidgets-2.8.0/include" -I"C:/wxWidgets-2.8.0/contrib/include" -I"C:/wxWidgets-2.8.0/lib/gcc_dll/msw" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


