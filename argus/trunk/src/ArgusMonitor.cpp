/********************************************************************
  ArgusMonitor.cpp, R.Sawada

  $Log$
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
#endif
#if !defined( R__MACOSX )
#include <sys/io.h>
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

const char* ArgusMonitor::LineToProcess = NULL;
void writeLineToProcess(const char* str) {
   while (ArgusMonitor::LineToProcess) {
      gArgus->ss_sleep(10);
   }
   ArgusMonitor::LineToProcess = str;
}

#if defined(USE_TRINT)
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

bool ArgusMonitor::Start(int argc, char **argv)
{
   gArgus = (ArgusMonitor*)gPassToArgus;
   
   gArgus->ss_getchar(0);
   if (gROOT->IsBatch()) {
      printf("%s: cannot run in batch mode\n", argv[0]);
      return kFALSE;
   }
   
   if (!ReadParameters(argc,argv)) return false;
   
   int j;

#if defined(HAVE_MIDAS)
   // Connect to the experiment
   if (cm_connect_experiment(gArgus->GetOnlineHost(), gArgus->GetOnlineExperiment(),gArgus->GetProgramName(), NULL) != SUCCESS) {
      cout << "\nCannot connect to experiment" << endl;
      return false;
   }
#ifdef MIDAS_DEBUG
   cm_set_watchdog_params(TRUE, 0);
#endif
   // Connect to the online database
   if (cm_get_experiment_database(gArgus->GetMidasOnlineDataBasePointer(), NULL)!= CM_SUCCESS) {
      cout << "\nCannot connect to the online database" << endl;
      return false;
   }
   atexit((void (*)(void))cm_disconnect_experiment);
#endif
   
   if(!gArgus->StartMonitor())
      return false;
   
   return true;
}

void ArgusMonitor::ParameterUsage()
{
   cout<<"lpmonitor [option]"<<endl
       <<"-i configFile.xml"<<endl
       <<"-docu"<<endl;
   return;
}

bool ArgusMonitor::ReadParameters(int argc, char *argv[])
{
   // Reads the Inputlineparameters
   int i;
   const int workDirLen = 1000;
   ROMEString workDir(workDirLen);
   getcwd((char*)workDir.Data(),workDirLen);
   workDir.Append("/");
   this->SetDataBaseDir(workDir);

   ROMEString configFile("argusConfig.xml");

   char host_name[256] = "";
   char exp_name[32] = "";
#if defined(HAVE_MIDAS)
   cm_get_environment(host_name,sizeof(host_name),exp_name,sizeof(exp_name));
#endif
   
   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-help")||!strcmp(argv[i],"--help")) {
         ParameterUsage();
         return false;
      }
      if (!strcmp(argv[i],"-h")) {
         strcpy(host_name, argv[i+1]);
      }
      if (!strcmp(argv[i],"-e")) {
         strcpy(exp_name, argv[i+1]);
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
      if(strlen(host_name))
         fOnlineHost = host_name;
      if(strlen(exp_name))
         fOnlineExperiment = exp_name;
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

   return true;
}

Bool_t ArgusMonitor::ss_kbhit()
{
#if defined( R__VISUAL_CPLUSPLUS )
   return kbhit() != 0;
#endif
#if defined( R__UNIX )
   int n;
   ioctl(0, FIONREAD, &n);
   return (n > 0);
#endif
}

int ArgusMonitor::ss_sleep(int millisec)
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

int ArgusMonitor::ss_getchar(bool reset)
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

void ArgusMonitor::CheckLineToProcess()
{
   if (ArgusMonitor::LineToProcess){
      gArgus->GetApplication()->ProcessLine(ArgusMonitor::LineToProcess);
      ArgusMonitor::LineToProcess = NULL;
   }
}

