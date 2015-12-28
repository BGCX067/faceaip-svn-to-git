/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigMan.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigMan.h 5884 2012-07-02 09:15:02Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __CONFIG_MAN_H__
#define __CONFIG_MAN_H__

// 以zip压缩格式存放
// 这里注释， 以便手工修改， 2012-6-21 16:36:36 wujunjie
// #define HAVE_ZLIB_ZFILE

#include <string.h>
#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"
#include <json.h>
#include "ConfigBase.h"

//#ifdef HAVE_ZLIB_ZFILE
#include "zlib.h"
//#else
#include "File.h"
//#endif //HAVE_ZLIB_ZFILE

class CConfigMan2;
class CSaveDelay : public CEZThread
{
public:
    CSaveDelay();
    ~CSaveDelay();
    int Start(CConfigMan2 * pCConfigMan);
    int Stop();
    void ThreadProc();
private:
    CConfigMan2 *m_pCConfigMan;
};
class CConfigMan2 : public CEZObject
{
public:
    CConfigMan2();
    virtual ~CConfigMan2();
    
    //! 保存文件
    void saveFile();

protected:

};

#endif //__CONFIG_MAN_H__
