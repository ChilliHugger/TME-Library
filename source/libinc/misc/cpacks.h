#ifndef _CPACKS_H_INCLUDED_
#define _CPACKS_H_INCLUDED_

namespace chilli  {

	namespace lib {

		class packs
		{
		public:
			packs(void);
			virtual ~packs(void);

			void Init ( LPCSTR filespec );
			void* Load ( LPCSTR filename, u32* size ) ;
			BOOL Exists ( LPCSTR filename ) ;
//			BOOL ExistsDir ( LPCSTR filename ) ;
			void Unload ( u8** ptr ) ;


		protected:
			string				rootdir;
			string				rootspec;
			os::filelist*		m_dirlist;
			zip::collection*	m_ziplist;

		};

	} // lib

} // tme


#endif //_CPACKS_H_INCLUDED_
