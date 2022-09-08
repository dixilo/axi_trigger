#include <iostream>
#include "trigger.hpp"
using namespace std;


int main()
{

    dds_in data_in;
    hls::stream<double> phase_in;
    dds_in data_out;
    double trigger_low[N_CH];
    double trigger_high[N_CH];
    unsigned int pre_length = 100;
    unsigned int post_length = 100;

    ap_int<48> x, y;

    // Trigger setting
    for(int j = 0; j < N_CH; j++){
        trigger_low[j] = -1;
        trigger_high[j] = j;
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
        double phd = 0;
        phase_in.write(phd);
    }

    trigger(data_in, phase_in, data_out, trigger_low, trigger_high, pre_length, post_length);

    /*
    for(int j = 0; j < N_CH; j++){
        double result;
        dds_data pipe;
        data_out.read(result);
        data_pipe.read(pipe);
        cout << "result:" << result << endl;
        cout << "result (pipe):" << pipe.data << endl;
    }
    */

    cout << "Success: results match" << endl;
    return 0;
}
