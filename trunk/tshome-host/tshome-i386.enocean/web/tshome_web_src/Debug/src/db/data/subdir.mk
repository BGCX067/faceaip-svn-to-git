################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/db/data/ts_db_conn_infos.c \
../src/db/data/ts_db_protocol_infos.c \
../src/db/data/ts_db_web_infos.c 

OBJS += \
./src/db/data/ts_db_conn_infos.o \
./src/db/data/ts_db_protocol_infos.o \
./src/db/data/ts_db_web_infos.o 

C_DEPS += \
./src/db/data/ts_db_conn_infos.d \
./src/db/data/ts_db_protocol_infos.d \
./src/db/data/ts_db_web_infos.d 


# Each subdirectory must supply rules for building sources it contributes
src/db/data/%.o: ../src/db/data/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DTSHOME_WEB -I/usr/include -I/usr/include/c++/4.6/i686-linux-gnu -I/usr/include/c++/4.6 -I/usr/local/include/libxml2 -I/home/pangt/5.Coding/common_files -I/home/pangt/project/TSHome/trunk/TSHome-Host/5.Coding/KeYuyi/GatewayManager -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


