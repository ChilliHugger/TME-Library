#ifndef _CZIPFILE_H_INCLUDED_
#define _CZIPFILE_H_INCLUDED_

#include "unzip.h"


namespace chilli {

	namespace lib {

		namespace zip {

			class file 
			{
			public:
				file();
				~file();

				BOOL Open (LPSTR path);
				BOOL Close ( void );
				int GotoFirstFile ( void );
				int GotoNextFile ( void );

				int GetCurrentFileInfo ( unz_file_info *pfile_info, char *szFileName, unsigned long fileNameBufferSize, void *extraField, unsigned long extraFieldBufferSize, char *szComment, unsigned long commentBufferSize);
				int FindFile ( LPSTR filename, int iCaseSensitivity=UNZ_OSDEFAULTCASE );

				void* LoadFile ( LPSTR filename, u32* size );
				void UnloadFile ( u8** data );

				int DirectoryCount ( int index );

				void * bsearch (
					const void *key,
					const void *base,
					size_t num,
					size_t width,
					int (*compare)(const void *, const void *)
					);

				class fileentry {
				public:
					fileentry();
					fileentry(LPSTR filename,u32 pos,u32 num_file, u32 flags);
					fileentry(LPSTR filename);
					~fileentry();
				public:
					LPSTR	m_filename;
					u32	m_pos;
					u32	m_num_file;
					u32	m_index;
					u32	m_flags;
					u32	m_dircount;
				};


				fileentry* operator[] ( int index ) const;
				fileentry* LastFound ( ) const	{ return m_lastfound ; }
				int Count () const					{ return m_files; }


			private:
				void CreateFat ( void );
				void DestroyFat ( void );


			private:

				unzFile				m_zipfile;
				fileentry**			m_fat;
				int					m_files;
				fileentry*			m_lastfound;

			};

		} // namespace zip

	} // namespace lib

} // namespace tme

#endif //_CZIPFILE_H_INCLUDED_
