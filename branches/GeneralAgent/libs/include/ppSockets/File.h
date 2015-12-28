/** \file File.h
 **	\date  2005-04-25
**/
#ifndef _SOCKETS_File_H
#define _SOCKETS_File_H

#include "sockets-config.h"
#include "IFile.h"
#include <stdio.h>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


/** IFile implementation of a disk file. 
	\ingroup file */
class File : public IFile
{
public:
	File();
	File(FILE *);
	/** convenience: calls fopen() */
	File(const std::string& path, const std::string& mode);
	~File();

	bool fopen(const std::string& path, const std::string& mode);
	void fclose() const;

	size_t fread(char *, size_t, size_t) const;
	size_t fwrite(const char *, size_t, size_t);

	char *fgets(char *, int) const;
	void fprintf(const char *format, ...);

	off_t size() const;
	bool eof() const;

	void reset_read() const;
	void reset_write();

	const std::string& Path() const;

private:
	File(const File& ) {} // copy constructor
	File& operator=(const File& ) { return *this; } // assignment operator

	std::string m_path;
	std::string m_mode;
	mutable FILE *m_fil;
	bool m_b_close;
	mutable long m_rptr;
	long m_wptr;
};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_File_H

