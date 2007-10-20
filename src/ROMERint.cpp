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

#include <stdlib.h>
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include <TSystem.h>
#if defined( R__UNIX )
#   include <TUnixSystem.h>
#   include <signal.h>
#endif
#include <TSysEvtHandler.h>
#include <TSocket.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

#include "ROMERint.h"
#include "ROMEAnalyzer.h"
#include "ROMEString.h"
#include "TNetFolder.h"

//----- Interrupt signal handler -----------------------------------------------
class ROMEInterruptHandler : public TSignalHandler {
public:
   ROMEInterruptHandler() : TSignalHandler(kSigInterrupt, kFALSE) { }
   Bool_t  Notify();
};

//______________________________________________________________________________
static void cleaning()
{
   // cleaning at exit the program
   // all functions should be static
   ROMEAnalyzer::Cleaning();
}

#if defined( R__UNIX )
static struct Signalmap_t {
   int               fCode;
   SigHandler_t      fHandler;
   struct sigaction *fOldHandler;
   const char       *fSigName;
} gSignalMap[kMAXSIGNALS] = {       // the order of the signals should be identical
   { SIGBUS,   0, 0, "bus error" }, // to the one in SysEvtHandler.h
   { SIGSEGV,  0, 0, "segmentation violation" },
   { SIGSYS,   0, 0, "bad argument to system call" },
   { SIGPIPE,  0, 0, "write on a pipe with no one to read it" },
   { SIGILL,   0, 0, "illegal instruction" },
   { SIGQUIT,  0, 0, "quit" },
   { SIGINT,   0, 0, "interrupt" },
   { SIGWINCH, 0, 0, "window size change" },
   { SIGALRM,  0, 0, "alarm clock" },
   { SIGCHLD,  0, 0, "death of a child" },
   { SIGURG,   0, 0, "urgent data arrived on an I/O channel" },
   { SIGFPE,   0, 0, "floating point exception" },
   { SIGTERM,  0, 0, "termination signal" },
   { SIGUSR1,  0, 0, "user-defined signal 1" },
   { SIGUSR2,  0, 0, "user-defined signal 2" }
};

//______________________________________________________________________________
static void sighandler(int sig)
{
   // Call the signal handler associated with the signal.

   for (int i= 0; i < kMAXSIGNALS; i++) {
      if (gSignalMap[i].fCode == sig) {
         (*gSignalMap[i].fHandler)((ESignals)i);
         return;
      }
   }
}

#if defined(R__KCC)
   extern "C" {
      typedef void (*sighandlerFunc_t)(int);
   }
#endif

//______________________________________________________________________________
void UnixSignal(ESignals sig, SigHandler_t handler)
{
   // Set a signal handler for a signal.

   if (gSignalMap[sig].fHandler != handler) {
      struct sigaction sigact;

      gSignalMap[sig].fHandler    = handler;
      gSignalMap[sig].fOldHandler = new struct sigaction();

#if defined(R__SUN)
      sigact.sa_handler = (void (*)())sighandler;
#elif defined(R__SOLARIS)
      sigact.sa_handler = sighandler;
#elif defined(R__KCC)
      sigact.sa_handler = (sighandlerFunc_t)sighandler;
#elif (defined(R__SGI) && !defined(R__KCC)) || defined(R__LYNXOS)
#  if defined(R__SGI64) || (__GNUG__>=3)
      sigact.sa_handler = sighandler;
#  else
      sigact.sa_handler = (void (*)(...))sighandler;
#  endif
#else
      sigact.sa_handler = sighandler;
#endif
      sigemptyset(&sigact.sa_mask);
      sigact.sa_flags = 0;
#if defined(SA_RESTART)
      sigact.sa_flags |= SA_RESTART;
#endif
      if (sigaction(gSignalMap[sig].fCode, &sigact,
                    gSignalMap[sig].fOldHandler) < 0)
         gSystem->SysError("ROMERint::UnixSignal", "sigaction");
   }
}

//______________________________________________________________________________
static void SigHandler(ESignals sig)
{
   // Unix signal handler.

   cleaning();
   if (gSystem)
      ((TUnixSystem*)gSystem)->DispatchSignals(sig);
}
#endif

ClassImp(ROMERint)

//______________________________________________________________________________
Bool_t ROMEInterruptHandler::Notify()
{
   static Int_t nNotified = 0;
   nNotified++;
   cleaning();

   if (gROME->GetApplication()->isUseRintInterruptHandler()) {
      return gROME->GetApplication()->GetRintInterruptHandler()->Notify();
   }
   if (nNotified >= 2) {
      gSystem->Abort();
   } else {
      gROME->GetApplication()->Terminate(0);
   }
   return kTRUE;
}

//______________________________________________________________________________
void ROMERint::Terminate(int status)
{
   cleaning();
   TRint::Terminate(status);
}

//______________________________________________________________________________
ROMERint::ROMERint(const char *appClassName, int *argc, char **argv,
                   void *options, Int_t numOptions, Bool_t noLogo)
:TRint(appClassName, argc, argv, options, numOptions,noLogo)
,fRunning(kFALSE)
,fUseRintInterruptHandler(kFALSE)
,fRintInterruptHandler(0)
,fROMEInterruptHandler(0)
,fFPEMask(0)
,fRemoteProcess(kFALSE)
,fSocketClientNetFolder(0)
,fSocketClientHost("localhost")
,fSocketClientPort(9090)
{
   fRintInterruptHandler = gSystem->RemoveSignalHandler(GetSignalHandler());
   fROMEInterruptHandler = new ROMEInterruptHandler();
   fROMEInterruptHandler->Add();
   SetSignalHandler(fROMEInterruptHandler);
   fUseRintInterruptHandler = kFALSE;

#if defined( R__UNIX )
   //--- install default handlers
//   UnixSignal(kSigChild,                 SigHandler);
   UnixSignal(kSigBus,                   SigHandler);
   UnixSignal(kSigSegmentationViolation, SigHandler);
   UnixSignal(kSigIllegalInstruction,    SigHandler);
   UnixSignal(kSigSystem,                SigHandler);
   UnixSignal(kSigPipe,                  SigHandler);
   UnixSignal(kSigAlarm,                 SigHandler);
//   UnixSignal(kSigUrgent,                SigHandler);
   UnixSignal(kSigFloatingException,     SigHandler);
//   UnixSignal(kSigWindowChanged,         SigHandler);
#endif
}

//______________________________________________________________________________
ROMERint::~ROMERint()
{
   SafeDelete(fROMEInterruptHandler);
   SafeDelete(fSocketClientNetFolder);
}

//______________________________________________________________________________
Bool_t ROMERint::HandleTermInput()
{
   if (fRunning)
      return TRint::HandleTermInput();
   return true;
}

//______________________________________________________________________________
void ROMERint::Run(Bool_t retrn)
{
   fRunning = true;
   fUseRintInterruptHandler = kTRUE;
   TRint::Run(retrn);
   fUseRintInterruptHandler = kFALSE;
   fRunning = false;
}

//______________________________________________________________________________
Long_t ROMERint::ProcessLine(const char *line, Bool_t sync, Int_t *err)
{
   // process line in local session or remote application over a socket.

   if (!line || !*line) return 0;

   if (!fRemoteProcess) {
      return TRint::ProcessLine(line, sync, err);
   }

   // process CINT command in local session.
   TString cmd = line;
   cmd.Strip(TString::kLeading);
   if (cmd.BeginsWith(".")) {
      return TRint::ProcessLine(line, sync, err);
   }

   // process line on remote application over a socket
   if (ConnectSocketClient()) {
      fSocketClientNetFolder->ExecuteCommand(line);
   }

   return 0;
}

//______________________________________________________________________________
Long_t ROMERint::ProcessFile(const char *name, int *error)
{
   // process macro file in local session or remote application over a socket.

   if (!name || !*name) return 0;

   if (!fRemoteProcess) {
      return TRint::ProcessFile(name, error);
   }
   if (ConnectSocketClient()) {
      fSocketClientNetFolder->ExecuteMacro(name);
   }

   return 0;
}

//______________________________________________________________________________
void ROMERint::SetSocketClientConnection(const char* connection)
{
   ROMEString con = connection;
   con.StripSpaces();
   Int_t pos = con.Index(":", 1, 0, TString::kIgnoreCase);
   char *cstop = 0;

   if (!connection || !con.Length()) {
      fSocketClientHost = "localhost";
      fSocketClientPort = 9090;
   } else if (pos != -1) {
      fSocketClientHost = con(0, pos);
      fSocketClientPort = strtol(con(pos + 1, con.Length()).Data(), &cstop, 10);
   } else {
      fSocketClientHost = con;
      fSocketClientPort = 9090;
   }
}

//______________________________________________________________________________
Bool_t ROMERint::ConnectSocketClient()
{
   if (fSocketClientNetFolder != 0) {
      if (fSocketClientNetFolder->HaveValidSocket()) {
         return true;
      }
   }
   TSocket *sock = new TSocket (fSocketClientHost.Data(), fSocketClientPort);
   while (!sock->IsValid()) {
      delete sock;
      ROMEPrint::Warning("can not make socket connection to the ROME analyzer on host '%s' through port %d.\n",
                         fSocketClientHost.Data(), fSocketClientPort);
      ROMEPrint::Warning("program sleeps for 5s and tries again.\n");
      gSystem->Sleep(5000);
      sock = new TSocket (fSocketClientHost.Data(), fSocketClientPort);
   }
   SafeDelete(fSocketClientNetFolder);
   fSocketClientNetFolder = new TNetFolder("", "", sock, kTRUE);
   return true;
}
