/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * StringOpr.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: StringOpr.h 5884 2012-09-05 10:45:49Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-05 10:45:49  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _STRINGOPR_H
#define _STRINGOPR_H

#include <fstream>
#include <string>

template <class T>
std::string ezConvertToString(T);
template <class T>
std::string ezConvertToString(T value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

template <class T> 
void ezConvertFromString(T &value, const std::string &s) {
  std::stringstream ss(s);
  ss >> value;
}

int ezGBKToUTF8(std::string & gbkStr);
#endif // #ifndef _STRINGOPR_H

#if 0

/**************** USAGE SAMPLE BEGIN ****************/

#include <iostream.h>
#include "IniFile.h"

const char *inIniFile = "inifile_in.ini";
const char *outIniFile = "inifile_out.ini";

int main()
{

	std::string m_HostName;
	CIniFile IniFile_Host;

/* Read */
#if 1
	bool bExist=IniFile_Host.Open(inIniFile, nearst::EnOpenFileRead);

	if(bExist)
	{
		if (IniFile_Host.GetItem("Host","HostName", m_HostName))
		{
			cout << "HostName:[" << m_HostName.c_str() << "]" << endl;
		}
		else
		{
			cout << "Not found:HostName" << endl;
		}

		// read string
		if(IniFile_Host.GetItem("UpdateFile","HostName",m_HostName))
		{
			cout << "HostName:[" << m_HostName.c_str() << "]" << endl;
		}
		else
		{
			cout << "Not found:HostName" << endl;
		}
		
		long iFileNum=-1;
		if(IniFile_Host.GetItem("UpdateFile","FileNum", iFileNum))
		{
			cout << "HostName:[" << iFileNum << "]" << endl;
		}
		else
		{
			cout << "Not found:FileNum" << endl;
		}

		//IniFile_Host.Close();
	}
	else
	{
		cout << "Can not open ini file:" << inIniFile << endl;
	}

#endif

/* Write */
#if 1
	bExist=IniFile_Host.Open(outIniFile, nearst::EnOpenFileWrite);

	if(bExist)
	{
		if (IniFile_Host.WriteSection("Host") )
		{
			cout << "IniFile_Host.WriteSection OK" << endl;
		}
		else
		{
			cout << "IniFile_Host.WriteSection:Host Failed" << endl;
		}

		// 
		if(IniFile_Host.WriteItem("HostName", "ftp.nearst.com"))
		{
			cout << "IniFile_Host.WriteItem:HostName OK" << endl;
		}
		else
		{
			cout << "IniFile_Host.WriteItem:HostName Failed" << endl;
		}

		if(IniFile_Host.WriteItem("FileNum", 20))
		{
			cout << "IniFile_Host.WriteItem:FileNum OK" << endl;
		}
		else
		{
			cout << "IniFile_Host.WriteItem:FileNum Failed" << endl;
		}
	}
	else
	{
		cout << "Can not open ini file:" << outIniFile << endl;
	}
#endif

	return 0;
}

/**************** USAGE SAMPLE END ****************/
#endif

