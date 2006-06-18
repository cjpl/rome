/********************************************************************
  ROMEPrint.h, M. Schneebeli PSI

  $Id: ROMEPrint.h 1141 2006-06-10 14:54:29Z sawada $

********************************************************************/
#ifndef ROMEPrint_H
#define ROMEPrint_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TObject.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include <Varargs.h>

class ROMEPrint : public TObject
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
   static Int_t GetVerboseLevel() { return fgVerboseLevel; };
   static void  SetVerboseLevel(Int_t level = kNormal) { fgVerboseLevel = level; };

   ClassDef(ROMEPrint, 0) // Message system in ROME
};

#endif   // ROMEPrint_H
