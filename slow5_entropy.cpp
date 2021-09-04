#include <stdio.h>
#include <iostream>
#include <vector>
#include "slow5/slow5.h"
#include <unordered_map>

#define FILE1 "test/100_reads.slow5"
#define FILE2 "../slow5tools/build/test/P2.blow5"

int main() {

    slow5_file_t *sp = slow5_open(FILE1 ,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }
    slow5_rec_t *rec = NULL;
    int ret=0;

    std::unordered_map<std::int16_t ,uint64_t> signal_frequency;
    std::unordered_map<std::int16_t ,uint64_t> signal_difference_frequency;
    signal_frequency.reserve(8000);
    signal_difference_frequency.reserve(1000);
    int16_t prev_value;
    uint64_t num_observations1 = 0;
    uint64_t num_records =0;
    while((ret = slow5_get_next(&rec,sp)) >= 0){
        num_records++;
        uint64_t len_raw_signal = rec->len_raw_signal;
        num_observations1 += len_raw_signal;
        for(uint64_t i=0;i<len_raw_signal;i++){
            int16_t value = rec->raw_signal[i];
            signal_frequency[value]++;
            if (i>0){
                signal_difference_frequency[value-prev_value]++;
//                if (abs(value-prev_value)>500){
//                    break;
//                }
            }
            prev_value = value;
        }
    }
    if(ret != SLOW5_ERR_EOF){  //check if proper end of file has been reached
        fprintf(stderr,"Error in slow5_get_next. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }

    slow5_rec_free(rec);
    slow5_close(sp);

    fprintf(stderr,"num_records\t%" PRIu64 "\n", num_records);

    int16_t max_sig_value = INT16_MIN;
    int16_t min_sig_value =INT16_MAX;
    for (auto& x: signal_frequency) {
        if(max_sig_value < x.first){
            max_sig_value = x.first;
        }
        if (min_sig_value > x.first){
            min_sig_value = x.first;
        }
    }

    int16_t sig_freq_map_size = signal_frequency.size();
    int16_t num_events1 = max_sig_value-min_sig_value;
    double entropy1 = 0;
    for (auto& x: signal_frequency) {
        double prob = 1.0*x.second/num_observations1;
        entropy1 += prob*log2(prob);
    }
    entropy1 = -entropy1;
    double uniform_prob_entropy1 = -1.0*log2(1.0/num_events1);
    fprintf(stderr,"sig_freq_map_siz\t%d\nnum_events1\t%d\nentropy\t%.3f\nuniform_prob_entropy1\t%.3f\n", sig_freq_map_size, num_events1, entropy1, uniform_prob_entropy1);

    int16_t max_diff_value = INT16_MIN;
    int16_t min_diff_value =INT16_MAX;
    for (auto& x: signal_difference_frequency) {
        if(max_diff_value < x.first){
            max_diff_value = x.first;
        }
        if (min_diff_value > x.first){
            min_diff_value = x.first;
        }
    }
    uint64_t num_observations2 = num_observations1 - num_records;
    int16_t sig_freq_diff_map_size = signal_difference_frequency.size();
    int16_t num_events2 = max_diff_value-min_diff_value;
    double entropy2 = 0;
    for (auto& x: signal_difference_frequency) {
        double prob = 1.0*x.second/num_observations2;
        entropy2 += prob*log2(prob);
    }
    entropy2 = -entropy2;
    double uniform_prob_entropy2 = -1.0*log2(1.0/num_events2);
    fprintf(stderr,"sig_freq_diff_map_siz\t%d\nnum_events2\t%d\nentropy\t%.3f\nuniform_prob_entropy2\t%.3f\n", sig_freq_diff_map_size, num_events2, entropy2, uniform_prob_entropy2);

    return 0;
}
