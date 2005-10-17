/********************************************************************
  ArgusMonitor.cpp, R.Sawada

  $Id:$

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

ClassImp(ArgusMonitor)
   
ArgusMonitor* gArgus;        // global ArgusMonitor Handle
void*         gPassToArgus;  // void ArgusMonitor Handle

#if defined( USE_TRINT )
ArgusMonitor::ArgusMonitor(TRint *app)
#else
ArgusMonitor::ArgusMonitor(TApplication *app)
#endif
{
// Initialisations
   fApplication = app;
//   fOnlineHost = "";
//   fOnlineExperiment = "";
   fWindowScale = 1;
   fRunNumber = 0;
}

ArgusMonitor::~ArgusMonitor() {
}

Bool_t ArgusMonitor::Start(Int_t argc, Char_t **argv)
{
   gArgus = (ArgusMonitor*)gPassToArgus;
   
   if (gROOT->IsBatch()) {
      printf("%s: cannot run in batch mode\n", GetProgramName());
      return kFALSE;
   }
   
   if (!ReadParameters(argc,argv)) return kFALSE;
   
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
   
   if(!gArgus->StartMonitor())
      return kFALSE;
   
   return kTRUE;
}

Bool_t ArgusMonitor::StartMonitor()
{
   // Connect net folders
   if(!ConnectNetFolders())
      return kFALSE;
   
   // Read Data Base
   InitSingleFolders();
   if (!ReadSingleDataBaseFolders()){
      Error("StartMonitor","Error while reading the data base !");
      return kFALSE;
   }
   InitArrayFolders();
   if (!ReadArrayDataBaseFolders()) {
      Error("StartMonitor","while reading the data base !");
      return kFALSE;
   }
  
   // main window 
   if(!StartWindow())
      return kFALSE;
   
   fApplication->Run();
   
   return kTRUE;
}

void ArgusMonitor::ParameterUsage()
{
   cout<<"  -i       Configuration file (default argusConfig.xml)"<<endl
/*
       <<"  -h       MIDAS server host name"<<endl
       <<"  -e       MIDAS experiment name"<<endl
*/
       <<"  -docu    Generates a Root-Html-Documentation (no Argument)"<<endl;
   gArgus->UserParameterUsage();
   return;
}

Bool_t ArgusMonitor::ReadParameters(Int_t argc, Char_t *argv[])
{
   // Reads the Inputlineparameters
   Int_t i;
   const Int_t workDirLen = 1000;
   ROMEString workDir(workDirLen);
   getcwd((Char_t*)workDir.Data(),workDirLen);
   workDir.Append("/");
   this->SetConfigDir(workDir);
   ROMEString configFile("argusConfig.xml");
   
   Char_t host_name[256] = "";
   Char_t exp_name[32] = "";
#if defined( HAVE_MIDAS )
   cm_get_environment(host_name,sizeof(host_name),exp_name,sizeof(exp_name));
#endif
   
   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-help")||!strcmp(argv[i],"--help")) {
         ParameterUsage();
         return kFALSE;
      }
      if (!strcmp(argv[i],"-docu")) {
         THtml html;
         html.MakeAll(kTRUE);
         return kFALSE;
      }
      if (!strcmp(argv[i],"-i")&&i<argc-1) {
         configFile = argv[i+1];
         i++;
      }
   }
   
   Char_t answer = 0;
   struct stat buf;
   if( stat( configFile.Data(), &buf )) {
      cout << "Configuration file '" << configFile.Data() << "' not found." << endl;
      cout << "Do you like the framework to generate a new configuration file ([y]/n) ? " << flush;
      answer = getchar();
      if (answer!='n') {
         if (!this->fConfiguration->WriteConfigurationFile((Char_t*)configFile.Data())) {
            cout << "\nTerminate program.\n" << endl;
            return kFALSE;
         }
         cout << "\nThe framework generated a new configuration file." << endl;
         cout << "Please edit this file and restart the program.\n" << endl;
      }
      else {
         cout << "\nTerminate program.\n" << endl;
      }
      return kFALSE;
   }
   if (!this->GetConfiguration()->ReadConfigurationFile((Char_t*)configFile.Data())) {
      cout << "\nTerminate program.\n" << endl;
      return kFALSE;
   }
   if (!this->fConfiguration->WriteConfigurationFile((Char_t*)configFile.Data())) {
      cout << "\nTerminate program.\n" << endl;
      return kFALSE;
   }

   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-h")){
	 strcpy(host_name, argv[i+1]);
	 i++;
      }
      else if (!strcmp(argv[i],"-e")){
	 strcpy(exp_name, argv[i+1]);
	 i++;
      }
      else if (!strcmp(argv[i],"-help")||!strcmp(argv[i],"--help"))
	 ;
      else if (!strcmp(argv[i],"-docu"))
	 ;
      else if (!strcmp(argv[i],"-i"))
	 i++;
      else if(!ReadUserParameter(argv[i], i<argc-1 ? argv[i+1] : "", i)){
         cout<<"Inputlineparameter '"<<argv[i]<<"' not available."<<endl
             <<"Available inputlineparameter are : "<<endl;
         ParameterUsage();
         return kFALSE;
      }
   }
   if(strlen(host_name))
      gArgus->SetOnlineHost(host_name);
   if(strlen(exp_name))
      gArgus->SetOnlineExperiment(exp_name);
   
   return kTRUE;
}

Int_t ArgusMonitor::stricmp(const Char_t* c1,const Char_t* c2)
{
#if defined( R__UNIX )
   return strcasecmp(c1,c2);
#elif defined( R__VISUAL_CPLUSPLUS )
   return _stricmp(c1,c2);
#else
   return 0;
#endif
}

Bool_t ArgusMonitor::strtobool(const Char_t* str) 
{
   Char_t *cstop;
   if (!strcmp(str,"true"))
      return kTRUE;
   if (!strcmp(str,"false"))
      return kFALSE;
   return strtol(str,&cstop,10)!=0;
}

ROMEDataBase* ArgusMonitor::GetDataBase(Int_t i) {
   if(i<fNumberOfDataBases && fDataBaseHandle[i]!=NULL)
      return fDataBaseHandle[i];
   Error("GetDataBase","You have tried to access a database without initialisation.\nTo use the databases you have to add it to the list of databases in the\nROME configuration file under <DataBases>.\n\nShutting down the program.");
#if defined( USE_TRINT )
   fApplication->Terminate(1);
#else
   fApplication->Terminate();
#endif
   return NULL;
};

ROMEDataBase* ArgusMonitor::GetDataBase(const Char_t *name) { 
   for (Int_t i=0;i<fNumberOfDataBases;i++) 
      if (!stricmp(fDataBaseHandle[i]->GetName(),name))
         return fDataBaseHandle[i];
   ROMEString str;
   Error("GetDataBase","\nYou have tried to access the %s database without initialisation.\nTo use the %s database you have to add it to the list of databases in the\nROME configuration file under <DataBases>.\n\nShutting down the program.\n",name,name);
#if defined( USE_TRINT )
   fApplication->Terminate(1);
#else
   fApplication->Terminate();
#endif
   return NULL;
};

Bool_t ArgusMonitor::isDataBaseActive(const Char_t *name) { 
   for (Int_t i=0;i<fNumberOfDataBases;i++) 
      if (!stricmp(fDataBaseHandle[i]->GetName(),name))
         return kTRUE;
   return kFALSE;
};

void ArgusMonitor::InitDataBases(Int_t number) {
   fDataBaseHandle     = new ROMEDataBase*[number];
   fDataBaseConnection = new ROMEString[number];
   fDataBaseDir        = new ROMEString[number];
   fNumberOfDataBases  = number;
};

TNetFolder* ArgusMonitor::GetNetFolder(const Char_t *name) { 
   for (Int_t i=0;i<fNumberOfNetFolders;i++) {
      if (!stricmp(fNetFolderName[i].Data(),name)){
         if(!fNetFolderActive[i]){
            Warning("GetNetFolder","%s is not activated.",name);
            return NULL;
         }
         return fNetFolder[i];
      }
   }
   Error("GetNetFolder","Netfolder '%s' is not defined",name);
   return NULL;
};

Bool_t ArgusMonitor::ConnectNetFolder(const Char_t* name) {
   Int_t i;
   for(i=0;i<fNumberOfNetFolders;i++){
      if (!stricmp(fNetFolderName[i].Data(),name))
         break;
   }
   return ConnectNetFolder(i);
}

Bool_t ArgusMonitor::ConnectNetFolder(Int_t i) {
   if( ! (0<=i && i<fNumberOfNetFolders) )
      return kFALSE;
   
   if (!fNetFolderActive[i])
      return kTRUE;

   DisconnectNetFolder(i);
   fNetFolderSocket[i] = new TSocket(fNetFolderHost[i].Data(), fNetFolderPort[i]);
   while (!fNetFolderSocket[i]->IsValid()){
      delete fNetFolderSocket[i];
      Print("can not make socket connection for ");
      Print(fNetFolderName[i].Data());
      Println(".");
      Println("program sleeps for 5s and tries again.");
      gSystem->Sleep(5000);
      fNetFolderSocket[i] = new TSocket(fNetFolderHost[i].Data(), fNetFolderPort[i]);
   }
   fNetFolder[i] = new TNetFolder(fNetFolderRoot[i].Data(),fNetFolderName[i].Data(),fNetFolderSocket[i],fNetFolderReconnect[i]);
   ROMEString errMessage;
   if(!fNetFolder[i]->GetPointer()){
      errMessage.SetFormatted("%s failed to connect to %s folder of %s.",fNetFolderName[i].Data(),fNetFolderRoot[i].Data(),fNetFolderHost[i].Data());
      Warning("ConnectNetFolder",errMessage);
   }

   return kTRUE;
}

Bool_t ArgusMonitor::DisconnectNetFolder(const Char_t* name) {
   Int_t i;
   for(i=0;i<fNumberOfNetFolders;i++){
      if (!stricmp(fNetFolderName[i].Data(),name))
         break;
   }
   return DisconnectNetFolder(i);
}

Bool_t ArgusMonitor::DisconnectNetFolder(Int_t i) {
   if( ! (0<=i && i<fNumberOfNetFolders) )
      return kFALSE;
   if(fNetFolderSocket[i]){
      fNetFolderSocket[i]->Close();
      delete fNetFolderSocket[i];
      fNetFolderSocket[i] = 0;
   }
  
   return kTRUE;
}

Bool_t ArgusMonitor::ConnectNetFolders() {
   Int_t i;
   for(i=0;i<fNumberOfNetFolders;i++){
      if (!ConnectNetFolder(i))
         return kFALSE;
   }
   return kTRUE;
}

Bool_t ArgusMonitor::DisconnectNetFolders() {
   Int_t i;
   for(i=0;i<fNumberOfNetFolders;i++){
      if (!DisconnectNetFolder(i))
         return kFALSE;
   }
   return kTRUE;
}

void ArgusMonitor::InitNetFolders(Int_t number) {
   if(number<1)
      return;
   fNetFolder            = new TNetFolder*[number];
   fNetFolderActive      = new Bool_t[number];
   fNetFolderReconnect   = new Bool_t[number];
   fNetFolderSocket      = new TSocket*[number];
   fNetFolderPort        = new Int_t[number];
   fNetFolderName        = new ROMEString[number];
   fNetFolderHost        = new ROMEString[number];
   fNetFolderRoot        = new ROMEString[number];
   fNumberOfNetFolders   = number;
}

void ArgusMonitor::Print(Char_t text)
{
   cout << text;
   return;
}

void ArgusMonitor::Print(const Char_t* text)
{
   cout << text;
   return;
}

void ArgusMonitor::Println(const Char_t* text)
{
   cout << text << endl;
   return;
}

void ArgusMonitor::Printfl(const Char_t* text)
{
   cout << text << flush;
   return;
}
