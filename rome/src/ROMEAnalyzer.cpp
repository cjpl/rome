// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMEAnalyzer
//
//  Basic class in ROME. This class is the base-class
//  for the analysis dependent main class, which should implement the
//  Folders, Trees and Task definitions.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   if !defined( __CINT__ )
#      include <Windows4Root.h>
#      include <conio.h>
#   endif
#   include <direct.h>
#endif

#if defined( R__UNIX )
#   include <unistd.h>
#   include <sys/ioctl.h>
#   include <termios.h>
#   include <sys/types.h>
#   include <sys/select.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#endif

#if !defined( R__MACOSX )
#   if defined( R__VISUAL_CPLUSPLUS )
#      include <io.h>
#   endif
#   if defined( R__UNIX )
#      include <sys/io.h>
#   endif
#endif // R__MACOSX
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#if defined( HAVE_MIDAS )
#   include "midas.h"
#endif

#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <TTask.h>
#include <TTree.h>
#include <TFolder.h>
#include <TList.h>
#include <TObjString.h>
#include <TPRegexp.h>
#include <TROOT.h>
#include <TBrowser.h>
#include <THtml.h>
#include <TSystem.h>
#include <TFile.h>
#include <TArrayI.h>
#include <TArrayL.h>
#include <TVirtualMutex.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROMENetFolderServer.h"
#include "ROMETask.h"
#include "ROMEAnalyzer.h"
#include "ROMEEventLoop.h"
#include "ROMEUtilities.h"
#include "ArgusWindow.h"
#include "ROMEiostream.h"
#if defined( HAVE_MIDAS )
#   include "ROMEMidasDAQ.h"
#endif

#include "ROMENetFolder.h"
#include "ROMERint.h"
#include "ROMEConfig.h"
#include "ArgusTab.h"
#include "ROMEDataBase.h"
#include "ROMEXML.h"

ClassImp(ROMEAnalyzer)

ROMEAnalyzer *gROME = 0;  // global ROMEAnalyzer Handle
static TVirtualMutex *fgRunEventNumberMutex = 0;
static TVirtualMutex *fgUpdateObjectStorageMutex = 0;
TVirtualMutex        *gObjectStorageMutex = 0;
extern const char*    kHTMLSourceDir; // must be declared in generated main.cpp

//______________________________________________________________________________
ROMEAnalyzer::ROMEAnalyzer(ROMERint *app, Bool_t batch, Bool_t daemon, Bool_t nographics, Int_t analysisMode,
                           const char* programName, const char* onlineName, ROMEConfig* config, int numNetFolder)
:TObject()
,fProgramMode(kStandAloneROME)
,fWindowClosed(kFALSE)
,fApplication(app)
,fCintInitialisation("")
,fActiveDAQ(0)
,fAnalysisMode(analysisMode)
,fAnalysisModeConfig(kAnalyzeOffline)
,fBatchMode(batch)
,fDaemonMode(daemon)
,fQuitMode(batch || daemon)
,fGraphicalConfigEdit(kTRUE)
,fPreserveConfig(kFALSE)
,fNoGraphics(batch || daemon || nographics)
,fSplashScreen(!nographics)
,fDontReadNextEvent(kFALSE)
,fSkipEvent(kFALSE)
,fInputDir("./")
,fInputDirConstructed("")
,fOutputDir("./")
,fOutputDirConstructed("")
,fOutputFileOption("RECREATE")
,fOutputObjOption(TObject::kOverwrite)
,fDataBaseDir(0)
,fDataBaseDirConstructed(0)
,fConfigDir("./")
,fConfigDirConstructed("")
,fMakeOutputDirectory(kFALSE)
,fCurrentRunNumber(0)
,fRunNumber()
,fRunNumberString("")
,fCurrentEventNumber(0)
,fEventNumber()
,fEventNumberString("")
,fCurrentInputFileName("")
,fInputFileNames()
,fInputFileNamesString("")
,fIOType(kNotBased)
,fUserEvent(kFALSE)
,fUserEventQ(kFALSE)
,fUserEventE(kFALSE)
,fUserEventS(kFALSE)
,fUserEventR(kFALSE)
,fUserEventO(kFALSE)
,fUserEventC(kFALSE)
,fUserEventJ(kFALSE)
,fUserEventJEventNumber(0)
,fUserEventG(kFALSE)
,fUserEventGRunNumber(0)
,fUserEventGEventNumber(0)
,fUserEventI(kFALSE)
,fEventID('a')
,fTerminate(kFALSE)
,fProgramTerminated(kFALSE)
,fFillEvent(kFALSE)
,fTaskObjects(0)
,fMainTask(0)
,fMainFolder(0)
,fHistoFiles(0)
,fNetFolderServer(0)
,fTreeObjects(new TObjArray(0))
,fTreeAccumulation(kFALSE)
,fMainHistoFolder(0)
,fHistoFolders(new TObjArray(0))
,fHistoRead(kFALSE)
,fHistoRun(0)
,fHistoWrite(kTRUE)
,fProgramName(programName)
,fOnlineHost("")
,fOnlineExperiment("")
,fOnlineAnalyzerName(onlineName)
,fOnlineMemoryBuffer("SYSTEM")
,fSocketServerActive(kFALSE)
,fSocketServerPortNumber(9090)
,fObjectStorageUpdated(kFALSE)
,fSocketClientNetFolder(0)
,fSocketClientHost("localhost")
,fSocketClientPort(9090)
,fStatistics(0)
,fMaxEventID(0)
,fDataBaseHandle(0)
,fDataBaseName(0)
,fDataBaseConnection(0)
,fNumberOfDataBases(0)
,fConfiguration(config)
,fShowRunStat(kTRUE)
,fEventBasedDataBase(kFALSE)
,fNumberOfNetFolders(numNetFolder)
,fNetFolder(0)
,fNetFolderActive(0)
,fNetFolderReconnect(0)
,fNetFolderSocket(0)
,fNetFolderPort(0)
,fNetFolderName(0)
,fNetFolderHost(0)
,fNetFolderRoot(0)
,fWindow(0)
,fWindowUpdatePeriod(1)
,fMidasOnlineDataBase(0)
{
// Initialisations
   fQuitMode = fQuitMode || !STDOutIsTerminal();
   fRunNumber.Reset();
   fEventNumber.Reset();
//   fInputFileNames.Reset();
}

//______________________________________________________________________________
ROMEAnalyzer::~ROMEAnalyzer()
{
   SafeDelete(fNetFolderServer);
   SafeDelete(fTreeObjects);
   SafeDelete(fHistoFolders);
   SafeDelete(fSocketClientNetFolder);
   SafeDelete(fTaskObjects);
   SafeDelete(fMainTask);
   SafeDelete(fMainFolder);
   SafeDelete(fMainHistoFolder);
   SafeDelete(fHistoFiles);
   SafeDelete(fHistoFolders);
   SafeDelete(fWindow);

   Int_t i;
   for(i = 0; i < fNumberOfNetFolders; i++) {
      SafeDelete(fNetFolder[i]);
      SafeDelete(fNetFolderSocket[i]);
   }
   SafeDeleteArray(fNetFolder);
   SafeDeleteArray(fNetFolderActive);
   SafeDeleteArray(fNetFolderReconnect);
   SafeDeleteArray(fNetFolderSocket);
   SafeDeleteArray(fNetFolderPort);
   SafeDeleteArray(fNetFolderName);
   SafeDeleteArray(fNetFolderHost);
   SafeDeleteArray(fNetFolderRoot);

   for(i = 0; i < fNumberOfDataBases; i++) {
      SafeDelete(fDataBaseHandle[i]);
   }
   SafeDeleteArray(fDataBaseConnection);
   SafeDeleteArray(fDataBaseName);
   SafeDeleteArray(fDataBaseDir);
   SafeDeleteArray(fDataBaseDirConstructed);
   SafeDeleteArray(fDataBaseHandle);
   SafeDeleteArray(fStatistics);
   gROME = 0;
   Cleaning();
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::Start(int argc, char **argv)
{
   if (this->isDaemonMode()) {
      ss_daemon_init(kFALSE);
   }

/*
   if (this->isBatchMode())
      ss_batch_init(kFALSE);
*/

   ROMEPrint::Debug("Starting analyzer\n");
// Starts the ROME Analyzer
   ROMEString text;

   // disable writing date and time in XML files written by mxml.
   ROMEXML::SuppressWritingDate();

   ROMEPrint::Debug("Executing init tasks\n");
   fMainTask->ExecuteTask("init");

   if (!ReadParameters(argc, argv)) {
      return false;
   }

   if (gROME->isNoGraphics()) {
      SetStandAloneROME();
   }

   if (!gROME->IsFileNameBasedIO() && !gROME->IsRunNumberAndFileNameBasedIO()) {
      // check of tree file name when <InputFileNames> is specified is not implemented yet.
      if (!CheckTreeFileNames()) {
         return false;
      }
   }

   // Commented out by shuei, because this makes directories with nice
   // name if #(or ##, ###) substitution is used in <OutputFilePath>
//   gSystem->MakeDirectory(this->GetOutputDir());

   static_cast<ROMEEventLoop*>(fMainTask)->AddTreeBranches();

   if (gROME->IsROMEMonitor()) {
      ConnectSocketClient();
   }
   if (!ConnectNetFolders()) {
      return false;
   }

   if (!this->isDaemonMode() && !this->isBatchMode()) {
      gROME->ss_getchar(0);
   }

   if (IsStandAloneROME() || IsROMEAndARGUS()) {
      consoleStartScreen();
      if (isSplashScreen()) {
         startSplashScreen();
      }

      if (isSocketServerActive()) {
         StartNetFolderServer();
      }

      if (!this->isBatchMode()) {
         ROMEPrint::Print("Program steering\n");
         ROMEPrint::Print("----------------\n");
         ROMEPrint::Print("q : Terminates the program\n");
         ROMEPrint::Print("e : Ends the program\n");
         ROMEPrint::Print("s : Stops the program\n");
         ROMEPrint::Print("r : Restarts the program\n");
         ROMEPrint::Print("c : Continuous Analysis\n");
         ROMEPrint::Print("o : Step by step Analysis\n");
         ROMEPrint::Print("g : Runs until event #\n");
         ROMEPrint::Print("j : Jumps to event #\n");
         ROMEPrint::Print("i : ROOT interpreter\n");
         ROMEPrint::Print("\n");
      }
   }
   if (IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
      ShowRunStat(false);
   }

   fMainTask->ExecuteTask("start");

   ROMEPrint::ReportSummary();

   StopNetFolderServer();

   ss_getchar(1);

   if (fTerminate) {
      return false;
   }

   return true;
}

//______________________________________________________________________________
void ROMEAnalyzer::ParameterUsage() const
{
   ROMEPrint::Print("  -i       Configuration file\n");
   ROMEPrint::Print("  -r       Runnumbers\n");
   ROMEPrint::Print("  -e       Eventnumbers\n");
   ROMEPrint::Print("  -pi      Inputpath\n");
   ROMEPrint::Print("  -po      Outputpath\n");
   ROMEPrint::Print("  -ph      Histospath\n");
   ROMEPrint::Print("  -m       Analysing Mode : (online/[offline])\n");
   ROMEPrint::Print("  -p       Program Mode : (0 : analyzer\n");
   ROMEPrint::Print("                           1 : monitor\n");
   ROMEPrint::Print("                           2 : analyzer and monitor\n");
   ROMEPrint::Print("                           3 : monitor connected to an analyzer)\n");
   ROMEPrint::Print("  -o       Start Analyzer in Step by Step Mode\n");
   ROMEPrint::Print("  -b       Batch Mode (no Argument)\n");
   ROMEPrint::Print("  -D       Daemon Mode (no Argument)\n");
   ROMEPrint::Print("  -q       Quit Mode (no Argument)\n");
   ROMEPrint::Print("  -v       Verbose level :(mute/error/warning/normal/verbose/debug)\n");
   ROMEPrint::Print("  -ng      No graphics is used\n");
   ROMEPrint::Print("  -c       Turn on graphical configuration panel\n");
   ROMEPrint::Print("  -nc      Turn off graphical configuration panel\n");
   ROMEPrint::Print("  -ns      Splash Screen is not displayed (no Argument)\n");
   ROMEPrint::Print("  -docu    Generates a Root-Html-Documentation (no Argument)\n");
   ROMEPrint::Print("  -I       Go into interactive session. Other options are ignored.\n");
   ROMEPrint::Print("  -R       Go into remote session. Other options are ignored.\n");
   ROMEPrint::Print("           -R hostname:port (e.g -R localhost:9090)\n");
   UserParameterUsage();
   return;
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::ReadParameters(int argc, char *argv[])
{
   // Reads the Inputlineparameters
   int i;
   Int_t graphicalConfigEditCommandOption = -1; // not specified
   char *cstop = 0;

   ROMEString configFile("");

   ROMEPrint::Debug("Reading command line options\n");
   for (i = 1 ; i < argc; i++) {
      if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help") || !strcmp(argv[i], "--help")) {
         ParameterUsage();
         return false;
      }
      if (!strcmp(argv[i], "-docu")) {
         THtml html;
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,15,2))
         html.SetProductName(fProgramName.Data());
#endif
         ROMEString sourceDir = html.GetSourceDir();
         sourceDir.Append(kHTMLSourceDir);
         html.SetSourceDir(sourceDir.Data());
         html.MakeAll(false);
         return false;
      }
      if (!strcmp(argv[i], "-i") && i < argc - 1) {
         if (i + 1 >= argc) {
            break;
         }
         configFile = argv[i + 1];
         i++;
      }
      if (!strcmp(argv[i], "-v")) {
         if (i + 1 >= argc) {
            break;
         }
         Int_t verboseLevel = strtol(argv[i + 1], &cstop, 10);
         if (!strcmp(argv[i + 1], "mute") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kMute)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kMute);
         } else if (!strcmp(argv[i + 1], "error") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kError)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kError);
         } else if (!strcmp(argv[i + 1], "warning") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kWarning)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kWarning);
         } else if (!strcmp(argv[i + 1], "normal") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kNormal)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kNormal);
         } else if (!strcmp(argv[i + 1], "verbose") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kVerbose)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kVerbose);
         } else if (!strcmp(argv[i + 1], "debug") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kDebug)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kDebug);
         }
         i++;
      }
      if (!strcmp(argv[i], "-c")) {
         graphicalConfigEditCommandOption = 1;
      }
      if (!strcmp(argv[i], "-nc")) {
         graphicalConfigEditCommandOption = 0;
      }
   }

   ROMEString answerString;
   ROMEString printString;
   char answerLine[80];

   if (!configFile.Length()) {
      ROMEStrArray foundFiles;
      foundFiles.RemoveAll();
      TString curDir = gSystem->WorkingDirectory();
      TString dirName = curDir;
      ROMEUtilities::SearchXMLFiles(foundFiles, dirName.Data(), "/Configuration");
      dirName += "/config";
      ROMEUtilities::SearchXMLFiles(foundFiles, dirName.Data(), "/Configuration");
      const int nFile = foundFiles.GetEntries();
      for (i = 0; i < nFile; i++) {
         if (foundFiles.At(i).BeginsWith(curDir.Data())) {
            foundFiles.AddAt(foundFiles.At(i).Remove(0, curDir.Length() + 1), i);
         }
      }

      if (foundFiles.GetEntries() == 0) {
         ;
      } else if (foundFiles.GetEntries() == 1) {
         configFile = foundFiles.At(0);
      } else {
         if (this->isBatchMode() || this->isDaemonMode()) {
            ROMEPrint::Error("Several configuration files were found.\n");
            for (i = 0; i < nFile; i++) {
               ROMEPrint::Error("   %s\n", foundFiles.At(i).Data());
            }
            ROMEPrint::Error("Please specify with -i option.\n");
            return false;
         } else {
            i = -1;
            while (i < 0 || i >= nFile) {
               ROMEPrint::PrintAlways("Please select a configuration file.\n");
               for (i = 0; i < nFile; i++) {
                  ROMEPrint::PrintAlways("   [%d] %s\n", i, foundFiles.At(i).Data());
               }
               ROMEPrint::PrintAlways("   [q] Quit\n");
               ROMEPrint::PrintAlways("File number: ");
               cin.getline(answerLine, sizeof(answerLine));
               answerString = answerLine;
               if (answerString == "q" || answerString == "Q") {
                  return false;
               }
               if (answerString.Index(".") != -1) {
                  graphicalConfigEditCommandOption = 0;
                  answerString = answerString(0, answerString.Index("."));
               }
               i = answerString.ToInteger();
               if (!answerString.IsDigit() || i < 0 || i >= nFile) {
                  ROMEPrint::PrintAlways("File number %s is not found.\n", answerString.Data());
                  i = -1;
               }
            }
            configFile = foundFiles[i];
         }
      }
   }

   char answer = 0;
   bool overwrite = true;
   if ( !configFile.Length() || gSystem->AccessPathName(configFile.Data(), kFileExists)) {
      if (isBatchMode() || isDaemonMode()) {
         if (configFile.Length()) {
            ROMEPrint::Error("Configuration file '%s' not found.\n", configFile.Data());
         } else {
            ROMEPrint::Error("Please specify configuration file with -i option.\n");
         }
         return false;
      }

      if (configFile.Length()) {
         ROMEPrint::Warning("Configuration file '%s' not found.", configFile.Data());
      } else {
         configFile = "romeConfig.xml";
      }

      ROMEPrint::PrintAlways("\n");
      ROMEPrint::PrintAlways("The framework can generate a new configuration file for you.\n");
      ROMEPrint::PrintAlways("Available configuration types are :\n");
      ROMEPrint::PrintAlways("   [R] ROME Framework\n");
      ROMEPrint::PrintAlways("   [A] ARGUS Monitor\n");
      ROMEPrint::PrintAlways("   [M] ROME Framework with ARGUS Monitor\n");
      ROMEPrint::PrintAlways("   [N] Don't generate a configuration file\n");
      ROMEPrint::PrintAlways("Please select a configuration type[R/A/M/N]: ");
      gROME->ss_getchar(0);
      while (answer == 0) {
         while (this->ss_kbhit()) {
            answer = this->ss_getchar(0);
         }
      }
      gROME->ss_getchar(1);
      answerString = answer;
      answerString.ToUpper();
      if (answerString == "R" || answerString == "A" || answerString == "M") {
         if (answerString == "R") {
            SetStandAloneROME();
         } else if (answerString == "A") {
            SetStandAloneARGUS();
         } else if (answerString == "M") {
            SetROMEAndARGUS();
         }
         ROMEPrint::PrintAlways("\nPlease specify file name (default='%s'): ", configFile.Data());
         cin.getline(answerLine, sizeof(answerLine));
         if (strlen(answerLine)) {
            configFile = answerLine;
         }
         if (!configFile.EndsWith(".xml") && !configFile.EndsWith(".XML")) {
            configFile += ".xml";
         }
         if (!gSystem->AccessPathName(configFile.Data(), kFileExists)) {
            ROMEPrint::PrintAlways("overwrite '%s'? ", configFile.Data());
/*
            gROME->ss_getchar(0);
            while (answer == 0) {
               while (this->ss_kbhit()) {
                  answer = this->ss_getchar(0);
               }
            }
            gROME->ss_getchar(1);
*/
            cin>>answer;
            if (answer != 'y' && answer != 'Y') {
               overwrite = false;
            }
         }
         ROMEPrint::PrintAlways("\nPlease specify amount of comments in file [0-5], default=1(few comments): ");
         cin.getline(answerLine, sizeof(answerLine));
         if (strlen(answerLine)) {
            fConfiguration->SetCommentLevel(strtol(answerLine, &cstop, 10));
         }
         if (overwrite) {
            if (!this->WriteConfigurationFile(configFile)) {
               ROMEPrint::Print("\nTerminate program.\n");
               return false;
            }
            ROMEPrint::Print("\nThe framework generated a new configuration file.\n%s\n", configFile.Data());
            ROMEPrint::Print("Please edit this file and restart the program.\n");
         }
      } else {
         ROMEPrint::Print("\nTerminate program.\n");
      }
      return false;
   }
   ROMEPrint::Print("Using configuration file %s\n", configFile.Data());
   if (!this->GetConfiguration()->ReadConfigurationFile(configFile.Data())) {
      ROMEPrint::Print("\nTerminate program.\n");
      return false;
   }
   if (!isNoGraphics() /* GUI available */ &&
       graphicalConfigEditCommandOption != 0 /* user didn't disabled explicitly */ &&
       ((graphicalConfigEditCommandOption < 0 /* user didn't specify by command option */ && isGraphicalConfigEdit()) ||
        graphicalConfigEditCommandOption == 1 /* user enabled by command option */)) {
      if (!this->ShowConfigurationFile()) {
         ROMEPrint::Print("\nTerminate program.\n");
         return false;
      }
   }

   if (fAnalysisMode == kAnalyzeNotSpecified) {
      fAnalysisMode = kAnalyzeOffline;
   }

   if (!this->WriteConfigurationFile(configFile)) {
      ROMEPrint::Print("\nTerminate program.\n");
      return false;
   }

   fMainTask->PropagateDeactivation();

   ROMEPrint::Debug("Reading command line options\n");
   for (i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "-q")) {
         fQuitMode = true;
      } else if (!strcmp(argv[i], "-v")) {
         if (i + 1 >= argc) {
            break;
         }
         Int_t verboseLevel = strtol(argv[i + 1], &cstop, 10);
         if (!strcmp(argv[i + 1], "mute") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kMute)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kMute);
         } else if (!strcmp(argv[i + 1], "error") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kError)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kError);
         } else if (!strcmp(argv[i + 1], "warning") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kWarning)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kWarning);
         } else if (!strcmp(argv[i + 1], "normal") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kNormal)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kNormal);
         } else if (!strcmp(argv[i + 1], "verbose") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kVerbose)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kVerbose);
         } else if (!strcmp(argv[i + 1], "debug") || ((!cstop || !strlen(cstop)) && verboseLevel == ROMEPrint::kDebug)) {
            ROMEPrint::SetVerboseLevel(ROMEPrint::kDebug);
         }
         i++;
      } else if (!strcmp(argv[i], "-ns")) {
         fSplashScreen = false;
      } else if (!strcmp(argv[i], "-c")) {
      } else if (!strcmp(argv[i], "-nc")) {
      } else if (!strcmp(argv[i], "-m")) {
         if (i + 1 >= argc) {
            break;
         }
         if (!strcmp(argv[i + 1], "online")) {
            this->SetOnline();
         } else {
            this->SetOffline();
         }
         i++;
      } else if (!strcmp(argv[i], "-p")) {
         if (i + 1 >= argc) {
            break;
         }
         if (!strcmp(argv[i + 1], "1")) {
            this->SetStandAloneARGUS();
         } else if (!strcmp(argv[i + 1], "2")) {
            this->SetROMEAndARGUS();
         } else if (!strcmp(argv[i + 1], "3")) {
            this->SetROMEMonitor();
         } else {
            this->SetStandAloneROME();
         }
         i++;
      } else if (!strcmp(argv[i], "-r") && i < argc - 1) {
         if (i + 1 >= argc) {
            break;
         }
         this->SetRunNumbers(argv[i + 1]);
         i++;
      } else if (!strcmp(argv[i], "-e") && i < argc - 1) {
         if (i + 1 >= argc) {
            break;
         }
         this->SetEventNumbers(argv[i + 1]);
         i++;
      } else if (!strcmp(argv[i], "-o")) {
         static_cast<ROMEEventLoop*>(fMainTask)->SetContinuousMode(false);
      } else if (!strcmp(argv[i], "-i")) {
         if (i + 1 >= argc) {
            break;
         }
         i++;
      } else if (!strcmp(argv[i], "-pi") && i < argc - 1) {
         if (i + 1 >= argc) {
            break;
         }
         this->SetInputDir(argv[i + 1]);
         i++;
      } else if (!strcmp(argv[i], "-po") && i < argc - 1) {
         if (i + 1 >= argc) {
            break;
         }
         this->SetOutputDir(argv[i + 1]);
         i++;
      } else if (!strcmp(argv[i], "-ph") && i < argc - 1) {
         if (i + 1 >= argc) {
            break;
         }
         this->SetHistosPath(argv[i + 1]);
         i++;
      } else if (!strcmp(argv[i], "-b")) {
         fBatchMode = kTRUE;
      } else if (!strcmp(argv[i], "-D")) {
         fDaemonMode = kTRUE;
      } else if (!strcmp(argv[i], "-ng")) {
         fNoGraphics = kTRUE;
      } else if (!strcmp(argv[i], "-I")) {
      } else if (!strcmp(argv[i], "-R")) {
         if (i + 1 >= argc) {
            break;
         }
         i++;
      } else if (!ReadUserParameter(argv[i], i < argc - 1 ? argv[i + 1] : "", i)) {
         ROMEPrint::Error("Input line parameter '%s' not available.\n", argv[i]);
         ROMEPrint::Print("Available input line parameters are :\n");
         ParameterUsage();
         return false;
      }
   }
   fNoGraphics = fBatchMode || fDaemonMode || fNoGraphics;
   fQuitMode = fBatchMode || fDaemonMode || fQuitMode;
   fQuitMode = fQuitMode || !STDOutIsTerminal();
   fSplashScreen =  fSplashScreen && !fNoGraphics;

   return true;
}

//______________________________________________________________________________
Int_t ROMEAnalyzer::CheckEventNumber(Long64_t eventNumber) const
{
   return CheckNumber(eventNumber, fEventNumber);
}

//______________________________________________________________________________
Int_t ROMEAnalyzer::CheckRunNumber(Long64_t runNumber) const
{
   return CheckNumber(runNumber, fRunNumber);
}

//______________________________________________________________________________
Int_t ROMEAnalyzer::CheckNumber(Long64_t number, const TArrayL64 &numbers)
{
   const int nNumbers = numbers.GetSize();
   if (nNumbers == 0) {
      return 1;
   }
   for (Int_t i = 0; i < nNumbers; i++) {
      if (TMath::Abs(numbers.At(i)) > number) {
         return 0;
      }
      if (numbers.At(i) < 0) {
         if (TMath::Abs(numbers.At(i)) <= number && TMath::Abs(numbers.At(i + 1)) >= number) {
            return 1;
         }
      } else {
         if (numbers.At(i) == number) {
            return 1;
         }
      }
   }
   return -1;
}

//______________________________________________________________________________
Long64_t ROMEAnalyzer::GetNextRunNumber(const Long64_t runNumber) const
{
   const Int_t nRunNumber = fRunNumber.GetSize();
   Int_t i;

   // search current run number from the list
   for (i = 0; i < nRunNumber; i++) {
      if (fRunNumber.At(i) < 0) {
         if (TMath::Abs(fRunNumber.At(i)) <= runNumber && TMath::Abs(fRunNumber.At(i + 1)) > runNumber) {
            return runNumber + 1;
         }
      } else {
         if (fRunNumber.At(i) == runNumber && i < nRunNumber - 1) {
            return TMath::Abs(fRunNumber.At(i + 1));
         }
      }
   }

   // in case that current run number is not in the list,
   // return first run number larther than current run number
   for (i = 0; i < nRunNumber; i++) {
      if (TMath::Abs(fRunNumber.At(i)) > runNumber) {
         return TMath::Abs(fRunNumber.At(i));
      }
   }

   return -1;
}

//______________________________________________________________________________
void ROMEAnalyzer::DecodeNumbers(ROMEString& str, TArrayL64& arr)
{
   char cminus = '-';
   char ccomma = ',';
   char csemi  =';';
   char *tmpStr = new char[str.Length() + 1];
   strcpy(tmpStr, str.Data());
   char *pstr = tmpStr;
   char *pend = pstr + str.Length();
   Long64_t num;
   Int_t na  = 0;
   Int_t nat = 1;
   arr.Set(10);
   Int_t arraySize = arr.GetSize();
   while (pstr < pend) {
      if (na >= arraySize * nat) {
         nat++;
         arr.Set(arraySize * nat);
      }
      num = strtol(pstr, &pstr, 10);
      if (pstr[0] == cminus) {
         arr.AddAt(-num, na);
         if (num == 0) {
            na++;
            arr.AddAt(-1, na);
         }
         na++;
         pstr++;
      } else if (pstr[0] == ccomma || pstr[0] == csemi) {
         arr.AddAt(num, na);
         na++;
         pstr++;
      } else {
         arr.AddAt(num, na);
         arr.Set(na + 1);
         delete [] tmpStr;
         return;
      }
   }
   arr.Set(na);
   delete [] tmpStr;
}

//______________________________________________________________________________
void ROMEAnalyzer::DecodeInputFileNames(ROMEString& str, ROMEStrArray& arr)
{
   int ind, num=0;
   TString temp;
   ROMEString str2 = str;
   while ((ind=str2.Index(",")) != -1) {
      temp = str2(0, ind);
      arr.AddAtAndExpand(temp, num);
      num++;
      str2 = str2(ind + 1, str2.Length()-ind - 1);
   }
   if (str.Length() > 0) {
      arr.AddAtAndExpand(str, num);
   }
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::toBool(Int_t value)
{
   return value != 0;
}

//______________________________________________________________________________
UInt_t ROMEAnalyzer::ss_kbhit()
{
#if defined( R__VISUAL_CPLUSPLUS )
   return toBool(kbhit());
#endif
#if defined( R__UNIX )
   int n;
   ioctl(0, FIONREAD, &n);
   return (n > 0);
#endif
}

//______________________________________________________________________________
/*int ROMEAnalyzer::ss_sleep(int millisec)
{
   fd_set readfds;
   struct timeval timeout;
   int status;
   static int sock = 0;

   timeout.tv_sec = millisec / 1000;
   timeout.tv_usec = (millisec % 1000) * 1000;

   if (!sock)
      sock = socket(AF_INET, SOCK_DGRAM, 0);

   FD_ZERO(&readfds);
   FD_SET(sock, &readfds);
   do {
      status = select(FD_SETSIZE, &readfds, 0, 0, &timeout);

      // if an alarm signal was cought, restart select with reduced timeout
      if (status == -1 && timeout.tv_sec >= 1)
         timeout.tv_sec -= 1;

   } while (status == -1);      // dont return if an alarm signal was cought

   return 1;
}*/

#define CH_BS             8
#define CH_TAB            9
#define CH_CR            13
#define CH_EXT 0x100
#define CH_HOME   (CH_EXT+0)
#define CH_INSERT (CH_EXT+1)
#define CH_DELETE (CH_EXT+2)
#define CH_END    (CH_EXT+3)
#define CH_PUP    (CH_EXT+4)
#define CH_PDOWN  (CH_EXT+5)
#define CH_UP     (CH_EXT+6)
#define CH_DOWN   (CH_EXT+7)
#define CH_RIGHT  (CH_EXT+8)
#define CH_LEFT   (CH_EXT+9)

//______________________________________________________________________________
Int_t ROMEAnalyzer::ss_getchar(UInt_t reset)
{
#if defined( R__UNIX )

   // do nothing when STDIN is redirected
   if(!STDOutIsTerminal())
      return 0;

   static unsigned long int init = 0;
   static struct termios save_termios;
   struct termios buf;
   int i, fd;
   char c[3];

   fd = fileno(stdin);

   if (reset) {
      if (init) {
         tcsetattr(fd, TCSAFLUSH, &save_termios);
      }
      init = 0;
      return 0;
   }

   if (!init) {
      tcgetattr(fd, &save_termios);
      memcpy(&buf, &save_termios, sizeof(buf));

      buf.c_lflag &= ~(ECHO | ICANON | IEXTEN);

      buf.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON);

      buf.c_cflag &= ~(CSIZE | PARENB);
      buf.c_cflag |= CS8;
      /* buf.c_oflag &= ~(OPOST); */
      buf.c_cc[VMIN] = 0;
      buf.c_cc[VTIME] = 0;

      tcsetattr(fd, TCSAFLUSH, &buf);
      init = 1;
   }

   memset(c, 0, 3);
   i = read(fd, c, 1);

   if (i == 0) {
      return 0;
   }

   /* check if ESC */
   if (c[0] == 27) {
      i = read(fd, c, 2);
      if (i == 0) {              /* return if only ESC */
         return 27;
      }

      /* cursor keys return 2 chars, others 3 chars */
      if (c[1] < 65) {
         i = read(fd, c, 1); // return value of 'read' must be used in GCC4.
      }

      /* convert ESC sequence to CH_xxx */
      switch (c[1]) {
      case 49:
         return CH_HOME;
      case 50:
         return CH_INSERT;
      case 51:
         return CH_DELETE;
      case 52:
         return CH_END;
      case 53:
         return CH_PUP;
      case 54:
         return CH_PDOWN;
      case 65:
         return CH_UP;
      case 66:
         return CH_DOWN;
      case 67:
         return CH_RIGHT;
      case 68:
         return CH_LEFT;
      }
   }

   /* BS/DEL -> BS */
   if (c[0] == 127) {
      return CH_BS;
   }

   return c[0];

#elif defined( R__VISUAL_CPLUSPLUS )

   static bool init = false;
   static int repeat_count = 0;
   static int repeat_char;
   HANDLE hConsole;
   unsigned long nCharsRead;
   INPUT_RECORD ir;
   OSVERSIONINFO vi;

   /* find out if we are under W95 */
   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   if (vi.dwPlatformId != VER_PLATFORM_WIN32_NT) {
      /* under W95, console doesn't work properly */
      int c;

      if (!kbhit()) {
         return 0;
      }

      c = getch();
      if (c == 224) {
         c = getch();
         switch (c) {
         case 71:
            return CH_HOME;
         case 72:
            return CH_UP;
         case 73:
            return CH_PUP;
         case 75:
            return CH_LEFT;
         case 77:
            return CH_RIGHT;
         case 79:
            return CH_END;
         case 80:
            return CH_DOWN;
         case 81:
            return CH_PDOWN;
         case 82:
            return CH_INSERT;
         case 83:
            return CH_DELETE;
         }
      }
      return c;
   }

   hConsole = GetStdHandle(STD_INPUT_HANDLE);

   if (reset) {
      SetConsoleMode(hConsole, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT |
                     ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
      init = FALSE;
      return 0;
   }

   if (!init) {
      SetConsoleMode(hConsole, ENABLE_PROCESSED_INPUT);
      init = TRUE;
   }

   if (repeat_count) {
      repeat_count--;
      return repeat_char;
   }

   PeekConsoleInput(hConsole, &ir, 1, &nCharsRead);

   if (nCharsRead == 0) {
      return 0;
   }

   ReadConsoleInput(hConsole, &ir, 1, &nCharsRead);

   if (ir.EventType != KEY_EVENT) {
      return ss_getchar(0);
   }

   if (!ir.Event.KeyEvent.bKeyDown) {
      return ss_getchar(0);
   }

   if (ir.Event.KeyEvent.wRepeatCount > 1) {
      repeat_count = ir.Event.KeyEvent.wRepeatCount - 1;
      repeat_char = ir.Event.KeyEvent.uChar.AsciiChar;
      return repeat_char;
   }

   if (ir.Event.KeyEvent.uChar.AsciiChar) {
      return ir.Event.KeyEvent.uChar.AsciiChar;
   }

   if (ir.Event.KeyEvent.dwControlKeyState & (ENHANCED_KEY)) {
      switch (ir.Event.KeyEvent.wVirtualKeyCode) {
      case 33:
         return CH_PUP;
      case 34:
         return CH_PDOWN;
      case 35:
         return CH_END;
      case 36:
         return CH_HOME;
      case 37:
         return CH_LEFT;
      case 38:
         return CH_UP;
      case 39:
         return CH_RIGHT;
      case 40:
         return CH_DOWN;
      case 45:
         return CH_INSERT;
      case 46:
         return CH_DELETE;
      }

      return ir.Event.KeyEvent.wVirtualKeyCode;
   }

   return ss_getchar(0);
#else
   return -1;
#endif
}

//______________________________________________________________________________
Int_t ROMEAnalyzer::ss_daemon_init(Bool_t keep_stdout)
{
#if defined( R__UNIX )   // only implemented for UNIX
   int i, fd;
   int pid = fork();

   if (pid < 0) {
      return kFALSE;
   } else if (pid != 0) {
      ROMEPrint::Print("Becoming a daemon... (PID = %d)\n", pid);
      fApplication->Terminate(0); // parent finished
   }


   /* try and use up stdin, stdout and stderr, so other
      routines writing to stdout etc won't cause havoc. Copied from smbd */
   for (i = 0; i < 3; i++) {
      if (keep_stdout && ((i == 1) || (i == 2))) {
         continue;
      }

      close(i);
      fd = open("/dev/null", O_RDWR, 0);
      if (fd < 0) {
         fd = open("/dev/null", O_WRONLY, 0);
      }
      if (fd < 0) {
         ROMEPrint::Error("Can't open /dev/null\n");
         return kFALSE;
      }
      if (fd != i) {
         ROMEPrint::Error("Did not get file descriptor\n");
         return kFALSE;
      }
   }

   setsid();                    // become session leader
   umask(0);                    // clear our file mode createion mask
#endif
   return kTRUE;
}

//______________________________________________________________________________
Int_t ROMEAnalyzer::ss_batch_init()
{
// redirect std input.
// this method is not used yet.
#if defined( R__UNIX )   // only implemented for UNIX
   int fd;

   close(0);
   fd = open("/dev/null", O_RDWR, 0);
   if (fd < 0) {
      fd = open("/dev/null", O_WRONLY, 0);
   }
   if (fd < 0) {
      ROMEPrint::Error("Can't open /dev/null\n");
      return kFALSE;
   }
   if (fd != 0) {
      ROMEPrint::Error("Did not get file descriptor\n");
      return kFALSE;
   }

   umask(0);                    // clear our file mode createion mask
#endif
   return kTRUE;
}

//______________________________________________________________________________
Int_t ROMEAnalyzer::stricmp(const char* c1, const char* c2)
{
#if defined( R__UNIX )
   return strcasecmp(c1, c2);
#elif defined( R__VISUAL_CPLUSPLUS )
   return _stricmp(c1, c2);
#else
   return 0;
#endif
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::strtobool(const char* str)
{
   char *cstop;
   if (!strcmp(str, "true")) {
      return true;
   }
   if (!strcmp(str, "false")) {
      return false;
   }
   return strtol(str, &cstop, 10) != 0;
}

// stream
//______________________________________________________________________________
void ROMEAnalyzer::redirectOutput(Bool_t redirect)
{
   static ofstream *romeOutputFile = 0;
   static streambuf *oldbuf = 0;
   if (redirect) {
      // redirect
      if (!oldbuf) {
         oldbuf = cout.rdbuf();
      }
      if (!romeOutputFile) {
         romeOutputFile = new ofstream("romeOutput.txt");
      }
      cout.rdbuf(romeOutputFile->rdbuf());
   } else {
      // restore
      if (oldbuf) {
         cout.rdbuf(oldbuf);
      }
      if (romeOutputFile) {
         romeOutputFile->close();
         SafeDelete(romeOutputFile);
      }
   }
}

//______________________________________________________________________________
void ROMEAnalyzer::Cleaning()
{
   // cleaning at exit.
   // all functions should be static
   ss_getchar(1);
   restoreOutput();
#if defined( HAVE_MIDAS )
   if (gROME && gROME->fActiveDAQ && gROME->isOnline() && gROME->IsActiveDAQ("midas")) {
      static_cast<ROMEMidasDAQ*>(gROME->fActiveDAQ)->
            StopOnlineCommunication(static_cast<ROMEMidasDAQ*>(gROME->fActiveDAQ));
   }
#endif
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::IsNetFolderActive(const char *name) const
{
   for (Int_t i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name)) {
         if (!fNetFolderActive[i])
            return false;
         return true;
      }
   }
   return false;
}

//______________________________________________________________________________
ROMENetFolder *ROMEAnalyzer::GetNetFolder(const char *name) const
{
   ROMEString str;
   for (Int_t i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name)) {
         if (!fNetFolderActive[i]) {
            ROMEPrint::Warning("%s is not activated.\n", name);
            return 0;
         }
         return fNetFolder[i];
      }
   }
   ROMEPrint::Warning("Netfolder '%s' is not defined", name);
   return 0;
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::ConnectNetFolder(const char *name) const
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name)) {
         break;
      }
   }
   return ConnectNetFolder(i);
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::ConnectNetFolder(Int_t i) const
{
   if (!(0 <= i && i < fNumberOfNetFolders)) {
      return kFALSE;
   }

   if (!fNetFolderActive[i]) {
      return kTRUE;
   }

   DisconnectNetFolder(i);
   fNetFolderSocket[i] = new TSocket (fNetFolderHost[i].Data(), fNetFolderPort[i]);
   while (!fNetFolderSocket[i]->IsValid()) {
      delete fNetFolderSocket[i];
      ROMEPrint::Warning("can not make socket connection for %s.\n", fNetFolderName[i].Data());
      ROMEPrint::Warning("program sleeps for 5s and tries again.\n\n");
      gSystem->Sleep(5000);
      fNetFolderSocket[i] = new TSocket (fNetFolderHost[i].Data(), fNetFolderPort[i]);
   }
   fNetFolder[i] = new ROMENetFolder(fNetFolderRoot[i].Data(), fNetFolderName[i].Data(), fNetFolderSocket[i], fNetFolderReconnect[i]);
   ROMEString errMessage;
   if (!fNetFolder[i]->GetPointer()) {
      ROMEPrint::Warning("%s failed to connect to %s folder of %s.\n", fNetFolderName[i].Data(), fNetFolderRoot[i].Data(), fNetFolderHost[i].Data());
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::ConnectSocketClient(const char* hostname, Int_t port)
{
   if (hostname) {
      fSocketClientHost = hostname;
   }
   if (port > 0) {
      fSocketClientPort = port;
   }

   if (fSocketClientNetFolder != 0) {
      if (fSocketClientNetFolder->HaveValidSocket()) {
         return true;
      }
   }

   TSocket *sock = new TSocket(fSocketClientHost.Data(), fSocketClientPort);
   while (!sock->IsValid()) {
      delete sock;
      ROMEPrint::Warning("can not make socket connection to the ROME analyzer on host '%s' through port %d.\n",
                         fSocketClientHost.Data(), fSocketClientPort);
      ROMEPrint::Warning("program sleeps for 5s and tries again.\n");
      gSystem->Sleep(5000);
      sock = new TSocket(fSocketClientHost.Data(), fSocketClientPort);
   }
   ConnectSocketClientNetFolder(sock);
   return true;
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::DisconnectNetFolder(const char *name) const
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name)) {
         break;
      }
   }
   return DisconnectNetFolder(i);
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::DisconnectNetFolder(Int_t i) const
{
   if (!(0 <= i && i < fNumberOfNetFolders)) {
      return kFALSE;
   }
   if (fNetFolderSocket[i]) {
      fNetFolderSocket[i]->Close();
      delete fNetFolderSocket[i];
      fNetFolderSocket[i] = 0;
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::ConnectNetFolders() const
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!ConnectNetFolder(i)) {
         return kFALSE;
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::DisconnectNetFolders() const
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!DisconnectNetFolder(i)) {
         return kFALSE;
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
void ROMEAnalyzer::InitNetFolders(Int_t number)
{
   if (number < 1) {
      return;
   }
   fNetFolder = new ROMENetFolder*[number];
   fNetFolderActive = new Bool_t[number];
   fNetFolderReconnect = new Bool_t[number];
   fNetFolderSocket = new TSocket*[number];
   fNetFolderPort = new Int_t[number];
   fNetFolderName = new ROMEString[number];
   fNetFolderHost = new ROMEString[number];
   fNetFolderRoot = new ROMEString[number];
   fNumberOfNetFolders = number;
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::StartWindow()
{
   return fWindow->Start();
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::IsWindowBusy() const
{
   Bool_t busy = false;
   for (int i = 0; i < fWindow->GetTabObjectEntries(); i++) {
      if (fWindow->GetTabObjectAt(i)->IsBusy()) {
         busy = true;
      }
   }
   return busy;
}

//______________________________________________________________________________
ROMEDAQSystem* ROMEAnalyzer::GetActiveDAQ() const
{
   if (fActiveDAQ != 0) {
      return fActiveDAQ;
   }
   ROMEPrint::Error("\nYou have tried to access the active DAQ system but none is active .\n"
                    "Please select a DAQ system in the ROME configuration file under:\n"
                    "<Modes>\n"
                    "   <DAQSystem>\n\n"
                    "Shutting down the program.\n");
   fApplication->Terminate(1);
   return 0;
}

//______________________________________________________________________________
ROMEDataBase* ROMEAnalyzer::GetDataBase(Int_t i) const
{
   if(i<fNumberOfDataBases && fDataBaseHandle[i] != 0) {
      return fDataBaseHandle[i];
   }
   ROMEPrint::Error("\nYou have tried to access a database without initialisation.\n"
                    "To use the databases you have to add it to the list of databases in the\n"
                    "ROME configuration file under <DataBases>.\n\n"
                    "Shutting down the program.\n");
   fApplication->Terminate(1);
   return 0;
}

//______________________________________________________________________________
ROMEDataBase* ROMEAnalyzer::GetDataBase(const char *name) const
{
   for (Int_t i = 0; i < fNumberOfDataBases; i++) {
      if (!stricmp(fDataBaseHandle[i]->GetName(), name)) {
         return fDataBaseHandle[i];
      }
   }
   ROMEPrint::Error("\nYou have tried to access the %s database without initialisation.\n"
                    "To use the %s database you have to add it to the list of databases in the\n"
                    "ROME configuration file under <DataBases>.\n\nShutting down the program.\n", name, name);
   fApplication->Terminate(1);
   return 0;
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::isDataBaseActive(const char *name) const
{
   for (Int_t i = 0; i < fNumberOfDataBases; i++) {
      if (!stricmp(fDataBaseHandle[i]->GetName(), name)) {
         return true;
      }
   }
   return false;
}

//______________________________________________________________________________
void ROMEAnalyzer::InitDataBases(Int_t number)
{
   int i;
   fDataBaseHandle = new ROMEDataBase*[number];
   fDataBaseConnection = new ROMEString[number];
   fDataBaseName = new ROMEString[number];
   fDataBaseDir = new ROMEString[number];
   fDataBaseDirConstructed = new ROMEString[number];
   fNumberOfDataBases = number;
   for(i = 0; i < fNumberOfDataBases; i++) {
      fDataBaseHandle[i] = 0;
   }
}

//______________________________________________________________________________
void ROMEAnalyzer::NextEvent()
{
   static_cast<ROMEEventLoop*>(fMainTask)->NextEvent();
}

//______________________________________________________________________________
void ROMEAnalyzer::GotoEvent(Long64_t eventNumber)
{
   static_cast<ROMEEventLoop*>(fMainTask)->GotoEvent(eventNumber);
}

//______________________________________________________________________________
void ROMEAnalyzer::GetRunNumberStringAt(ROMEString &buffer, Int_t i, const char* format) const
{
   ROMEString form;
   if (format) {
      form = format;
   }
   else {
#if defined( R__VISUAL_CPLUSPLUS )
      form = "%05I64d";
#else
      form = "%05lld";
#endif
   }
   if (i >= fRunNumber.GetSize()) {
      buffer.SetFormatted(form.Data(), 0);
   } else {
      buffer.SetFormatted(form.Data(), fRunNumber.At(i));
   }
}

//______________________________________________________________________________
void ROMEAnalyzer::GetCurrentRunNumberString(ROMEString &buffer, const char* format)
{
   ROMEString form;
   if (format) {
      form = format;
   } else {
#if defined( R__VISUAL_CPLUSPLUS )
      form = "%05I64d";
#else
      form = "%05lld";
#endif
   }
   buffer.SetFormatted(form.Data(), GetCurrentRunNumber());
}

//______________________________________________________________________________
ROMEString& ROMEAnalyzer::ConstructFilePath(const ROMEString &dir, const ROMEString &base, ROMEString& filename)
{
   ROMEString basetmp = base;
   ReplaceWithRunAndEventNumber(basetmp);
   gSystem->ExpandPathName(basetmp);
   if (basetmp.BeginsWith("/") || basetmp.BeginsWith("./") || basetmp.BeginsWith("../")) {
      // Ignore InputDir.
      filename = basetmp;
   } else {
      ROMEString dirtmp = dir;
      ReplaceWithRunAndEventNumber(dirtmp);
      filename = dirtmp;
      if (filename.Length() && !filename.EndsWith("/")) {
         filename += "/";
      }
      filename += basetmp;
   }
   filename.StripSpaces();
   ReplaceWithRunAndEventNumber(filename);
   gSystem->ExpandPathName(filename);
   return filename;
}

//______________________________________________________________________________
static Long64_t getNumber(const char op, const Long64_t a, const Long64_t b)
{
   // a helper function for ReplaceWithRunAndEventNumber
   switch(op) {
   case '+':
      return a + b;
      break;
   case '-':
      return a - b;
      break;
   case '*':
      return a * b;
      break;
   case '/':
      return a / b;
      break;
   case '%':
      return a % b;
      break;
   default:
      return 0;
      break;
   }
   return 0;
}

//______________________________________________________________________________
void ROMEAnalyzer::ReplaceWithRunAndEventNumber(ROMEString &buffer)
{
// Conversion from #, ##, ### to run number, event number, process
// id, respectively, with integer operaton and printf-style format.
//
// {# op number}(format) will be substituted to result of integer
// operaton according to format, where 'op' is one of following c/c++
// operators, namely '*', '/', '%', '+', '-'. Number shall be integer
// value.
//
// Example : filename "run{#/100}(%03d)abc/file{#}(%06d).root" becomes
// "run012abc/file001234.root" if the run number is 1234.
//
// If (format) is omitted, default format will be used, which is %d
// for process id and %05lld(Un*x)/%05I64d(windows) for run number and
// event number.

   Ssiz_t   numberLength = 0;
   Int_t    nMatch;
   Long64_t number, val1, val2;
   char     op = '\0';
   ROMEString valStr;
   ROMEString format;;
   ROMEString insertStr;
   TArrayI pos;

   static TPRegexp pattern1(
         //     2      3    4       5    6    7            8   9         10     11   12      13   14   15
         "\\{\\s*(#{1,3})\\s*([*/%+-])\\s*(\\d+)\\s*\\}\\s*\\((.+?)\\)|\\{\\s*(#{1,3})\\s*([*/%+-])\\s*(\\d+)\\s*\\}"
         ); // pattern for full expression

   static TPRegexp pattern2("\\{\\s*#{1,3}\\s*\\}");
   static TPRegexp pattern3("#{1,3}");

//   cout << "ReplaceWithRunAndEventNumber" << endl;
   while (1) {
//      cout << buffer << endl;

      // with operation
      nMatch = pattern1.Match(buffer, "o", 0, 30, &pos);
      if (nMatch==8) { // with operation, without format
         // operand#1
         pos[2] = pos[10];
         pos[3] = pos[11];
         // operator
         pos[4] = pos[12];
         pos[5] = pos[13];
         // operand#2
         pos[6] = pos[14];
         pos[7] = pos[15];

         // reduce to "with operation, with format" case
         nMatch = 5;
      }

      if (nMatch==5){ // with operation, with format
//         cout << nMatch << " : " << pos[0] << " " << pos[1] << endl;
         // operand#1 (pos[2],pos[3])
         numberLength = pos[3] - pos[2];
         if (numberLength==1) {       // Run#
            val1 = GetCurrentRunNumber();
         } else if (numberLength==2) { // Event#
            val1 = GetCurrentEventNumber();
         } else if (numberLength==3) { // PID
            val1 = gSystem->GetPid();
         } else {
            val1 = 0;
         }

         // operator (pos[4],pos[5])
         op = buffer[pos[4]];

         // operand#2 (pos[6],pos[7])
         valStr = buffer(pos[6], pos[7]-pos[6]);
         val2   = valStr.Atoll();

         // format (pos[8],pos[9])
         if (pos[8]>0) {
            format = buffer(pos[8], pos[9]-pos[8]);
         } else {
            if (numberLength==3) { // PID
               format = "%d";
            } else { // Run# or Event#
#if defined(R__UNIX)
               format = "%05lld";
#else
               format = "%05I64d";
#endif
            
            }
         }

         // Remove the pattern
         buffer.Remove(pos[0], pos[1]-pos[0]);

         // Insert the number
         number = getNumber(op, val1, val2);
         insertStr.SetFormatted(format, number);
         buffer.Insert(pos[0], insertStr);

         continue;
      }

      // witout operation, with {}
      nMatch = pattern2.Match(buffer, "o", 0, 30, &pos);
      if (nMatch == 1) {
//         cout << nMatch << " : " << pos[0] << " " << pos[1] << endl;
//         cout << "adding +0" << endl;
         // reduce to {#+0} type
         insertStr = "+0";
         buffer.Insert(pos[1]-1, insertStr);
         continue;
      }

      // without operation, without {}
      nMatch = pattern3.Match(buffer, "o", 0, 30, &pos);
      if (nMatch == 1) {
//         cout << nMatch << " : " << pos[0] << " " << pos[1] << endl;
//         cout << "adding { and +0}" << endl;
         // reduce to {#+0} type
         insertStr = "+0}";
         buffer.Insert(pos[1], insertStr);
         insertStr = "{";
         buffer.Insert(pos[0], insertStr);
         continue;
      }

      break;
   }
//   cout << "end" << endl;

   return;
}

//______________________________________________________________________________
THREADTYPE ROMEAnalyzer::FillObjectsInNetFolderServer(ROMEAnalyzer *localThis)
{
   ROME_LOCKGUARD(gObjectStorageMutex);
   localThis->GetNetFolderServer()->UpdateObjects();
   localThis->SetObjectStorageUpdated();
   localThis->GetNetFolderServer()->SetEventStorageAvailable(true);
   return THREADRETURN;
}

//______________________________________________________________________________
void ROMEAnalyzer::CopyTObjectWithStreamer(TBuffer *buffer, TObject* source, TObject* destination)
{
   if (!buffer || !source || !destination) {
      return;
   }

   buffer->Reset();
   buffer->SetWriteMode();
   buffer->MapObject(source);  //register obj in map to handle self reference
   static_cast<TObject*>(source)->Streamer(*buffer);
   // read new object from buffer
   buffer->SetReadMode();
   buffer->ResetMap();
   buffer->SetBufferOffset(0);
   buffer->MapObject(destination);  //register obj in map to handle self reference
   destination->Streamer(*buffer);
   destination->ResetBit(kIsReferenced);
   destination->ResetBit(kCanDelete);
}

//______________________________________________________________________________
Long64_t ROMEAnalyzer::GetCurrentEventNumber()
{
   ROME_LOCKGUARD(fgRunEventNumberMutex);

   Long64_t tempNumber;
   if (gROME->IsROMEMonitor()) {
      fCurrentEventNumber = gROME->GetSocketClientNetFolder()->GetCurrentEventNumber();
   }
   tempNumber = fCurrentEventNumber;
   return tempNumber;
}

//______________________________________________________________________________
Long64_t ROMEAnalyzer::GetCurrentRunNumber()
{
   ROME_LOCKGUARD(fgRunEventNumberMutex);

   Long64_t tempNumber;
   if (gROME->IsROMEMonitor()) {
      fCurrentRunNumber = gROME->GetSocketClientNetFolder()->GetCurrentRunNumber();
   }
   tempNumber = fCurrentRunNumber;
   return tempNumber;
}

//______________________________________________________________________________
void ROMEAnalyzer::SetCurrentEventNumber(Long64_t eventNumber)
{
   ROME_LOCKGUARD(fgRunEventNumberMutex);

   fCurrentEventNumber = eventNumber;
}

//______________________________________________________________________________
void ROMEAnalyzer::SetCurrentRunNumber(Long64_t runNumber)
{
   ROME_LOCKGUARD(fgRunEventNumberMutex);

   fCurrentRunNumber = runNumber;
}

//______________________________________________________________________________
void ROMEAnalyzer::UpdateObjectStorage()
{
   ROME_LOCKGUARD(fgUpdateObjectStorageMutex);

   fObjectStorageUpdated = kFALSE;
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::IsObjectStorageUpdated() const
{
   ROME_LOCKGUARD(fgUpdateObjectStorageMutex);

   Bool_t ret;
   ret = fObjectStorageUpdated;
   return ret;
}

//______________________________________________________________________________
void ROMEAnalyzer::SetObjectStorageUpdated()
{
   ROME_LOCKGUARD(fgUpdateObjectStorageMutex);

   fObjectStorageUpdated = kTRUE;
}

//______________________________________________________________________________
void ROMEAnalyzer::SetDataBase(Int_t i, ROMEDataBase *dataBase)
{
   if(i >= 0 && i < fNumberOfDataBases) {
      SafeDelete(fDataBaseHandle[i]);
      fDataBaseHandle[i] = dataBase;
   }
}

//______________________________________________________________________________
void ROMEAnalyzer::StopNetFolderServer()
{
   if (fNetFolderServer) {
      fNetFolderServer->StopServer();
   }
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::IsProgramTerminated() const
{
   if (gROME->IsROMEMonitor()) {
      return gROME->GetSocketClientNetFolder()->IsProgramTerminated();
   } else {
      return fProgramTerminated;
   }
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::STDOutIsTerminal()
{
#if defined( R__UNIX )
   return isatty(fileno(stdout));
#else
   return kTRUE;
#endif
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::STDErrIsTerminal()
{
#if defined( R__UNIX )
   return isatty(fileno(stderr));
#else
   return kTRUE;
#endif
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::WriteConfigurationFile(ROMEString &configFile) const
{
   if (isPreserveConfig()) {
      return kTRUE;
   }


   ROMEString tmpFileName = gSystem->BaseName(configFile.Data());
#if defined( R__UNIX )
   tmpFileName.ReplaceAll(".xml", "");
   fclose(gSystem->TempFileName(tmpFileName));
   gSystem->Unlink(tmpFileName.Data());
#else
   char buf[1000];
   GetTempFileName("c:\\", "", 0, buf);
   tmpFileName = buf;
#endif
   tmpFileName += ".xml";

   if (!this->fConfiguration->WriteConfigurationFile(tmpFileName.Data())) {
      ROMEPrint::Print("\nTerminate program.\n");
      gSystem->Unlink(tmpFileName.Data());
      return false;
   }

   fstream *fileStream;
   ROMEString oldFileBuffer = "";
   ROMEString newFileBuffer = "";

   if ((fileStream = new fstream(configFile.Data(), ios::in))) {
      oldFileBuffer.ReadFile(*fileStream);
      delete fileStream;
   }

   if ((fileStream = new fstream(tmpFileName.Data(), ios::in))) {
      newFileBuffer.ReadFile(*fileStream);
      delete fileStream;
   }

   if (oldFileBuffer != newFileBuffer) {
      // we can even ask users if overwrite (not implemented)
      if (!(fileStream = new fstream(configFile.Data(), ios::out | ios::trunc))) {
         gSystem->Unlink(tmpFileName.Data());
         return kFALSE;
      }
      *fileStream<<newFileBuffer.Data();
      fileStream->close();
      delete fileStream;
   }

   gSystem->Unlink(tmpFileName.Data());

   return kTRUE;
}
//______________________________________________________________________________
Bool_t ROMEAnalyzer::IsTaskActive(Int_t taskIndex)
{
   if (IsROMEMonitor()) {
      return GetSocketClientNetFolder()->IsTaskActive(taskIndex);
   }
   return GetTaskObjectAt(taskIndex)->IsActive();
}
//______________________________________________________________________________
Bool_t ROMEAnalyzer::IsHistoActive(Int_t taskIndex,Int_t histoIndex)
{
   if (IsROMEMonitor()) {
      return GetSocketClientNetFolder()->IsHistoActive(taskIndex,histoIndex);
   }
   return GetTaskObjectAt(taskIndex)->GetHistoParameterAt(histoIndex)->IsActive();
}

//______________________________________________________________________________
Bool_t ROMEAnalyzer::IsGraphActive(Int_t taskIndex,Int_t graphIndex)
{
   if (IsROMEMonitor()) {
      return GetSocketClientNetFolder()->IsGraphActive(taskIndex,graphIndex);
   }
   return GetTaskObjectAt(taskIndex)->GetGraphParameterAt(graphIndex)->IsActive();
}

//______________________________________________________________________________
const ROMEString& ROMEAnalyzer::GetInputDirString()
{
   ConstructFilePath(fInputDir, "", fInputDirConstructed);
   return fInputDirConstructed;
}

//______________________________________________________________________________
const ROMEString& ROMEAnalyzer::GetOutputDirString()
{
   ConstructFilePath(fOutputDir, "", fOutputDirConstructed);
   return fOutputDirConstructed;
}

//______________________________________________________________________________
const ROMEString& ROMEAnalyzer::GetDataBaseDirString(Int_t i)
{
   ConstructFilePath(fDataBaseDir[i], "", fDataBaseDirConstructed[i]);
   return fDataBaseDirConstructed[i];
}

//______________________________________________________________________________
const ROMEString& ROMEAnalyzer::GetConfigDirString()
{
   ConstructFilePath(fConfigDir, "", fConfigDirConstructed);
   return fConfigDirConstructed;
}
