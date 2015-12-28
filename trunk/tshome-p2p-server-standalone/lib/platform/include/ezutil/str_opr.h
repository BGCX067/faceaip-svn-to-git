/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * str_opr.h - string opr func
 *
 * Copyright (C) 2005 - joy.woo@hotmail.com -, All Rights Reserved.
 *
 * $Id: str_opr.h 10069 2007-07-12 07:36:25Z WuJunjie $
 *
 *  Explain:
 *     Some useful string function, find more yourself
 *
 *  Update:
 *     2005-03-23 17:19:24 WuJunjie 10221 Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _STR_OPR_H
#define _STR_OPR_H

#include <stdio.h>
#include <time.h>

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * l_count_c - 
     * @ src : 
     *		data buf
     * @ __c : 
     *		the obj char
     *
     * the '__c' count in src from left
     */
    size_t l_count_c(char *src, const char __c);
    size_t r_count_c(char *src, const char __c);

    size_t l_trim_c(char *src, const char trimmer, size_t src_len);
    size_t r_trim_c(char *src, const char trimmer, size_t src_len);
    size_t trim_c(char *src, const char trimmer, size_t src_len);
    size_t trim_blank(char *src, size_t src_len);

    size_t replace_c(
        char *src,
        const char fromc,
        const char toc,
        size_t src_len);
    size_t replace_chars_with_char(
        char *strbuf,
        char *chrstorm,
        char chartorlcwith);

    /* danger func */
    size_t ini_str(char *src, const char __c, const  size_t src_size);

    int split_str(
        const char * const src,
        char *left,
        char *right,
        const char * const spliter,
        size_t src_len);
    int r_split_str(
        const char * const src,
        char *right,
        const char * const spliter,
        size_t src_len);

#define SHOW_HEXAD 0x01
	#define SHOW_ASCII 0x02
	#define SHOW_BINAR 0x04
	#define SHOW_LINER 0x08

    int dumpBuffer2Buffer(
        char *p_dst,
        const size_t i_dst_len,
        unsigned char *buf,
        const size_t len,
        size_t uiPrintMask);
    int dumpBuffer(
        FILE *fp,
        unsigned char *buf,
        const size_t len,
        size_t uiPrintMask);
    //		dumpBuffer(stdout
    //							, (U8 *)buff, bufflen,
    //			SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    char * Ip2Str(const unsigned int iIp, char *pStr);
    unsigned int Str2Ip(const char *pStr);


    int isCommentLine(char *src);
    int is_ascii_string(const char * const src, const size_t len);

    int ez_gbk_to_utf8(char *toStr, size_t toStrSize, const char * gbkStr);

    /**
     * char pBuff[32] or bigger;
     * if pFormat=NULL : "%04d-%02d-%02d %02d:%02d:%02d"
     */
    #define GET_DATE_TIME_STRING_DEFAULT_STR_LEN 20
    char * get_date_time_string(char *pBuff, const char *pFormat);
    // replaced by:get_date_time_string("%04d%02d%02d%02d%02d%02d");
    //char * get_date_time_string_compress();

    // convert from "Sep 27 2012 09:04:31" to struct tm,
    // use as int r = get_build_date_time(&__tm, __DATE__, __TIME__);
    int get_build_date_time(struct tm *p_tm
                            , const char * const p_date_str
                            , const char * const p_time_str);

    /**
     * time1: timt_t value
     * szTime: dsc string
     * if pFormat=NULL : "%-4d-%-2d-%-2d %-2d:%-2d:%-2d"
     */
    int time2string(const time_t time1, char *szTime, const char *pFormat);
    // szTime: 2012-11-2 13:33:17
    time_t string2time(char * szTime);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef __cplusplus
}
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif //_STR_OPR_H
