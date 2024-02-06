#include "buffer.hpp"

constexpr int length = 1024;

void axi_buffer(trig_stream_t &data_in,
                trig_stream_t &data_out,
                int &pre_length){
// Stream in/out.
#pragma HLS INTERFACE axis port=data_in
#pragma HLS INTERFACE axis port=data_out
#pragma HLS INTERFACE mode=s_axilite port=pre_length clock=s_axi_aclk
// Ctrl interface suppression.
#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS pipeline II=16
    static int index_in = 0;
    static int counter = 0;

    static dds_t buffer[MAX_PRE_LEN][N_CH];
#pragma HLS DEPENDENCE dependent=false type=intra variable=buffer

    
    // Data buffering and trigger acq.
    bool tmpt = false;
    for(int i = 0; i < N_CH; i++){
        trig_t tmpd;
        data_in >> tmpd;
        buffer[index_in][i] = tmpd.data;
        tmpt |= tmpd.user;
    }

    // Trigger start.
    if ((counter == 0) && tmpt){
        counter = length;
    }

    // Output.
    if (counter > 0){
        int index_out = (index_in >= pre_length) ?
                            index_in - pre_length :
                            index_in - pre_length + MAX_PRE_LEN;

        for(int i = 0; i < N_CH; i++){
            dds_t tmpd = buffer[index_out][i];
            trig_t outd;
            outd.data = tmpd;
            outd.user = true;
            data_out << outd;
        }
        counter--;
    }

    // Ring buffering.
    index_in = (index_in == MAX_PRE_LEN - 1) ? 0 : index_in + 1;
}
