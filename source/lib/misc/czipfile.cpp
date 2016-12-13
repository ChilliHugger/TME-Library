#include "../../libinc/library.h"

//#include <io.h>
#define _A_NORMAL       0x00    /* Normal file - No read/write restrictions */
#define _A_RDONLY       0x01    /* Read only file */
#define _A_HIDDEN       0x02    /* Hidden file */
#define _A_SYSTEM       0x04    /* System file */
#define _A_SUBDIR       0x10    /* Subdirectory */
#define _A_ARCH         0x20    /* Archive file */

//#include <malloc.h>
//#include "unzip.h"

void __ConvertDOSToUnixName( char *dst, const char *src );

//int __cdecl zipfile_compare( const void *arg1, const void *arg2 );


namespace chilli {


		void __ConvertDOSToUnixName( char *dst, const char *src )
		{
			while ( *src )
			{
				if ( *src == '\\' )
					*dst = '/';
				else
					*dst = *src;
				dst++; src++;
			}
			*dst = 0;
		}

		int zipfile_compare( const void *arg1, const void *arg2 )
		{
		/* Compare all of both strings: */
			lib::zip::file::fileentry* f1 = (lib::zip::file::fileentry*)*(u32*)arg1;
			lib::zip::file::fileentry* f2 = (lib::zip::file::fileentry*)*(u32*)arg2;
			return lib::stricmp( f1->m_filename, f2->m_filename );
		}



	namespace lib {

		namespace zip {

			file::file()
			{
				m_zipfile=NULL;
				m_fat =NULL;
			}


			file::~file()
			{
				if ( m_zipfile )
					Close();
			}


			BOOL file::Open ( LPSTR filename )
			{
				m_zipfile = unzOpen (filename);
				if ( m_zipfile == NULL )
					return FALSE;
				CreateFat();
				return TRUE;
			}

			BOOL file::Close ( void )
			{
				DestroyFat();
				unzClose ( m_zipfile );
				m_zipfile=NULL;
				return TRUE;
			}

			int file::GotoFirstFile ( void )
			{
				return unzGoToFirstFile (m_zipfile);
			}

			int file::GotoNextFile ( void )
			{
				return unzGoToNextFile(m_zipfile);
			}

			int file::GetCurrentFileInfo ( unz_file_info *pfile_info, char *szFileName, unsigned long fileNameBufferSize, void *extraField, unsigned long extraFieldBufferSize, char *szComment, unsigned long commentBufferSize)
			{
				return unzGetCurrentFileInfo ( m_zipfile, pfile_info, szFileName, fileNameBufferSize, extraField, extraFieldBufferSize, szComment, commentBufferSize);
			}

			int file::FindFile ( LPSTR filename, int iCaseSensitivity )
			{
			char path[MAX_PATH];

				__ConvertDOSToUnixName( path, filename );

				fileentry* key = new fileentry(path);

				void* temp = bsearch( &key, m_fat, Count(), sizeof( fileentry * ), zipfile_compare );
				
				SAFEDELETE ( key );

				if ( temp ) {
					file::fileentry* f2 = (file::fileentry*)*(u32*)temp;

					return unzGoToFile (m_zipfile, f2->m_pos, f2->m_num_file );
				}

				return UNZ_END_OF_LIST_OF_FILE;
			}

			void* file::LoadFile ( LPSTR filename, u32* size )
			{
			char path[MAX_PATH];
				if ( size )
					*size=0;

				__ConvertDOSToUnixName( path, filename );

				if ( FindFile(path) == UNZ_OK ) {

					unz_file_info file_info;
					GetCurrentFileInfo ( &file_info, NULL, 0, NULL, 0, NULL, 0);

					char* data = (char*) malloc(file_info.uncompressed_size+1 );
					memset ( data, 0, file_info.uncompressed_size+1 );

					unzOpenCurrentFile (m_zipfile);
					unzReadCurrentFile (m_zipfile, data, file_info.uncompressed_size);
					unzCloseCurrentFile (m_zipfile);

					if ( size )
						*size = file_info.uncompressed_size;
					return data;
				}

				return NULL ;
			}

			void file::UnloadFile ( u8** data )
			{
				SAFEFREE ( *data );
			}

			void file::CreateFat ( void )
			{
			unz_global_info gi;
			int count;
			char path[MAX_PATH];

			//unz_file_info			file_info;
			//unz_file_info_internal	file_info_internal;

				unzGetGlobalInfo(m_zipfile,&gi);
				m_files = gi.number_entry ;

				m_fat = new fileentry*[Count()];

				int err = unzGoToFirstFile(m_zipfile);
				
				u32 memoryalloc = sizeof(fileentry*) * Count() ;

				count=0;
				while ( err == UNZ_OK ) {

					unz_s* s = (unz_s*)m_zipfile;

					GetCurrentFileInfo ( NULL, path, MAX_PATH, NULL, 0, NULL, 0);

					m_fat[count] = new fileentry(path,s->pos_in_central_dir, s->num_file, s->cur_file_info.external_fa) ;

					memoryalloc += sizeof ( fileentry );
					//memoryalloc += strlen(path);

					count++;

					err = unzGoToNextFile (m_zipfile);
				}


			/* Sort remaining args using Quicksort algorithm: */
			qsort( (void *)m_fat, (size_t)Count(), sizeof( fileentry * ), zipfile_compare );

			// we need and index reference for later
			for ( int ii=0; ii<Count(); ii++ ) {
				m_fat[ii]->m_index = ii;
			}

			for ( int ii=0; ii<Count(); ii++) {
				if ( m_fat[ii]->m_flags & _A_SUBDIR && m_fat[ii]->m_dircount == 0 ) {
					ii=DirectoryCount(ii);
				}
			}

			}


			int file::DirectoryCount ( int index )
			{

			int len = strlen(m_fat[index]->m_filename);
			int ii=0;
				
				m_fat[index]->m_dircount=0;

			// we need and index reference for later
			for ( ii=index+1; ii<Count(); ii++) {
					// check for return
				int cmp = lib::strnicmp(m_fat[index]->m_filename, m_fat[ii]->m_filename, len );
				if ( cmp == 0 ) {
						m_fat[index]->m_dircount++;
						if ( m_fat[ii]->m_flags & _A_SUBDIR ) {
							ii=DirectoryCount(ii);
						}
				}else
					break;
			}
			   
			;

			return ii-1;
			}

			void file::DestroyFat ( void )
			{
				if ( m_fat ) {
					for ( int ii=0; ii<Count(); ii++ )
						SAFEDELETE ( m_fat[ii] );
					SAFEDELETEARRAY ( m_fat );
				}
			}



			file::fileentry::fileentry(LPSTR filename)
			{
				m_filename = strdup(filename) ;
				m_pos = 0 ;
				m_num_file = 0 ;
				m_index=0;
				m_flags=0;
				m_dircount=0;
			}

			file::fileentry::fileentry(LPSTR filename,u32 pos,u32 num_file,u32 flags)
			{
				m_filename = strdup(filename) ;
				m_pos = pos ;
				m_num_file = num_file ;
				m_index = num_file ;
				m_flags = flags ;
				m_dircount=0;
			}


			file::fileentry::fileentry()
			{
				m_filename = NULL ;
			}

			file::fileentry::~fileentry()
			{
				SAFEFREE ( m_filename );
			}




			/***
			*char *bsearch() - do a binary search on an array
			*
			*Purpose:
			*       Does a binary search of a sorted array for a key.
			*
			*Entry:
			*       const char *key    - key to search for
			*       const char *base   - base of sorted array to search
			*       unsigned int num   - number of elements in array
			*       unsigned int width - number of bytes per element
			*       int (*compare)()   - pointer to function that compares two array
			*               elements, returning neg when #1 < #2, pos when #1 > #2, and
			*               0 when they are equal. Function is passed pointers to two
			*               array elements.
			*
			*Exit:
			*       if key is found:
			*               returns pointer to occurrence of key in array
			*       if key is not found:
			*               returns NULL
			*
			*Exceptions:
			*
			*******************************************************************************/

			void * file::bsearch (
					const void *key,
					const void *base,
					size_t num,
					size_t width,
					int (*compare)(const void *, const void *)
					)
			{
					char *lo = (char *)base;
					char *hi = (char *)base + (num - 1) * width;
					char *mid;
					size_t half;
					int result;

					while (lo <= hi)
							if ( (half = (num / 2)) )
							{
									mid = lo + (num & 1 ? half : (half - 1)) * width;

									m_lastfound = (file::fileentry*)*(u32*)mid;

									if (!(result = (*compare)(key,mid)))
											return(mid);
									else if (result < 0)
									{
											hi = mid - width;
											num = num & 1 ? half : half-1;
									}
									else    {
											lo = mid + width;
											num = half;
									}
							}
							else if (num) {
									m_lastfound = (file::fileentry*)*(u32*)lo;
									return((*compare)(key,lo) ? NULL : lo);
							} else
									break;

					return(NULL);
			}



			file::fileentry* file::operator[] ( int index ) const
			{
			static fileentry blank_zip;

				_ASSERTE ( index>=0 && index<Count() );

				if ( index>=0 && index<Count() )
					return m_fat[index];

				return &blank_zip ;
			}

		} // namespace zip

	} // namespace lib

} // namespace tme
