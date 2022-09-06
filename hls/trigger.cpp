#include "trigger.hpp"

static unsigned int count = 0;
static unsigned int pre_count = 0;
static bool triggered = false;
static bool pre_ready = false;


void trigger_core(hls::stream<double> &phase_in,
                  const double trigger_low[N_CH],
                  const double trigger_high[N_CH],
                  hls::stream<bool> &trig_out){
    bool trigger = false;
    for (int i = 0; i < N_CH; i++){
#pragma HLS pipeline II=1
        double phase;
        phase_in.read(phase);
        if ((phase < trigger_low[i]) || (trigger_high[i] < phase)){
            if (pre_ready){
                trigger = true;
            }
        }
    }

    trig_out.write(trigger);
}

void data_buffer(dds_in &data_in,
                 dds_in &data_out,
                 const int pre_length){
    int length = 0;
    dds_in buffer;
    while(length < pre_length*N_CH){
        buffer.write(data_in.read());
    }

    while (true)
        data_out.write(buffer.read());
}

void data_publisher(dds_in &data_delayed, dds_in &data_out, hls::stream<bool> &trig_in){
    if(trig_in.read()){
        for(int i = 0; i < N_CH; i++){
#pragma HLS pipeline II=1
            data_out.write(data_delayed.read());
        }
    } else {
        for(int i = 0; i < N_CH; i++){
#pragma HLS pipeline II=1
            data_delayed.read();
        }
    }
}

void trigger(dds_in &data_in,
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

    dds_in data_delayed;
    dds_in buffer;
    hls::stream<bool> trig_buf;

    unsigned int total_length = (pre_length + post_length) * N_CH;

#pragma HLS DATAFLOW
    trigger_core(phase_in, trigger_low, trigger_high, trig_buf);
    data_buffer(data_in, data_delayed, pre_length);
    data_publisher(data_delayed, data_out, trig_buf);
}
