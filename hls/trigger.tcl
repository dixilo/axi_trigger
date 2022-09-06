open_project -reset proj_trigger

# Add design files
add_files trigger.cpp
add_files trigger.hpp
# Add test bench & files
add_files -tb trigger_test.cpp


# Set the top-level function
set_top trigger

# ########################################################
# Create a solution
open_solution -reset solution1 -flow_target vivado

# Define technology and clock rate
set_part {xczu28dr-ffvg1517-2-e}
create_clock -period 2
set_clock_uncertainty 0.2

csynth_design
export_design -format ip_catalog

exit
