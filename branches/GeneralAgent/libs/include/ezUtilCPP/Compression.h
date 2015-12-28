/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Compression.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: Compression.h 5884 2013-08-28 01:51:06Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-08-28 01:51:06 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include <zlib.h>
#include <zconf.h>
#include <iostream>
#include <vector>

//#define COMPRESSION_LEVEL Z_DEFAULT_COMPRESSION
#define BUFFER_SIZE_FOR_ZLIB 16384

typedef std::vector<unsigned char> StlVecUnChar;

class Compression
{
public:
    enum CompressLevel
    {
        NO_COMPRESSION      = Z_NO_COMPRESSION,
        BEST_SPEED          = Z_BEST_SPEED,
        BEST_COMPRESSION    = Z_BEST_COMPRESSION,
        DEFAULT_COMPRESSION = Z_DEFAULT_COMPRESSION
    };

    Compression();
    Compression(bool isCom, CompressLevel level = DEFAULT_COMPRESSION);
    virtual ~Compression();

    int Init();

    // Z_OK : Z_DATA_ERROR
    int Deflate(const StlVecUnChar &inStr, StlVecUnChar &outStr);
    // Z_OK : Z_DATA_ERROR
    int Inflate(const StlVecUnChar &inStr, StlVecUnChar &outStr);

private:

    z_stream      m_zstream;               // Stream structure used by zlib
    bool          m_IsCompress;            // True: compress. False: decompress
    unsigned char m_bufferIn[BUFFER_SIZE_FOR_ZLIB]; // Input buffer for zlib
    unsigned char m_bufferOut[BUFFER_SIZE_FOR_ZLIB];// Output Buffer for zlib
    const int     m_bufferInCapa;          // Input buffer capacity
    const int     m_bufferOutCapa;         // Output buffer capacity
    CompressLevel m_compressLevel;         // Compress level
};

#endif /* _COMPRESSION_H_ */
