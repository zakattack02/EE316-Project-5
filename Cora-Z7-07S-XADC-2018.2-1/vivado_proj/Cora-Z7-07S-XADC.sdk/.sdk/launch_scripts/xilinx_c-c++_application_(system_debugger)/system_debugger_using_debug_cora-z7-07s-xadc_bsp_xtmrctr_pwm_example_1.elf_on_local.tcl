connect -url tcp:127.0.0.1:3121
source C:/Temp/EE316/p5/xadc_example/EE316-Project-5/Cora-Z7-07S-XADC-2018.2-1/vivado_proj/Cora-Z7-07S-XADC.sdk/design_1_wrapper_hw_platform_0/ps7_init.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Cora Z7 - 7007S 210370AFD27FA"} -index 0
loadhw -hw C:/Temp/EE316/p5/xadc_example/EE316-Project-5/Cora-Z7-07S-XADC-2018.2-1/vivado_proj/Cora-Z7-07S-XADC.sdk/design_1_wrapper_hw_platform_0/system.hdf -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Cora Z7 - 7007S 210370AFD27FA"} -index 0
stop
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Cora Z7 - 7007S 210370AFD27FA"} -index 0
rst -processor
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Cora Z7 - 7007S 210370AFD27FA"} -index 0
dow C:/Temp/EE316/p5/xadc_example/EE316-Project-5/Cora-Z7-07S-XADC-2018.2-1/vivado_proj/Cora-Z7-07S-XADC.sdk/Cora-Z7-07S-XADC_bsp_xtmrctr_pwm_example_1/Debug/Cora-Z7-07S-XADC_bsp_xtmrctr_pwm_example_1.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Cora Z7 - 7007S 210370AFD27FA"} -index 0
con
