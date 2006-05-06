/********************************************************************
  ROMERint.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMERint_H
#define ROMERint_H

#include <RConfig.h>
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
      void *options = 0, Int_t numOptions = 0, Bool_t noLogo = kFALSE)
      : TRint(appClassName, argc, argv, options, numOptions,noLogo) 
   { fRunning = false; };

   Bool_t HandleTermInput() {
             if (fRunning)
                return TRint::HandleTermInput();
             return true;
          }
   void   Run(Bool_t retrn) {
             fRunning = true;
             TRint::Run(retrn);
             fRunning = false;
          }

   ClassDef(ROMERint, 0) // Customized TRint for ROME
};

#endif   // ROMERint_H
