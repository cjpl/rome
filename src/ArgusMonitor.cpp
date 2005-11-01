/********************************************************************
  ArgusMonitor.cpp, R.Sawada

  $Id$

********************************************************************/
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#ifndef __CINT__
#include <Windows4Root.h>
#include <conio.h>
#endif
#include <io.h>
#include <direct.h>
#include <windows.h>
#endif
#if defined( R__UNIX )
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <TThread.h>
#ifndef R__MACOSX
#include <sys/io.h>
#endif
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <Riostream.h>
#include <THtml.h>
#include <TSystem.h>
#include <ROMEDataBase.h>
#include <ROMENoDataBase.h>
#include <ArgusMonitor.h>
#include <ArgusWindow.h>
#if defined( HAVE_MIDAS )	 
#   include <midas.h>	 
#endif

ClassImp(ArgusMonitor)

ArgusMonitor *gArgus;           // global ArgusMonitor Handle
void *gPassToArgus;             // void ArgusMonitor Handle

ArgusMonitor::ArgusMonitor(TApplication * app):ROMEAnalyzer((TRint *) app)
{
// Initialisations
   fWindowScale = 1;
   fTreeInfo = new ROMETreeInfo();
   fApplication = (TApplication *) fRint;
}

ArgusMonitor::~ArgusMonitor()
{
}

Bool_t ArgusMonitor::Start(Int_t argc, Char_t ** argv)
{
   gArgus = (ArgusMonitor *) gPassToArgus;
   gROME = (ROMEAnalyzer *) gArgus;

   if (gROOT->IsBatch()) {
      printf("%s: cannot run in batch mode\n", GetProgramName());
      return kFALSE;
   }

   gArgus->InitTrees();

   if (!ReadParameters(argc, argv))
      return kFALSE;

/*
#if defined( HAVE_MIDAS )
   // Connect to the experiment
   if (cm_connect_experiment(gArgus->GetOnlineHost(), gArgus->GetOnlineExperiment(),gArgus->GetProgramName(), NULL) != SUCCESS) {
      cout << "\nCannot connect to experiment" << endl;
      return kFALSE;
   }
#ifdef MIDAS_DEBUG
   cm_set_watchdog_params(TRUE, 0);
#endif
   // Connect to the online database
   if (cm_get_experiment_database(gArgus->GetMidasOnlineDataBasePointer(), NULL)!= CM_SUCCESS) {
      cout << "\nCannot connect to the online database" << endl;
      return kFALSE;
   }
   atexit((void (*)(void))cm_disconnect_experiment);
#endif
*/

   if (!gArgus->StartMonitor())
      return kFALSE;

   return kTRUE;
}

Bool_t ArgusMonitor::StartMonitor()
{
   this->InitSingleFolders();

   // Connect net folders
   if (!ConnectNetFolders())
      return kFALSE;

   // Read Data Base
   if (!ReadSingleDataBaseFolders()) {
      Error("StartMonitor", "Error while reading the data base !");
      return kFALSE;
   }
   InitArrayFolders();
   if (!ReadArrayDataBaseFolders()) {
      Error("StartMonitor", "while reading the data base !");
      return kFALSE;
   }
   // main window 
   if (!StartWindow())
      return kFALSE;

   fApplication->Run();

   return kTRUE;
}

void ArgusMonitor::ParameterUsage()
{
   gArgus->Println("  -i       Configuration file (default argusConfig.xml)");
   gArgus->Println("  -m       Analysing Mode : (online/[offline])");
   gArgus->Println("  -r       Runnumbers");
/*
       <<"  -h       MIDAS server host name"<<endl
       <<"  -e       MIDAS experiment name"<<endl
*/
   gArgus->Println("  -docu    Generates a Root-Html-Documentation (no Argument)");
   gArgus->UserParameterUsage();
   return;
}

Bool_t ArgusMonitor::ReadParameters(Int_t argc, Char_t * argv[])
{
   // Reads the Inputlineparameters
   Int_t i;
   this->SetConfigDir("./");
   ROMEString configFile("argusConfig.xml");

   Char_t host_name[256] = "";
   Char_t exp_name[32] = "";

#if defined( HAVE_MIDAS )
   cm_get_environment(host_name, sizeof(host_name), exp_name, sizeof(exp_name));
#endif

   for (i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help") || !strcmp(argv[i], "--help")) {
         ParameterUsage();
         return false;
      }
      if (!strcmp(argv[i], "-docu")) {
         THtml html;
         html.MakeAll(kTRUE);
         return kFALSE;
      }
      if (!strcmp(argv[i], "-i") && i < argc - 1) {
         configFile = argv[i + 1];
         i++;
      }
   }

   Char_t answer = 0;
   struct stat buf;
   if (stat(configFile.Data(), &buf)) {
      cout << "Configuration file '" << configFile.Data() << "' not found." << endl;
      cout << "Do you like the framework to generate a new configuration file ([y]/n) ? " << flush;
      answer = getchar();
      if (answer != 'n') {
         if (!this->GetConfiguration()->WriteConfigurationFile((Char_t *) configFile.Data())) {
            cout << "\nTerminate program.\n" << endl;
            return kFALSE;
         }
         cout << "\nThe framework generated a new configuration file." << endl;
         cout << "Please edit this file and restart the program.\n" << endl;
      } else {
         cout << "\nTerminate program.\n" << endl;
      }
      return kFALSE;
   }

   if (!this->GetConfiguration()->ReadConfigurationFile((Char_t *) configFile.Data())) {
      cout << "\nTerminate program.\n" << endl;
      return kFALSE;
   }
   if (!this->GetConfiguration()->WriteConfigurationFile((Char_t *) configFile.Data())) {
      cout << "\nTerminate program.\n" << endl;
      return kFALSE;
   }

   for (i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "-m")) {
         if (!strcmp(argv[i + 1], "online"))
            this->SetOnline();
         else
            this->SetOffline();
         i++;
      } else if (!strcmp(argv[i], "-r") && i < argc - 1) {
         this->SetRunNumbers(argv[i + 1]);
         i++;
      } else if (!strcmp(argv[i], "-i")) {
         i++;
      } else if (!ReadUserParameter(argv[i], i < argc - 1 ? argv[i + 1] : "", i)) {
         gArgus->Print("Input line parameter '");
         gArgus->Print(argv[i]);
         gArgus->Println("' not available.");
         gArgus->Println("Available input line parameters are : ");
         gArgus->Println();
         ParameterUsage();
         return false;
      }
   }
/*
   if (strlen(host_name))
      gArgus->SetOnlineHost(host_name);
   if (strlen(exp_name))
      gArgus->SetOnlineExperiment(exp_name);
*/

   return kTRUE;
}

Bool_t ArgusMonitor::IsNetFolderActive(const Char_t * name)
{
   for (Int_t i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name)) {
         if (!fNetFolderActive[i])
            return false;
         return true;
      }
   }
   return false;
};

TNetFolder *ArgusMonitor::GetNetFolder(const Char_t * name)
{
   for (Int_t i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name)) {
         if (!fNetFolderActive[i]) {
            Warning("GetNetFolder", "%s is not activated.", name);
            return NULL;
         }
         return fNetFolder[i];
      }
   }
   Error("GetNetFolder", "Netfolder '%s' is not defined", name);
   return NULL;
};

Bool_t ArgusMonitor::ConnectNetFolder(const Char_t * name)
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name))
         break;
   }
   return ConnectNetFolder(i);
}

Bool_t ArgusMonitor::ConnectNetFolder(Int_t i)
{
   if (!(0 <= i && i < fNumberOfNetFolders))
      return kFALSE;

   if (!fNetFolderActive[i])
      return kTRUE;

   DisconnectNetFolder(i);
   fNetFolderSocket[i] = new TSocket(fNetFolderHost[i].Data(), fNetFolderPort[i]);
   while (!fNetFolderSocket[i]->IsValid()) {
      delete fNetFolderSocket[i];
      Print("can not make socket connection for ");
      Print(fNetFolderName[i].Data());
      Println(".");
      Println("program sleeps for 5s and tries again.");
      gSystem->Sleep(5000);
      fNetFolderSocket[i] = new TSocket(fNetFolderHost[i].Data(), fNetFolderPort[i]);
   }
   fNetFolder[i] = new TNetFolder(fNetFolderRoot[i].Data(), fNetFolderName[i].Data(), fNetFolderSocket[i], fNetFolderReconnect[i]);
   ROMEString errMessage;
   if (!fNetFolder[i]->GetPointer()) {
      errMessage.SetFormatted("%s failed to connect to %s folder of %s.", fNetFolderName[i].Data(), fNetFolderRoot[i].Data(), fNetFolderHost[i].Data());
      Warning("ConnectNetFolder", errMessage);
   }

   return kTRUE;
}

Bool_t ArgusMonitor::DisconnectNetFolder(const Char_t * name)
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name))
         break;
   }
   return DisconnectNetFolder(i);
}

Bool_t ArgusMonitor::DisconnectNetFolder(Int_t i)
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

Bool_t ArgusMonitor::ConnectNetFolders()
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!ConnectNetFolder(i))
         return kFALSE;
   }
   return kTRUE;
}

Bool_t ArgusMonitor::DisconnectNetFolders()
{
   Int_t i;
   for (i = 0; i < fNumberOfNetFolders; i++) {
      if (!DisconnectNetFolder(i))
         return kFALSE;
   }
   return kTRUE;
}

void ArgusMonitor::InitNetFolders(Int_t number)
{
   if (number < 1)
      return;
   fNetFolder = new TNetFolder *[number];
   fNetFolderActive = new Bool_t[number];
   fNetFolderReconnect = new Bool_t[number];
   fNetFolderSocket = new TSocket *[number];
   fNetFolderPort = new Int_t[number];
   fNetFolderName = new ROMEString[number];
   fNetFolderHost = new ROMEString[number];
   fNetFolderRoot = new ROMEString[number];
   fNumberOfNetFolders = number;
}

Bool_t ArgusMonitor::DAQInit()
{
   // Initialize the analyzer. Called before the init tasks.

   // Check IO System
   if (gArgus->GetNumberOfRunNumbers() > 0 && gArgus->GetNumberOfInputFileNames() > 0)
      gArgus->SetIOType(gArgus->kRunNumberAndFileNameBased);
   else if (gArgus->GetNumberOfRunNumbers() > 0)
      gArgus->SetIOType(gArgus->kRunNumberBased);
   else if (gArgus->GetNumberOfInputFileNames() > 0)
      gArgus->SetIOType(gArgus->kFileNameBased);
   else if (gArgus->isOffline() && strcmp(gArgus->GetActiveDAQ()->GetName(), "none")) {
      gArgus->Println("No run numbers or input file names specified.\n");
      return kFALSE;
   }
   // Initialize DAQ System
   if (!gArgus->GetActiveDAQ()->Init())
      return kFALSE;

   return kTRUE;
}

Bool_t ArgusMonitor::DAQBeginOfRun()
{
   // Connect the Analyzer to the current run. Called before the BeginOfRun tasks.
   ROMEString runNumberString;

   if (gArgus->isOffline() && (gArgus->IsRunNumberBasedIO() || gArgus->IsRunNumberAndFileNameBasedIO())) {
      // Check Configuration
      gArgus->SetCurrentRunNumber(gArgus->GetRunNumberAt(0));
      gArgus->GetConfiguration()->CheckConfiguration(gArgus->GetCurrentRunNumber());
   }
   // Begin Of Run Of Active DAQ
   if (!gArgus->GetActiveDAQ()->BeginOfRun())
      return kFALSE;

   // Check Configuration
   if (gArgus->isOffline() && gArgus->IsFileNameBasedIO()) {
      // file name based IO
      gArgus->GetConfiguration()->CheckConfiguration(gArgus->GetCurrentInputFileName());
   }
   // Set Run Number
   gArgus->GetCurrentRunNumberString(runNumberString);

   // Update Data Base
   if (!gArgus->ReadSingleDataBaseFolders()) {
      gArgus->Println("\nError while reading the data base !");
      return kFALSE;
   }
   this->InitArrayFolders();
   if (!gArgus->ReadArrayDataBaseFolders()) {
      gArgus->Println("\nError while reading the data base !");
      return kFALSE;
   }

   return true;
}

Bool_t ArgusMonitor::DAQEvent(Int_t event)
{
   // Reads an event. Called before the Event tasks.
   this->ResetFolders();

   if (gArgus->isOffline()) {
      // check event numbers
      Int_t status = gArgus->CheckEventNumber(event);
      if (status == 0) {
         return kTRUE;
      }
      if (status == -1) {
         return kTRUE;
      }
   }

   if (!gArgus->GetActiveDAQ()->Event(event))
      return kFALSE;

   return kTRUE;
}

Bool_t ArgusMonitor::DAQEndOfRun()
{
   // Disconnects the current run. Called after the EndOfRun tasks.

   if (!gArgus->GetActiveDAQ()->EndOfRun())
      return kFALSE;

   return kTRUE;
}

Bool_t ArgusMonitor::DAQTerminate()
{
   // Clean up the analyzer. Called after the Terminate tasks.
   // Write accumulative output tree files
   // Close all files

   if (!gArgus->GetActiveDAQ()->Terminate())
      return kFALSE;

   return kTRUE;
}
