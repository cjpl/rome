/********************************************************************
  ArgusMonitor.cpp, R.Sawada

  $Log$
  Revision 1.16  2005/05/05 20:08:05  sawada
  code clean up.

  Revision 1.15  2005/04/22 12:58:24  schneebeli_m
  removed windows errors

  Revision 1.14  2005/04/12 17:51:05  sawada
  changes for mxml

  Revision 1.13  2005/03/13 08:40:43  sawada
  modified handling of recursiveDepth.
  removed unused variables.

  Revision 1.12  2005/02/24 23:41:54  sawada
  bug fix of read parameter.

  Revision 1.11  2005/02/24 15:04:03  sawada
  Reduced number of configuration to 1.
  Replaced ss_getchar to getchar().
  Removed LineToProcess.
  Removed bk_is32,bk_find.
  Improved help.
  Handling of midas host and experiment.

  Revision 1.10  2005/02/21 23:07:50  sawada
  several UNIX support

  Revision 1.9  2005/02/21 19:06:56  sawada
  changed platform specifying macros

  Revision 1.8  2005/02/06 00:39:35  sawada
   Changed TRint to TApplication
   Stop method of thread function
   Name of method for start/stop

  Revision 1.7  2005/02/04 22:58:46  sawada
  ROMEFolder

  Revision 1.6  2005/02/03 11:44:54  sawada
  IO to MIDAS ODB

  Revision 1.5  2005/02/02 23:54:30  sawada
  link with midas library.

  Revision 1.4  2005/02/02 18:58:02  sawada
  small change.

  Revision 1.3  2005/02/01 14:44:33  sawada
  Resize frames in window.

  Revision 1.2  2005/01/31 10:59:46  sawada
  bug fix

  Revision 1.1.1.1  2005/01/29 22:45:08  sawada
  Advanced Root based GUi monitoring System


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
#include <ROMEDataBase.h>
#include <ROMENoDataBase.h>
#include <ArgusMonitor.h>

ClassImp(ArgusMonitor)
   
ArgusMonitor *gArgus;  // global ArgusMonitor Handle
void *gPassToArgus;  // void ArgusMonitor Handle

#if defined( USE_TRINT )
ArgusMonitor::ArgusMonitor(TRint *app)
#else
ArgusMonitor::ArgusMonitor(TApplication *app)
#endif
{
// Initialisations
   fApplication = app;
   fDataBaseConnection = "";
   fDataBaseHandle = new ROMENoDataBase();
   fOnlineHost = "";
   fOnlineExperiment = "";
   fSocketInterfacePortNumber = 9090;
   fSocketInterfaceHost = "";
   fWindowScale = 1;
   fRunNumber = 0;
}

ArgusMonitor::~ArgusMonitor() {
}

Bool_t ArgusMonitor::Start(Int_t argc, Char_t **argv)
{
   gArgus = (ArgusMonitor*)gPassToArgus;
   
   if (gROOT->IsBatch()) {
      printf("%s: cannot run in batch mode\n", argv[0]);
      return kFALSE;
   }
   
   if (!ReadParameters(argc,argv)) return kFALSE;
   
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
   
   if(!gArgus->StartMonitor())
      return kFALSE;
   
   return kTRUE;
}

void ArgusMonitor::ParameterUsage()
{
   cout<<"  -i       Configuration file (default argusConfig.xml)"<<endl
       <<"  -h       MIDAS server host name"<<endl
       <<"  -e       MIDAS experiment name"<<endl
       <<"  -docu    Generates a Root-Html-Documentation (no Argument)"<<endl;
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
   this->SetDataBaseDir(workDir);
   
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
      else {
         cout<<"Inputlineparameter '"
	     <<argv[i]
	     <<"' not available."<<endl
	     <<"Available inputlineparameters are : "<<endl;
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

Bool_t ArgusMonitor::strtobool(const Char_t* str) 
{
   Char_t *cstop;
   if (!strcmp(str,"true"))
      return kTRUE;
   if (!strcmp(str,"false"))
      return kFALSE;
   return strtol(str,&cstop,10)!=0;
}
