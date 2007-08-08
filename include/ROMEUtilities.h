/********************************************************************
  ROMEUtilities.h, J. Wouters LANL

  $Id$

********************************************************************/
#ifndef ROMEUtilities_H
#define ROMEUtilities_H

#include <typeinfo>
#include <Bytes.h>

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#  pragma warning( push )
#  pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#include <TVirtualMutex.h>
#if defined( R__VISUAL_CPLUSPLUS )
#  pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#if defined( R__MACOSX )
#   include <libkern/OSByteOrder.h>
#endif

#if defined( R__VISUAL_CPLUSPLUS )
#   include <float.h>
#endif

class ROMEString;
class ROMEStrArray;

namespace ROMEUtilities {
   void ByteSwap(UShort_t *aValue);
   void ByteSwap(Short_t *aValue);
   void ByteSwap(UInt_t *aValue);
   void ByteSwap(Int_t *aValue);
   void ByteSwap(Float_t *aValue);
   void ByteSwap(ULong64_t *aValue);
   void ByteSwap(Long64_t *aValue);
   void ByteSwap(Double_t *aValue);

   Int_t GetFPEMask();
   Int_t SetFPEMask(const Int_t mask);

   void GetMidasTID(ROMEString *buf,Char_t *type);
   void SearchXMLFiles(ROMEStrArray& files, const char* filepath, const char* xmlpath);

   const char* FastCrypt(const char *str);
   const char* FastDecrypt(const char *str);

#if !defined(__CINT__)
   const char* GetFormat(const type_info &t);
#endif

}

const char kFalseTrueString[2][10] = {"false", "true"};

//
// Byte swapping big endian <-> little endian
//
inline void ROMEUtilities::ByteSwap(UShort_t *x)
{
#if defined( R__USEASMSWAP )
   *x = Rbswap_16(*x);
#elif defined( R__MACOSX )
   *x = OSSwapInt16(*x);
#else
   *x = static_cast<UShort_t>(((static_cast<UShort_t>(*x) & 0xff00) >> 8) |
                              ((static_cast<UShort_t>(*x) & 0x00ff) << 8));
#endif
}

inline void ROMEUtilities::ByteSwap(Short_t *x)
{
   ByteSwap(reinterpret_cast<UShort_t*>(x));
}

inline void ROMEUtilities::ByteSwap(UInt_t *x)
{
#if defined( R__USEASMSWAP )
   *x = Rbswap_32(*x);
#elif defined( R__MACOSX )
   *x = OSSwapInt32(*x);
#else
   *x = static_cast<UInt_t>(((static_cast<UInt_t>(*x) & 0xff000000) >> 24) |
                            ((static_cast<UInt_t>(*x) & 0x00ff0000) >>  8) |
                            ((static_cast<UInt_t>(*x) & 0x0000ff00) <<  8) |
                            ((static_cast<UInt_t>(*x) & 0x000000ff) << 24));
#endif
}

inline void ROMEUtilities::ByteSwap(Int_t *x)
{
   ByteSwap(reinterpret_cast<UInt_t *>(x));
}

inline void ROMEUtilities::ByteSwap(Float_t *x)
{
   ByteSwap(reinterpret_cast<UInt_t*>(x));
}

inline void ROMEUtilities::ByteSwap(ULong64_t *x)
{
#if defined( R__USEASMSWAP )
   *x = Rbswap_64(*x);
#elif defined( R__MACOSX )
   *x = OSSwapInt64(*x);
#elif defined( R__VISUAL_CPLUSPLUS )
   *x = static_cast<ULong64_t>(((static_cast<ULong64_t>(*x) & 0xff00000000000000UI64) >> 56) |
                               ((static_cast<ULong64_t>(*x) & 0x00ff000000000000UI64) >> 40) |
                               ((static_cast<ULong64_t>(*x) & 0x0000ff0000000000UI64) >> 24) |
                               ((static_cast<ULong64_t>(*x) & 0x000000ff00000000UI64) >>  8) |
                               ((static_cast<ULong64_t>(*x) & 0x00000000ff000000UI64) <<  8) |
                               ((static_cast<ULong64_t>(*x) & 0x0000000000ff0000UI64) << 24) |
                               ((static_cast<ULong64_t>(*x) & 0x000000000000ff00UI64) << 40) |
                               ((static_cast<ULong64_t>(*x) & 0x00000000000000ffUI64) << 56));
#else
   *x = static_cast<ULong64_t>(((static_cast<ULong64_t>(*x) & 0xff00000000000000ULL) >> 56) |
                               ((static_cast<ULong64_t>(*x) & 0x00ff000000000000ULL) >> 40) |
                               ((static_cast<ULong64_t>(*x) & 0x0000ff0000000000ULL) >> 24) |
                               ((static_cast<ULong64_t>(*x) & 0x000000ff00000000ULL) >>  8) |
                               ((static_cast<ULong64_t>(*x) & 0x00000000ff000000ULL) <<  8) |
                               ((static_cast<ULong64_t>(*x) & 0x0000000000ff0000ULL) << 24) |
                               ((static_cast<ULong64_t>(*x) & 0x000000000000ff00ULL) << 40) |
                               ((static_cast<ULong64_t>(*x) & 0x00000000000000ffULL) << 56));
#endif
}

inline void ROMEUtilities::ByteSwap(Long64_t *x)
{
   ByteSwap(reinterpret_cast<ULong64_t*>(x));
}

inline void ROMEUtilities::ByteSwap(Double_t *x)
{
   ByteSwap(reinterpret_cast<ULong64_t*>(x));
}

// Mutex

// Zero overhead macros in case not compiled with thread support
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,0,0))
#   define ROME_LOCKGUARD(mutex) R__LOCKGUARD2(mutex)
#else
#   define ROME_LOCKGUARD(mutex)                        \
   if (gAllocMutex && !mutex) {                         \
      gAllocMutex->Lock();                              \
      if (!mutex)                                       \
         mutex = new TVirtualMutex(kTRUE);              \
      gAllocMutex->UnLock();                            \
   }                                                    \
   R__LOCKGUARD(mutex)
#endif

inline Int_t ROME_TRYLOCK(TVirtualMutex* &mutex)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,0,0))
   if (gGlobalMutex && !mutex) {
      gGlobalMutex->Lock();
      if (!mutex)
         mutex = gGlobalMutex->Factory(kTRUE);
      gGlobalMutex->UnLock();
   }
#else
   if (gAllocMutex && !mutex) {
      gAllocMutex->Lock();
      if (!mutex)
         mutex = new TVirtualMutex(kTRUE);
      gAllocMutex->UnLock();
   }
#endif
   return mutex->TryLock();
}

#endif // ROMEUtilities_H
