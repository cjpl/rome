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
//  Revision 1.47  2004/12/07 15:03:03  schneebeli_m
//  online steering
//
//  Revision 1.46  2004/12/06 16:03:02  sawada
//  code cleanup (tab -> space)
//
//  Revision 1.45  2004/12/06 11:25:23  schneebeli_m
//  user input on linux
//
//  Revision 1.42  2004/12/06 09:20:50  schneebeli_m
//  ss_getchar on linux
//
//  Revision 1.41  2004/12/03 14:42:08  schneebeli_m
//  some minor changes
//
//  Revision 1.40  2004/12/02 17:46:43  sawada
//  Macintosh port
//
//  Revision 1.39  2004/11/19 13:29:55  schneebeli_m
//  added stuff for sample
//
//  Revision 1.38  2004/11/16 16:14:00  schneebeli_m
//  implemented task hierarchy
//
//  Revision 1.37  2004/11/11 12:55:27  schneebeli_m
//  Implemented XML database with new path rules
//
//  Revision 1.36  2004/10/20 09:22:16  schneebeli_m
//  bugs removed
//
//  Revision 1.35  2004/10/20 03:12:28  pierre
//  add termios.h for linux, comment out undef _daemon_flag
//
//  Revision 1.34  2004/10/15 11:51:28  schneebeli_m
//  bugs removed
//
//  Revision 1.33  2004/10/14 09:53:41  schneebeli_m
//  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag
//
//  Revision 1.32  2004/10/05 13:30:32  schneebeli_m
//  make -e, Port number
//
//  Revision 1.31  2004/10/05 08:12:33  schneebeli_m
//  linked on linux
//
//  Revision 1.30  2004/10/05 07:52:44  schneebeli_m
//  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed
//
//  Revision 1.29  2004/10/01 14:33:29  schneebeli_m
//  Fixed some tree file problems
//
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
#ifndef __CINT__
#include <Windows4Root.h>
#include <conio.h>
#endif
#include <io.h>
#include <direct.h>
#include <windows.h>
#define O_RDONLY_BINARY O_RDONLY | O_BINARY
#endif
#if defined ( __linux__ ) || defined ( __APPLE__ )
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <TThread.h>
#define O_RDONLY_BINARY O_RDONLY
#endif
#if defined ( __linux__ )
#include <sys/io.h>
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
      gROME->ss_sleep(10);
   }
   ROMEAnalyzer::LineToProcess = str;
}

void StartServer(int port);


ROMEAnalyzer::ROMEAnalyzer(TRint *app)
{
// Initialisations

   int i=0;
   fLastEventNumberIndex = 0;
   fApplication = app;
   fAnalysisMode = kAnalyzeOffline; 
   fDataFormat = kRoot; 
   fBatchMode = false;
   fSplashScreen = true;
   fDontReadNextEvent = false;
   fCurrentRunNumber = 0; 
   fEventID = 'a';
   fTerminate = false;
   fTreeAccumulation = false;
   fPortNumber = 9090;
   fSocketOffline = false;
   fTreeObjects = new TObjArray(0);
   fOnlineHost = "";
   fDataBaseConnection = "";
   fDataBaseHandle = new ROMENoDataBase();
   fPortNumber = 9090;
   fSocketOffline = false;
}

ROMEAnalyzer::~ROMEAnalyzer() {
}


bool ROMEAnalyzer::Start(int argc, char **argv)
{
// Starts the ROME Analyzer

   gROME = (ROMEAnalyzer*)gPassToROME;

   gROME->ss_getchar(0);

   fMainTask->ExecuteTask("init");

   if (!ReadParameters(argc,argv)) return false;

   if (this->isBatchMode()) {
      redirectOutput();
   }

   consoleStartScreen();

   if (isSplashScreen()) startSplashScreen();

   if (gROME->isOnline() || gROME->isSocketOffline()) {
      StartServer(gROME->GetPortNumber());
      printf("Root server listening on port %d\n\n\n", gROME->GetPortNumber());
   }

   cout << "Program steering" << endl;
   cout << "----------------" << endl;
   cout << "q : Terminates the program" << endl;
   cout << "e : Ends the program" << endl;
   cout << "s : Stops the program" << endl;
   cout << "r : Restarts the program" << endl;
   cout << "c : Continuous Analysis" << endl;
   cout << "o : Step by step Analysis" << endl;
   cout << "g : Run until event #" << endl;
   cout << "i : Root interpreter" << endl;
   cout << endl;

   TFolder *fHistoFolder = fMainFolder->AddFolder("histos","Histogram Folder");
   TList *taskList = fMainTask->GetListOfTasks();
   CreateHistoFolders(taskList,fHistoFolder);

   fMainTask->ExecuteTask("start");

   gROME->ss_getchar(1);
   if (fTerminate) return false;

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
      cout << "Do you like the framework to generate a new configuration file ([y]/n) ? " << flush;
      while (answer==0) {
         while (this->ss_kbhit()) {
            answer = this->ss_getchar(0);
         }
      }
      if (answer!='n') {
         if (!this->fConfiguration->WriteConfigurationFile((char*)configFile.Data())) {
            cout << "\nTerminate program.\n" << endl;
            return false;
         }
         cout << "\nThe framework generated a new configuration file." << endl;
         cout << "Please edit this file and restart the program.\n" << endl;
      }
      else {
         cout << "\nTerminate program.\n" << endl;
      }
      return false;
   }
   if (!this->GetConfiguration()->ReadConfigurationFile((char*)configFile.Data())) {
      cout << "\nTerminate program.\n" << endl;
      return false;
   }
   if (!this->fConfiguration->WriteConfigurationFile((char*)configFile.Data())) {
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

bool ROMEAnalyzer::CreateHistoFolders(TList *taskList,TFolder *folder)
{
   // Recursive Creation of Histogram Subfolders
   ROMEString name;
   ROMEString title;
   bool folderCreated = false;
   for (int j=0;j<taskList->GetSize();j++) {
      ROMETask *task = (ROMETask*)taskList->At(j);
      if (!task->IsActive()) continue;
      if (task->hasHistograms())
         folderCreated = true;
      name.SetFormatted("%sHistos",task->GetName());
      title.SetFormatted("Histograms of Task '%s'",task->GetName());
      TFolder *subFolder = folder->AddFolder(name.Data(),title.Data());
      TList *subTaskList = task->GetListOfTasks();
      if (!CreateHistoFolders(subTaskList,subFolder) && !task->hasHistograms())
         folder->Remove(subFolder);
   }
   return folderCreated;
}

#ifndef HAVE_MIDAS
#define PTYPE int
#endif
#if defined ( __linux__ ) || defined ( __APPLE__ )
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
#if defined ( __linux__ ) || defined ( __APPLE__ )
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

#if defined ( __linux__ ) || defined ( __APPLE__ )
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
#if defined ( __linux__ ) || defined ( __APPLE__ )
   TThread *thread = new TThread("server_loop", ServerLoop, &pport);
   thread->Run();
#endif
#if defined( _MSC_VER )
   LPDWORD lpThreadId=0;
   CloseHandle(CreateThread(NULL,1024,&ServerLoop,&pport,0,lpThreadId));
#endif
}

#ifndef HAVE_MIDAS
bool ROMEAnalyzer::bk_is32(void *event)
{
   return ((((BANK_HEADER *) event)->flags & (1<<4)) > 0);
}

int ROMEAnalyzer::bk_find(void* pbkh, const char *name, unsigned long * bklen, unsigned long * bktype,void *pdata)
{
   int tid_size[] = {0,1,1,1,2,2,4,4,4,4,8,1,0,0,0,0,0};
   BANK *pbk;
   BANK32 *pbk32;
   unsigned long dname;

   if (bk_is32(pbkh)) {
      pbk32 = (BANK32 *) (((BANK_HEADER *)pbkh) + 1);
      strncpy((char *) &dname, name, 4);
      do {
         if (*((unsigned long *) pbk32->name) == dname) {
            *((void **) pdata) = pbk32 + 1;
            if (tid_size[pbk32->type & 0xFF] == 0)
               *bklen = pbk32->data_size;
            else
               *bklen = pbk32->data_size / tid_size[pbk32->type & 0xFF];

            *bktype = pbk32->type;
            return 1;
         }
         pbk32 = (BANK32 *) ((char *) (pbk32 + 1) + ALIGN8(pbk32->data_size));
      } while ((unsigned long) pbk32 - (unsigned long) pbkh < ((BANK_HEADER *) pbkh)->data_size + sizeof(BANK_HEADER));
   } else {
      pbk = (BANK *) (((BANK_HEADER *)pbkh) + 1);
      strncpy((char *) &dname, name, 4);
      do {
         if (*((unsigned long *) pbk->name) == dname) {
            *((void **) pdata) = pbk + 1;
            if (tid_size[pbk->type & 0xFF] == 0)
               *bklen = pbk->data_size;
            else
               *bklen = pbk->data_size / tid_size[pbk->type & 0xFF];

            *bktype = pbk->type;
            return 1;
         }
         pbk = (BANK *) ((char *) (pbk + 1) + ALIGN8(pbk->data_size));
      } while ((unsigned long) pbk - (unsigned long) pbkh < ((BANK_HEADER *) pbkh)->data_size + sizeof(BANK_HEADER));
   }
   *((void **) pdata) = NULL;
   return 0;
}
#endif

int ROMEAnalyzer::CheckEventNumber(int eventNumber) 
{
   if (fEventNumber.GetSize()==0)
      return 1;
   for (int i=fLastEventNumberIndex;i<fEventNumber.GetSize();i++) {
      if (fEventNumber.At(i)==eventNumber) {
         fLastEventNumberIndex = i+1;
         return 1;
      }
      else if (fEventNumber.At(i)>eventNumber) {
         fLastEventNumberIndex = i;
         return 0;
      }
   }
   return -1;
}

TArrayI ROMEAnalyzer::decodeRunNumbers(ROMEString& str)
{
   char cminus='-';
   char ccomma=',';
   char csemi =';';
   char *pstr = (char*)str.Data();
   bool bminus = false;
   int num;
   int i;
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
      if (bminus) {
         for (i=arr.At(na-1)+1;i<num;i++) {
            arr.AddAt(i,na);
            na++;
            if (na>=arraySize*nat) {
               nat++;
               arr.Set(arraySize*nat);
            }
         }
         bminus = false;
      }
      if (pstr[0]==cminus) {
         arr.AddAt(num,na);
         na++;
         pstr++;
         bminus = true;
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

Bool_t ROMEAnalyzer::ss_kbhit()
{
#if defined( _MSC_VER )
   return toBool(kbhit());
#endif
#if defined ( __linux__ ) || defined ( __APPLE__ )
   int n;
   ioctl(0, FIONREAD, &n);
   return (n > 0);
#endif
}

int ROMEAnalyzer::ss_sleep(int millisec)
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
      status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

      /* if an alarm signal was cought, restart select with reduced timeout */
      if (status == -1 && timeout.tv_sec >= 1)
         timeout.tv_sec -= 1;

   } while (status == -1);      /* dont return if an alarm signal was cought */

   return 1;
}

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

int ROMEAnalyzer::ss_getchar(bool reset)
{
#if defined ( __linux__ )  || defined ( __APPLE__ )
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

#elif defined( _MSC_VER )

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
