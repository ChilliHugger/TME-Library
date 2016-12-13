#include "../../libinc/library.h"

namespace chilli {
	namespace lib {

		packs::packs(void)
		{
			m_dirlist = NULL ;
			m_ziplist = NULL ;
		}

		packs::~packs(void)
		{
			SAFEDELETE( m_dirlist );
			SAFEDELETE( m_ziplist );
		}

		void packs::Init ( LPCSTR filespec )
		{
		char path[MAX_PATH];
		char file[MAX_PATH];
		char dir[MAX_PATH];

			m_dirlist = new os::filelist();
			m_ziplist = new zip::collection();

			lib::splitpath(filespec,NULL,dir,file,NULL);
			rootdir = dir ;
			rootspec = file ;

			// create a directory list
			sprintf ( path, "%s*.*", filespec );
			m_dirlist->CreateDir(path,os::filelist::SUBDIR );
			m_dirlist->Sort();
			//
				
			// if can't find/open a bollocks file then try a normal zip
			sprintf ( path, "%s*.pak", file );
			if( !m_ziplist->Open(dir, path ) ) {
				sprintf ( path, "%s*.zip", file );
				m_ziplist->Open(dir, path );
			}
		}


		void packs::Unload ( u8** ptr ) 
		{
			os::filemanager::Unload( ptr );
		};

		void* packs::Load ( LPCSTR filename, u32* size ) 
		{
		char path[MAX_PATH];

			for ( int ii=m_dirlist->Count()-1; ii>=0; ii-- ) {
				lib::strcpy(path, rootdir );
				lib::strcat(path, m_dirlist->GetAt(ii).m_filename );
				lib::strcat(path, "/" );
				lib::strcat(path, filename );
				if ( os::filemanager::Exists( path ) ) {
					return os::filemanager::Load( path, size );
				}
			}

			return m_ziplist->LoadFile((LPSTR)filename,size);
		};

		BOOL packs::Exists ( LPCSTR filename ) 
		{
		char path[MAX_PATH];

			for ( int ii=m_dirlist->Count()-1; ii>=0; ii-- ) {
				lib::strcpy(path, rootdir );
				lib::strcat(path, m_dirlist->GetAt(ii).m_filename );
				lib::strcat(path, "/" );
				lib::strcat(path, filename );
				if ( os::filemanager::Exists( path ) )
					return TRUE;
			}
			return m_ziplist->FindFile((LPSTR)filename)==UNZ_OK;

		};
/*
		BOOL packs::ExistsDir ( LPCSTR filename ) 
		{
		char path[MAX_PATH];

			for ( int ii=m_dirlist->Count()-1; ii>=0; ii-- ) {
				lib::strcpy(path, rootdir );
				lib::strcat(path, m_dirlist->GetAt(ii).m_filename );
				lib::strcat(path, "/" );
				lib::strcat(path, filename );
				if ( os::filemanager::ExistsDir( path ) )
					return TRUE;
			}

			lib::strcpy( path, filename );
			lib::strcat( path, "/");
			return m_ziplist->FindFile((LPSTR)path)==UNZ_OK;
		};
*/



	} // lib

} // tme
