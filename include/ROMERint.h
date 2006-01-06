/********************************************************************
  ROMERint.h, M. Schneebeli PSI

  $Id: ROMERint.h 480 2005-10-17 12:12:54Z schneebeli_m $

********************************************************************/
#ifndef ROMERint_H
#define ROMERint_H

#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include "TRint.h"
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

class ROMERint : public TRint {
private:
   Bool_t fRunning;
public:
   ROMERint(const char *appClassName, int *argc, char **argv,
      void *options = 0, int numOptions = 0, Bool_t noLogo = kFALSE)
      : TRint(appClassName, argc, argv, options, numOptions,noLogo) 
   { fRunning = false; };
   Bool_t HandleTermInput() {
      if (fRunning)
         return TRint::HandleTermInput();
      return true;
   }
   void Run(Bool_t retrn) {
      fRunning = true;
      TRint::Run(retrn);
      fRunning = false;
   }
};

#endif   // ROMERint_H
