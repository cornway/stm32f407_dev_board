#ifndef MAIL_TYPES_H
#define MAIL_TYPES_H

typedef _PACKED struct {
    char *message;
    void *object;
    WORD_T type;
} MAIL_HANDLE;

#endif /*MAIL_TYPES_H*/
