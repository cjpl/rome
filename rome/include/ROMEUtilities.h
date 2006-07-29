/********************************************************************
  ROMEUtilities.h, J. Wouters LANL

  $Id$

********************************************************************/
#ifndef ROMEUtilities_H
#define ROMEUtilities_H

#include <Bytes.h>

class ROMEString;
class ROMEStrArray;

namespace ROMEUtilities {
   inline void ByteSwap( UShort_t *aValue );
   inline void ByteSwap( Short_t *aValue );
   inline void ByteSwap( UInt_t *aValue );
   inline void ByteSwap( Int_t *aValue );
   inline void ByteSwap( Float_t *aValue );
   inline void ByteSwap( ULong64_t *aValue );
   inline void ByteSwap( Long64_t *aValue );
   inline void ByteSwap( Double_t *aValue );
   void GetMidasTID(ROMEString *buf,Char_t *type);
   void SearchXMLFiles(ROMEStrArray& files, const char* filepath, const char* xmlpath);
}

//
// Byte swapping big endian <-> little endian
//

inline void ROMEUtilities::ByteSwap( UShort_t *aValue )
{
#if defined(R__USEASMSWAP)
   UShort_t tmp = Rbswap_16(*aValue);
   memcpy(aValue, &tmp, sizeof(UShort_t));
#else
   Byte_t _tmp;
   _tmp = *( (Byte_t *)( aValue ) );
   *( (Byte_t *)( aValue ) ) = *( ( (Byte_t *)( aValue ) ) + 1 );
   *( ( (Byte_t *)( aValue ) ) + 1 ) = _tmp;
#endif
}

inline void ROMEUtilities::ByteSwap( Short_t *aValue )
{
    ByteSwap( (UShort_t *)aValue );
}

inline void ROMEUtilities::ByteSwap( UInt_t *aValue )
{
#if defined(R__USEASMSWAP)
   UInt_t tmp = Rbswap_32(*aValue);
   memcpy(aValue, &tmp, sizeof(UInt_t));
#else
   Byte_t _tmp;
   _tmp = *( (Byte_t *)( aValue ) );
   *( (Byte_t *)( aValue ) ) = *( ( (Byte_t *)( aValue ) ) + 3 );
   *( ((Byte_t *)( aValue ) ) + 3) = _tmp;

   _tmp = *( ( (Byte_t *)( aValue ) ) + 1);
   *( ( (Byte_t *)( aValue ) ) + 1 ) = *( ( (Byte_t *)( aValue ) ) + 2 );
   *( ( (Byte_t *)( aValue ) ) + 2 ) = _tmp;
#endif
}

inline void ROMEUtilities::ByteSwap( Int_t *aValue )
{
    ByteSwap( (UInt_t *)aValue );
}

inline void ROMEUtilities::ByteSwap( Float_t *aValue )
{
    ByteSwap( (UInt_t *)aValue );
}

inline void ROMEUtilities::ByteSwap( ULong64_t *aValue )
{
#if defined(R__USEASMSWAP)
   ULong64_t tmp = Rbswap_64(*aValue);
   memcpy(aValue, &tmp, sizeof(ULong64_t));
#else
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
#endif
}

inline void ROMEUtilities::ByteSwap( Long64_t *aValue )
{
    ByteSwap( (ULong64_t *)aValue );
}

inline void ROMEUtilities::ByteSwap( Double_t *aValue )
{
    ByteSwap( (ULong64_t *)aValue );
}


#endif // ROMEUtilities_H
