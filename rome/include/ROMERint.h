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
#include "TSystem.h"
#include "TSysEvtHandler.h"
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

class ROMERint : public TRint {
private:
   Bool_t fRunning;
   Bool_t fInterruptHandlerEnabled; // flag if TRint signal handler is enabled
   TSignalHandler *fRintInterruptHandler; // original signal handler of TRint

public:
   ROMERint(const char *appClassName, int *argc, char **argv,
      void *options = 0, Int_t numOptions = 0, Bool_t noLogo = kFALSE)
      : TRint(appClassName, argc, argv, options, numOptions,noLogo) 
   {
      fRunning = false;
      fRintInterruptHandler = 0;
      fInterruptHandlerEnabled = kTRUE;
   };

   Bool_t HandleTermInput()
   {
      if (fRunning)
         return TRint::HandleTermInput();
      return true;
   }

   void Run(Bool_t retrn)
   {
      fRunning = true;
      TRint::Run(retrn);
      fRunning = false;
   }

   void SwitchInterruptHandler(Bool_t enabled)
   {
      if (enabled) {
         if ( fInterruptHandlerEnabled || !fRintInterruptHandler)
            return;
         fRintInterruptHandler->Add();
         SetSignalHandler(fRintInterruptHandler);
         fInterruptHandlerEnabled = kTRUE;
      }
      else {
         if ( !fInterruptHandlerEnabled )
            return;
         fRintInterruptHandler = gSystem->RemoveSignalHandler(GetSignalHandler());
         fInterruptHandlerEnabled = kFALSE;
      }
      return;
   }

   ClassDef(ROMERint, 0) // Customized TRint for ROME
};

#endif   // ROMERint_H
