#include "./logger.h"

#include <__stdarg_va_list.h>
#include <stdarg.h>

#include <stdio.h>

#define LOG_SIZE 1024

void log_output(LogSeverity sev,const char* message, ...){
    static const char* sev_str[6]  = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN]: ",
        "[INFO]: ",
        "[DBUG]: ",
        "[TRACE]: "
    };

    // b8 is_error = sev < 2;

    va_list args_ptr;
    va_start(args_ptr, message);

    char buf[LOG_SIZE] = {0};
    vsnprintf(buf, LOG_SIZE, message, args_ptr);

    va_end(args_ptr);

    char out[LOG_SIZE] = {0};
    sprintf(out, "%s%s\n", sev_str[sev], buf);

    printf("%s", out);
}
