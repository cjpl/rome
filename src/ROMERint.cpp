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

//______________________________________________________________________________
static void cleaning()
{
   // cleaning at exit the program
   // all functions should be static
   ROMEAnalyzer::Cleaning();
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

   atexit((void (*)(void))cleaning);
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
#if defined(R__UNIX)
   dbgprintf("\n");
#endif
   fSocketClientNetFolder = new TNetFolder("", "", sock, kTRUE);
   return true;
}
