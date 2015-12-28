/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ezserial.h - _explain_
 *
 * Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ezserial.h 0001 2012-4-19 10:45:01Z wu_junjie $
 *
 *  Explain:
 *     -serial comm , linux only-
 *
 *     usage - Event driven mode:
 *     ---------------
 *     	ezserial_init
 *     		ezserial_open
 *     			ezserial_putdata
 *     			ezserial_putstring
 *     		ezserial_close
 *     	ezserial_cleanup
 *
 *  Update:
 *     2012-4-19 10:45:01 WuJunjie 549 Create
 * 
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _EZSERIAL_H
#define _EZSERIAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <signal.h>
#include <termios.h>

typedef void (*ezserial_callback_t)(int id,          /* id passed to callback */
                                    uint8_t *buffer, /* data received */
                                    int len);        /* length of data in bytes */

typedef struct __ezserial_t
{
    uint8_t buffer[255];          /* input buffer */
    int fd;                       /* tty file descriptor */
    struct termios tio;           /* termios structure for the port */
    struct termios oldtio;        /* old termios structure */
    ezserial_callback_t callback; /* callback function */
    int id;                       /* id which would be passed to callback */

    struct __ezserial_t *next;
}
ezserial_t;

typedef enum {
    EZSERIAL_OK,                 /* everything is all right */
    EZSERIAL_ERROR_NOSIGNAL,     /* there's no free signal */
    EZSERIAL_ERROR_NOTSTARTED,   /* you should first start ezserial */
    EZSERIAL_ERROR_NULLPOINTER,  /* you gave a null pointer to the function */
    EZSERIAL_ERROR_OOPS,         /* internal error, something's erong */
    EZSERIAL_ERROR_MEMORY,       /* there's no memory for ezserial_t structure */
    EZSERIAL_ERROR_OPEN          /* file doesnt exist or you aren't good user */
} ezserial_error_t;

// add user interface structure
#define COM_NAME_LENGTH		64
    typedef struct __com_info_t
    {
        int id;                                                       // id of com record in db
        char com_name[COM_NAME_LENGTH];
        int baud;
        int data_bits;
        int parity;
        int stop_bits;                                                // 1, 15, 2
    }
    com_info_t;

/* get the error message */
const char *ezserial_geterrormsg();

/* get the error code */
int ezserial_geterror();

/* start the ezserial */
void ezserial_init();

/* finish all jobs, clear memory, etc. */
void ezserial_cleanup();

/* alloc new ezserial_t struct and open the port */
ezserial_t *ezserial_open(const char *fname,            /* pathname of port file for example "/dev/ttyS0" */
                          ezserial_callback_t callback, /* callback function*/
                          int id,     /* your own id for the port, it can help to identify the port in callback f.*/
                          int baud,   /* baudrate, integer:75 110 150 300 600 1200 2400 4800 9600 19200 38400 57600 115200 */
                          int bits,   /* data bits: 7 or 8 */
                          int parity, /* parity: 0 - none, 1-odd, 2-even */
                          int stop);  /* stop bits: 1 or 2 */

/* closes the port, and frees its ezserial_t struct */
void ezserial_close(ezserial_t *serial);

/* setups the port, look at ezserial_open */
void ezserial_setup(ezserial_t *serial,
                    int baud,
                    int bits,
                    int parity,
                    int stop);

void ezserial_setflowcontrol(ezserial_t *serial,
                             int rtscts,   /* Boolean: 0 - no rts/cts control, 1 - rts/cts control         */
                             int xonxoff); /* Boolean: 0 - no xon/xoff, 1 - xon/xoff                   */

/* sends a char via serial port */
void ezserial_putchar(ezserial_t *serial,
                      char c);

/* sends a null terminated string */
void ezserial_putstring(ezserial_t *serial,
                        char *str);

/* sends a data of known size */
void ezserial_putdata(ezserial_t *serial,
                      uint8_t *data, /* data */
                      int datalen);  /* length of data */

/* waits until all data has been transmited */
void ezserial_drain(ezserial_t *serial);
#ifdef __cplusplus
}
#endif

#endif /* _EZSERIAL_H */
