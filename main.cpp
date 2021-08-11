//#include <stdio.h>
#include "vbz.h"
#include <iostream>
#include <vector>
#include <inttypes.h>
#include "slow5/slow5.h"

#define FILE1 "test/100_reads.slow5"

void perform_compression_test(
        std::vector<int16_t> const& data,
        CompressionOptions const& options)
{
auto const input_data_size = vbz_size_t(data.size() * sizeof(data[0]));
vbz_size_t upper_limit = vbz_max_compressed_size(input_data_size, &options);
std::vector<int8_t> dest_buffer(upper_limit);
    auto final_byte_count = vbz_compress(
        data.data(),
        input_data_size,
        dest_buffer.data(),
        vbz_size_t(dest_buffer.size()),
        &options);
    //REQUIRE(!vbz_is_error(final_byte_count));
    if(vbz_is_error(final_byte_count)){
        fprintf(stderr,"compression failed");
        return;
    }
    dest_buffer.resize(final_byte_count);

    std::vector<int8_t> decompressed_bytes(input_data_size);
    auto const decompress_data_size = vbz_size_t(decompressed_bytes.size());
    auto decompressed_byte_count = vbz_decompress(
            dest_buffer.data(),
            vbz_size_t(dest_buffer.size()),
            decompressed_bytes.data(),
            decompress_data_size,
            &options
    );
    //REQUIRE(!vbz_is_error(decompressed_byte_count));
    if(vbz_is_error(decompressed_byte_count)){
        fprintf(stderr,"decompression failed");
        return;
    }
    decompressed_bytes.resize(decompressed_byte_count);
    fprintf(stderr,"%" PRIu32 "\t", vbz_size_t(data.size()*sizeof(data[0])));
//    fprintf(stderr,"sizeof(data[0])=%zu\n", sizeof(data[0]));
    fprintf(stderr,"%" PRIu32 "\t", upper_limit);
    fprintf(stderr,"%" PRIu32 "\t", vbz_size_t(dest_buffer.size()*sizeof(dest_buffer[0])));
    fprintf(stderr,"%" PRIu32 "\n", vbz_size_t(decompressed_bytes.size()*sizeof(decompressed_bytes[0])));
    //auto decompressed = gsl::make_span(decompressed_bytes).as_span<T>();

    //INFO("Original     " << dump_explicit<std::int64_t>(data));
    //INFO("Decompressed " << dump_explicit<std::int64_t>(decompressed));
    //CHECK(decompressed == gsl::make_span(data));
}

int main() {
    CompressionOptions compression_options{
            false,
            sizeof(int16_t),
            1,
            VBZ_DEFAULT_VERSION
    };

    slow5_file_t *sp = slow5_open(FILE1 ,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }
    slow5_rec_t *rec = NULL;
    int ret=0;

    fprintf(stderr,"input_data_size(bytes)\tcompression_upper_limit(bytes)\tcompressed_data_size(bytes)\tdecompressed_data_size(bytes)\n");

    while((ret = slow5_get_next(&rec,sp)) >= 0){
        std::vector<int16_t>data;
        uint64_t len_raw_signal = rec->len_raw_signal;
        for(uint64_t i=0;i<len_raw_signal;i++){
            data.push_back(rec->raw_signal[i]);
        }
        perform_compression_test(data,compression_options);
    }

    if(ret != SLOW5_ERR_EOF){  //check if proper end of file has been reached
        fprintf(stderr,"Error in slow5_get_next. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }

    slow5_rec_free(rec);
    slow5_close(sp);
    return 0;
}
