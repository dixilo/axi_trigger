open_project -reset proj_buffer

# Add design files
add_files buffer.cpp
add_files buffer.hpp
# Add test bench & files
add_files -tb buffer_test.cpp


# Set the top-level function
set_top axi_buffer

# ########################################################
# Create a solution
open_solution -reset solution1 -flow_target vivado

# Define technology and clock rate
set_part {xczu28dr-ffvg1517-2-e}
create_clock -period 2
set_clock_uncertainty 0.2
config_rtl -reset all

csynth_design
export_design -format ip_catalog

exit
