/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * IniFile.h - _explain_
 *
 * Copyright (C) 2003 , All Rights Reserved.
 *
 * $Id: IniFile.h, v 1.0.0.1 2003-12-14 11:47:07 wjj Exp $
 *
 *  Explain:
 *     Read/write win ini file
 *
 *  Update:
 *     1.0 2003-12-14 11:47 joy.woo(ezlibs.com) Create
 *     1.1 2006-08-28 10:37 WuJunjie(10221) Modify
 *          decrease interface
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _INIFILE_H
#define _INIFILE_H

#include <fstream>
#include <string>

using namespace std;
//
namespace nearst
{
	const int MAX_LINE = 256;
}

class CIniFile
{
public:

	enum EnOpenFileMode
	{
		EnOpenFileNotOpen,
		EnOpenFileRead,
		EnOpenFileWrite,
	};

	//open ini file for read
	bool Open(const std::string &strName,
	          const CIniFile::EnOpenFileMode enOpenMode=CIniFile::EnOpenFileRead);

	//read value of int from ini file
	bool GetItem(const std::string &strSection,
	             const std::string &strItem,
	             long & iValue);

	//read value of std::string from ini file
	bool GetItem(const std::string &strSection,
	             const std::string &strItem,
	             std::string &strValue);

	//write Section to ini file
	bool WriteSection(const std::string &strSection);

	//write Item and value of int to ini file
	bool WriteItem(const std::string &strItem,
	               const int iValue);

	//write Item and value of std::string to ini file
	bool WriteItem(const std::string &strItem,
	               const std::string &strValue);

	//set file comment
	void SetCommentChar(const char cCom);

	//close ini file
	virtual void Close(void);

	//default constructor
	CIniFile();

	//default destructor
	virtual ~CIniFile();

	//private function
private:

	//
	const std::string &TrimString(std::string &strToTrim);

	//find Section in ini file
	bool FindSection(std::string strSection);

	//read one line from file
	bool GetOneLine(std::string &str);

	//all private variables
private:

	// comment char
	char m_cComment;
	//for read
	ifstream m_ifsIniFileIn;

	//for write
	ofstream m_ofsIniFileOut;

	//stores name of ini file to read/write
	std::string m_strFilePathName;
	//
	CIniFile::EnOpenFileMode m_enOpenMode;

};

#endif // #ifndef _INIFILE_H

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

