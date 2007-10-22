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

class TNetFolder;
class TSocket;

class ROMERint : public TRint {
private:
   Bool_t          fRunning;
   Bool_t          fUseRintInterruptHandler; // flag if TRint signal handler is used
   TSignalHandler *fRintInterruptHandler;    // original signal handler of TRint
   TSignalHandler *fROMEInterruptHandler;    // special handler of ROMERint
   Int_t           fFPEMask;                 // Customized FPE mode
   Bool_t          fRemoteProcess;           // Flag if lines are processed remotely
   TNetFolder     *fSocketClientNetFolder;   // Socket client for remote execution
   TString         fSocketClientHost;        // Hostname for remote execution
   Int_t           fSocketClientPort;        // Port number for remote execution

private:
   ROMERint(const ROMERint &app); // not implemented
   ROMERint &operator=(const ROMERint &rhs); // not implemented

public:
   ROMERint(const char *appClassName, int *argc, char **argv,
            void *options = 0, Int_t numOptions = 0, Bool_t noLogo = kFALSE);
   virtual ~ROMERint();

   Bool_t          HandleTermInput();
   Long_t          ProcessLine(const char *line, Bool_t sync = kFALSE, Int_t *error = 0);
   Long_t          ProcessFile(const char *name, int *error = 0);
   void            Run(Bool_t retrn);
   Bool_t          isUseRintInterruptHandler() const { return fUseRintInterruptHandler; }
   TSignalHandler* GetRintInterruptHandler() const { return fRintInterruptHandler; }

   void            SetFPEMask(Int_t mode) { fFPEMask = mode; }
   void            SetFPEInvalid(Bool_t flag);
   void            SetFPEDivByZero(Bool_t flag);
   void            SetFPEOverflow(Bool_t flag);
   void            SetFPEUnderflow(Bool_t flag);
   void            SetFPEInexact(Bool_t flag);

   Int_t           GetFPEMask() const      { return  fFPEMask; }
   Bool_t          GetFPEInvalid() const   { return  (fFPEMask & kInvalid)!=0; }
   Bool_t          GetFPEDivByZero() const { return  (fFPEMask & kDivByZero)!=0; }
   Bool_t          GetFPEOverflow() const  { return  (fFPEMask & kOverflow)!=0; }
   Bool_t          GetFPEUnderflow() const { return  (fFPEMask & kUnderflow)!=0; }
   Bool_t          GetFPEInexact() const   { return  (fFPEMask & kInexact)!=0; }

   void            EnableFPETrap() const   { ROMEUtilities::SetFPEMask(fFPEMask); }
   void            DisableFPETrap() const  { ROMEUtilities::SetFPEMask(kNoneMask); }

   void            SetRemoteProcess(Bool_t flg) { fRemoteProcess = flg; }
   void            SetSocketClientConnection(const char* connection = "localhost:9090");
   Bool_t          ConnectSocketClient();

   void            Terminate(int status);

   static Bool_t   InterruptHandler();
   static void     InterruptHandlerInt(Int_t i) { WarningSuppression(i); InterruptHandler(); }

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

#endif   // ROMERint_H
