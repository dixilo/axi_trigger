set ip_name "axi_trigger"
create_project $ip_name "./${ip_name}" -force -part "xczu28dr-ffvg1517-2-e"
#set_property board_part xilinx.com:zcu111:part0:1.4 [current_project]
source ./util.tcl

create_bd_design "axi_trigger"
update_compile_order -fileset sources_1

# IP repository setting to import "rewind" core produced by Vitis HLS
set_property  ip_repo_paths {"./hls/proj_trigger/solution1/impl/ip" \
                             "./hls/proj_buffer/solution1/impl/ip"} [current_project]
update_ip_catalog

# Implementation of rewind core
create_bd_cell -type ip -vlnv [latest_ip axi_buffer] axi_buffer
create_bd_cell -type ip -vlnv [latest_ip trigger] trigger

# Block memory
create_bd_cell -type ip -vlnv [latest_ip blk_mem_gen] bram_trig_low
create_bd_cell -type ip -vlnv [latest_ip blk_mem_gen] bram_trig_high

connect_bd_intf_net [get_bd_intf_pins trigger/trigger_low_PORTA] [get_bd_intf_pins bram_trig_low/BRAM_PORTA]
connect_bd_intf_net [get_bd_intf_pins trigger/trigger_high_PORTA] [get_bd_intf_pins bram_trig_high/BRAM_PORTA]


set_property -dict [list CONFIG.Memory_Type {True_Dual_Port_RAM}] [get_bd_cells bram_trig_low]
set_property -dict [list CONFIG.Memory_Type {True_Dual_Port_RAM}] [get_bd_cells bram_trig_high]

create_bd_cell -type ip -vlnv [latest_ip axi_bram_ctrl] axi_bc_trig_low
create_bd_cell -type ip -vlnv [latest_ip axi_bram_ctrl] axi_bc_trig_high

set_property -dict [list CONFIG.DATA_WIDTH {32} CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells axi_bc_trig_low]
set_property -dict [list CONFIG.DATA_WIDTH {32} CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells axi_bc_trig_high]

connect_bd_intf_net [get_bd_intf_pins axi_bc_trig_low/BRAM_PORTA]  [get_bd_intf_pins bram_trig_low/BRAM_PORTB]
connect_bd_intf_net [get_bd_intf_pins axi_bc_trig_high/BRAM_PORTA] [get_bd_intf_pins bram_trig_high/BRAM_PORTB]

# Clock and reset
## Device clock (expected to be 256 MHz)
create_bd_net dev_clk
create_bd_port -dir I -type clk dev_clk
connect_bd_net -net [get_bd_nets dev_clk] [get_bd_ports dev_clk]
connect_bd_net -net [get_bd_nets dev_clk] [get_bd_pins trigger/ap_clk]
connect_bd_net -net [get_bd_nets dev_clk] [get_bd_pins axi_buffer/ap_clk]

## Device reset
create_bd_net dev_rstn
create_bd_port -dir I -type rst dev_rstn
connect_bd_net -net [get_bd_nets dev_rstn] [get_bd_ports dev_rstn]
connect_bd_net -net [get_bd_nets dev_rstn] [get_bd_pins trigger/ap_rst_n]
connect_bd_net -net [get_bd_nets dev_rstn] [get_bd_pins axi_buffer/ap_rst_n]

## AXI clock
create_bd_net s_axi_aclk
create_bd_port -dir I -type clk s_axi_aclk
connect_bd_net -net [get_bd_nets s_axi_aclk] [get_bd_ports s_axi_aclk]

connect_bd_net -net [get_bd_nets s_axi_aclk] [get_bd_pins axi_bc_trig_low/s_axi_aclk]
connect_bd_net -net [get_bd_nets s_axi_aclk] [get_bd_pins axi_bc_trig_high/s_axi_aclk]
connect_bd_net -net [get_bd_nets s_axi_aclk] [get_bd_pins axi_buffer/s_axi_aclk]

## AXI resetn
create_bd_net axi_aresetn
create_bd_port -dir I -type rst axi_aresetn
connect_bd_net -net [get_bd_nets axi_aresetn] [get_bd_ports axi_aresetn]
connect_bd_net -net [get_bd_nets axi_aresetn] [get_bd_pins axi_buffer/ap_rst_n_s_axi_aclk]
connect_bd_net -net [get_bd_nets axi_aresetn] [get_bd_pins axi_bc_trig_low/s_axi_aresetn]
connect_bd_net -net [get_bd_nets axi_aresetn] [get_bd_pins axi_bc_trig_high/s_axi_aresetn]

# AXI interfaces
create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_trig_low
create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_trig_high
create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_control

set_property -dict [list CONFIG.PROTOCOL   AXI4LITE \
                         CONFIG.HAS_BURST  0 \
                         CONFIG.HAS_CACHE  0 \
                         CONFIG.HAS_LOCK   0 \
                         CONFIG.HAS_PROT   0 \
                         CONFIG.HAS_QOS	   0 \
                         CONFIG.HAS_REGION 0] [get_bd_intf_ports s_axi_control]

connect_bd_intf_net [get_bd_intf_ports s_axi_trig_low]  [get_bd_intf_pins axi_bc_trig_low/S_AXI]
connect_bd_intf_net [get_bd_intf_ports s_axi_trig_high] [get_bd_intf_pins axi_bc_trig_high/S_AXI]
connect_bd_intf_net [get_bd_intf_ports s_axi_control]   [get_bd_intf_pins axi_buffer/s_axi_control]

set_property CONFIG.ID_WIDTH 16 [get_bd_intf_ports s_axi_trig_low]
set_property CONFIG.ID_WIDTH 16 [get_bd_intf_ports s_axi_trig_high]
set_property CONFIG.ID_WIDTH 0 [get_bd_intf_ports s_axi_control]

# AXIS interfaces
create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 axis_data_in
set_property -dict [list  \
                         CONFIG.HAS_TLAST {1} \
                         CONFIG.HAS_TKEEP {1} \
                         CONFIG.HAS_TSTRB {1} \
                         CONFIG.HAS_TREADY {1} \
                         CONFIG.TDATA_NUM_BYTES {12}] [get_bd_intf_ports axis_data_in]
connect_bd_intf_net [get_bd_intf_ports axis_data_in] [get_bd_intf_pins trigger/data_in]

create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 axis_phase_in
set_property -dict [list  \
                         CONFIG.HAS_TLAST {0} \
                         CONFIG.HAS_TKEEP {0} \
                         CONFIG.HAS_TSTRB {0} \
                         CONFIG.HAS_TREADY {1} \
                         CONFIG.TDATA_NUM_BYTES {8}] [get_bd_intf_ports axis_phase_in]
connect_bd_intf_net [get_bd_intf_ports axis_phase_in] [get_bd_intf_pins trigger/phase_in]

create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 axis_data_out
set_property -dict [list  \
                         CONFIG.HAS_TLAST {1} \
                         CONFIG.HAS_TKEEP {1} \
                         CONFIG.HAS_TSTRB {1} \
                         CONFIG.HAS_TREADY {1} \
                         CONFIG.HAS_TUSER {1} \
                         CONFIG.TDATA_NUM_BYTES {12} \
                         CONFIG.TUSER_WIDTH {1}] [get_bd_intf_ports axis_data_out]
connect_bd_intf_net [get_bd_intf_ports axis_data_out] [get_bd_intf_pins axi_buffer/data_out]

connect_bd_intf_net [get_bd_intf_pins trigger/data_out] [get_bd_intf_pins axi_buffer/data_in]

save_bd_design
validate_bd_design

ipx::package_project -root_dir "./ip_repo" -vendor kuhep -library user -taxonomy /UserIP -module axi_trigger -import_files

set axi_trigger [ipx::find_open_core kuhep:user:axi_trigger:1.0]

set_property display_name axi_trigger                     $axi_trigger
set_property description {Self trigger function for KIDs} $axi_trigger
set_property vendor_display_name kuhep                    $axi_trigger
set_property core_revision 2                              $axi_trigger

ipx::create_xgui_files                                    $axi_trigger
ipx::update_checksums                                     $axi_trigger
ipx::check_integrity                                      $axi_trigger
ipx::save_core                                            $axi_trigger
#set_property  ip_repo_paths  {"./ip_repo" "./hls/proj_rewind/solution1/impl/ip"} [current_project]
#update_ip_catalog

ipx::infer_bus_interface dev_clk xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface s_axi_aclk xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]

ipx::associate_bus_interfaces -busif axis_data_in -clock dev_clk [ipx::current_core]
ipx::associate_bus_interfaces -busif axis_phase_in -clock dev_clk [ipx::current_core]
ipx::associate_bus_interfaces -busif axis_data_out -clock dev_clk [ipx::current_core]

ipx::associate_bus_interfaces -busif s_axi_trig_low -clock s_axi_aclk [ipx::current_core]
ipx::associate_bus_interfaces -busif s_axi_trig_high -clock s_axi_aclk [ipx::current_core]
ipx::associate_bus_interfaces -busif s_axi_control -clock s_axi_aclk [ipx::current_core]

ipx::save_core [ipx::current_core]
