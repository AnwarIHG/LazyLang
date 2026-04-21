#pragma once

#ifndef _KEYWORDS_H
#define _KEYWORDS_H

typedef enum  {
    keywords_LET = 0,
    keywords_EXIT = 1,
    keywords_RETURN = 2,
} keywords_inx;

char *keywords[] = {
    "let",
    "exit",
    "return",
};

#endif //_KEYWORDS_H
