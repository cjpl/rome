// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMEAnalyzer
//
//  Basic class in ROME. This class is the base-class
//  for the analysis dependent main class, which should implement the
//  Folders, Trees and Task definitions.
//
//  $Log$
//  Revision 1.28  2004/09/30 10:18:05  schneebeli_m
//  gAnalyzer and gROME
//
//  Revision 1.27  2004/09/30 09:50:29  schneebeli_m
//  Samples updated
//
//  Revision 1.26  2004/09/25 01:36:36  schneebeli_m
//  implemented FW dependent EventLoop and DataBase classes
//
//  Revision 1.25  2004/09/25 01:34:48  schneebeli_m
//  implemented FW dependent EventLoop and DataBase classes
//
//  Revision 1.24  2004/09/23 15:48:16  midas
//  Added tag in header
//
//////////////////////////////////////////////////////////////////////////

#if defined( _MSC_VER )
#include <io.h>
#include <direct.h>
#define O_RDONLY_BINARY O_RDONLY | O_BINARY
#endif
#if defined ( __linux__ )
#include <unistd.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/types.h>
#include <TThread.h>
#define O_RDONLY_BINARY O_RDONLY
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <THtml.h>
#include <TArrayI.h>
#include <TTask.h>
#include <TTree.h>
#include <TFolder.h>
#include <TList.h>
#include <ROMEString.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TBrowser.h>
#include <TH1.h>
#include <TMath.h>
#include <ROMEAnalyzer.h>
#include <ROMEEventLoop.h>
#include <ROMETree.h>
#include <ROMETask.h>
#include <ROMEString.h>
#include <ROMEXMLDataBase.h>
#include <ROMENoDataBase.h>
#include <Riostream.h>

ClassImp(ROMEAnalyzer)

ROMEAnalyzer *gROME;  // global ROMEAnalyzer Handle
void *gPassToROME;  // void ROMEAnalyzer Handle

const char* ROMEAnalyzer::LineToProcess = NULL;
void writeLineToProcess(const char* str) {
   while (ROMEAnalyzer::LineToProcess) {
      ROMEStatic::ss_sleep(10);
   }
   ROMEAnalyzer::LineToProcess = str;
}

void StartServer(int port);


ROMEAnalyzer::ROMEAnalyzer(TRint *app)
{
// Initialisations

   int i=0;
   fApplication = app;
   fSplashScreen = true;
   fBatchMode = false;
   fTerminate = false;
   fAnalysisMode = kAnalyzeOffline; 
   fDataFormat = kRoot; 
   fCurrentRunNumber = 0; 
   fTreeAccumulation = false;
   fTreeObjects = new TObjArray(0);
   fEventID = 'a';
   fOnlineHost = "";
   fDontReadNextEvent = false;
   fDataBaseConnection = "";
   fDataBaseHandle = new ROMENoDataBase();
}

ROMEAnalyzer::~ROMEAnalyzer() {
}


bool ROMEAnalyzer::Start(int argc, char **argv)
{
// Starts the ROME Analyzer
   gROME = (ROMEAnalyzer*)gPassToROME;

   consoleStartScreen();

   fMainTask->ExecuteTask("init");

   if (!ReadParameters(argc,argv)) return false;

   if (isSplashScreen()) startSplashScreen();

   int port = 9091;
   StartServer(port);
   printf("Root server listening on port %d\n\n\n", port);

   cout << "Program steering" << endl;
   cout << "----------------" << endl;
   cout << "q : Terminates the program" << endl;
   cout << "e : Ends the program" << endl;
   cout << "s : Stopps the program" << endl;
   cout << "r : Restarts the program" << endl;
   cout << "c : Continuous Analysis" << endl;
   cout << "o : Step by step Analysis" << endl;
   cout << "i : Root interpreter" << endl;
   cout << endl;

   CreateHistoFolders();

   fMainTask->ExecuteTask("start");
   if (fTerminate) return false;

//   if (!isBatchMode()) TBrowser *t = new TBrowser();

   return true;
}

void ROMEAnalyzer::ParameterUsage()
{
   cout << "  -i       Configuration file (default ROMEConfig.xml)" << endl;
   cout << "  -b       Batch Mode (no Argument)" << endl;
   cout << "  -ns      Splash Screen is not displayed (no Argument)" << endl;
   cout << "  -m       Analysing Mode : (online/[offline])" << endl;
   cout << "  -f       Input Data Format : ([midas]/root)" << endl;
   cout << "  -r       Runnumbers" << endl;
   cout << "  -e       Eventnumbers" << endl;
   cout << "  -docu    Generates a Root-Html-Documentation (no Argument)" << endl;
   return;
}
bool ROMEAnalyzer::ReadParameters(int argc, char *argv[])
{
   // Reads the Inputlineparameters
   int i;
   const int workDirLen = 1000;
   ROMEString workDir(workDirLen);
   getcwd((char*)workDir.Data(),workDirLen);
   workDir.Append("/");
   this->SetDataBaseDir(workDir);
   this->SetInputDir(workDir);
   this->SetOutputDir(workDir);

   ROMEString configFile("romeConfig.xml");

   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-h")||!strcmp(argv[i],"-help")) {
         ParameterUsage();
         return false;
      }
      if (!strcmp(argv[i],"-docu")) {
         THtml html;
         html.MakeAll(true);
         return false;
      }
      if (!strcmp(argv[i],"-i")&&i<argc-1) {
         configFile = argv[i+1];
         i++;
      }
   }
   char answer = 0;
   struct stat buf;
   if( stat( configFile.Data(), &buf )) {
      cout << "Configuration file '" << configFile.Data() << "' not found." << endl;
      cout << "Do you like the framework to generate a new configuration file ([y]/n) ? ";
      while (answer==0) {
         while (ROMEStatic::ss_kbhit()) {
            answer = ROMEStatic::ss_getchar(0);
         }
      }
      if (answer!='n') {
         cout << "\nThe framework generated a new configuration file." << endl;
         cout << "Please edit this file and restart the program.\n" << endl;
         if (!WriteROMEConfigXML((char*)configFile.Data())) {
            cout << "\nTerminate program.\n" << endl;
            return false;
         }
      }
      else {
         cout << "\nTerminate program.\n" << endl;
      }
      return false;
   }
   if (!ReadROMEConfigXML((char*)configFile.Data())) {
      cout << "\nTerminate program.\n" << endl;
      return false;
   }
   if (!WriteROMEConfigXML((char*)configFile.Data())) {
      cout << "\nTerminate program.\n" << endl;
      return false;
   }

   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-b")) {
         fBatchMode = true;
      }
      else if (!strcmp(argv[i],"-ns")) {
         fSplashScreen = false;
      }
      else if (!strcmp(argv[i],"-m")) {
         if (!strcmp(argv[i+1],"online")) this->SetOnline();
         else this->SetOffline();
         i++;
      }
      else if (!strcmp(argv[i],"-f")) {
         if (!strcmp(argv[i+1],"root")) this->SetRoot();
         else this->SetMidas();
         i++;
      }
      else if (!strcmp(argv[i],"-r")&&i<argc-1) {
         this->SetRunNumbers(argv[i+1]);
         i++;
      }
      else if (!strcmp(argv[i],"-e")&&i<argc-1) {
         this->SetEventNumbers(argv[i+1]);
         i++;
      }
      else if (!strcmp(argv[i],"-i")) {
         i++;
      }
      else {
         cout << "Inputlineparameter '" << argv[i] << "' not available." << endl;
         cout << "Available inputlineparameter are : " << endl;
         cout << endl;
         ParameterUsage();
	      return false;
      }
   }

   return true;
}

void ROMEAnalyzer::CreateHistoFolders()
{
   // Creates a Folder for each active Task
   int i,j;
   ROMEString name;
   ROMEString title;
   TFolder *fHistoFolder = fMainFolder->AddFolder("histos","Histogram Folder");
   TList *taskList = fMainTask->GetListOfTasks();
   for (i=0;i<taskList->GetSize();i++) {
      ROMETask *task = (ROMETask*)taskList->At(i);
      if (!task->IsActive() || !task->hasHistograms()) continue;
      name.SetFormatted("%sHistos",task->GetName());
      title.SetFormatted("Histograms of Task '%s'",task->GetName());
      TFolder *folder = fHistoFolder->AddFolder(name.Data(),title.Data());
      TList *subTaskList = task->GetListOfTasks();
      for (j=0;j<subTaskList->GetSize();j++) {
         ROMETask *task = (ROMETask*)subTaskList->At(j);
         if (!task->IsActive() || !task->hasHistograms()) continue;
         name.SetFormatted("%sHistos",task->GetName());
         title.SetFormatted("Histograms of Task '%s'",task->GetName());
         folder->AddFolder(name.Data(),title.Data());
      }
   }
}






#ifndef HAVE_MIDAS
#define PTYPE int
#endif
#if defined ( __linux__ )
#define THREADRETURN NULL
#define THREADTYPE void*
#endif
#if defined( _MSC_VER )
#define THREADRETURN 0
#define THREADTYPE DWORD WINAPI
#endif

TFolder *ReadFolderPointer(TSocket *fSocket)
{
   //read pointer to current folder
   TMessage *message = new TMessage(kMESS_OBJECT);
   fSocket->Recv(message);
   Int_t p;
   *message>>p;
   delete message;
   return (TFolder*)p;
}

int ResponseFunction(TSocket *fSocket) {
   char str[80];
   if (fSocket->Recv(str, sizeof(str)) <= 0) {
      fSocket->Close();
      delete fSocket;
      return 0;
   }
   TMessage *message = new TMessage(kMESS_OBJECT);
   if (strcmp(str, "GetListOfFolders") == 0) {
      TFolder *folder = ReadFolderPointer(fSocket);
      if (folder==NULL) {
         message->Reset(kMESS_OBJECT);
         message->WriteObject(NULL);
         fSocket->Send(*message);
         delete message;
         return 1;
      }

      //get folder names
      TObject *obj;
      TObjArray *names = new TObjArray(100);

      TCollection *folders = folder->GetListOfFolders();
      TIterator *iterFolders = folders->MakeIterator();
      while (obj = iterFolders->Next()) {
         ROMEString str;
         str.SetFormatted("%s",obj->GetName());
         names->Add(new TObjString(str.Data()));
      }

      //write folder names
      message->Reset(kMESS_OBJECT);
      message->WriteObject(names);
      fSocket->Send(*message);

      for (int i = 0; i < names->GetLast() + 1; i++)
         delete(TObjString *) names->At(i);

      delete names;

      delete message;
      return 1;
   }
   else if (strncmp(str, "FindObject", 10) == 0) {
      TFolder *folder = ReadFolderPointer(fSocket);

      //get object
      TObject *obj;
      if (strncmp(str+10, "Any", 3) == 0) {
         obj = folder->FindObjectAny(str+14);
      }
      else {
         obj = folder->FindObject(str+11);
      }

      //write object
      if (!obj) {
         fSocket->Send("Error");
      } else {
         message->Reset(kMESS_OBJECT);
         message->WriteObject(obj);
         fSocket->Send(*message);
      }
      delete message;
      return 1;
   }

   else if (strncmp(str, "FindFullPathName", 16) == 0) {
      TFolder *folder = ReadFolderPointer(fSocket);

      //find path
      const char* path = folder->FindFullPathName(str+17);

      //write path
      if (!path) {
         fSocket->Send("Error");
      } else {
         TObjString *obj = new TObjString(path);
         message->Reset(kMESS_OBJECT);
         message->WriteObject(obj);
         fSocket->Send(*message);
         delete obj;
      }
      delete message;
      return 1;
   }

   else if (strncmp(str, "Occurence", 9) == 0) {
      TFolder *folder = ReadFolderPointer(fSocket);

      //read object
      message->Reset(kMESS_OBJECT);
      fSocket->Recv(message);
      TObject *obj = ((TObject*) message->ReadObject(message->GetClass()));

      //get occurence
      Int_t retValue = folder->Occurence(obj);

      //write occurence
      message->Reset(kMESS_OBJECT);
      *message<<retValue;
      fSocket->Send(*message);

      delete message;
      return 1;
   }

   else if (strncmp(str, "GetPointer", 10) == 0) {
      //find object
      TObject *obj = gROOT->FindObjectAny(str+11);

      //write pointer
      message->Reset(kMESS_ANY);
      int p = (PTYPE)obj;
      *message<<p;
      fSocket->Send(*message);

      delete message;
      return 1;
   }
   else if (strncmp(str, "Command", 7) == 0) {
      char objName[100];
      char method[100];
      char type[100];
      char arg[100];
      fSocket->Recv(objName, sizeof(objName));
      fSocket->Recv(type, sizeof(type));
      fSocket->Recv(method, sizeof(method));
      fSocket->Recv(arg, sizeof(arg));

      TString str = "temporarySocketObject = gROOT->FindObjectAny(\"";
      str += objName;
      str += "\");";
      writeLineToProcess(str.Data());
      str = "((";
      str += type;
      str += ")temporarySocketObject)->";
      str += method;
      str += "(";
      str += arg;
      str += ");";
      writeLineToProcess(str.Data());

      delete message;
      return 1;
   }
/*
   else if (strncmp(str, "CLEAR", 5) == 0) {
      TObject *obj;

      TCollection *folders = ((TFolder*)gROOT->FindObjectAny(currentDirectory[directoryDepth].Data()))->GetListOfFolders();
      TIterator *iterFolders = folders->MakeIterator();
      while (obj = iterFolders->Next()) {
         if (strcmp(str + 6, obj->GetName()) == 0)
            break;
      }

      if (!obj) {
         fSocket->Send("Error");
      } else {
#if defined ( __linux__ )
         TThread::Lock();
         ((TH1 *) obj)->Reset();
         TThread::UnLock();
#endif
         fSocket->Send("OK");
      }
   }*/
   return 1;
}




THREADTYPE Server(void *arg)
{
   TSocket *fSocket = (TSocket *) arg;

   while (ResponseFunction(fSocket))
   {}
   return THREADRETURN;
}


THREADTYPE ServerLoop(void *arg)
{
// Server loop listening for incoming network connections on port
// specified by command line option -s. Starts a searver_thread for
// each connection.
   int port = *(int*)arg;
   TServerSocket *lsock = new TServerSocket(port, kTRUE);
   writeLineToProcess("TObject* temporarySocketObject;");

   do {
      TSocket *sock = lsock->Accept();

#if defined ( __linux__ )
      TThread *thread = new TThread("Server", Server, sock);
      thread->Run();
#endif
#if defined( _MSC_VER )
      LPDWORD lpThreadId=0;
      CloseHandle(CreateThread(NULL,1024,&Server,sock,0,lpThreadId));
#endif
   } while (1);
   return THREADRETURN;
}
int pport;
void StartServer(int port) {
   pport = port;
// start fSocket server loop
#if defined ( __linux__ )
   TThread *thread = new TThread("server_loop", ServerLoop, &pport);
   thread->Run();
#endif
#if defined( _MSC_VER )
   LPDWORD lpThreadId=0;
   CloseHandle(CreateThread(NULL,1024,&ServerLoop,&pport,0,lpThreadId));
#endif
}

TArrayI ROMEAnalyzer::decodeRunNumbers(ROMEString& str)
{
   char cminus='-';
   char ccomma=',';
   char csemi =';';
   char *pstr = (char*)str.Data();
   int num;
   int na=0;
   int nat=1;
   int arraySize = 1000;
   TArrayI arr(arraySize);
   while (pstr<str.Data()+str.Length()) {
      if (na>=arraySize*nat) {
         nat++;
         arr.Set(arraySize*nat);
      }
      num = strtol(pstr,&pstr,10);
      if (pstr[0]==cminus) {
         arr.AddAt(-num,na);
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
         return arr;
      }
   }
   arr.Set(na);
   return arr;
}

