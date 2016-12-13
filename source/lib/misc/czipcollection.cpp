#include "../../libinc/library.h"


namespace chilli {

	namespace lib {

		namespace zip {

			collection::collection(void)
			{
			}

			collection::~collection(void)
			{
				Close();
			}

			int collection::Count ( void ) const
			{
				return m_files.Count();
			}

			void collection::Close ( void )
			{
				if ( m_zipfiles ) {
					for ( int ii=0; ii<Count(); ii++ )
						m_zipfiles[ii].Close();
				}
				SAFEDELETEARRAY ( m_zipfiles );

				m_files.Destroy();
			}

			BOOL collection::Open ( LPSTR basedir, LPSTR ext )
			{
			char path[MAX_PATH];

				if ( basedir == NULL )
					return FALSE ;

				if ( ext == NULL )
					return FALSE ;

				strcpy ( path, basedir );
				strcat ( path, ext );
				m_files.CreateDir( path );

				if( !Count() )
					return FALSE;

				m_zipfiles = new file[ Count() ] ;
				if ( m_zipfiles == NULL )
					return FALSE ;

				// sort the file list in order
				m_files.Sort();

				for ( int ii=0; ii<Count(); ii++ ) {

					strcpy ( path, basedir );
					strcat ( path, m_files[ii].m_filename );
					m_zipfiles[ii].Open( path );

				}

				return TRUE ;
			}


			file& collection::operator[] ( int index ) const
			{
			static file blank_zip;

				_ASSERTE ( index>=0 && index<Count() );

				if ( index>=0 && index<Count() )
					return m_zipfiles[index];

				return blank_zip ;
			}

			// finds the latest version of the file, using the
			// zips in reverse order
			int collection::FindFile ( LPSTR filename, int iCaseSensitivity, int where )
			{
				if ( where != collection::all ) {
					if ( where <0 || where >=Count() )
						return UNZ_END_OF_LIST_OF_FILE ;
					return m_zipfiles[where].FindFile( filename, iCaseSensitivity );
				}

				for ( int ii=Count()-1; ii>=0; ii-- ) {
					int temp = m_zipfiles[ii].FindFile( filename, iCaseSensitivity );
					if ( temp != UNZ_END_OF_LIST_OF_FILE )
						return temp;
				}

				return UNZ_END_OF_LIST_OF_FILE ;
			}


			void* collection::LoadFile ( LPSTR filename, u32* size, int where )
			{
				if ( where != collection::all ) {
					if ( where <0 || where >=Count() )
						return NULL ;
					return m_zipfiles[where].LoadFile ( filename, size );
				}

				for ( int ii=Count()-1; ii>=0; ii-- ) {
					void* temp = m_zipfiles[ii].LoadFile ( filename, size );
					if ( temp != NULL )
						return temp ;
				}
				return NULL ;
			}


			void collection::UnloadFile ( u8** data )
			{
				SAFEFREE ( *data );
			}

		} // namespace zip

	} // namespace lib

} // namespace tme
