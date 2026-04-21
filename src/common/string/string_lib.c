#include "string_lib.h"

#include "../types.h"

#include <string.h>
#include <limits.h>
#include <ctype.h>

const char *STRING_LIB_NOINIT = "NOINIT";

void *String_malloc(u64 size){
    return malloc(size);
}

void *String_realloc(void* ptr, u64 size){
    return realloc(ptr,size);
}

void String_free(void* ptr){
    free(ptr);
}

static inline i32 String_header_size(String_flags type) {
    switch(type&STRING_LIB_T_MASK) {
        case STRING_LIB_T_5:
            return sizeof(struct String_5_t);
        case STRING_LIB_T_8:
            return sizeof(struct String_8_t);
        case STRING_LIB_T_16:
            return sizeof(struct String_16_t);
        case STRING_LIB_T_32:
            return sizeof(struct String_32_t);
        case STRING_LIB_T_64:
            return sizeof(struct String_64_t);
    }
    return 0;
}

static inline char String_request_type(u64 string_size) {
    if (string_size < 1<<5)
        return STRING_LIB_T_5;
    if (string_size < 1<<8)
        return STRING_LIB_T_8;
    if (string_size < 1<<16)
        return STRING_LIB_T_16;
#if (LONG_MAX == LLONG_MAX)
    if (string_size < 1ll<<32)
        return STRING_LIB_T_32;
    return STRING_LIB_T_64;
#else
    return STRING_LIB_T_32;
#endif
}

inline u64 String_len(const String str) {
    String_flags flags = str[-1];
    switch(flags&STRING_LIB_T_MASK) {
        case STRING_LIB_T_5:
            return STRING_LIB_TYPE_5_LEN(flags);
        case STRING_LIB_T_8:
            return STRING_LIB_HDR(8,str)->len;
        case STRING_LIB_T_16:
            return STRING_LIB_HDR(16,str)->len;
        case STRING_LIB_T_32:
            return STRING_LIB_HDR(32,str)->len;
        case STRING_LIB_T_64:
            return STRING_LIB_HDR(64,str)->len;
    }
    return 0;
}

static inline u64 String_avail(const String str) {
    String_flags flags = str[-1];
    switch(flags&STRING_LIB_T_MASK) {
        case STRING_LIB_T_5: {
            return 0;
        }
        case STRING_LIB_T_8: {
            STRING_LIB_HDR_VAR(8,str);
            return sh->cap - sh->len;
        }
        case STRING_LIB_T_16: {
            STRING_LIB_HDR_VAR(16,str);
            return sh->cap - sh->len;
        }
        case STRING_LIB_T_32: {
            STRING_LIB_HDR_VAR(32,str);
            return sh->cap - sh->len;
        }
        case STRING_LIB_T_64: {
            STRING_LIB_HDR_VAR(64,str);
            return sh->cap - sh->len;
        }
    }
    return 0;
}

static inline void String_set_len(String s, u64 newlen) {
    String_flags flags = s[-1];
    switch(flags&STRING_LIB_T_MASK) {
        case STRING_LIB_T_5:
            {
                unsigned char *fp = ((unsigned char*)s)-1;
                *fp = STRING_LIB_T_5 | (newlen << STRING_LIB_T_BITS);
            }
            break;
        case STRING_LIB_T_8:
            STRING_LIB_HDR(8,s)->len = newlen;
            break;
        case STRING_LIB_T_16:
            STRING_LIB_HDR(16,s)->len = newlen;
            break;
        case STRING_LIB_T_32:
            STRING_LIB_HDR(32,s)->len = newlen;
            break;
        case STRING_LIB_T_64:
            STRING_LIB_HDR(64,s)->len = newlen;
            break;
    }
}

static inline void String_inc_len(String s, u64 inc) {
    String_flags flags = s[-1];
    switch(flags&STRING_LIB_T_MASK) {
        case STRING_LIB_T_5:
            {
                unsigned char *fp = ((unsigned char*)s)-1;
                unsigned char newlen = STRING_LIB_TYPE_5_LEN(flags)+inc;
                *fp = STRING_LIB_T_5 | (newlen << STRING_LIB_T_BITS);
            }
            break;
        case STRING_LIB_T_8:
            STRING_LIB_HDR(8,s)->len += inc;
            break;
        case STRING_LIB_T_16:
            STRING_LIB_HDR(16,s)->len += inc;
            break;
        case STRING_LIB_T_32:
            STRING_LIB_HDR(32,s)->len += inc;
            break;
        case STRING_LIB_T_64:
            STRING_LIB_HDR(64,s)->len += inc;
            break;
    }
}


static inline u64 String_cap(const String str) {
    String_flags flags = str[-1];
    switch(flags&STRING_LIB_T_MASK) {
        case STRING_LIB_T_5:
            return STRING_LIB_TYPE_5_LEN(flags);
        case STRING_LIB_T_8:
            return STRING_LIB_HDR(8,str)->cap;
        case STRING_LIB_T_16:
            return STRING_LIB_HDR(16,str)->cap;
        case STRING_LIB_T_32:
            return STRING_LIB_HDR(32,str)->cap;
        case STRING_LIB_T_64:
            return STRING_LIB_HDR(64,str)->cap;
    }
    return 0;
}


static inline void String_set_cap(String str, u64 newlen) {
    unsigned char flags = str[-1];
    switch(flags&STRING_LIB_T_MASK) {
        case STRING_LIB_T_5:
            /* Nothing to do, this type has no total allocation info. */
            break;
        case STRING_LIB_T_8:
            STRING_LIB_HDR(8,str)->cap = newlen;
            break;
        case STRING_LIB_T_16:
            STRING_LIB_HDR(16,str)->cap = newlen;
            break;
        case STRING_LIB_T_32:
            STRING_LIB_HDR(32,str)->cap = newlen;
            break;
        case STRING_LIB_T_64:
            STRING_LIB_HDR(64,str)->cap = newlen;
            break;
    }
}

String String_create_len(const void *init, u64 initlen) {
    void *sh;
    String str;
    char type = String_request_type(initlen);
    /* Empty strings are usually created in order to append. Use type 8
     * since type 5 is not good at this. */
    if (type == STRING_LIB_T_5 && initlen == 0) type = STRING_LIB_T_8;
    int hdrlen = String_header_size(type);
    String_flags *fp; /* flags pointer. */

    sh = String_malloc(hdrlen+initlen+1);
    if (sh == NULL) return NULL;
    if (init==STRING_LIB_NOINIT)
        init = NULL;
    else if (!init)
        memset(sh, 0, hdrlen+initlen+1);
    str = (char*)sh+hdrlen;
    fp = ((String_flags*)str)-1;
    switch(type) {
        case STRING_LIB_T_5: {
            *fp = type | (initlen << STRING_LIB_T_BITS);
            break;
        }
        case STRING_LIB_T_8: {
            STRING_LIB_HDR_VAR(8,str);
            sh->len = initlen;
            sh->cap = initlen;
            *fp = type;
            break;
        }
        case STRING_LIB_T_16: {
            STRING_LIB_HDR_VAR(16,str);
            sh->len = initlen;
            sh->cap = initlen;
            *fp = type;
            break;
        }
        case STRING_LIB_T_32: {
            STRING_LIB_HDR_VAR(32,str);
            sh->len = initlen;
            sh->cap = initlen;
            *fp = type;
            break;
        }
        case STRING_LIB_T_64: {
            STRING_LIB_HDR_VAR(64,str);
            sh->len = initlen;
            sh->cap = initlen;
            *fp = type;
            break;
        }
    }
    if (initlen && init)
        memcpy(str, init, initlen);
    str[initlen] = '\0';
    return str;
}

String String_create(const char *init) {
    u64 initlen = (init == NULL) ? 0 : strlen(init);
    return String_create_len(init, initlen);
}

String String_dup(const String s) {
    return String_create_len(s, String_len(s));
}

String String_create_empty(void) {
    return String_create_len("",0);
}

void   String_distroy(String str) {
    if (!str) return;
    String_free((void*)str-String_header_size(str[-1])); 
}

/* Set the String string length to the length as obtained with strlen(), so
 * considering as content only up to the first null term character.
 *
 * This function is useful when the String string is hacked manually in some
 * way, like in the following example:
 *
 * s = sdsnew("foobar");
 * s[2] = '\0';
 * sdsupdatelen(s);
 * printf("%d\n", String_len(s));
 *
 * The output will be "2", but if we comment out the call to sdsupdatelen()
 * the output will be "6" as the string was modified but the logical length
 * remains 6 bytes. */
void String_update_len(String s) {
    u64 reallen = strlen(s);
    String_set_len(s, reallen);
}

/* Modify an String string in-place to make it empty (zero length).
 * However all the existing buffer is not discarded but set as free space
 * so that next append operations will not require allocations up to the
 * number of bytes previously available. */
void String_clear(String s) {
    String_set_len(s, 0);
    s[0] = '\0';
}

/* Enlarge the free space at the end of the String string so that the caller
 * is sure that after calling this function can overwrite up to addlen
 * bytes after the end of the string, plus one more byte for nul term.
 *
 * Note: this does not change the *length* of the String string as returned
 * by String_len(), but only the free buffer space we have. */
String String_MakeRoomFor(String s, u64 addlen) {
    void *sh, *newsh;
    u64 avail = String_avail(s);
    u64 len, newlen, reqlen;
    char type, oldtype = s[-1] & STRING_LIB_T_MASK;
    int hdrlen;

    /* Return ASAP if there is enough space left. */
    if (avail >= addlen) return s;

    len = String_len(s);
    sh = (char*)s-String_header_size(oldtype);
    reqlen = newlen = (len+addlen);
    if (newlen < STRING_LIB_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += STRING_LIB_MAX_PREALLOC;

    type = String_request_type(newlen);

    /* Don't use type 5: the user is appending to the string and type 5 is
     * not able to remember empty space, so sdsMakeRoomFor() must be called
     * at every appending operation. */
    if (type == STRING_LIB_T_5) type = STRING_LIB_T_8;

    hdrlen = String_header_size(type);
    assert(hdrlen + newlen + 1 > reqlen); /* Catch u64 overflow */
    if (oldtype==type) {
        newsh = String_realloc(sh, hdrlen+newlen+1);
        if (newsh == NULL) return NULL;
        s = (char*)newsh+hdrlen;
    } else {
        /* Since the header size changes, need to move the string forward,
         * and can't use realloc */
        newsh = String_malloc(hdrlen+newlen+1);
        if (newsh == NULL) return NULL;
        memcpy((char*)newsh+hdrlen, s, len+1);
        String_free(sh);
        s = (char*)newsh+hdrlen;
        s[-1] = type;
        String_set_len(s, len);
    }
    String_set_cap(s, newlen);
    return s;
}

/* Reallocate the String string so that it has no free space at the end. The
 * contained string remains not altered, but next concatenation operations
 * will require a reallocation.
 *
 * After the call, the passed String string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
String String_RemoveFreeSpace(String s) {
    void *sh, *newsh;
    char type, oldtype = s[-1] & STRING_LIB_T_MASK;
    int hdrlen, oldhdrlen = String_header_size(oldtype);
    u64 len = String_len(s);
    u64 avail = String_avail(s);
    sh = (char*)s-oldhdrlen;

    /* Return ASAP if there is no space left. */
    if (avail == 0) return s;

    /* Check what would be the minimum SDS header that is just good enough to
     * fit this string. */
    type = String_request_type(len);
    hdrlen = String_header_size(type);

    /* If the type is the same, or at least a large enough type is still
     * required, we just realloc(), letting the allocator to do the copy
     * only if really needed. Otherwise if the change is huge, we manually
     * reallocate the string to use the different header type. */
    if (oldtype==type || type > STRING_LIB_T_8) {
        newsh = String_realloc(sh, oldhdrlen+len+1);
        if (newsh == NULL) return NULL;
        s = (char*)newsh+oldhdrlen;
    } else {
        newsh = String_malloc(hdrlen+len+1);
        if (newsh == NULL) return NULL;
        memcpy((char*)newsh+hdrlen, s, len+1);
        String_free(sh);
        s = (char*)newsh+hdrlen;
        s[-1] = type;
        String_set_len(s, len);
    }
    String_set_cap(s, len);
    return s;
}

/* Return the total size of the allocation of the specified String string,
 * including:
 * 1) The String header before the pointer.
 * 2) The string.
 * 3) The free buffer at the end if any.
 * 4) The implicit null term.
 */
u64 String_AllocSize(String s) {
    u64 cap = String_cap(s);
    return String_header_size(s[-1])+cap+1;
}

/* Return the pointer of the actual SDS allocation (normally SDS strings
 * are referenced by the start of the string buffer). */
void *String_AllocPtr(String s) {
    return (void*) (s-String_header_size(s[-1]));
}

/* Increment the String length and decrements the left free space at the
 * end of the string according to 'incr'. Also set the null term
 * in the new end of the string.
 *
 * This function is used in order to fix the string length after the
 * user calls sdsMakeRoomFor(), writes something after the end of
 * the current string, and finally needs to set the new length.
 *
 * Note: it is possible to use a negative increment in order to
 * right-trim the string.
 *
 * Usage example:
 *
 * Using sdsIncrLen() and sdsMakeRoomFor() it is possible to mount the
 * following schema, to cat bytes coming from the kernel to the end of an
 * String string without copying into an intermediate buffer:
 *
 * oldlen = String_len(s);
 * s = sdsMakeRoomFor(s, BUFFER_SIZE);
 * nread = read(fd, s+oldlen, BUFFER_SIZE);
 * ... check for nread <= 0 and handle it ...
 * sdsIncrLen(s, nread);
 */
void String_Incr_Len(String s, u64 incr) {
    unsigned char flags = s[-1];
    u64 len;
    switch(flags&STRING_LIB_T_MASK) {
        case STRING_LIB_T_5: {
            unsigned char *fp = ((unsigned char*)s)-1;
            unsigned char oldlen = STRING_LIB_TYPE_5_LEN(flags);
            assert((incr > 0 && oldlen+incr < 32) || (incr < 0 && oldlen >= (u32)(-incr)));
            *fp = STRING_LIB_T_5 | ((oldlen+incr) << STRING_LIB_T_BITS);
            len = oldlen+incr;
            break;
        }
        case STRING_LIB_T_8: {
            STRING_LIB_HDR_VAR(8,s);
            assert((incr >= 0 && sh->cap-sh->len >= incr) || (incr < 0 && sh->len >= (u32)(-incr)));
            len = (sh->len += incr);
            break;
        }
        case STRING_LIB_T_16: {
            STRING_LIB_HDR_VAR(16,s);
            assert((incr >= 0 && sh->cap-sh->len >= incr) || (incr < 0 && sh->len >= (u32)(-incr)));
            len = (sh->len += incr);
            break;
        }
        case STRING_LIB_T_32: {
            STRING_LIB_HDR_VAR(32,s);
            assert((incr >= 0 && sh->cap-sh->len >= (u32)incr) || (incr < 0 && sh->len >= (u32)(-incr)));
            len = (sh->len += incr);
            break;
        }
        case STRING_LIB_T_64: {
            STRING_LIB_HDR_VAR(64,s);
            assert((incr >= 0 && sh->cap-sh->len >= (uint64_t)incr) || (incr < 0 && sh->len >= (uint64_t)(-incr)));
            len = (sh->len += incr);
            break;
        }
        default: len = 0; /* Just to avoid compilation warnings. */
    }
    s[len] = '\0';
}

/* Grow the String to have the specified length. Bytes that were not part of
 * the original length of the String will be set to zero.
 *
 * if the specified length is smaller than the current length, no operation
 * is performed. */
String String_growzero(String s, u64 len) {
    u64 curlen = String_len(s);

    if (len <= curlen) return s;
    s = String_MakeRoomFor(s,len-curlen);
    if (s == NULL) return NULL;

    /* Make sure added region doesn't contain garbage */
    memset(s+curlen,0,(len-curlen+1)); /* also set trailing \0 byte */
    String_set_len(s, len);
    return s;
}

/* Append the specified binary-safe string pointed by 't' of 'len' bytes to the
 * end of the specified String string 's'.
 *
 * After the call, the passed String string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
String String_cat_len(String s, const void *t, u64 len) {
    u64 curlen = String_len(s);

    s = String_MakeRoomFor(s,len);
    if (s == NULL) return NULL;
    memcpy(s+curlen, t, len);
    String_set_len(s, curlen+len);
    s[curlen+len] = '\0';
    return s;
}

/* Append the specified null termianted C string to the String string 's'.
 *
 * After the call, the passed String string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
String String_cat(String s, const char *t) {
    return String_cat_len(s, t, strlen(t));
}

/* Append the specified String 't' to the existing String 's'.
 *
 * After the call, the modified String string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
String String_cat_String(String s, const String t) {
    return String_cat_len(s, t, String_len(t));
}

/* Destructively modify the String string 's' to hold the specified binary
 * safe string pointed by 't' of length 'len' bytes. */
String String_cpy_len(String s, const char *t, u64 len) {
    if (String_cap(s) < len) {
        s = String_MakeRoomFor(s,len-String_len(s));
        if (s == NULL) return NULL;
    }
    memcpy(s, t, len);
    s[len] = '\0';
    String_set_len(s, len);
    return s;
}

/* Like String_cpy_len() but 't' must be a null-terminated string so that the length
 * of the string is obtained with strlen(). */
String String_cpy(String s, const char *t) {
    return String_cpy_len(s, t, strlen(t));
}

/* Helper for sdscatlonglong() doing the actual number -> string
 * conversion. 's' must point to a string with room for at least
 * SDS_LLSTR_SIZE bytes.
 *
 * The function returns the length of the null-terminated string
 * representation stored at 's'. */
#define SDS_LLSTR_SIZE 21
int String_ll2str(char *s, long long value) {
    char *p, aux;
    unsigned long long v;
    u64 l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    if (value < 0) {
        /* Since v is unsigned, if value==LLONG_MIN then
         * -LLONG_MIN will overflow. */
        if (value != LLONG_MIN) {
            v = -value;
        } else {
            v = ((unsigned long long)LLONG_MAX) + 1;
        }
    } else {
        v = value;
    }

    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);
    if (value < 0) *p++ = '-';

    /* Compute length and add null term. */
    l = p-s;
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}

/* Identical sdsll2str(), but for unsigned long long type. */
int String_ull2str(char *s, unsigned long long v) {
    char *p, aux;
    u64 l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);

    /* Compute length and add null term. */
    l = p-s;
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}

/* Create an String string from a long long value. It is much faster than:
 *
 * String_cat_printf(sdsempty(),"%lld\n", value);
 */
String String_from_long_long(long long value) {
    char buf[SDS_LLSTR_SIZE];
    int len = String_ll2str(buf,value);

    return String_create_len(buf,len);
}

/* Like String_cat_printf() but gets va_list instead of being variadic. */
String String_cat_vprintf(String s, const char *fmt, va_list ap) {
    va_list cpy;
    char staticbuf[1024], *buf = staticbuf, *t;
    u64 buflen = strlen(fmt)*2;
    int bufstrlen;

    /* We try to start using a static buffer for speed.
     * If not possible we revert to heap allocation. */
    if (buflen > sizeof(staticbuf)) {
        buf = String_malloc(buflen);
        if (buf == NULL) return NULL;
    } else {
        buflen = sizeof(staticbuf);
    }

    /* Alloc enough space for buffer and \0 after failing to
     * fit the string in the current buffer size. */
    while(1) {
        va_copy(cpy,ap);
        bufstrlen = vsnprintf(buf, buflen, fmt, cpy);
        va_end(cpy);
        if (bufstrlen < 0) {
            if (buf != staticbuf) String_free(buf);
            return NULL;
        }
        if (((u64)bufstrlen) >= buflen) {
            if (buf != staticbuf) String_free(buf);
            buflen = ((u64)bufstrlen) + 1;
            buf = String_malloc(buflen);
            if (buf == NULL) return NULL;
            continue;
        }
        break;
    }

    /* Finally concat the obtained string to the SDS string and return it. */
    t = String_cat_len(s, buf, bufstrlen);
    if (buf != staticbuf) String_free(buf);
    return t;
}

/* Append to the String string 's' a string obtained using printf-alike format
 * specifier.
 *
 * After the call, the modified String string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = sdsnew("Sum is: ");
 * s = String_cat_printf(s,"%d+%d = %d",a,b,a+b).
 *
 * Often you need to create a string from scratch with the printf-alike
 * format. When this is the need, just use sdsempty() as the target string:
 *
 * s = String_cat_printf(sdsempty(), "... your format ...", args);
 */
String String_cat_printf(String s, const char *fmt, ...) {
    va_list ap;
    char *t;
    va_start(ap, fmt);
    t = String_cat_vprintf(s,fmt,ap);
    va_end(ap);
    return t;
}

/* This function is similar to String_cat_printf, but much faster as it does
 * not rely on sprintf() family functions implemented by the libc that
 * are often very slow. Moreover directly handling the String string as
 * new data is concatenated provides a performance improvement.
 *
 * However this function only handles an incompatible subset of printf-alike
 * format specifiers:
 *
 * %s - C String
 * %S - SDS string
 * %i - signed int
 * %I - 64 bit signed integer (long long, int64_t)
 * %u - unsigned int
 * %U - 64 bit unsigned integer (unsigned long long, uint64_t)
 * %% - Verbatim "%" character.
 */
String String_cat_fmt(String s, char const *fmt, ...) {
    u64 initlen = String_len(s);
    const char *f = fmt;
    long i;
    va_list ap;

    /* To avoid continuous reallocations, let's start with a buffer that
     * can hold at least two times the format string itself. It's not the
     * best heuristic but seems to work in practice. */
    s = String_MakeRoomFor(s, initlen + strlen(fmt)*2);
    va_start(ap,fmt);
    f = fmt;    /* Next format specifier byte to process. */
    i = initlen; /* Position of the next byte to write to dest str. */
    while(*f) {
        char next, *str;
        u64 l;
        long long num;
        unsigned long long unum;

        /* Make sure there is always space for at least 1 char. */
        if (String_avail(s)==0) {
            s = String_MakeRoomFor(s,1);
        }

        switch(*f) {
        case '%':
            next = *(f+1);
            if (next == '\0') break;
            f++;
            switch(next) {
            case 's':
            case 'S':
                str = va_arg(ap,char*);
                l = (next == 's') ? strlen(str) : String_len(str);
                if (String_avail(s) < l) {
                    s = String_MakeRoomFor(s,l);
                }
                memcpy(s+i,str,l);
                String_inc_len(s,l);
                i += l;
                break;
            case 'i':
            case 'I':
                if (next == 'i')
                    num = va_arg(ap,int);
                else
                    num = va_arg(ap,long long);
                {
                    char buf[SDS_LLSTR_SIZE];
                    l = String_ll2str(buf,num);
                    if (String_avail(s) < l) {
                        s = String_MakeRoomFor(s,l);
                    }
                    memcpy(s+i,buf,l);
                    String_inc_len(s,l);
                    i += l;
                }
                break;
            case 'u':
            case 'U':
                if (next == 'u')
                    unum = va_arg(ap,u32);
                else
                    unum = va_arg(ap,unsigned long long);
                {
                    char buf[SDS_LLSTR_SIZE];
                    l = String_ull2str(buf,unum);
                    if (String_avail(s) < l) {
                        s = String_MakeRoomFor(s,l);
                    }
                    memcpy(s+i,buf,l);
                    String_inc_len(s,l);
                    i += l;
                }
                break;
            default: /* Handle %% and generally %<unknown>. */
                s[i++] = next;
                String_inc_len(s,1);
                break;
            }
            break;
        default:
            s[i++] = *f;
            String_inc_len(s,1);
            break;
        }
        f++;
    }
    va_end(ap);

    /* Add null-term */
    s[i] = '\0';
    return s;
}

/* Remove the part of the string from left and from right composed just of
 * contiguous characters found in 'cset', that is a null terminated C string.
 *
 * After the call, the modified String string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = sdsnew("AA...AA.a.aa.aHelloWorld     :::");
 * s = sdstrim(s,"Aa. :");
 * printf("%s\n", s);
 *
 * Output will be just "HelloWorld".
 */
String String_trim(String s, const char *cset) {
    char *end, *sp, *ep;
    u64 len;

    sp = s;
    ep = end = s+String_len(s)-1;
    while(sp <= end && strchr(cset, *sp)) sp++;
    while(ep > sp && strchr(cset, *ep)) ep--;
    len = (ep-sp)+1;
    if (s != sp) memmove(s, sp, len);
    s[len] = '\0';
    String_set_len(s,len);
    return s;
}

/* Turn the string into a smaller (or equal) string containing only the
 * substring specified by the 'start' and 'end' indexes.
 *
 * start and end can be negative, where -1 means the last character of the
 * string, -2 the penultimate character, and so forth.
 *
 * The interval is inclusive, so the start and end characters will be part
 * of the resulting string.
 *
 * The string is modified in-place.
 *
 * Example:
 *
 * s = sdsnew("Hello World");
 * sdsrange(s,1,-1); => "ello World"
 */
void String_range(String s, i64 start, i64 end) {
    u64 newlen, len = String_len(s);

    if (len == 0) return;
    if (start < 0) {
        start = len+start;
        if (start < 0) start = 0;
    }
    if (end < 0) {
        end = len+end;
        if (end < 0) end = 0;
    }
    newlen = (start > end) ? 0 : (end-start)+1;
    if (newlen != 0) {
        if (start >= (u64)len) {
            newlen = 0;
        } else if (end >= (u64)len) {
            end = len-1;
            newlen = (end-start)+1;
        }
    }

    if (start && newlen) memmove(s, s+start, newlen);
    s[newlen] = 0;
    String_set_len(s,newlen);
}

/* Apply tolower() to every character of the String string 's'. */
void String_tolower(String s) {
    u64 len = String_len(s), j;

    for (j = 0; j < len; j++) s[j] = tolower(s[j]);
}

/* Apply toupper() to every character of the String string 's'. */
void String_toupper(String s) {
    u64 len = String_len(s), j;

    for (j = 0; j < len; j++) s[j] = toupper(s[j]);
}

/* Compare two String strings s1 and s2 with memcmp().
 *
 * Return value:
 *
 *     positive if s1 > s2.
 *     negative if s1 < s2.
 *     0 if s1 and s2 are exactly the same binary string.
 *
 * If two strings share exactly the same prefix, but one of the two has
 * additional characters, the longer string is considered to be greater than
 * the smaller one. */
int String_cmp(const String s1, const String s2) {
    u64 l1, l2, minlen;
    int cmp;

    l1 = String_len(s1);
    l2 = String_len(s2);
    minlen = (l1 < l2) ? l1 : l2;
    cmp = memcmp(s1,s2,minlen);
    if (cmp == 0) return l1>l2? 1: (l1<l2? -1: 0);
    return cmp;
}

/* Split 's' with separator in 'sep'. An array
 * of String strings is returned. *count will be set
 * by reference to the number of tokens returned.
 *
 * On out of memory, zero length string, zero length
 * separator, NULL is returned.
 *
 * Note that 'sep' is able to split a string using
 * a multi-character separator. For example
 * sdssplit("foo_-_bar","_-_"); will return two
 * elements "foo" and "bar".
 *
 * This version of the function is binary-safe but
 * requires length arguments. sdssplit() is just the
 * same function but for zero-terminated strings.
 */

String *String_split_len(const char *s, i64 len, const char *sep, i32 seplen, i32 *count) {
    int elements = 0, slots = 5;
    long start = 0, j;
    String *tokens;

    if (seplen < 1 || len <= 0) {
        *count = 0;
        return NULL;
    }

    tokens = String_malloc(sizeof(String)*slots);
    if (tokens == NULL) return NULL;

    for (j = 0; j < (len-(seplen-1)); j++) {
        /* make sure there is room for the next element and the final one */
        if (slots < elements+2) {
            String *newtokens;

            slots *= 2;
            newtokens = String_realloc(tokens,sizeof(String)*slots);
            if (newtokens == NULL) goto cleanup;
            tokens = newtokens;
        }
        /* search the separator */
        if ((seplen == 1 && *(s+j) == sep[0]) || (memcmp(s+j,sep,seplen) == 0)) {
            tokens[elements] = String_create_len(s+start,j-start);
            if (tokens[elements] == NULL) goto cleanup;
            elements++;
            start = j+seplen;
            j = j+seplen-1; /* skip the separator */
        }
    }
    /* Add the final element. We are sure there is room in the tokens array. */
    tokens[elements] = String_create_len(s+start,len-start);
    if (tokens[elements] == NULL) goto cleanup;
    elements++;
    *count = elements;
    return tokens;

cleanup:
    {
        int i;
        for (i = 0; i < elements; i++) String_distroy(tokens[i]);
        String_free(tokens);
        *count = 0;
        return NULL;
    }
}

/* Free the result returned by sdssplitlen(), or do nothing if 'tokens' is NULL. */
void String_splitres_distroy(String *tokens, int count) {
    if (!tokens) return;
    while(count--)
        String_distroy(tokens[count]);
    String_free(tokens);
}

/* Append to the String string "s" an escaped string representation where
 * all the non-printable characters (tested with isprint()) are turned into
 * escapes in the form "\n\r\a...." or "\x<hex-number>".
 *
 * After the call, the modified String string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
String String_cat_repr(String s, const char *p, u64 len) {
    s = String_cat_len(s,"\"",1);
    while(len--) {
        switch(*p) {
        case '\\':
        case '"':
            s = String_cat_printf(s,"\\%c",*p);
            break;
        case '\n': s = String_cat_len(s,"\\n",2); break;
        case '\r': s = String_cat_len(s,"\\r",2); break;
        case '\t': s = String_cat_len(s,"\\t",2); break;
        case '\a': s = String_cat_len(s,"\\a",2); break;
        case '\b': s = String_cat_len(s,"\\b",2); break;
        default:
            if (isprint(*p))
                s = String_cat_printf(s,"%c",*p);
            else
                s = String_cat_printf(s,"\\x%02x",(unsigned char)*p);
            break;
        }
        p++;
    }
    return String_cat_len(s,"\"",1);
}

/* Helper function for sdssplitargs() that returns non zero if 'c'
 * is a valid hex digit. */
int is_hex_digit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

/* Helper function for sdssplitargs() that converts a hex digit into an
 * integer from 0 to 15 */
int hex_digit_to_int(char c) {
    switch(c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
    default: return 0;
    }
}

/* Split a line into arguments, where every argument can be in the
 * following programming-language REPL-alike form:
 *
 * foo bar "newline are supported\n" and "\xff\x00otherstuff"
 *
 * The number of arguments is stored into *argc, and an array
 * of String is returned.
 *
 * The caller should free the resulting array of String strings with
 * String_distroysplitres().
 *
 * Note that sdscatrepr() is able to convert back a string into
 * a quoted string in the same format sdssplitargs() is able to parse.
 *
 * The function returns the allocated tokens on success, even when the
 * input string is empty, or NULL if the input contains unbalanced
 * quotes or closed quotes followed by non space characters
 * as in: "foo"bar or "foo'
 */
String *String_split_args(const char *line, int *argc) {
    const char *p = line;
    char *current = NULL;
    char **vector = NULL;

    *argc = 0;
    while(1) {
        /* skip blanks */
        while(*p && isspace(*p)) p++;
        if (*p) {
            /* get a token */
            int inq=0;  /* set to 1 if we are in "quotes" */
            int insq=0; /* set to 1 if we are in 'single quotes' */
            int done=0;

            if (current == NULL) current = String_create_empty();
            while(!done) {
                if (inq) {
                    if (*p == '\\' && *(p+1) == 'x' &&
                                             is_hex_digit(*(p+2)) &&
                                             is_hex_digit(*(p+3)))
                    {
                        unsigned char byte;

                        byte = (hex_digit_to_int(*(p+2))*16)+
                                hex_digit_to_int(*(p+3));
                        current = String_cat_len(current,(char*)&byte,1);
                        p += 3;
                    } else if (*p == '\\' && *(p+1)) {
                        char c;

                        p++;
                        switch(*p) {
                        case 'n': c = '\n'; break;
                        case 'r': c = '\r'; break;
                        case 't': c = '\t'; break;
                        case 'b': c = '\b'; break;
                        case 'a': c = '\a'; break;
                        default: c = *p; break;
                        }
                        current = String_cat_len(current,&c,1);
                    } else if (*p == '"') {
                        /* closing quote must be followed by a space or
                         * nothing at all. */
                        if (*(p+1) && !isspace(*(p+1))) goto err;
                        done=1;
                    } else if (!*p) {
                        /* unterminated quotes */
                        goto err;
                    } else {
                        current = String_cat_len(current,p,1);
                    }
                } else if (insq) {
                    if (*p == '\\' && *(p+1) == '\'') {
                        p++;
                        current = String_cat_len(current,"'",1);
                    } else if (*p == '\'') {
                        /* closing quote must be followed by a space or
                         * nothing at all. */
                        if (*(p+1) && !isspace(*(p+1))) goto err;
                        done=1;
                    } else if (!*p) {
                        /* unterminated quotes */
                        goto err;
                    } else {
                        current = String_cat_len(current,p,1);
                    }
                } else {
                    switch(*p) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                    case '\0':
                        done=1;
                        break;
                    case '"':
                        inq=1;
                        break;
                    case '\'':
                        insq=1;
                        break;
                    default:
                        current = String_cat_len(current,p,1);
                        break;
                    }
                }
                if (*p) p++;
            }
            /* add the token to the vector */
            vector = String_realloc(vector,((*argc)+1)*sizeof(char*));
            vector[*argc] = current;
            (*argc)++;
            current = NULL;
        } else {
            /* Even on empty input string return something not NULL. */
            if (vector == NULL) vector = String_malloc(sizeof(void*));
            return vector;
        }
    }

err:
    while((*argc)--)
        String_distroy(vector[*argc]);
    String_free(vector);
    if (current) String_distroy(current);
    *argc = 0;
    return NULL;
}

/* Modify the string substituting all the occurrences of the set of
 * characters specified in the 'from' string to the corresponding character
 * in the 'to' array.
 *
 * For instance: sdsmapchars(mystring, "ho", "01", 2)
 * will have the effect of turning the string "hello" into "0ell1".
 *
 * The function returns the String string pointer, that is always the same
 * as the input pointer since no resize is needed. */
String String_map_chars(String s, const char *from, const char *to, u64 setlen) {
    u64 j, i, l = String_len(s);

    for (j = 0; j < l; j++) {
        for (i = 0; i < setlen; i++) {
            if (s[j] == from[i]) {
                s[j] = to[i];
                break;
            }
        }
    }
    return s;
}

/* Join an array of C strings using the specified separator (also a C string).
 * Returns the result as an String string. */
String String_join(char **argv, int argc, char *sep) {
    String join = String_create_empty();
    int j;

    for (j = 0; j < argc; j++) {
        join = String_cat(join, argv[j]);
        if (j != argc-1) join = String_cat(join,sep);
    }
    return join;
}

/* Like String_join, but joins an array of SDS strings. */
String String_join_String(String *argv, int argc, const char *sep, u64 seplen) {
    String join = String_create_empty();
    int j;

    for (j = 0; j < argc; j++) {
        join = String_cat_String(join, argv[j]);
        if (j != argc-1) join = String_cat_len(join,sep,seplen);
    }
    return join;
}
