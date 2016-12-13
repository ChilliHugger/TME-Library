// CFile.cpp: implementation of the CFile class.
//
//////////////////////////////////////////////////////////////////////
#include "../../libinc/library.h"

#ifdef _UNIX_

#include "fcntl.h"
#include "unistd.h"

#include <sys/stat.h>

using namespace chilli ;

BOOL os::file::Open(LPCSTR lpszFileName, u32 nOpenFlags )
{
	// CFile objects are always binary and CreateFile does not need flag
	//nOpenFlags &= ~(u32)typeBinary;

	m_bCloseOnDelete = FALSE;
	m_hFile = (u32)hFileNull;

	memset( m_strFileName, 0x00, sizeof(m_strFileName) );

	os::Fullpath( m_strFileName, lpszFileName, MAX_PATH );

//	_ASSERTE(shareCompat == 0);

	// map read/write mode
	_ASSERTE((modeRead|modeWrite|modeReadWrite) == 3);


	DWORD dwAccess = O_BINARY; // "b"
	switch (nOpenFlags & 3)	{
	case modeRead:
		dwAccess = O_RDONLY; // "r"
		break;
	case modeWrite:
		dwAccess = O_WRONLY; // "w"
		break;
	case modeReadWrite:
		dwAccess = O_RDWR; // "r+"
		break;
	default:
		_ASSERTE(FALSE);  // invalid share mode
	}

	// map share mode
	DWORD dwShareMode=0;
	switch (nOpenFlags & 0x70)	{
	case shareCompat:       // map compatibility mode to exclusive
	case shareExclusive:
		dwShareMode = 0;
		break;
	case shareDenyWrite:
		dwShareMode = S_IREAD;
		break;
	case shareDenyRead:
		dwShareMode = S_IWRITE;
		break;
	case shareDenyNone:
		dwShareMode = S_IREAD|S_IWRITE;
		break;
	default:
		_ASSERTE(FALSE);  // invalid share mode?
	}

	// Note: typeText and typeBinary are used in derived classes only.

	// map modeNoInherit flag
	//SECURITY_ATTRIBUTES sa;
	//sa.nLength = sizeof(sa);
	//sa.lpSecurityDescriptor = NULL;
	//sa.bInheritHandle = (nOpenFlags & modeNoInherit) == 0;

	// map creation flags
	DWORD dwCreateFlag;
	if (nOpenFlags & modeCreate)	{
		dwShareMode = S_IREAD|S_IWRITE ;
		if (nOpenFlags & modeNoTruncate)
			dwCreateFlag = O_CREAT; // "a"
		else
			dwCreateFlag = O_CREAT|O_TRUNC; // "w+"
	}
	else
		dwCreateFlag = 0;


	// attempt file creation
	//HANDLE hFile = ::CreateFile(lpszFileName, dwAccess, dwShareMode, &sa,
	//	dwCreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
	//if (hFile == INVALID_HANDLE_VALUE )
	//	return FALSE;
	//m_hFile = (HFILE)hFile;


	int file = open ( lpszFileName, dwAccess|dwCreateFlag, dwShareMode );
	if ( file == -1 ) 
		return FALSE;

	m_hFile = file;

	m_bCloseOnDelete = TRUE;
	return TRUE;

}

u32 os::file::Read(void* lpBuf, u32 nCount)
{
	_ASSERTE(IsValidHandle());

	if (nCount == 0)
		return 0;

	_ASSERTE(lpBuf != NULL);

	//DWORD dwRead;
	//::ReadFile((HANDLE)m_hFile, lpBuf, nCount, &dwRead, NULL);
	//return (u32)dwRead;

	return read ( m_hFile, lpBuf, nCount );

}

BOOL os::file::Write(const void* lpBuf, u32 nCount)
{
	_ASSERTE(IsValidHandle());

	if (nCount == 0)
		return(TRUE);     // avoid Win32 "null-write" option

	_ASSERTE(lpBuf != NULL);

	//DWORD nWritten;
	//BOOL bRet = ::WriteFile((HANDLE)m_hFile, lpBuf, nCount, &nWritten, NULL);
	// Win32s will not return an error all the time (usually DISK_FULL)
	//if (nWritten != nCount)
	//	return (FALSE);

	if ( (u32)write ( m_hFile, lpBuf, nCount ) != nCount )
		return ( FALSE );

	return TRUE ;
}

LONG os::file::Seek(LONG lOff, u32 nFrom) const
{
	_ASSERTE(IsValidHandle());
	_ASSERTE(nFrom == begin || nFrom == end || nFrom == current);
//	_ASSERTE(begin == FILE_BEGIN && end == FILE_END && current == FILE_CURRENT);

	//DWORD dwNew = ::SetFilePointer((HANDLE)m_hFile, lOff, NULL, (DWORD)nFrom);
	//return dwNew;

	return lseek ( m_hFile, lOff, nFrom );
}

DWORD os::file::GetPosition() const
{
	_ASSERTE(IsValidHandle());
	//DWORD dwPos = ::SetFilePointer((HANDLE)m_hFile, 0, NULL, FILE_CURRENT);
	//return dwPos;
#ifdef WIN32
	return _tell(m_hFile);
#else
	return Seek ( 0, current );
#endif
}

BOOL os::file::Flush()
{
	_ASSERTE(IsValidHandle());
	//return ::FlushFileBuffers((HANDLE)m_hFile);
#ifdef WIN32
	return _commit ( m_hFile );
#else
	return TRUE ;
#endif
}

void os::file::Close()
{
	_ASSERTE(IsValidHandle());

//	BOOL bError = FALSE;
//	bError = !::CloseHandle((HANDLE)m_hFile);

	close ( m_hFile );

	m_hFile = -1;
	m_bCloseOnDelete = FALSE;
	memset( m_strFileName, 0x00, sizeof(m_strFileName) );

}

void os::file::Abort()
{
	_ASSERTE(IsValidHandle());

//	if (m_hFile != (u32)hFileNull)	{
//		// close but ignore errors
//		::CloseHandle((HANDLE)m_hFile);
//		m_hFile = (u32)hFileNull;
//	}
	close ( m_hFile );
	memset( m_strFileName, 0x00, sizeof(m_strFileName) );
}

BOOL os::file::SetLength(DWORD dwNewLen)
{
//	_ASSERTE(m_hFile != (u32)hFileNull);
//
//	Seek((LONG)dwNewLen, (u32)begin);
//
//	return ::SetEndOfFile((HANDLE)m_hFile);
	return FALSE;
}

DWORD os::file::GetLength() const
{
//DWORD dwLen;
//	dwLen = GetFileSize ( (HANDLE)m_hFile, NULL );
//	return dwLen;

	int curOffset = GetPosition();
	Seek(0,end);
	int length = GetPosition();
	Seek(curOffset,begin);
	return length;
}

#endif //_UNIX_
