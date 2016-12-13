#ifndef _CZIPCOLLECTION_H_INCLUDED_
#define _CZIPCOLLECTION_H_INCLUDED_

using namespace chilli::types ;

namespace chilli {

	namespace lib {

		namespace zip {

			class collection
			{
			public:
				collection(void);
				~collection(void);

				enum {
					all = -1
				};

				BOOL Open ( LPSTR basedir, LPSTR ext );
				void Close( void );
				int Count ( void ) const;

				file& operator[] ( int index ) const ;

				int FindFile ( LPSTR filename, int iCaseSensitivity=UNZ_OSDEFAULTCASE, int where=all );
				void* LoadFile ( LPSTR filename, u32* size=NULL, int where=all );
				void UnloadFile ( u8** data );


			private:
				os::filelist		m_files;
				file*			m_zipfiles;

			};

		} // namespace zip

	} // namespace lib

} // namespace chilli

#endif //_CZIPCOLLECTION_H_INCLUDED_
