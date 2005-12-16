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
#   include <windows.h>
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
#   include <midas.h>
#endif

#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
//#include <THtml.h>
#include <TArrayI.h>
#include <TTask.h>
#include <TTree.h>
#include <TFolder.h>
#include <TList.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TBrowser.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include <TNetFolderServer.h>
#include <ROMETask.h>
#include <ROMEAnalyzer.h>
#include <ROMEEventLoop.h>
#include <Riostream.h>

ClassImp(ROMEAnalyzer)

ROMEAnalyzer *gROME;  // global ROMEAnalyzer Handle
void *gPassToROME;  // void ROMEAnalyzer Handle

ROMEAnalyzer::ROMEAnalyzer(TApplication *app)
{
// Initialisations

   fIOType = kNotBased;
   fRunNumber.Reset();
   fLastEventNumberIndex = 0;
   fApplication = app;
   fAnalysisMode = kAnalyzeOffline;
   fBatchMode = false;
   fSplashScreen = true;
   fDontReadNextEvent = false;
   fCurrentRunNumber = 0;
   fCurrentEventNumber = 0;
   fEventID = 'a';
   fTerminate = false;
   fTreeAccumulation = false;
   fPortNumber = 9090;
   fSocketOffline = false;
   fTreeObjects = new TObjArray(0);
   fHistoFolders = new TObjArray(0);
   fOnlineHost = "";
   fPortNumber = 9090;
   fSocketOffline = false;
   fActiveDAQ = NULL;
   fUserEvent = false;
   fUserEventQ = false;
   fUserEventE = false;
   fUserEventS = false;
   fUserEventR = false;
   fUserEventO = false;
   fUserEventC = false;
   fUserEventG = false;
   fUserEventI = false;
   fShowRunStat = true;
   fInputDir = "./";
   fOutputDir = "./";
   fEventBasedDataBase = false;
   fOldbuf = 0;
}

ROMEAnalyzer::~ROMEAnalyzer() {
   restoreOutput();
}


bool ROMEAnalyzer::Start(int argc, char **argv)
{
// Starts the ROME Analyzer
   ROMEString text;

#if defined( HAVE_MIDAS )
   cm_set_msg_print(0,0,NULL);
#endif

   gROME = (ROMEAnalyzer*)gPassToROME;

   fMainTask->ExecuteTask("init");

   if (!ReadParameters(argc,argv)) return false;

   if (this->isBatchMode())
      redirectOutput();
   else
      gROME->ss_getchar(0);

   consoleStartScreen();

   if (isSplashScreen()) startSplashScreen();

   if (gROME->isOnline() || gROME->isSocketOffline()) {
      TNetFolderServer *tnet = new TNetFolderServer();
      tnet->StartServer(gROME->GetApplication(),gROME->GetPortNumber());
      text.SetFormatted("Root server listening on port %d\n\n", gROME->GetPortNumber());
      gROME->PrintLine(text.Data());
   }

   gROME->PrintLine("Program steering");
   gROME->PrintLine("----------------");
   gROME->PrintLine("q : Terminates the program");
   gROME->PrintLine("e : Ends the program");
   gROME->PrintLine("s : Stops the program");
   gROME->PrintLine("r : Restarts the program");
   gROME->PrintLine("c : Continuous Analysis");
   gROME->PrintLine("o : Step by step Analysis");
   gROME->PrintLine("g : Run until event #");
   gROME->PrintLine("i : Root interpreter");
   gROME->PrintLine();

   fMainTask->ExecuteTask("start");

   if (!this->isBatchMode())
      gROME->ss_getchar(1);

   if (fTerminate) return false;

   return true;
}

void ROMEAnalyzer::PrintText(char text)
{
   cout << text;
#if defined( HAVE_MIDAS )
   ROMEString strText = text;
//   cm_msg(MINFO, "ROMEAnalyzer::Print", strText.Data());
#endif
   return;
}

void ROMEAnalyzer::PrintText(const char* text)
{
   cout << text;
#if defined( HAVE_MIDAS )
//   cm_msg(MINFO, "ROMEAnalyzer::Print", text);
#endif
   return;
}

void ROMEAnalyzer::PrintLine(const char* text)
{
   cout << text << endl;
#if defined( HAVE_MIDAS )
//   cm_msg(MINFO, "ROMEAnalyzer::PrintLine", text);
#endif
   return;
}

void ROMEAnalyzer::PrintFlush(const char* text)
{
   cout << text << flush;
#if defined( HAVE_MIDAS )
//   cm_msg(MINFO, "ROMEAnalyzer::PrintFlush", text);
#endif
   return;
}

void ROMEAnalyzer::ParameterUsage()
{
   gROME->PrintLine("  -i       Configuration file (default romeConfig.xml)");
   gROME->PrintLine("  -b       Batch Mode (no Argument)");
   gROME->PrintLine("  -ns      Splash Screen is not displayed (no Argument)");
   gROME->PrintLine("  -m       Analysing Mode : (online/[offline])");
   gROME->PrintLine("  -r       Runnumbers");
   gROME->PrintLine("  -e       Eventnumbers");
   gROME->PrintLine("  -o       Start Analyzer in Step by Step Mode");
   gROME->PrintLine("  -docu    Generates a Root-Html-Documentation (no Argument)");
   gROME->UserParameterUsage();
   return;
}
bool ROMEAnalyzer::ReadParameters(int argc, char *argv[])
{
   // Reads the Inputlineparameters
   int i;

   ROMEString configFile("romeConfig.xml");

   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-h")||!strcmp(argv[i],"-help")) {
         ParameterUsage();
         return false;
      }
/*      if (!strcmp(argv[i],"-docu")) {
         THtml html;
         html.MakeAll(true);
         return false;
      }*/
      if (!strcmp(argv[i],"-i")&&i<argc-1) {
         configFile = argv[i+1];
         i++;
      }
   }
   char answer = 0;
   struct stat buf;
   if( stat( configFile.Data(), &buf )) {
      gROME->PrintText("Configuration file '");
      gROME->PrintText(configFile.Data());
      gROME->PrintLine("' not found.");
      gROME->PrintFlush("Do you like the framework to generate a new configuration file ([y]/n) ? ");
      gROME->ss_getchar(0);
      while (answer==0) {
         while (this->ss_kbhit()) {
            answer = this->ss_getchar(0);
         }
      }
      gROME->ss_getchar(1);
      if (answer!='n') {
         if (!this->fConfiguration->WriteConfigurationFile(configFile.Data())) {
            gROME->PrintLine("\nTerminate program.\n");
            return false;
         }
         gROME->PrintLine("\nThe framework generated a new configuration file.");
         gROME->PrintLine("Please edit this file and restart the program.\n");
      }
      else {
         gROME->PrintLine("\nTerminate program.\n");
      }
      return false;
   }
   if (!this->GetConfiguration()->ReadConfigurationFile(configFile.Data())) {
      gROME->PrintLine("\nTerminate program.\n");
      return false;
   }
   if (!this->fConfiguration->WriteConfigurationFile(configFile.Data())) {
      gROME->PrintLine("\nTerminate program.\n");
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
      else if (!strcmp(argv[i],"-o")) {
         ((ROMEEventLoop*)fMainTask)->SetContinuousMode(false);
         i++;
      }
      else if (!strcmp(argv[i],"-i")) {
         i++;
      }
      else if(!ReadUserParameter(argv[i], i<argc-1 ? argv[i+1] : "", i)){
         gROME->PrintText("Input line parameter '");
         gROME->PrintText(argv[i]);
         gROME->PrintLine("' not available.");
         gROME->PrintLine("Available input line parameters are : ");
         gROME->PrintLine();
         ParameterUsage();
         return false;
      }
   }

   return true;
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

void ROMEAnalyzer::DecodeRunNumbers(ROMEString& str,TArrayI& arr)
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
   arr.Set(10);
   int arraySize = arr.GetSize();
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
         return;
      }
   }
   arr.Set(na);
}

void ROMEAnalyzer::DecodeInputFileNames(ROMEString& str,ROMEStrArray& arr)
{
   int ind,num=0;
   TString temp;
   ROMEString string = str;
   while ((ind=string.Index(","))!=-1) {
      temp = string(0,ind);
      arr.AddAtAndExpand(temp,num);
      num++;
      string = string(ind+1,string.Length()-ind-1);
   }
   arr.AddAtAndExpand(str,num);
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

void ROMEAnalyzer::redirectOutput() {
   ofstream *romeOutputFile;
   if (!fOldbuf)
      fOldbuf = cout.rdbuf();
   romeOutputFile = new ofstream("romeOutput.txt");
   cout.rdbuf(romeOutputFile->rdbuf());
}

void ROMEAnalyzer::restoreOutput() {
   if(fOldbuf)
      cout.rdbuf(fOldbuf);
}
