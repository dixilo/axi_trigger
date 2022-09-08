#include "trigger.hpp"


void trigger(dds_str_t &data_in,
             hls::stream<double> &phase_in,
             dds_in &data_out,
             const double trigger_low[N_CH],
             const double trigger_high[N_CH],
             const unsigned int pre_length,
             const unsigned int post_length)
{
    // Stream in/out.
    #pragma HLS INTERFACE axis port=data_in
    #pragma HLS INTERFACE axis port=data_out
    #pragma HLS INTERFACE axis port=phase_in
    // Bram interface.
    #pragma HLS INTERFACE bram port=trigger_low
    #pragma HLS INTERFACE bram port=trigger_high
    // Ctrl interface suppression.
    #pragma HLS INTERFACE ap_ctrl_none port=return

    dds_str_t data_delayed;
    dds_str_t buffer;
    hls::stream<bool> trig_buf;
    hls::stream_of_blocks<dds_block_t> data_buf;

    unsigned int total_length = (pre_length + post_length) * N_CH;

#pragma HLS DATAFLOW
    
    trigger_core(phase_in, data_in, trigger_low, trigger_high, data_buf, trig_buf);

    data_buffer(data_buf, trig_buf, data_out, pre_length);
}

void trigger_core(hls::stream<double> &phase_in,
                  dds_str_t &data_in,
                  const double trigger_low[N_CH],
                  const double trigger_high[N_CH],
                  hls::stream_of_blocks<dds_block_t> &data_block_out,
                  hls::stream<bool> &trig_out){

#pragma HLS INLINE off

    // Sampling loop.
    core_loop0: for (int i = 0; i < MAX_PRE_LEN; i++){
#pragma HLS pipeline II=16
        bool trigger = false;

        hls::write_lock<block_data_t> outL(data_block_out);

        // Channel loop.
        core_loop1: for (unsigned int j = 0; j < N_CH; j++) {
            double phase = phase_in.read();
            if ((phase < trigger_low[j]) || (trigger_high[j] < phase)){
                trigger = true;
            }
            outL[j] = data_in.read();
        }
        trig_out.write(trigger);
    }
}

void data_buffer(hls::stream_of_blocks<dds_block_t> &data_block_in,
                 hls::stream<bool> &trig_in,
                 dds_str_t &data_out,
                 const int pre_length){
#pragma HLS INLINE off

    dds_str_t data_buffer;
    // Sampling loop
    buffer_loop0: for(int i = 0; i < pre_length; i++){
#pragma HLS pipeline II=16
        hls::read_lock<block_data_t> inL(data_block_in);

        buffer_loop1: for(int j = 0; j < N_CH; j++){
            data_buffer.write(inL[j]);
        }
    }

    // trigger
    trigger: while (true) {
#pragma HLS pipeline II=16
        bool trigger = trig_in.read();
        if (trigger) {
            for(int j = 0; j < N_CH; j++){
                data_out.write(data_buffer.read());
            }
            return;
        } else {
            for(int j = 0; j < N_CH; j++){
                data_buffer.read();
            }
        }
    }
}
