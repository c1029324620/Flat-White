################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../cc13x2_cc26x2_tirtos.cmd 

SYSCFG_SRCS += \
../portable.syscfg 

C_SRCS += \
../console.c \
../main_tirtos.c \
./syscfg/ti_devices_config.c \
./syscfg/ti_drivers_config.c \
../temperature.c 

GEN_FILES += \
./syscfg/ti_devices_config.c \
./syscfg/ti_drivers_config.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./console.d \
./main_tirtos.d \
./syscfg/ti_devices_config.d \
./syscfg/ti_drivers_config.d \
./temperature.d 

OBJS += \
./console.obj \
./main_tirtos.obj \
./syscfg/ti_devices_config.obj \
./syscfg/ti_drivers_config.obj \
./temperature.obj 

GEN_MISC_FILES += \
./syscfg/ti_drivers_config.h \
./syscfg/ti_utils_build_linker.cmd.genlibs \
./syscfg/syscfg_c.rov.xs \
./syscfg/ti_utils_runtime_model.gv \
./syscfg/ti_utils_runtime_Makefile 

GEN_MISC_DIRS__QUOTED += \
"syscfg\" 

OBJS__QUOTED += \
"console.obj" \
"main_tirtos.obj" \
"syscfg\ti_devices_config.obj" \
"syscfg\ti_drivers_config.obj" \
"temperature.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_config.h" \
"syscfg\ti_utils_build_linker.cmd.genlibs" \
"syscfg\syscfg_c.rov.xs" \
"syscfg\ti_utils_runtime_model.gv" \
"syscfg\ti_utils_runtime_Makefile" 

C_DEPS__QUOTED += \
"console.d" \
"main_tirtos.d" \
"syscfg\ti_devices_config.d" \
"syscfg\ti_drivers_config.d" \
"temperature.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_devices_config.c" \
"syscfg\ti_drivers_config.c" 

C_SRCS__QUOTED += \
"../console.c" \
"../main_tirtos.c" \
"./syscfg/ti_devices_config.c" \
"./syscfg/ti_drivers_config.c" \
"../temperature.c" 

SYSCFG_SRCS__QUOTED += \
"../portable.syscfg" 


