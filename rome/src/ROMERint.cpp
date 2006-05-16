// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMERint
//
// Customized TRint for ROME
//
//  $Id:$
//
//////////////////////////////////////////////////////////////////////////
#include "ROMERint.h"
#include "ROMEAnalyzer.h"

//----- Interrupt signal handler -----------------------------------------------
class ROMEInterruptHandler : public TSignalHandler {
public:
   ROMEInterruptHandler() : TSignalHandler(kSigInterrupt, kFALSE) { }
   Bool_t  Notify();
};

Bool_t ROMEInterruptHandler::Notify()
{
   gROME->ss_getchar(1);
   gROME->restoreOutput();
   if (strcmp(gROME->GetActiveDAQ()->GetName(), "midas") == 0
       && gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      cm_disconnect_experiment();
#endif
   }
   if (gROME->GetApplication()->isUseRintInterruptHandler())
      return gROME->GetApplication()->GetRintInterruptHandler()->Notify();
   gROME->GetApplication()->Terminate(0);
   return kTRUE;
}

ROMERint::ROMERint(const char *appClassName, int *argc, char **argv,
                   void *options, Int_t numOptions, Bool_t noLogo)
   : TRint(appClassName, argc, argv, options, numOptions,noLogo) 
{
   fRunning = false;
   fRintInterruptHandler = gSystem->RemoveSignalHandler(GetSignalHandler());
   fROMEInterruptHandler = new ROMEInterruptHandler();
   fROMEInterruptHandler->Add();
   SetSignalHandler(fROMEInterruptHandler);
   fUseRintInterruptHandler = kFALSE;
};

Bool_t ROMERint::HandleTermInput()
{
   if (fRunning)
      return TRint::HandleTermInput();
   return true;
}

void ROMERint::Run(Bool_t retrn)
{
   fRunning = true;
   fUseRintInterruptHandler = kTRUE;
   TRint::Run(retrn);
   fUseRintInterruptHandler = kFALSE;
   fRunning = false;
}
