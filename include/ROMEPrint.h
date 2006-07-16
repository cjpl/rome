/********************************************************************
  ROMEPrint.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEPrint_H
#define ROMEPrint_H

#include <RConfig.h>
#include <Rtypes.h>
#include <Varargs.h>

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
   virtual ~ROMEPrint(){}

   // Output
#ifndef __MAKECINT__
   static void  PrintAlways(const char *msgfmt, ...);
   static void  Debug(const char *msgfmt, ...);
   static void  Print(const char *msgfmt, ...);
   static void  Info(const char *msgfmt, ...);
   static void  Warning(const char *msgfmt, ...);
   static void  Error(const char *msgfmt, ...);
#endif
   static Int_t GetVerboseLevel() { return fgVerboseLevel; }
   static void  SetVerboseLevel(Int_t level = kNormal) { fgVerboseLevel = level; }

   ClassDef(ROMEPrint, 0) // Message system in ROME
};

#endif   // ROMEPrint_H
