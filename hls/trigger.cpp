#include "trigger.hpp"


void trigger(dds_str_t &data_in,
             hls::stream<double> &phase_in,
             dds_str_t &data_out,
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
    //#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS DATAFLOW

    dds_str_t data_delayed;
    dds_str_t buffer;
    hls::stream<bool, MAX_PRE_LEN> trig_buf;
    hls::stream<bool, MAX_PRE_LEN> trig_buf_2;
    hls::stream_of_blocks<dds_block_t> data_buf;
    hls::stream_of_blocks<dds_block_t> data_buf_2;

    unsigned int total_length = (pre_length + post_length) * N_CH;
    
    trigger_core(phase_in, data_in, trigger_low, trigger_high, data_buf, trig_buf);
    data_buffer(data_buf, data_buf_2, trig_buf, trig_buf_2, pre_length);
    publisher(data_buf_2, trig_buf_2, data_out);
}

void trigger_core(hls::stream<double> &phase_in,
                  dds_str_t &data_in,
                  const double trigger_low[N_CH],
                  const double trigger_high[N_CH],
                  //hls::stream<double> &data_out,
                  hls::stream_of_blocks<dds_block_t> &data_block_out,
                  hls::stream<bool> &trig_out){

#pragma HLS INLINE off

    // Sampling loop.
    core_loop0: for (int i = 0; i < MAX_PRE_LEN; i++){
#pragma HLS pipeline II=16
        bool trigger = false;

        hls::write_lock<dds_block_t> outL(data_block_out);

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
                 hls::stream_of_blocks<dds_block_t> &data_block_out,
                 hls::stream<bool> &trig_in,
                 hls::stream<bool> &trig_out,
                 const int pre_length){
#pragma HLS INLINE off

    hls::stream<dds_t, N_CH*MAX_PRE_LEN> data_buffer;

    int length = 0;
    buffer_loop0: while (true){
#pragma HLS pipeline II=16
        hls::read_lock<dds_block_t> inL(data_block_in);
        hls::write_lock<dds_block_t> outL(data_block_out);

        buffer_loop1: for(int j = 0; j < N_CH; j++){
            data_buffer.write(inL[j]);
            if (length == pre_length){
                outL[j] = data_buffer.read();
                trig_out.write(trig_in.read());
            }
        }

        if (length < pre_length){
            length++;
        }
    }
}

void publisher(hls::stream_of_blocks<dds_block_t> &data_block_in,
               hls::stream<bool> &trig_in,
               dds_str_t &data_out){
#pragma HLS INLINE off

    publisher_loop0: while (true){
#pragma HLS pipeline II=16
        hls::read_lock<dds_block_t> inL(data_block_in);
        bool trigger = trig_in.read();

        for(int j = 0; j < N_CH; j++){
            if (trigger){
                data_out.write(inL[j]);
            } else {
                inL[j];
            }
        }
    }
}
