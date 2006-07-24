// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//
// ROMEPrint
//
//  Message system in ROME
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#if defined( HAVE_MIDAS )
#   include "midas.h"
#endif
#include "Riostream.h"
#include "ROMEPrint.h"
#include "ROMEString.h"

ClassImp(ROMEPrint)

Int_t ROMEPrint::fgVerboseLevel = ROMEPrint::kNormal;

void ROMEPrint::PrintAlways(const char* va_(fmt),...)
{
// Print always independently of verbose level
// STDOUT
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
   va_end(ap);
}

void ROMEPrint::Debug(const char* va_(fmt),...)
{
// Print when verbose level >= kDebug
// STDOUT
   if (fgVerboseLevel < kDebug)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
   va_end(ap);
}

void ROMEPrint::Print(const char* va_(fmt),...)
{
// Print when verbose level >= kNormal
// STDOUT
   if (fgVerboseLevel < kNormal)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
   va_end(ap);
}

void ROMEPrint::Info(const char* va_(fmt),...)
{
// Print when verbose level >= kNormal
// STDOUT and midas message
   if (fgVerboseLevel < kNormal)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
#if defined( HAVE_MIDAS )
   ROMEString text = ROMEString::Format(va_(fmt), ap);
   cm_msg(MINFO, "ROMEPrint::Info", text.Data());
   cout<<text<<flush;
#else
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
#endif
   va_end(ap);
}

void ROMEPrint::Warning(const char* va_(fmt),...)
{
// Print when verbose level >= kErrorAndWarning
// STDERR and midas message
   if (fgVerboseLevel < kErrorAndWarning)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
#if defined( HAVE_MIDAS )
   ROMEString text = ROMEString::Format(va_(fmt), ap);
   cm_msg(MINFO, "ROMEPrint::Warning", text.Data());
   cerr<<text<<flush;
#else
   cerr<<ROMEString::Format(va_(fmt), ap)<<flush;
#endif
   va_end(ap);
}

void ROMEPrint::Error(const char* va_(fmt),...)
{
// Print when verbose level >= kError
// STDERR and midas message
   if (fgVerboseLevel < kErrorOnly)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
#if defined( HAVE_MIDAS )
   ROMEString text = ROMEString::Format(va_(fmt), ap);
   cm_msg(MERROR, "ROMEPrint::Error", text.Data());
   cerr<<text<<flush;
#else
   cerr<<ROMEString::Format(va_(fmt), ap)<<flush;
#endif
   va_end(ap);
}
