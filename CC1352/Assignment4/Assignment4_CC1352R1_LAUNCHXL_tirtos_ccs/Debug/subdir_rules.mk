################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/c1029/workspace_v10/Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs" --include_path="C:/Users/c1029/workspace_v10/Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs/Debug" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/posix/ccs" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" --define=DeviceFamily_CC13X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/c1029/workspace_v10/Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-583584319:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-583584319-inproc

build-583584319-inproc: ../Assignment4.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/ccs1010/xdctools_3_61_02_27_core/xs" --xdcpath="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source;C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/kernel/tirtos/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.simplelink:CC13X2_CC26X2 -r release -c "C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path=\"C:/Users/c1029/workspace_v10/Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs\" --include_path=\"C:/Users/c1029/workspace_v10/Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs/Debug\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/posix/ccs\" --include_path=\"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include\" --define=DeviceFamily_CC13X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-583584319 ../Assignment4.cfg
configPkg/compiler.opt: build-583584319
configPkg/: build-583584319

build-954245679: ../Assignment4.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1010/ccs/utils/sysconfig_1.6.0/sysconfig_cli.bat" -s "C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/.metadata/product.json" -o "syscfg" --compiler ccs "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_devices_config.c: build-954245679 ../Assignment4.syscfg
syscfg/ti_drivers_config.c: build-954245679
syscfg/ti_drivers_config.h: build-954245679
syscfg/ti_utils_build_linker.cmd.genlibs: build-954245679
syscfg/syscfg_c.rov.xs: build-954245679
syscfg/ti_utils_runtime_model.gv: build-954245679
syscfg/ti_utils_runtime_Makefile: build-954245679
syscfg/: build-954245679

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/c1029/workspace_v10/Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs" --include_path="C:/Users/c1029/workspace_v10/Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs/Debug" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/posix/ccs" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" --define=DeviceFamily_CC13X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/c1029/workspace_v10/Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

