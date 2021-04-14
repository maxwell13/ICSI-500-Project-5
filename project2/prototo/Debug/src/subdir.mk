################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ErrorGen.cpp \
../src/Receiver.cpp \
../src/RunName.cpp \
../src/transmitter.cpp 

OBJS += \
./src/ErrorGen.o \
./src/Receiver.o \
./src/RunName.o \
./src/transmitter.o 

CPP_DEPS += \
./src/ErrorGen.d \
./src/Receiver.d \
./src/RunName.d \
./src/transmitter.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


