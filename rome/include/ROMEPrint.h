/********************************************************************
  ROMEPrint.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEPrint_H
#define ROMEPrint_H

#include <RConfig.h>
#include <Rtypes.h>
#include <Varargs.h>
#include "ROME.h"

class ROMEPrint
{
public:
   // Verbose level
   enum {
      kMute            = 0,
      kErrorOnly       = 1,
      kErrorAndWarning = 2,
      kNormal          = 3,
      kDebug           = 4
   };

private:
   static Int_t fgVerboseLevel;          // Verbose level

public:
   ROMEPrint(){}
   virtual ~ROMEPrint() {}

   // Output
#ifndef __MAKECINT__
   static void  PrintAlways(const char *msgfmt, ...) G_GNUC_PRINTF(1, 2);
   static void  Debug(const char *msgfmt, ...) G_GNUC_PRINTF(1, 2);
   static void  Print(const char *msgfmt, ...) G_GNUC_PRINTF(1, 2);
   static void  Info(const char *msgfmt, ...) G_GNUC_PRINTF(1, 2);
   static void  Warning(const char *msgfmt, ...) G_GNUC_PRINTF(1, 2);
   static void  Error(const char *msgfmt, ...) G_GNUC_PRINTF(1, 2);
#endif
   static Int_t GetVerboseLevel() { return fgVerboseLevel; }
   static void  SetVerboseLevel(Int_t level = kNormal) { fgVerboseLevel = level; }

   ClassDef(ROMEPrint, 0) // Message system in ROME
};

#endif   // ROMEPrint_H
