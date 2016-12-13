#ifndef _XML_H_INCLUDED_
#define _XML_H_INCLUDED_

#include "chilli.h"

using namespace chilli;

namespace chilli  {

	namespace lib {

		class xml : public TiXmlDocument
		{
		public:

			xml();
			~xml();

			BOOL Load ( LPCSTR filename, lib::packs* pak = NULL );

			class node : public TiXmlElement
			{
			public:
				node* Find ( LPCSTR section );
				node* Find( LPCSTR element, LPCSTR id );

				BOOL IsType(LPCSTR name) const ;

				LPCSTR ReadValue ( LPCSTR name );

				int ReadValueArray ( LPCSTR name, collections::c_s32& c );
				int ReadValueArray ( LPCSTR  name, collections::c_float& c );

				int ReadIntProperty ( LPCSTR  name, int defaultvalue=0 );
				BOOL ReadBoolProperty ( LPCSTR  name, BOOL defaultvalue=FALSE );
				f32 ReadFloatProperty ( LPCSTR  name, f32 defaultvalue=0 );
				LPCSTR ReadStrProperty ( LPCSTR  name, LPCSTR defaultvalue=NULL );


				int ReadArray ( LPCSTR name, collections::c_s32& c );
				int ReadArray ( LPCSTR name, collections::c_s32& c, LPCSTR delim );

				int ReadToken( LPCSTR token, token_t array[], int max, int defaultvalue=0 );
				f32 ReadFloat ( LPCSTR name, f32 defaultvalue=0 );
				BOOL ReadBool( LPCSTR name, BOOL defaultvalue=FALSE );
				int ReadInt( LPCSTR name, int defaultvalue=0 );
				LPCSTR ReadStr( LPCSTR name, LPCSTR defaultvalue = NULL );

				LPCSTR ReadItemEx ( LPCSTR name );
					LPCSTR ReadItem ( LPCSTR name, LPCSTR defaultvalue= NULL  );
					int ReadItem ( LPCSTR name, int defaultvalue=0 );
					f32 ReadItem ( LPCSTR name, f32 defaultvalue=0 );

				BOOL Exists( LPCSTR name );
				int Count ();

				LPCSTR ReadElement( LPCSTR element, LPCSTR id, LPCSTR tag );
				point ReadPoint( LPCSTR name, point& po );
				size ReadSize( LPCSTR name, size& s1 );

				int ReadColour( LPCSTR name, int defaultvalue );
			};

			node* Find ( LPCSTR section );


			//CDate ReadXml_Date ( TiXmlElement* element, LPCSTR name );
			//TiXmlDocument* Doc() const { return m_doc; }


		};
	}
}

#define FOREACHELEMENT( parent,element ) \
	for( chilli::lib::xml::node* element=(chilli::lib::xml::node*)parent->FirstChildElement(); element; element = (chilli::lib::xml::node*)element->NextSiblingElement() )


#endif //_XML_H_INCLUDED_
