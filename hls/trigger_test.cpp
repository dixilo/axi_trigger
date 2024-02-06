#include <iostream>
#include "trigger.hpp"
using namespace std;


int main()
{
    dds_stream_t data_in;
    hls::stream<double> phase_in;
    trig_stream_t data_out;
    double trigger_low[N_CH];
    double trigger_high[N_CH];
    unsigned int pre_length = 100;
    unsigned int post_length = 100;

    ap_int<48> x, y;

    // Trigger setting
    for(int j = 0; j < N_CH; j++){
        trigger_low[j] = -1;
        trigger_high[j] = 0.5*j;
    }

    // Raw data setting.
    for(int j = 0; j < N_CH; j++){
        dds_t tmpd;
        x = j+1;
        y = 0;
        tmpd = y.concat(x);
        data_in.write(tmpd);
    }

    // Phase setting.
    for(int j = 0; j < N_CH; j++){
        double phd = j;
        phase_in.write(phd);
    }

    trigger(data_in, phase_in, data_out, trigger_low, trigger_high);


    for(int j = 0; j < N_CH; j++){
        trig_t result;
        data_out >> result;
        
        cout << "result data:" << result.data << endl;
        cout << "result user:" << result.user << endl;
        cout << endl;
    }

    cout << "Success: results match" << endl;
    return 0;
}
