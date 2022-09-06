#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define N_CH 16
#define DDS_BW 48
#define MAX_PRE_LEN 128

typedef ap_int<DDS_BW*2> dds_data;

typedef hls::stream<dds_data> dds_in;

void trigger_core(hls::stream<double> &phase_in,
                  const double trigger_low[N_CH],
                  const double trigger_high[N_CH],
                  hls::stream<bool> &trig_out);

void data_buffer(dds_in &data_in,
                 dds_in &data_out,
                 const int pre_length);

void data_publisher(dds_in &data_delayed,
                    dds_in &data_out,
                    bool trigger,
                    const int length);

void trigger(dds_in &data_in,
             hls::stream<double> &phase_in,
             dds_in &data_out,
             const double trigger_low[N_CH],
             const double trigger_high[N_CH],
             const unsigned int pre_length,
             const unsigned int post_length);

