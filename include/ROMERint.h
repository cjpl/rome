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
#include "ROMEUtilities.h"

class ROMERint : public TRint {
private:
   Bool_t          fRunning;
   Bool_t          fUseRintInterruptHandler; // flag if TRint signal handler is used
   TSignalHandler *fRintInterruptHandler;    // original signal handler of TRint
   TSignalHandler *fROMEInterruptHandler;    // special handler of ROMERint
   Int_t           fFPEMaskOriginal;         // FPE mask when this instance is created
   Int_t           fFPEMask;                 // Customized FPE mode

public:
   ROMERint(const char *appClassName, int *argc, char **argv,
            void *options = 0, Int_t numOptions = 0, Bool_t noLogo = kFALSE);
   virtual ~ROMERint();
   Bool_t          HandleTermInput();
   void            Run(Bool_t retrn);
   Bool_t          isUseRintInterruptHandler() { return fUseRintInterruptHandler; }
   TSignalHandler* GetRintInterruptHandler(){ return fRintInterruptHandler; }

   void            SetFPEMask(Int_t mode) { fFPEMask = mode; }
   void            SetFPEInvalid(Bool_t flag);
   void            SetFPEDivByZero(Bool_t flag);
   void            SetFPEOverflow(Bool_t flag);
   void            SetFPEUnderflow(Bool_t flag);
   void            SetFPEInexact(Bool_t flag);

   Int_t           GetFPEMask()           { return  fFPEMask; }
   Int_t           GetFPEMaskOriginal()   { return  fFPEMaskOriginal; }
   Bool_t          GetFPEInvalid()        { return  (fFPEMask & kInvalid)!=0; }
   Bool_t          GetFPEDivByZero()      { return  (fFPEMask & kDivByZero)!=0; }
   Bool_t          GetFPEOverflow()       { return  (fFPEMask & kOverflow)!=0; }
   Bool_t          GetFPEUnderflow()      { return  (fFPEMask & kUnderflow)!=0; }
   Bool_t          GetFPEInexact()        { return  (fFPEMask & kInexact)!=0; }

   void            EnableFPETrap();
   void            DisableFPETrap();
   void            RestoreFPETrap();

   ClassDef(ROMERint, 0) // Customized TRint for ROME
};

inline void ROMERint::SetFPEInvalid(Bool_t flag)
{
   if (flag) {
      fFPEMask |= kInvalid;
   } else {
      fFPEMask &= ~kInvalid;
   }
}

inline void ROMERint::SetFPEDivByZero(Bool_t flag)
{
   if (flag) {
      fFPEMask |= kDivByZero;
   } else {
      fFPEMask &= ~kDivByZero;
   }
}

inline void ROMERint::SetFPEOverflow(Bool_t flag)
{
   if (flag) {
      fFPEMask |= kOverflow;
   } else {
      fFPEMask &= ~kOverflow;
   }
}

inline void ROMERint::SetFPEUnderflow(Bool_t flag)
{
   if (flag) {
      fFPEMask |= kUnderflow;
   } else {
      fFPEMask &= ~kUnderflow;
   }
}

inline void ROMERint::SetFPEInexact(Bool_t flag)
{
   if (flag) {
      fFPEMask |= kInexact;
   } else {
      fFPEMask &= ~kInexact;
   }
}

inline void ROMERint::EnableFPETrap(){
   // Enable FPE trap with the mask set by GETFPE??? functions in this class
   ROMEUtilities::SetFPEMask(fFPEMask);
}

inline void ROMERint::DisableFPETrap()
{
   // Diable all FPE trap
   ROMEUtilities::SetFPEMask(kNoneMask);
}

inline void ROMERint::RestoreFPETrap()
{
   // Restore FPE trap mode to original one when this instance is created
   ROMEUtilities::SetFPEMask(fFPEMaskOriginal);
}

#endif   // ROMERint_H
