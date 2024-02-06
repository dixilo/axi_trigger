#pragma once
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define N_CH 16
#define DDS_BW 48
#define MAX_PRE_LEN 128

typedef ap_int<DDS_BW*2> dds_t;

typedef hls::stream<dds_t> dds_stream_t;

typedef ap_axis<DDS_BW*2,1,0,0> trig_t;
typedef hls::stream<trig_t> trig_stream_t;

void trigger(dds_stream_t &data_in,
             hls::stream<double> &phase_in,
             trig_stream_t &data_out,
             const double trigger_low[N_CH],
             const double trigger_high[N_CH]);
