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
#include <TRint.h>
#include <TSysEvtHandler.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

class ROMERint : public TRint {
private:
   Bool_t          fRunning;
   Bool_t          fUseRintInterruptHandler; // flag if TRint signal handler is used
   TSignalHandler *fRintInterruptHandler;    // original signal handler of TRint
   TSignalHandler *fROMEInterruptHandler;    // original signal handler of TRint

public:
   ROMERint(const char *appClassName, int *argc, char **argv,
            void *options = 0, Int_t numOptions = 0, Bool_t noLogo = kFALSE);
   Bool_t          HandleTermInput();
   void            Run(Bool_t retrn);
   Bool_t          isUseRintInterruptHandler() { return fUseRintInterruptHandler; };
   TSignalHandler* GetRintInterruptHandler(){ return fRintInterruptHandler; };

   ClassDef(ROMERint, 0) // Customized TRint for ROME
};

#endif   // ROMERint_H
