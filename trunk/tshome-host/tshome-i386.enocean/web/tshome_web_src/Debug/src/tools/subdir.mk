################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/tools/ts_db_tools.c 

OBJS += \
./src/tools/ts_db_tools.o 

C_DEPS += \
./src/tools/ts_db_tools.d 


# Each subdirectory must supply rules for building sources it contributes
src/tools/%.o: ../src/tools/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DTSHOME_WEB -I/usr/include -I/usr/include/c++/4.6/i686-linux-gnu -I/usr/include/c++/4.6 -I/usr/local/include/libxml2 -I/home/pangt/5.Coding/common_files -I/home/pangt/project/TSHome/trunk/TSHome-Host/5.Coding/KeYuyi/GatewayManager -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


