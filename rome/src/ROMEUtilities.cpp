// Author: Jan Wouters
//////////////////////////////////////////////////////////////////////////
//
//  ROMEUtilities
//
//  General purpose utilities.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#include <ROMEUtilities.h>
#include <ROMEXML.h>
#if defined( R__VISUAL_CPLUSPLUS )
#  pragma warning( push )
#  pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#  pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

// Byte swapping big endian <-> little endian
void ROMEUtilities::ByteSwap( UShort_t *aValue ) 
{
   Byte_t _tmp;
   _tmp = *( (Byte_t *)( aValue ) );
   *( (Byte_t *)( aValue ) ) = *( ( (Byte_t *)( aValue ) ) + 1 );
   *( ( (Byte_t *)( aValue ) ) + 1 ) = _tmp;
}

void ROMEUtilities::ByteSwap( Short_t *aValue )
{
    ByteSwap( (UShort_t *)aValue );
}

void ROMEUtilities::ByteSwap( UInt_t *aValue ) 
{
   Byte_t _tmp;
   _tmp = *( (Byte_t *)( aValue ) );
   *( (Byte_t *)( aValue ) ) = *( ( (Byte_t *)( aValue ) ) + 3 );
   *( ((Byte_t *)( aValue ) ) + 3) = _tmp;
   
   _tmp = *( ( (Byte_t *)( aValue ) ) + 1);
   *( ( (Byte_t *)( aValue ) ) + 1 ) = *( ( (Byte_t *)( aValue ) ) + 2 ); 
   *( ( (Byte_t *)( aValue ) ) + 2 ) = _tmp;
}

void ROMEUtilities::ByteSwap( Int_t *aValue )
{
    ByteSwap( (UInt_t *)aValue );
}

void ROMEUtilities::ByteSwap( Float_t *aValue )
{
    ByteSwap( (UInt_t *)aValue );
}

void ROMEUtilities::ByteSwap( ULong64_t *aValue ) 
{ 
   Byte_t _tmp;
   _tmp= *( (Byte_t *)( aValue ) );
   *( (Byte_t *)( aValue ) ) = *(((Byte_t *)( aValue ) ) + 7 );
   *( ( (Byte_t *)( aValue ) ) + 7 ) = _tmp;
   
   _tmp = *( ( (Byte_t *)( aValue ) ) + 1 );
   *( ( (Byte_t *)( aValue ) ) + 1 ) = *( ( (Byte_t *)( aValue ) ) + 6 );
   *( ( (Byte_t *)( aValue ) ) + 6 ) = _tmp;
   
   _tmp = *(((Byte_t *)( aValue ))+2);
   *( ( (Byte_t *)( aValue ) ) + 2 ) = *(((Byte_t *)( aValue ) ) + 5 );
   *( ( (Byte_t *)( aValue ) ) + 5 ) = _tmp;
   
   _tmp = *( ( (Byte_t *)( aValue ) ) + 3 );
   *( ( (Byte_t *)( aValue ) ) + 3 ) = *(((Byte_t *)( aValue ) ) + 4 );
   *( ( (Byte_t *)( aValue ) ) + 4 ) = _tmp;
}

void ROMEUtilities::ByteSwap( Double_t *aValue )
{
    ByteSwap( (ULong64_t *)aValue );
}

void ROMEUtilities::GetMidasTID(ROMEString *buf, Char_t *type)
{
   buf->Resize(0);
   if (!strcmp(type, "Byte_t")
       || !strcmp(type, "BYTE"))
      buf->Append("TID_BYTE");  // < unsigned byte         0       255
   else if (!strcmp(type, "Char_t")
            || !strcmp(type, "Text_t")
            || !strcmp(type, "char"))
      buf->Append("TID_SBYTE"); //< signed byte         -128      127
   else if (!strcmp(type, "UChar_t")
            || !strcmp(type, "unsigned char"))
      buf->Append("TID_CHAR");  //< single character      0       255
   else if (!strcmp(type, "UShort_t")
            || !strcmp(type, "WORD")
            || !strcmp(type, "unsigned short")
            || !strcmp(type, "unsigned short int"))
      buf->Append("TID_WORD");  //< two bytes             0      65535
   else if (!strcmp(type, "Short_t")
            || !strcmp(type, "Version_t")
            || !strcmp(type, "short")
            || !strcmp(type, "short int"))
      buf->Append("TID_SHORT"); //< signed word        -32768    32767
   else if (!strcmp(type, "UInt_t")
            || !strcmp(type, "DWORD")
            || !strcmp(type, "unsigned int")
            || !strcmp(type, "unsigned long")
            || !strcmp(type, "unsigned long int"))
      buf->Append("TID_DWORD"); //< four bytes            0      2^32-1
   else if (!strcmp(type, "Int_t")
            || !strcmp(type, "INT")
            || !strcmp(type, "Ssize_t")
            || !strcmp(type, "int")
            || !strcmp(type, "long")
            || !strcmp(type, "long int"))
      buf->Append("TID_INT");   //< signed dword        -2^31    2^31-1
   else if (!strcmp(type, "Bool_t")
            || !strcmp(type, "BOOL")
            || !strcmp(type, "bool"))
      buf->Append("TID_BOOL");  //< four bytes bool       0        1
   else if (!strcmp(type, "Float_t")
            || !strcmp(type, "Real_t")
            || !strcmp(type, "float"))
      buf->Append("TID_FLOAT"); //< 4 Byte float format
   else if (!strcmp(type, "Double_t")
            || !strcmp(type, "Double32_t")
            || !strcmp(type, "double"))
      buf->Append("TID_DOUBLE");        //< 8 Byte float format
#if defined( R__B64 )          // Note: Long_t and ULong_t are currently not portable types
   else if (!strcmp(type, "Long_t")
            || !strcmp(type, "ULong_t")) {
      cout << "WARNING: Long_t and ULong_t is not supported on this architecture." << endl;
      buf->Append("TID_DOUBLE");        //< 8 Byte float format
   }
#else
   else if (!strcmp(type, "Long_t"))
      buf->Append("TID_INT");
   else if (!strcmp(type, "ULong_t"))
      buf->Append("TID_DWORD");
#endif
   else
      buf->Append("TID_STRING");        //< zero terminated string
}

void ROMEUtilities::SearchXMLFiles(ROMEStrArray& files, const char* filepath, const char* xmlpath)
{
   // search XML files which has 'xmlpath' from 'filepath' directory
   char *direntry;
   TString afile;
   void *dirp = gSystem->OpenDirectory(filepath);
   if (!dirp)
      return;
   ROMEXML *tmp;

   //loop on all entries of this directory
   while ((direntry=(char*)gSystem->GetDirEntry(dirp))) {
      TString afile = Form("%s/%s",filepath,direntry);

      if(!afile.EndsWith(".xml") && !afile.EndsWith(".XML"))
         continue;

      tmp = new ROMEXML();
      if (!tmp->OpenFileForRead(afile.Data()))
         continue;
      if(tmp->NumberOfOccurrenceOfPath(xmlpath))
         files.AddLast(afile);
      delete tmp;
   }

   return;
}
