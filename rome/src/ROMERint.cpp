// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMERint
//
// Customized TRint for ROME
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include <TSystem.h>
#include <TSysEvtHandler.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

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
   static Int_t nNotified = 0;
   nNotified++;
   gROME->Cleaning();

   if (gROME->GetApplication()->isUseRintInterruptHandler())
      return gROME->GetApplication()->GetRintInterruptHandler()->Notify();

#if defined( HAVE_MIDAS )
      cm_disconnect_experiment();
#endif
   if (nNotified >= 2) {
      gSystem->Abort();
   } else {
      gROME->GetApplication()->Terminate(0);
   }
   return kTRUE;
}

static void cleaning()
{
   // cleaning at exit the program
   if (gROME) {
      gROME->Cleaning();
   }
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

   fFPEMaskOriginal  = ROMEUtilities::GetFPEMask();
   fFPEMask  = fFPEMaskOriginal;

   atexit((void (*)(void))cleaning);
}

ROMERint::~ROMERint()
{
   SafeDelete(fROMEInterruptHandler);
}

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
