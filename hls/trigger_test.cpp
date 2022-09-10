
#include <iostream>
#include <fstream>
#include "trigger.hpp"
using namespace std;


int main()
{

    dds_str_t data_in;
    hls::stream<double> phase_in;
    dds_str_t data_out;
    double trigger_low[N_CH];
    double trigger_high[N_CH];
    unsigned int pre_length = 100;
    unsigned int post_length = 100;

    ap_int<48> x, y;

    char datafile[] = "tb_data.dat";
    char phasefile[] = "tb_phase.dat";

    ifstream f_data( datafile, ios::in | ios::binary );
    ifstream f_phase( phasefile, ios::in | ios::binary );

    if (!f_data | !f_phase){
        cout << "File not found." << endl;
        return -1;
    }

    // Trigger setting
    for(int j = 0; j < N_CH; j++){
        trigger_low[j] = -1;
        trigger_high[j] = j;
    }

    // Raw data setting.
    dds_t rawd;

    while(!f_data.eof()){
        f_data.read((char *) &rawd, sizeof(rawd));
        data_in.write(rawd);
    }
    f_data.close();

    // Phase setting.
    double phd;
    while(!f_phase.eof()){
        f_phase.read((char *) &phd, sizeof(phd));
        phase_in.write(phd);
    }
    f_phase.close();

    trigger(data_in, phase_in, data_out, trigger_low, trigger_high, pre_length, post_length);

    cout << "Success: results match" << endl;
    return 0;
}
