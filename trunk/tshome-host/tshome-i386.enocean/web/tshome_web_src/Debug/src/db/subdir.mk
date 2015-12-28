################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/db/sqlite3_ctrl.c \
../src/db/ts_db_conn_infos.c \
../src/db/ts_db_protocol_infos.c \
../src/db/ts_db_tools.c \
../src/db/ts_db_web_infos.c 

OBJS += \
./src/db/sqlite3_ctrl.o \
./src/db/ts_db_conn_infos.o \
./src/db/ts_db_protocol_infos.o \
./src/db/ts_db_tools.o \
./src/db/ts_db_web_infos.o 

C_DEPS += \
./src/db/sqlite3_ctrl.d \
./src/db/ts_db_conn_infos.d \
./src/db/ts_db_protocol_infos.d \
./src/db/ts_db_tools.d \
./src/db/ts_db_web_infos.d 


# Each subdirectory must supply rules for building sources it contributes
src/db/%.o: ../src/db/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DTSHOME_WEB -I/usr/include -I/home/pangt/5.Coding/tshome-i386 -I/opt/tshome-host/include -I/usr/include/c++/4.6/i686-linux-gnu -I/usr/include/c++/4.6 -I/usr/local/include/libxml2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


