#pragma once
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "hls_streamofblocks.h"


#define N_CH 16
#define DDS_BW 48
#define MAX_PRE_LEN 128

typedef ap_int<DDS_BW*2> dds_t;

typedef hls::stream<dds_t> dds_str_t;
typedef dds_t dds_block_t[N_CH];

void trigger_core(hls::stream<double> &phase_in,
                  dds_str_t &data_in,
                  const double trigger_low[N_CH],
                  const double trigger_high[N_CH],
                  hls::stream_of_blocks<dds_block_t> &data_block_out,
                  hls::stream<bool> &trig_out);

void data_buffer(hls::stream_of_blocks<dds_block_t> &data_block_in,
                 hls::stream_of_blocks<dds_block_t> &data_block_out,
                 hls::stream<bool> &trig_in,
                 hls::stream<bool> &trig_out,
                 const int pre_length);

void publisher(hls::stream_of_blocks<dds_block_t> &data_block_in,
               hls::stream<bool> &trig_in,
               dds_str_t &data_out);

void trigger(dds_str_t &data_in,
             hls::stream<double> &phase_in,
             dds_str_t &data_out,
             const double trigger_low[N_CH],
             const double trigger_high[N_CH],
             const unsigned int pre_length,
             const unsigned int post_length);
