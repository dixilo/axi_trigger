#include "trigger.hpp"

void trigger(dds_stream_t &data_in,
             hls::stream<double> &phase_in,
             trig_stream_t &data_out,
             const double trigger_low[N_CH],
             const double trigger_high[N_CH])
{
// Stream in/out.
#pragma HLS INTERFACE axis port=data_in
#pragma HLS INTERFACE axis port=data_out
#pragma HLS INTERFACE axis port=phase_in
// Bram interface.
#pragma HLS INTERFACE bram port=trigger_low storage_type=rom_1p
#pragma HLS INTERFACE bram port=trigger_high storage_type=rom_1p
// Ctrl interface suppression.
#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS pipeline II=16
    // Trigger 
    bool trigger = false;

    double phase;
    judge_loop: for (int i = 0; i < N_CH; i++){
        phase_in >> phase;

        if ((phase < trigger_low[i]) || (trigger_high[i] < phase)){
            trigger = true;
        }
    }

    // Data publishing
    dds_t dds_tmp;
    trig_t trig_tmp;
    publish_loop: for(int i = 0; i < N_CH; i++){
        data_in >> dds_tmp;
        trig_tmp.data = dds_tmp;
        trig_tmp.user = trigger;
        data_out << trig_tmp;
    }
}
