#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "main.h"
#define VERSION "0.1.0"

#include "../lexer/lexer.h"

static void print_version(void) {
    printf("Lazy Lang Compiler v%s\n", VERSION);
    printf("Copyright (c) 2026\n");
    printf("High-performance compiler with FASM backend\n");
}

static void print_help(void) {
    printf("Usage: lazy_lang [OPTIONS] <input_file>\n");
    printf("\n");
    printf("Basic Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -v, --version           Show version information\n");
    printf("  -o, --output <file>     Specify output file\n");
    printf("\n");
    printf("Optimization:\n");
    printf("  -O0                     No optimization (debug builds)\n");
    printf("  -O1                     Basic optimizations\n");
    printf("  -O2                     Standard optimizations\n");
    printf("  -O3                     Aggressive optimizations\n");
    printf("\n");
    printf("Feature Flags:\n");
    printf("  --simd                  Enable SIMD optimizations\n");
    printf("  --cuda                  Enable CUDA/GPU support\n");
    printf("  --debug                 Generate debug symbols\n");
    printf("  --verbose               Verbose output\n");
    printf("\n");
    printf("Output Control:\n");
    printf("  --emit-asm              Emit FASM assembly\n");
    printf("  --emit-obj              Emit object files\n");
    printf("  --emit-binary           Emit executable (default)\n");
    printf("\n");
    printf("Error Control:\n");
    printf("  -W, --warnings          Enable warnings\n");
    printf("  -E, --warnings-as-errors Treat warnings as errors\n");
    printf("  -M, --max-errors <n>    Maximum errors before abort\n");
    printf("\n");
    printf("Build Control:\n");
    printf("  -p, --parallel          Enable parallel builds\n");
    printf("  -j, --threads <n>       Number of threads to use\n");
    printf("  -t, --timings           Display build timings\n");
    printf("\n");
    printf("Testing:\n");
    printf("  --test-unit             Run unit tests\n");
    printf("  --test-integration      Run integration tests\n");
    printf("  --benchmark              Run performance benchmarks\n");
    printf("\n");
    printf("Examples:\n");
    printf("  lazy_lang source.lazy                 Compile to executable\n");
    printf("  lazy_lang -O3 source.lazy -o program  Optimized build\n");
    printf("  lazy_lang --simd source.lazy          Enable SIMD\n");
    printf("  lazy_lang --emit-asm source.lazy      Generate assembly only\n");
}

static void print_config(const CompilerConfig* config) {
    printf("\n");
    printf("Compiler Configuration:\n");
    printf("  Input file:     %s\n", config->input_file ? config->input_file : "(none)");
    printf("  Output file:    %s\n", config->output_file ? config->output_file : "(auto)");
    printf("  Output format:  %s\n", config->output_format ? config->output_format : "binary");
    printf("  Optimization:    -O%d\n", config->optimization_level);
    printf("  SIMD:           %s\n", config->enable_simd ? "enabled" : "disabled");
    printf("  CUDA:           %s\n", config->enable_cuda ? "enabled" : "disabled");
    printf("  Debug:          %s\n", config->enable_debug ? "enabled" : "disabled");
    printf("  Verbose:        %s\n", config->verbose ? "enabled" : "disabled");
    printf("  Max errors:     %d\n", config->max_errors);
    printf("\n");
}

static int compile(const CompilerConfig* config) {
    if (config->verbose) {
        printf("Compiling: %s\n", config->input_file);
    }

    if (config->test_unit) {
        printf("Running unit tests...\n");
        return 0;
    }

    if (config->test_integration) {
        printf("Running integration tests...\n");
        return 0;
    }

    if (config->run_benchmark) {
        printf("Running benchmarks...\n");
        return 0;
    }

    Lexer* lex = Lexer_init(config->input_file);

    Token tok = {0};
    while (tok.type != TokenKind_EOF) {
        tok = Lexer_next_token(lex);
        switch (tok.type) {
            case TokenKind_IDENTIFIER:
                printf("%s\n",tok.as.litreal_str_);
                String_distroy(tok.as.litreal_str_);
                break;
            case TokenKind_NUMBER:
                printf("%s\n",tok.as.litreal_str_);
                String_distroy(tok.as.litreal_str_);
                break;
            case TokenKind_STRING_LITERAL:
                printf("%s\n",tok.as.litreal_str_);
                String_distroy(tok.as.litreal_str_);
                break;
            case TokenKind_DOT:
                // printf(".");
                break;
            case TokenKind_PIPE:
                // printf(" | ");
                break;
            case TokenKind_EOF:
                printf("\n\n\nat the end\n");
                break;
            default:
                break;
        }
    }

    printf("lines: %d\n",lex->line);
    printf("not read: %lu\n",lex->file.left);
    printf("size: %lu\n",lex->file.size);

    Lexer_term(lex);

    printf("\n\033[1;33m⚠️  Lazy Lang Compiler - Work in Progress\033[0m\n");
    printf("\n");
    printf("Current Status:\n");
    printf("  ⏳ Phase 1: Foundation & Infrastructure - PENDING\n");
    printf("  ✅ Phase 2: Frontend - Lexer - COMPLETE\n");
    printf("  ⏳ Phase 3: Frontend - Parser - PENDING\n");
    printf("  ⏳ Phase 4: Middle-end - Type System - PENDING\n");
    printf("  ⏳ Phase 5: Middle-end - Semantic Analysis - PENDING\n");
    printf("  ⏳ Phase 6: Backend - FASM Code Generation - PENDING\n");
    printf("  ⏳ Phase 7: Optimization Passes - PENDING\n");
    printf("  ⏳ Phase 8: CUDA/GPU Integration - PENDING\n");
    printf("  ⏳ Phase 9: FFI & Inline Assembly - PENDING\n");
    printf("  ⏳ Phase 10: Standard Library - PENDING\n");
    printf("  ⏳ Phase 11: Build System & CLI - PENDING\n");
    printf("  ⏳ Phase 12: Testing & Benchmarking - PENDING\n");
    printf("\n");

    return 0;
}

int main (int argc, char* argv[]) {
    CompilerConfig config = {0};

    config.optimization_level = 0;
    config.enable_debug = 1;
    config.max_errors = 100;
    config.enable_warnings = 0;
    config.warnings_as_errors = 0;
    config.num_threads = 1;
    config.output_format = "binary";

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"output", required_argument, 0, 'o'},
        {"simd", no_argument, 0, 's'},
        {"cuda", no_argument, 0, 'c'},
        {"debug", no_argument, 0, 'd'},
        {"verbose", no_argument, 0, 1000},
        {"warnings", no_argument, 0, 'W'},
        {"warnings-as-errors", no_argument, 0, 'E'},
        {"max-errors", required_argument, 0, 'M'},
        {"timings", no_argument, 0, 't'},
        {"parallel", no_argument, 0, 'p'},
        {"threads", required_argument, 0, 'j'},
        {"test-unit", no_argument, 0, 2000},
        {"test-integration", no_argument, 0, 2001},
        {"benchmark", no_argument, 0, 2002},
        {"emit-asm", no_argument, 0, 3000},
        {"emit-obj", no_argument, 0, 3001},
        {"emit-binary", no_argument, 0, 3002},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hvo:O:WEtscdpj:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return 0;
            case 'v':
                print_version();
                return 0;
            case 'o':
                config.output_file = optarg;
                break;
            case 'O':
                config.optimization_level = atoi(optarg);
                if (config.optimization_level < 0 || config.optimization_level > 3) {
                    fprintf(stderr, "Invalid optimization level: %s\n", optarg);
                    return 1;
                }
                if (config.optimization_level > 0) {
                    config.enable_debug = 0;
                }
                break;
            case 'W':
                config.enable_warnings = 1;
                break;
            case 'E':
                config.warnings_as_errors = 1;
                break;
            case 'M':
                config.max_errors = atoi(optarg);
                break;
            case 't':
                config.show_timings = 1;
                break;
            case 's':
                config.enable_simd = 1;
                break;
            case 'c':
                config.enable_cuda = 1;
                break;
            case 'd':
                config.enable_debug = 1;
                break;
            case 'p':
                config.parallel_build = 1;
                break;
            case 'j':
                config.num_threads = atoi(optarg);
                break;
            case 1000:
                config.verbose = 1;
                break;
            case 2000:
                config.test_unit = 1;
                break;
            case 2001:
                config.test_integration = 1;
                break;
            case 2002:
                config.run_benchmark = 1;
                break;
            case 3000:
                config.output_format = "asm";
                break;
            case 3001:
                config.output_format = "obj";
                break;
            case 3002:
                config.output_format = "binary";
                break;
            default:
                fprintf(stderr, "Unknown option: %c\n", opt);
                print_help();
                return 1;
        }
    }

    if (optind < argc) {
        config.input_file = argv[optind];
    } else {
        if (!config.test_unit && !config.test_integration && !config.run_benchmark) {
            fprintf(stderr, "Error: No input file specified\n");
            print_help();
            return 1;
        }
    }

    if (config.verbose) {
        print_config(&config);
    }

    return compile(&config);
}
