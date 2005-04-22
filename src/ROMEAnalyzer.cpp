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
//  Revision 1.64  2005/04/22 16:25:15  sawada
//  user defined command line option.
//
//  Revision 1.63  2005/04/14 07:56:46  schneebeli_m
//  Implemented odb database (offline)
//
//  Revision 1.62  2005/04/08 17:08:08  schneebeli_m
//  TNetFolderServer changes
//
//  Revision 1.61  2005/04/08 14:55:46  schneebeli_m
//  Added TNetFolderServer class
//
//  Revision 1.60  2005/04/07 08:27:36  schneebeli_m
//  config bug, db bug
//
//  Revision 1.59  2005/04/01 14:56:23  schneebeli_m
//  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ
//
//  Revision 1.58  2005/03/23 09:06:11  schneebeli_m
//  libxml replaced by mxml, Bool SP error
//
//  Revision 1.57  2005/03/21 17:29:47  schneebeli_m
//  minor changes
//
//  Revision 1.56  2005/03/17 15:44:51  schneebeli_m
//  GetAbsolutePath error removed
//
//  Revision 1.55  2005/02/21 21:29:07  sawada
//
//  Changed OS specifying macros
//  Support for DEC,Ultrix,FreeBSD,Solaris
//
//  Revision 1.54  2005/01/31 17:13:33  schneebeli_m
//  cm_set_msg_print
//
//  Revision 1.53  2005/01/27 16:40:33  schneebeli_m
//  cm_msg in print method
//
//  Revision 1.52  2005/01/27 16:21:06  schneebeli_m
//  print method & no gROME in path
//
//  Revision 1.51  2005/01/24 15:45:04  schneebeli_m
//  ss_millitime
//
//  Revision 1.50  2005/01/24 15:38:04  schneebeli_m
//  ss_millitime
//
//  Revision 1.49  2005/01/24 15:25:08  schneebeli_m
//  Seperated DAQ classes
//
//  Revision 1.48  2004/12/21 08:49:54  schneebeli_m
//  Config SP read error
//
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

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#ifndef __CINT__
#include <Windows4Root.h>
#include <conio.h>
#endif
#include <direct.h>
#include <windows.h>
#define O_RDONLY_BINARY O_RDONLY | O_BINARY
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
#define O_RDONLY_BINARY O_RDONLY
#include <sys/time.h>
#endif

#ifndef R__MACOSX
#if defined( R__VISUAL_CPLUSPLUS )
#include <io.h>
#endif
#if defined( R__UNIX )
#include <sys/io.h>
#endif
#endif // R__MACOSX
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#ifdef HAVE_MIDAS
#include <midas.h>
#endif

#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <THtml.h>
#include <TArrayI.h>
#include <TTask.h>
#include <TTree.h>
#include <TFolder.h>
#include <TList.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TBrowser.h>
#include <TNetFolderServer.h>
#include <ROMETask.h>
#include <ROMEAnalyzer.h>
#include <Riostream.h>

ClassImp(ROMEAnalyzer)

ROMEAnalyzer *gROME;  // global ROMEAnalyzer Handle
void *gPassToROME;  // void ROMEAnalyzer Handle

ROMEAnalyzer::ROMEAnalyzer(TRint *app)
{
// Initialisations

   int i=0;
   fLastEventNumberIndex = 0;
   fApplication = app;
   fAnalysisMode = kAnalyzeOffline; 
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
   fPortNumber = 9090;
   fSocketOffline = false;
   fActiveDAQ = NULL;
}

ROMEAnalyzer::~ROMEAnalyzer() {
}


bool ROMEAnalyzer::Start(int argc, char **argv)
{
// Starts the ROME Analyzer

   ROMEString text;

#if defined( HAVE_MIDAS )
   cm_set_msg_print(0,0,NULL);
#endif

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
      TNetFolderServer *tnet = new TNetFolderServer();
      tnet->StartServer(gROME->GetApplication(),gROME->GetPortNumber());
      text.SetFormatted("Root server listening on port %d\n\n", gROME->GetPortNumber());
      gROME->Println(text.Data());
   }

   gROME->Println("Program steering");
   gROME->Println("----------------");
   gROME->Println("q : Terminates the program");
   gROME->Println("e : Ends the program");
   gROME->Println("s : Stops the program");
   gROME->Println("r : Restarts the program");
   gROME->Println("c : Continuous Analysis");
   gROME->Println("o : Step by step Analysis");
   gROME->Println("g : Run until event #");
   gROME->Println("i : Root interpreter");
   gROME->Println();

   TFolder *fHistoFolder = fMainFolder->AddFolder("histos","Histogram Folder");
   TList *taskList = fMainTask->GetListOfTasks();
   CreateHistoFolders(taskList,fHistoFolder);

   fMainTask->ExecuteTask("start");

   gROME->ss_getchar(1);
   if (fTerminate) return false;

   return true;
}

void ROMEAnalyzer::Print(char text)
{
   cout << text;
#if defined( HAVE_MIDAS )
   ROMEString strText = text;
//   cm_msg(MINFO, "ROMEAnalyzer::Print", strText.Data());
#endif
   return;
}

void ROMEAnalyzer::Print(const char* text)
{
   cout << text;
#if defined( HAVE_MIDAS )
//   cm_msg(MINFO, "ROMEAnalyzer::Print", text);
#endif
   return;
}

void ROMEAnalyzer::Println(const char* text)
{
   cout << text << endl;
#if defined( HAVE_MIDAS )
//   cm_msg(MINFO, "ROMEAnalyzer::Println", text);
#endif
   return;
}

void ROMEAnalyzer::Printfl(const char* text)
{
   cout << text << flush;
#if defined( HAVE_MIDAS )
//   cm_msg(MINFO, "ROMEAnalyzer::Printfl", text);
#endif
   return;
}

void ROMEAnalyzer::ParameterUsage()
{
   gROME->Println("  -i       Configuration file (default romeConfig.xml)");
   gROME->Println("  -b       Batch Mode (no Argument)");
   gROME->Println("  -ns      Splash Screen is not displayed (no Argument)");
   gROME->Println("  -m       Analysing Mode : (online/[offline])");
   gROME->Println("  -r       Runnumbers");
   gROME->Println("  -e       Eventnumbers");
   gROME->Println("  -docu    Generates a Root-Html-Documentation (no Argument)");
   gROME->UserParameterUsage();
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
      gROME->Print("Configuration file '");
      gROME->Print(configFile.Data());
      gROME->Println("' not found.");
      gROME->Printfl("Do you like the framework to generate a new configuration file ([y]/n) ? ");
      while (answer==0) {
         while (this->ss_kbhit()) {
            answer = this->ss_getchar(0);
         }
      }
      if (answer!='n') {
         if (!this->fConfiguration->WriteConfigurationFile(configFile.Data())) {
            gROME->Println("\nTerminate program.\n");
            return false;
         }
         gROME->Println("\nThe framework generated a new configuration file.");
         gROME->Println("Please edit this file and restart the program.\n");
      }
      else {
         gROME->Println("\nTerminate program.\n");
      }
      return false;
   }
   if (!this->GetConfiguration()->ReadConfigurationFile(configFile.Data())) {
      gROME->Println("\nTerminate program.\n");
      return false;
   }
   if (!this->fConfiguration->WriteConfigurationFile(configFile.Data())) {
      gROME->Println("\nTerminate program.\n");
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
      else if(!ReadUserParameter(argv[i], i<argc-1 ? argv[i+1] : "", i)){
         gROME->Print("Inputlineparameter '");
         gROME->Print(argv[i]);
         gROME->Println("' not available.");
         gROME->Println("Available inputlineparameter are : ");
         gROME->Println();
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

bool ROMEAnalyzer::toBool(int value) {
   return value!=0;
}

Bool_t ROMEAnalyzer::ss_kbhit()
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

long ROMEAnalyzer::ss_millitime()
{
#if defined( R__UNIX )
   {
      struct timeval tv;

      gettimeofday(&tv, NULL);

      return tv.tv_sec * 1000 + tv.tv_usec / 1000;
   }

#elif defined( R__VISUAL_CPLUSPLUS )

   return (int) GetTickCount();

#endif
}
int ROMEAnalyzer::stricmp(const char* c1,const char* c2)
{
#if defined( R__UNIX )
   return strcasecmp(c1,c2);
#elif defined( R__VISUAL_CPLUSPLUS )
   return _stricmp(c1,c2);
#else
   return 0;
#endif
}

bool ROMEAnalyzer::strtobool(const char* str) 
{
   char *cstop;
   if (!strcmp(str,"true"))
      return true;
   if (!strcmp(str,"false"))
      return false;
   return strtol(str,&cstop,10)!=0;
}
