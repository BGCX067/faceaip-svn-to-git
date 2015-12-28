################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/base_db_test.c \
../src/base_db_test_bak.c \
../src/bt.c \
../src/control_host.c \
../src/imp_xml.c \
../src/login_validate.c \
../src/main.c \
../src/on_off.c \
../src/set_channel.c \
../src/set_comm.c \
../src/set_ip_port.c \
../src/set_passageway.c \
../src/set_serial_info.c \
../src/tshome_web.c 

OBJS += \
./src/base_db_test.o \
./src/base_db_test_bak.o \
./src/bt.o \
./src/control_host.o \
./src/imp_xml.o \
./src/login_validate.o \
./src/main.o \
./src/on_off.o \
./src/set_channel.o \
./src/set_comm.o \
./src/set_ip_port.o \
./src/set_passageway.o \
./src/set_serial_info.o \
./src/tshome_web.o 

C_DEPS += \
./src/base_db_test.d \
./src/base_db_test_bak.d \
./src/bt.d \
./src/control_host.d \
./src/imp_xml.d \
./src/login_validate.d \
./src/main.d \
./src/on_off.d \
./src/set_channel.d \
./src/set_comm.d \
./src/set_ip_port.d \
./src/set_passageway.d \
./src/set_serial_info.d \
./src/tshome_web.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DTSHOME_WEB -I/usr/include -I/home/pangt/5.Coding/tshome-i386 -I/opt/tshome-host/include -I/usr/include/c++/4.6/i686-linux-gnu -I/usr/include/c++/4.6 -I/usr/local/include/libxml2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


