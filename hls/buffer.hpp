#pragma once
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "trigger.hpp"

void axi_buffer(trig_stream_t &data_in,
                trig_stream_t &data_out,
                int &pre_length);
