#pragma once

#ifndef _MAIN_H
#define _MAIN_H

typedef struct CompilerConfig {
    const char* input_file;
    const char* output_file;
    const char* output_format;
    
    char optimization_level;
    char enable_simd;
    char enable_cuda;
    char enable_debug;
    char verbose;
    char color_output;
    
    char max_errors;
    char warnings_as_errors;
    char enable_warnings;
    
    char show_timings;
    char show_version;
    char show_help;
    
    char test_unit;
    char test_integration;
    char run_benchmark;
    
    char parallel_build;
    char num_threads;
} CompilerConfig;

#endif //_MAIN_H
