/********************************************************************
  ROMEUtilities.h, J. Wouters LANL

  $Id$

********************************************************************/
#ifndef ROMEUtilities_H
#define ROMEUtilities_H

#include <typeinfo>
#include <RConfig.h>
#include <Bytes.h>
#if defined( R__MACOSX )
#   include <libkern/OSByteOrder.h>
#endif

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
   const char* FastCrypt(const char *str);
   const char* FastDecrypt(const char *str);
#if !defined(__CINT__)
   const char* GetFormat(const type_info &t);
#endif
}

//
// Byte swapping big endian <-> little endian
//
inline void ROMEUtilities::ByteSwap( UShort_t *x )
{
#if defined( R__USEASMSWAP )
   *x = Rbswap_16(*x);
#elif defined( R__MACOSX )
   *x = OSSwapInt16(*x);
#else
   *x = ((UShort_t)((((UShort_t)(*x) & 0xff00) >> 8) |
                    (((UShort_t)(*x) & 0x00ff) << 8)));
#endif
}

inline void ROMEUtilities::ByteSwap( Short_t *x )
{
    ByteSwap( (UShort_t *)x );
}

inline void ROMEUtilities::ByteSwap( UInt_t *x )
{
#if defined( R__USEASMSWAP )
   *x = Rbswap_32(*x);
#elif defined( R__MACOSX )
   *x = OSSwapInt32(*x);
#else
   *x =  ((UInt_t)((((UInt_t)(*x) & 0xff000000) >> 24) |
                   (((UInt_t)(*x) & 0x00ff0000) >>  8) |
                   (((UInt_t)(*x) & 0x0000ff00) <<  8) |
                   (((UInt_t)(*x) & 0x000000ff) << 24)));
#endif
}

inline void ROMEUtilities::ByteSwap( Int_t *x )
{
    ByteSwap( (UInt_t *)x );
}

inline void ROMEUtilities::ByteSwap( Float_t *x )
{
    ByteSwap( (UInt_t *)x );
}

inline void ROMEUtilities::ByteSwap( ULong64_t *x )
{
#if defined( R__USEASMSWAP )
   *x = Rbswap_64(*x);
#elif defined( R__MACOSX )
   *x = OSSwapInt64(*x);
#elif defined( R__VISUAL_CPLUSPLUS )
   *x = ((ULong64_t)((((ULong64_t)(*x) & 0xff00000000000000UI64) >> 56) |
                     (((ULong64_t)(*x) & 0x00ff000000000000UI64) >> 40) |
                     (((ULong64_t)(*x) & 0x0000ff0000000000UI64) >> 24) |
                     (((ULong64_t)(*x) & 0x000000ff00000000UI64) >>  8) |
                     (((ULong64_t)(*x) & 0x00000000ff000000UI64) <<  8) |
                     (((ULong64_t)(*x) & 0x0000000000ff0000UI64) << 24) |
                     (((ULong64_t)(*x) & 0x000000000000ff00UI64) << 40) |
                     (((ULong64_t)(*x) & 0x00000000000000ffUI64) << 56)));
#else
   *x = ((ULong64_t)((((ULong64_t)(*x) & 0xff00000000000000ULL) >> 56) |
                     (((ULong64_t)(*x) & 0x00ff000000000000ULL) >> 40) |
                     (((ULong64_t)(*x) & 0x0000ff0000000000ULL) >> 24) |
                     (((ULong64_t)(*x) & 0x000000ff00000000ULL) >>  8) |
                     (((ULong64_t)(*x) & 0x00000000ff000000ULL) <<  8) |
                     (((ULong64_t)(*x) & 0x0000000000ff0000ULL) << 24) |
                     (((ULong64_t)(*x) & 0x000000000000ff00ULL) << 40) |
                     (((ULong64_t)(*x) & 0x00000000000000ffULL) << 56)));
#endif
}

inline void ROMEUtilities::ByteSwap( Long64_t *x )
{
    ByteSwap( (ULong64_t *)x );
}

inline void ROMEUtilities::ByteSwap( Double_t *x )
{
    ByteSwap( (ULong64_t *)x );
}


#endif // ROMEUtilities_H
