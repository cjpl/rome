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
#include <TROOT.h>
#include <TBrowser.h>
#include <THtml.h>
#include <TSystem.h>
#include <TFile.h>
#include <TArrayI.h>
#include <TArrayL.h>
#include <TMutex.h>
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

#include "ROMENetFolder.h"
#include "ROMERint.h"
#include "ROMEConfig.h"
#include "ArgusTab.h"
#include "ROMEDataBase.h"
#include "ROMEXML.h"

ClassImp(ROMEAnalyzer)

ROMEAnalyzer *gROME;  // global ROMEAnalyzer Handle

ROMEAnalyzer::ROMEAnalyzer(ROMERint *app,Bool_t batch,Bool_t daemon,Bool_t nographics)
{
// Initialisations
   fMutex = new TMutex();
   fProgramMode = kStandAloneROME;
   fWindowClosed = false;
   fApplication = app;
   fCintInitialisation = "";
   fActiveDAQ = 0;
   fAnalysisMode = kAnalyzeOffline;
   fBatchMode = batch;
   fDaemonMode = daemon;
   fNoGraphics = batch || daemon || nographics;
   fQuitMode = batch || daemon;
   fSplashScreen = !nographics;
   fGraphicalConfigEdit = true;
   fPreserveConfig = false;
   fDontReadNextEvent = false;
   fInputDir = "./";
   fOutputDir = "./";
   fDataBaseDir = 0;
   fConfigDir = "./";
   fCurrentRunNumber = 0;
   fRunNumber.Reset();
   fRunNumberString = "";
   fCurrentEventNumber = 0;
   fEventNumber.Reset();
   fEventNumberString = "";
   fCurrentInputFileName = "";
//   fInputFileNames.Reset();
   fInputFileNamesString = "";
   fIOType = kNotBased;
   fUserEvent = false;
   fUserEventQ = false;
   fUserEventE = false;
   fUserEventS = false;
   fUserEventR = false;
   fUserEventO = false;
   fUserEventC = false;
   fUserEventJ = false;
   fUserEventJEventNumber = 0;
   fUserEventG = false;
   fUserEventGRunNumber = 0;
   fUserEventGEventNumber = 0;
   fUserEventI = false;
   fEventID = 'a';
   fMidasOnlineDataBase = 0;
   fTerminate = false;
   fFillEvent = false;
   fTaskObjects = 0;
   fMainTask = 0;
   fMainFolder = 0;
   fHistoFiles = 0;
   fTreeObjects = new TObjArray(0);
   fTreeAccumulation = false;
   fMainHistoFolder = 0;
   fHistoFolders = new TObjArray(0);
   fProgramName = "rome";
   fOnlineHost = "";
   fOnlineExperiment = "";
   fPortNumber = 9090;
   fSocketOffline = false;
   fDataBaseHandle = 0;
   fDataBaseName = 0;
   fDataBaseDir = 0;
   fDataBaseConnection = 0;
   fNumberOfDataBases = 0;
   fConfiguration = 0;
   fShowRunStat = true;
   fEventBasedDataBase = false;
   fNumberOfNetFolders = 0;
   fNetFolder = 0;
   fNetFolderActive = 0;
   fNetFolderReconnect = 0;
   fNetFolderSocket = 0;
   fNetFolderPort = 0;
   fNetFolderName = 0;
   fNetFolderHost = 0;
   fNetFolderRoot = 0;
   fOldbuf = 0;
   fSocketToROME = 0;
   fSocketToROMENetFolder = 0;
   fSocketToROMEActive = true;
   fSocketToROMEHost = "localhost";
   fSocketToROMEPort = 9090;
   fHistoRead = false;
   fHistoRun = 0;
   fRomeOutputFile = 0;
   fWindow = 0;
   fWindowUpdateFrequency = 0;
   fRequestEventHandling = false;
#if !defined (ENABLE_FASTCOPY)
   fFolderStorageTree = new TTree("ROMEFolderStorage", "Tree to store folders");
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   fFolderStorageTree->SetCircular(1);
#endif
#endif
}

ROMEAnalyzer::~ROMEAnalyzer() {
   ss_getchar(1);
   restoreOutput();
   SafeDelete(fTreeObjects);
   SafeDelete(fHistoFolders);
   SafeDelete(fRomeOutputFile);
   SafeDelete(fSocketToROME);
   SafeDelete(fSocketToROMENetFolder);
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
   SafeDeleteArray(fDataBaseHandle);
#if !defined (ENABLE_FASTCOPY)
   SafeDelete(fFolderStorageTree);
#endif
}

Bool_t ROMEAnalyzer::Start(int argc, char **argv)
{
#if defined( HAVE_MIDAS )
   cm_set_msg_print(0,0,0);
#endif

   if (this->isDaemonMode())
      ss_daemon_init(kFALSE);

/*
   if (this->isBatchMode())
      ss_batch_init(kFALSE);
*/

   ROMEPrint::Debug("Starting analyzer\n");
// Starts the ROME Analyzer
   ROMEString text;
#if !defined (ENABLE_FASTCOPY)
   AddFolderStorageTreeBranches();
#endif

   // disable writing date and time in XML files written by mxml.
   ROMEXML::SuppressWritingDate();

   ROMEPrint::Debug("Executing init tasks\n");
   fMainTask->ExecuteTask("init");

   if (!ReadParameters(argc,argv)) return false;

   gSystem->MakeDirectory(this->GetOutputDir());

   static_cast<ROMEEventLoop*>(fMainTask)->AddTreeBranches();

   if (IsStandAloneARGUS()) {
      ConnectSocketToROME();
   }
   if (!ConnectNetFolders())
      return false;

   gROME->ss_getchar(0);

   if (IsStandAloneROME() || IsROMEAndARGUS()) {
      consoleStartScreen();
      if (isSplashScreen()) startSplashScreen();

      if (isOnline() || isSocketOffline()) {
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
         ROMEPrint::Print("g : Run until event #\n");
         ROMEPrint::Print("j : Jump to event #\n");
         ROMEPrint::Print("i : Root interpreter\n");
         ROMEPrint::Print("\n");
      }
   }
   if (IsStandAloneARGUS()) {
      ShowRunStat(false);
   }

   fMainTask->ExecuteTask("start");

   gROME->ss_getchar(1);

   if (fTerminate) return false;

   return true;
}

void ROMEAnalyzer::ParameterUsage()
{
   ROMEPrint::Print("  -i       Configuration file\n");
   ROMEPrint::Print("  -b       Batch Mode (no Argument)\n");
   ROMEPrint::Print("  -D       Daemon Mode (no Argument)\n");
   ROMEPrint::Print("  -q       Quit Mode (no Argument)\n");
   ROMEPrint::Print("  -v       Verbose level :(mute/error/warning/normal/debug)\n");
   ROMEPrint::Print("  -ns      Splash Screen is not displayed (no Argument)\n");
   ROMEPrint::Print("  -m       Analysing Mode : (online/[offline])\n");
   ROMEPrint::Print("  -r       Runnumbers\n");
   ROMEPrint::Print("  -e       Eventnumbers\n");
   ROMEPrint::Print("  -o       Start Analyzer in Step by Step Mode\n");
   ROMEPrint::Print("  -docu    Generates a Root-Html-Documentation (no Argument)\n");
   ROMEPrint::Print("  -ng      No graphics is used\n");
   UserParameterUsage();
   return;
}

Bool_t ROMEAnalyzer::ReadParameters(int argc, char *argv[])
{
   // Reads the Inputlineparameters
   int i;

   ROMEString configFile("");

   ROMEPrint::Debug("Reading command line options\n");
   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-h")||!strcmp(argv[i],"-help")||!strcmp(argv[i],"--help")) {
         ParameterUsage();
         return false;
      }
      if (!strcmp(argv[i],"-docu")) {
         THtml html;
         html.MakeAll(false);
         return false;
      }
      if (!strcmp(argv[i],"-i")&&i<argc-1) {
         if (i+1 >= argc) break;
         configFile = argv[i+1];
         i++;
      }
      if (!strcmp(argv[i],"-v")) {
         if (i+1 >= argc) break;
         if (!strcmp(argv[i+1],"mute"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kMute);
         else if (!strcmp(argv[i+1],"error"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kErrorOnly);
         else if (!strcmp(argv[i+1],"warning"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kErrorAndWarning);
         else if (!strcmp(argv[i+1],"normal"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kNormal);
         else if (!strcmp(argv[i+1],"debug"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kDebug);
         i++;
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
         if (foundFiles.At(i).BeginsWith(curDir.Data()))
            foundFiles.AddAt(foundFiles.At(i).Remove(0, curDir.Length()+1), i);
      }

      if (foundFiles.GetEntries() == 0) {
         ;
      }
      else if (foundFiles.GetEntries() == 1) {
         configFile = foundFiles.At(0);
      }
      else {
         if (this->isBatchMode() || this->isDaemonMode()) {
            ROMEPrint::Error("Several configuration files were found.\n");
            for (i = 0; i < nFile; i++) {
               ROMEPrint::Error("   %s\n", foundFiles.At(i).Data());
            }
            ROMEPrint::Error("Please specify with -i option.\n");
            return false;
         }
         else {
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
   char* cstop;
   if ( !configFile.Length() || gSystem->AccessPathName(configFile.Data(), kFileExists)) {
      if (isBatchMode() || isDaemonMode()) {
         if (configFile.Length()) {
            ROMEPrint::Error("Configuration file '%s' not found.\n", configFile.Data());
         }
         else {
            ROMEPrint::Error("Please specify configuration file with -i option.\n");
         }
         return false;
      }

      if (configFile.Length()) {
         ROMEPrint::Warning("Configuration file '%s' not found.", configFile.Data());
      }
      else {
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
      while (answer==0) {
         while (this->ss_kbhit()) {
            answer = this->ss_getchar(0);
         }
      }
      gROME->ss_getchar(1);
      answerString = answer;
      answerString.ToUpper();
      if (answerString=="R" || answerString=="A" || answerString=="M") {
         if (answerString=="R")
            SetStandAloneROME();
         if (answerString=="A")
            SetStandAloneARGUS();
         if (answerString=="M")
            SetROMEAndARGUS();
         ROMEPrint::PrintAlways("\nPlease specify file name (default='%s'): ", configFile.Data());
         cin.getline(answerLine, sizeof(answerLine));
         if (strlen(answerLine))
            configFile = answerLine;
         if (!configFile.EndsWith(".xml") && !configFile.EndsWith(".XML"))
            configFile += ".xml";
         if (!gSystem->AccessPathName(configFile.Data(), kFileExists)) {
            ROMEPrint::PrintAlways("overwrite '%s'? ", configFile.Data());
/*
            gROME->ss_getchar(0);
            while (answer==0) {
               while (this->ss_kbhit()) {
                  answer = this->ss_getchar(0);
               }
            }
            gROME->ss_getchar(1);
*/
            cin>>answer;
            if (answer != 'y' && answer != 'Y')
               overwrite = false;
         }
         ROMEPrint::PrintAlways("\nPlease specify amount of comments in file [0-5], default=1(few comments): ");
         cin.getline(answerLine, sizeof(answerLine));
         if (strlen(answerLine))
            fConfiguration->SetCommentLevel(strtol(answerLine,&cstop,10));
         if (overwrite) {
            if (!this->fConfiguration->WriteConfigurationFile(configFile.Data())) {
               ROMEPrint::Print("\nTerminate program.\n");
               return false;
            }
            ROMEPrint::Print("\nThe framework generated a new configuration file.\n%s\n", configFile.Data());
            ROMEPrint::Print("Please edit this file and restart the program.\n");
         }
      }
      else {
         ROMEPrint::Print("\nTerminate program.\n");
      }
      return false;
   }
   if (!this->GetConfiguration()->ReadConfigurationFile(configFile.Data())) {
      ROMEPrint::Print("\nTerminate program.\n");
      return false;
   }
   ROMEPrint::Print("Using configuration file %s\n", configFile.Data());
   if (isGraphicalConfigEdit() && !isNoGraphics()) {
      if (!this->ShowConfigurationFile()) {
         ROMEPrint::Print("\nTerminate program.\n");
         return false;
      }
   }
   if (!isPreserveConfig()) {
      if (!this->fConfiguration->WriteConfigurationFile(configFile.Data())) {
         ROMEPrint::Print("\nTerminate program.\n");
         return false;
      }
   }

   ROMEPrint::Debug("Reading command line options\n");
   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-q")) {
         fQuitMode = true;
      }
      else if (!strcmp(argv[i],"-v")) {
         if (i+1 >= argc) break;
         if (!strcmp(argv[i+1],"mute"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kMute);
         else if (!strcmp(argv[i+1],"error"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kErrorOnly);
         else if (!strcmp(argv[i+1],"warning"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kErrorAndWarning);
         else if (!strcmp(argv[i+1],"normal"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kNormal);
         else if (!strcmp(argv[i+1],"debug"))
            ROMEPrint::SetVerboseLevel(ROMEPrint::kDebug);
         i++;
      }
      else if (!strcmp(argv[i],"-ns")) {
         fSplashScreen = false;
      }
      else if (!strcmp(argv[i],"-m")) {
         if (i+1 >= argc) break;
         if (!strcmp(argv[i+1],"online")) this->SetOnline();
         else this->SetOffline();
         i++;
      }
      else if (!strcmp(argv[i],"-r")&&i<argc-1) {
         if (i+1 >= argc) break;
         this->SetRunNumbers(argv[i+1]);
         i++;
      }
      else if (!strcmp(argv[i],"-e")&&i<argc-1) {
         if (i+1 >= argc) break;
         this->SetEventNumbers(argv[i+1]);
         i++;
      }
      else if (!strcmp(argv[i],"-o")) {
         ((ROMEEventLoop*)fMainTask)->SetContinuousMode(false);
      }
      else if (!strcmp(argv[i],"-i")) {
         if (i+1 >= argc) break;
         i++;
      }
      else if (!strcmp(argv[i],"-b")) {
         fBatchMode = kTRUE;
      }
      else if (!strcmp(argv[i],"-D")) {
         fDaemonMode = kTRUE;
      }
      else if (!strcmp(argv[i],"-ng")) {
         fNoGraphics = kTRUE;
      }
      else if (!ReadUserParameter(argv[i], i<argc-1 ? argv[i+1] : "", i)) {
         ROMEPrint::Error("Input line parameter '%s' not available.\n", argv[i]);
         ROMEPrint::Print("Available input line parameters are :\n");
         ParameterUsage();
         return false;
      }
   }
   fNoGraphics = fBatchMode || fDaemonMode || fNoGraphics;
   fQuitMode = fBatchMode || fDaemonMode || fQuitMode;
   fSplashScreen =  fSplashScreen && !fNoGraphics;

   return true;
}

Int_t ROMEAnalyzer::CheckEventNumber(Long64_t eventNumber)
{
   return CheckNumber(eventNumber,fEventNumber);
}

Int_t ROMEAnalyzer::CheckRunNumber(Long64_t runNumber)
{
   return CheckNumber(runNumber,fRunNumber);
}

Int_t ROMEAnalyzer::CheckNumber(Long64_t number,TArrayL64 &numbers)
{
   const int nNumbers = numbers.GetSize();
   if (nNumbers==0)
      return 1;
   for (Int_t i=0;i<nNumbers;i++) {
      if (TMath::Abs(numbers.At(i))>number) {
         return 0;
      }
      if (numbers.At(i)<0) {
         if (TMath::Abs(numbers.At(i))<=number && TMath::Abs(numbers.At(i+1))>=number) {
            return 1;
         }
      }
      else {
         if (numbers.At(i)==number) {
            return 1;
         }
      }
   }
   return -1;
}

Long64_t ROMEAnalyzer::GetNextRunNumber(const Long64_t runNumber)
{
   const Int_t nRunNumber = fRunNumber.GetSize();
   for (Int_t i=0;i<nRunNumber;i++) {
      if (fRunNumber.At(i)<0) {
         if (TMath::Abs(fRunNumber.At(i))<=runNumber && TMath::Abs(fRunNumber.At(i+1))>runNumber) {
            return runNumber+1;
         }
      }
      else {
         if (fRunNumber.At(i)==runNumber && i<nRunNumber-1) {
            return TMath::Abs(fRunNumber.At(i+1));
         }
      }
      if (TMath::Abs(fRunNumber.At(i))>runNumber) {
         return TMath::Abs(fRunNumber.At(i));
      }
   }
   return -1;
}

void ROMEAnalyzer::DecodeNumbers(ROMEString& str,TArrayL64& arr)
{
   char cminus='-';
   char ccomma=',';
   char csemi =';';
   char *pstr = (char*)str.Data();
   char *pend = (char*)str.Data()+str.Length();
   Long64_t num;
   Int_t na=0;
   Int_t nat=1;
   arr.Set(10);
   Int_t arraySize = arr.GetSize();
   while (pstr<pend) {
      if (na>=arraySize*nat) {
         nat++;
         arr.Set(arraySize*nat);
      }
      num = strtol(pstr,&pstr,10);
      if (pstr[0]==cminus) {
         arr.AddAt(-num,na);
         if (num==0) {
            na++;
            arr.AddAt(-1,na);
         }
         na++;
         pstr++;
      }
      else if (pstr[0]==ccomma||pstr[0]==csemi) {
         arr.AddAt(num,na);
         na++;
         pstr++;
      }
      else {
         arr.AddAt(num,na);
         arr.Set(na+1);
         return;
      }
   }
   arr.Set(na);
}

void ROMEAnalyzer::DecodeInputFileNames(ROMEString& str,ROMEStrArray& arr)
{
   int ind,num=0;
   TString temp;
   ROMEString str2 = str;
   while ((ind=str2.Index(","))!=-1) {
      temp = str2(0,ind);
      arr.AddAtAndExpand(temp,num);
      num++;
      str2 = str2(ind+1,str2.Length()-ind-1);
   }
   if (str.Length()>0)
      arr.AddAtAndExpand(str,num);
}

Bool_t ROMEAnalyzer::toBool(Int_t value) {
   return value!=0;
}

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

Int_t ROMEAnalyzer::ss_getchar(UInt_t reset)
{
   if (this->isDaemonMode() || this->isBatchMode())
      return 0;
#if defined( R__UNIX )
   static unsigned long int init = 0;
   static struct termios save_termios;
   struct termios buf;
   int i, fd;
   char c[3];

   fd = fileno(stdin);

   if (reset) {
      if (init)
         tcsetattr(fd, TCSAFLUSH, &save_termios);
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

   if (i == 0)
      return 0;

   /* check if ESC */
   if (c[0] == 27) {
      i = read(fd, c, 2);
      if (i == 0)               /* return if only ESC */
         return 27;

      /* cursor keys return 2 chars, others 3 chars */
      if (c[1] < 65)
         read(fd, c, 1);

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
   if (c[0] == 127)
      return CH_BS;

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

      if (!kbhit())
         return 0;

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

   if (nCharsRead == 0)
      return 0;

   ReadConsoleInput(hConsole, &ir, 1, &nCharsRead);

   if (ir.EventType != KEY_EVENT)
      return ss_getchar(0);

   if (!ir.Event.KeyEvent.bKeyDown)
      return ss_getchar(0);

   if (ir.Event.KeyEvent.wRepeatCount > 1) {
      repeat_count = ir.Event.KeyEvent.wRepeatCount - 1;
      repeat_char = ir.Event.KeyEvent.uChar.AsciiChar;
      return repeat_char;
   }

   if (ir.Event.KeyEvent.uChar.AsciiChar)
      return ir.Event.KeyEvent.uChar.AsciiChar;

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

Int_t ROMEAnalyzer::ss_daemon_init(Bool_t keep_stdout)
{
#if defined( R__UNIX )   // only implemented for UNIX
   int i, fd;
   int pid = fork();

   if (pid < 0)
      return kFALSE;
   else if (pid != 0) {
      ROMEPrint::Print("Becoming a daemon... (PID = %d)\n", pid);
      fApplication->Terminate(0); // parent finished
   }


   /* try and use up stdin, stdout and stderr, so other
      routines writing to stdout etc won't cause havoc. Copied from smbd */
   for (i = 0; i < 3; i++) {
      if (keep_stdout && ((i == 1) || (i == 2)))
         continue;

      close(i);
      fd = open("/dev/null", O_RDWR, 0);
      if (fd < 0)
         fd = open("/dev/null", O_WRONLY, 0);
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

Int_t ROMEAnalyzer::ss_batch_init()
{
// redirect std input.
// this method is not used yet.
#if defined( R__UNIX )   // only implemented for UNIX
   int fd;

   close(0);
   fd = open("/dev/null", O_RDWR, 0);
   if (fd < 0)
      fd = open("/dev/null", O_WRONLY, 0);
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

Int_t ROMEAnalyzer::stricmp(const char* c1,const char* c2)
{
#if defined( R__UNIX )
   return strcasecmp(c1,c2);
#elif defined( R__VISUAL_CPLUSPLUS )
   return _stricmp(c1,c2);
#else
   return 0;
#endif
}

Bool_t ROMEAnalyzer::strtobool(const char* str)
{
   char *cstop;
   if (!strcmp(str,"true"))
      return true;
   if (!strcmp(str,"false"))
      return false;
   return strtol(str,&cstop,10)!=0;
}

void ROMEAnalyzer::redirectOutput() {
   if (!fOldbuf)
      fOldbuf = cout.rdbuf();
   if (!fRomeOutputFile)
      fRomeOutputFile = new ofstream("romeOutput.txt");
   cout.rdbuf(fRomeOutputFile->rdbuf());
}

void ROMEAnalyzer::restoreOutput() {
   if (fOldbuf)
      cout.rdbuf(fOldbuf);
}

Bool_t ROMEAnalyzer::IsNetFolderActive(const char *name)
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

ROMENetFolder *ROMEAnalyzer::GetNetFolder(const char *name)
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

Bool_t ROMEAnalyzer::ConnectNetFolder(const char *name)
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name))
         break;
   }
   return ConnectNetFolder(i);
}

Bool_t ROMEAnalyzer::ConnectNetFolder(Int_t i)
{
   if (!(0 <= i && i < fNumberOfNetFolders))
      return kFALSE;

   if (!fNetFolderActive[i])
      return kTRUE;

   DisconnectNetFolder(i);
   fNetFolderSocket[i] = new TSocket (fNetFolderHost[i].Data(), fNetFolderPort[i]);
   while (!fNetFolderSocket[i]->IsValid()) {
      delete fNetFolderSocket[i];
      ROMEPrint::Warning("can not make socket connection for %s.\n", fNetFolderName[i].Data());
      ROMEPrint::Warning("program sleeps for 5s and tries again.\n");
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


Bool_t ROMEAnalyzer::ConnectSocketToROME()
{
   if (!IsSocketToROMEActive())
      return false;
   if (fSocketToROME!=0) {
      if (fSocketToROME->IsValid()) {
         return true;
      }
   }
   if (fSocketToROME==0)
      fSocketToROME = new TSocket (fSocketToROMEHost.Data(), fSocketToROMEPort);
   while (!fSocketToROME->IsValid()) {
      delete fSocketToROME;
      ROMEPrint::Warning("can not make socket connection to the ROME analyzer on host '%s' through port %d.\n", fSocketToROMEHost.Data(), fSocketToROMEPort);
      ROMEPrint::Warning("program sleeps for 5s and tries again.\n");
      gSystem->Sleep(5000);
      fSocketToROME = new TSocket (fSocketToROMEHost.Data(), fSocketToROMEPort);
   }
   ConnectSocketToROMENetFolder();
   return true;
}

Bool_t ROMEAnalyzer::DisconnectNetFolder(const char *name)
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name))
         break;
   }
   return DisconnectNetFolder(i);
}

Bool_t ROMEAnalyzer::DisconnectNetFolder(Int_t i)
{
   if (!(0 <= i && i < fNumberOfNetFolders))
      return kFALSE;
   if (fNetFolderSocket[i]) {
      fNetFolderSocket[i]->Close();
      delete fNetFolderSocket[i];
      fNetFolderSocket[i] = 0;
   }

   return kTRUE;
}

Bool_t ROMEAnalyzer::ConnectNetFolders()
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!ConnectNetFolder(i))
         return kFALSE;
   }
   return kTRUE;
}

Bool_t ROMEAnalyzer::DisconnectNetFolders()
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!DisconnectNetFolder(i))
         return kFALSE;
   }
   return kTRUE;
}

void ROMEAnalyzer::InitNetFolders(Int_t number)
{
   if (number < 1)
      return;
   fNetFolder = new ROMENetFolder *[number];
   fNetFolderActive = new Bool_t[number];
   fNetFolderReconnect = new Bool_t[number];
   fNetFolderSocket = new TSocket *[number];
   fNetFolderPort = new Int_t[number];
   fNetFolderName = new ROMEString[number];
   fNetFolderHost = new ROMEString[number];
   fNetFolderRoot = new ROMEString[number];
   fNumberOfNetFolders = number;
}

Bool_t ROMEAnalyzer::StartWindow()
{
   return fWindow->Start();
}

Bool_t ROMEAnalyzer::IsWindowBusy() {
   Bool_t busy = false;
   for (int i=0;i<fWindow->GetTabObjectEntries();i++)
      if (fWindow->GetTabObjectAt(i)->IsBusy())
         busy = true;
   return busy;
}

ROMEDAQSystem* ROMEAnalyzer::GetActiveDAQ()
{
   if (fActiveDAQ!=0)
      return fActiveDAQ;
   ROMEPrint::Error("\nYou have tried to access the active DAQ system but none is active .\nPlease select a DAQ system in the ROME configuration file under:\n<Modes>\n   <DAQSystem>\n\nShutting down the program.\n");
   fApplication->Terminate(1);
   return 0;
}

ROMEDataBase* ROMEAnalyzer::GetDataBase(Int_t i)
{
   if(i<fNumberOfDataBases && fDataBaseHandle[i]!=0)
      return fDataBaseHandle[i];
   ROMEPrint::Error("\nYou have tried to access a database without initialisation.\nTo use the databases you have to add it to the list of databases in the\nROME configuration file under <DataBases>.\n\nShutting down the program.\n");
   fApplication->Terminate(1);
   return 0;
}

ROMEDataBase* ROMEAnalyzer::GetDataBase(const char *name)
{
   for (Int_t i=0;i<fNumberOfDataBases;i++)
      if (!stricmp(fDataBaseHandle[i]->GetName(),name))
         return fDataBaseHandle[i];
   ROMEPrint::Error("\nYou have tried to access the %s database without initialisation.\nTo use the %s database you have to add it to the list of databases in the\nROME configuration file under <DataBases>.\n\nShutting down the program.\n",name,name);
   fApplication->Terminate(1);
   return 0;
}

Bool_t ROMEAnalyzer::isDataBaseActive(const char *name)
{
   for (Int_t i=0;i<fNumberOfDataBases;i++)
      if (!stricmp(fDataBaseHandle[i]->GetName(),name))
         return true;
   return false;
}

void ROMEAnalyzer::InitDataBases(Int_t number)
{
   fDataBaseHandle = new ROMEDataBase*[number];
   fDataBaseConnection = new ROMEString[number];
   fDataBaseName = new ROMEString[number];
   fDataBaseDir = new ROMEString[number];
   fNumberOfDataBases = number;
}

void ROMEAnalyzer::NextEvent()
{
   ((ROMEEventLoop*)fMainTask)->NextEvent();
}

void ROMEAnalyzer::GotoEvent(Long64_t eventNumber)
{
   ((ROMEEventLoop*)fMainTask)->GotoEvent(eventNumber);
}

void ROMEAnalyzer::GetRunNumberStringAt(ROMEString &buffer,Int_t i, const char* format)
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
   if (i>=fRunNumber.GetSize())
      buffer.SetFormatted(form.Data(),0);
   else
      buffer.SetFormatted(form.Data(),fRunNumber.At(i));
}

void ROMEAnalyzer::GetCurrentRunNumberString(ROMEString &buffer, const char* format)
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
   buffer.SetFormatted(form.Data(),fCurrentRunNumber);
}

void ROMEAnalyzer::ReplaceWithRunAndEventNumber(ROMEString &buffer)
{
// replace ## with event number.
// replace # with run number
// format can be specified like "file#(%05d).root"

   Int_t startStr = 0;
   Int_t endStr = 0;
   Int_t startForm = 0;
   Int_t endForm = 0;
   ROMEString format;
   ROMEString insertStr;

   // event number
   while((startStr = buffer.Index("##", strlen("##"), endStr, TString::kExact)) != -1) {
      endStr = startStr + strlen("##");
      if (buffer[endStr] == '(' && (endForm = buffer.Index(")", 1, endStr + 1, TString::kExact)) != -1) {
         startForm = endStr + 1;
         format = buffer(startForm, endForm - startForm);
         buffer.Remove(startForm - 1, endForm - startForm + strlen("()"));
         endStr = startStr + strlen("##");
      }
      else {
#if defined( R__VISUAL_CPLUSPLUS )
         format = "%05I64d";
#else
         format = "%05lld";
#endif
      }
      buffer.Remove(startStr, endStr - startStr);
      insertStr.SetFormatted(format.Data(), fCurrentEventNumber);
      buffer.Insert(startStr, insertStr);
      endStr = startStr + insertStr.Length();
   }

   // run number
   startStr = 0;
   endStr = 0;
   startForm = 0;
   endForm = 0;
   while((startStr = buffer.Index("#", strlen("#"), endStr, TString::kExact)) != -1) {
      endStr = startStr + strlen("#");
      if (buffer[endStr] == '(' && (endForm = buffer.Index(")", 1, endStr + 1, TString::kExact)) != -1) {
         startForm = endStr + 1;
         format = buffer(startForm, endForm - startForm);
         buffer.Remove(startForm - 1, endForm - startForm + strlen("()"));
         endStr = startStr + strlen("#");
      }
      else {
         if (gROME->isTreeAccumulation())
            format = gROME->GetRunNumberStringOriginal();
         else
#if defined( R__VISUAL_CPLUSPLUS )
            format = "%05I64d";
#else
            format = "%05lld";
#endif
      }
      buffer.Remove(startStr, endStr - startStr);
      insertStr.SetFormatted(format.Data(), fCurrentRunNumber);
      buffer.Insert(startStr, insertStr);
      endStr = startStr + insertStr.Length();
   }
}