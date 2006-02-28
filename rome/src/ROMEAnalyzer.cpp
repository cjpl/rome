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
#   include <Windows4Root.h>
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
#include <THtml.h>
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include <ROMENetFolderServer.h>
#include <ROMETask.h>
#include <ROMEAnalyzer.h>
#include <ROMEEventLoop.h>
#include <ROMEUtilities.h>
#include <Riostream.h>

ClassImp(ROMEAnalyzer)

ROMEAnalyzer *gROME;  // global ROMEAnalyzer Handle
void *gPassToROME;  // void ROMEAnalyzer Handle

ROMEAnalyzer::ROMEAnalyzer(TApplication *app)
{
// Initialisations
   fProgramMode = kStandAloneROME;
   fWindowClosed = false;
   fApplication = app;
   fCintInitialisation = "";
   fActiveDAQ = 0;
   fAnalysisMode = kAnalyzeOffline;
   fBatchMode = false;
   fQuitMode = false;
   fSplashScreen = true;
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
   fUserEventG = false;
   fUserEventGRunNumber = 0;
   fUserEventGEventNumber = 0;
   fUserEventI = false;
   fEventID = 'a';
   fMidasOnlineDataBase = 0;
   fTerminate = false;
   fFillEvent = false;
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
   fSocketToROME = NULL;
   fSocketToROMENetFolder = NULL;
   fSocketToROMEActive = true;
   fSocketToROMEHost = "localhost";
   fSocketToROMEPort = 9090;
   fHistoRead = false;
   fHistoRun = 0;
}

ROMEAnalyzer::~ROMEAnalyzer() {
   restoreOutput();
}


Bool_t ROMEAnalyzer::Start(int argc, char **argv)
{
// Starts the ROME Analyzer
   ROMEString text;

#if defined( HAVE_MIDAS )
   cm_set_msg_print(0,0,NULL);
#endif

   gROME = (ROMEAnalyzer*)gPassToROME;

   fMainTask->ExecuteTask("init");

   if (!ReadParameters(argc,argv)) return false;

   if (IsStandAloneARGUS()) {
      ConnectSocketToROME();
   }
   if (!ConnectNetFolders())
      return false;

   if (this->isBatchMode())
      redirectOutput();
   else
      gROME->ss_getchar(0);

   if (IsStandAloneROME() || IsROMEAndARGUS()) {
      consoleStartScreen();
      if (isSplashScreen()) startSplashScreen();

      if (gROME->isOnline() || gROME->isSocketOffline()) {
         ROMENetFolderServer *tnet = new ROMENetFolderServer();
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
   }
   if (IsStandAloneARGUS()) {
      ShowRunStat(false);
   }

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
   gROME->PrintLine("  -i       Configuration file");
   gROME->PrintLine("  -b       Batch Mode (no Argument)");
   gROME->PrintLine("  -q       Quit Mode (no Argument)");
   gROME->PrintLine("  -ns      Splash Screen is not displayed (no Argument)");
   gROME->PrintLine("  -m       Analysing Mode : (online/[offline])");
   gROME->PrintLine("  -r       Runnumbers");
   gROME->PrintLine("  -e       Eventnumbers");
   gROME->PrintLine("  -o       Start Analyzer in Step by Step Mode");
   gROME->PrintLine("  -docu    Generates a Root-Html-Documentation (no Argument)");
   gROME->UserParameterUsage();
   return;
}
Bool_t ROMEAnalyzer::ReadParameters(int argc, char *argv[])
{
   // Reads the Inputlineparameters
   int i;

   ROMEString configFile("");

   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-h")||!strcmp(argv[i],"-help")||!strcmp(argv[i],"--help")) {
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
         i = -1;
         while (i < 0 || i >= nFile) {
            gROME->PrintLine("Please select a configuration file.");
            for (i = 0; i < nFile; i++) {
               printString.SetFormatted("   [%d] %s", i, foundFiles.At(i).Data());
               gROME->PrintLine(printString.Data());
            }
            gROME->PrintLine("   [q] Quit");
            gROME->PrintFlush("File number: ");
            cin.getline(answerLine, sizeof(answerLine));
            answerString = answerLine;
            if (answerString == "q" || answerString == "Q") {
               return false;
            }
            i = answerString.ToInteger();
            if (!answerString.IsDigit() || i < 0 || i >= nFile) {
               printString.SetFormatted("File number %s is not found.", answerString.Data());
               gROME->PrintLine(printString.Data());
               gROME->PrintLine();
               i = -1;
            }
         }
         configFile = foundFiles[i];
      }
   }

   char answer = 0;
   bool overwrite = true;
   if ( !configFile.Length() || gSystem->AccessPathName(configFile.Data(), kFileExists)) {
      if (configFile.Length()) {
         gROME->PrintText("Configuration file '");
         gROME->PrintText(configFile.Data());
         gROME->PrintLine("' not found.");
      }
      else {
         configFile = "romeConfig.xml";
      }
      gROME->PrintLine();
      gROME->PrintLine("The framework can generate a new configuration file for you.");
      gROME->PrintLine("Available configuration types are :");
      gROME->PrintLine("   [R] ROME Framework");
      gROME->PrintLine("   [A] ARGUS Monitor");
      gROME->PrintLine("   [M] ROME Framework with ARGUS Monitor");
      gROME->PrintLine("   [N] Don't generate a configuration file");
      gROME->PrintFlush("Please select a configuration type[R/A/M/N]: ");
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
            gROME->SetStandAloneROME();
         if (answerString=="A")
            gROME->SetStandAloneARGUS();
         if (answerString=="M")
            gROME->SetROMEAndARGUS();
         gROME->PrintLine();
         printString.SetFormatted("Please specify file name (default='%s'): ", configFile.Data());
         gROME->PrintFlush(printString.Data());
         cin.getline(answerLine, sizeof(answerLine));
         if (strlen(answerLine))
            configFile = answerLine;
         if (!configFile.EndsWith(".xml") && !configFile.EndsWith(".XML"))
            configFile += ".xml";
         if (!gSystem->AccessPathName(configFile.Data(), kFileExists)) {
            printString.SetFormatted("overwrite '%s'? ", configFile.Data());
            gROME->PrintFlush(printString.Data());
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
         if (overwrite) {
            if (!this->fConfiguration->WriteConfigurationFile(configFile.Data())) {
               gROME->PrintLine("\nTerminate program.\n");
               return false;
            }
            gROME->PrintLine("\nThe framework generated a new configuration file.");
            gROME->PrintLine(configFile.Data());
            gROME->PrintLine("Please edit this file and restart the program.\n");
         }
      }
      else {
         gROME->PrintLine("\nTerminate program.\n");
      }
      return false;
   }
   printString.SetFormatted("reading configuration from %s", configFile.Data());
   gROME->PrintLine(printString.Data());
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
      else if (!strcmp(argv[i],"-q")) {
         fQuitMode = true;
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
      else if (!ReadUserParameter(argv[i], i<argc-1 ? argv[i+1] : "", i)) {
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
      status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

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

UInt_t ROMEAnalyzer::ss_millitime()
{
#if defined( R__UNIX )
   {
      struct timeval tv;

      gettimeofday(&tv, NULL);

      return tv.tv_sec * 1000 + tv.tv_usec / 1000;
   }

#elif defined( R__VISUAL_CPLUSPLUS )

   return (UInt_t) GetTickCount();

#endif
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
   ofstream *romeOutputFile;
   if (!fOldbuf)
      fOldbuf = cout.rdbuf();
   romeOutputFile = new ofstream("romeOutput.txt");
   cout.rdbuf(romeOutputFile->rdbuf());
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
};

ROMENetFolder *ROMEAnalyzer::GetNetFolder(const char *name)
{
   for (Int_t i = 0; i < fNumberOfNetFolders; i++) {
      if (!stricmp(fNetFolderName[i].Data(), name)) {
         if (!fNetFolderActive[i]) {
            Warning("GetNetFolder", "%s is not activated.", name);
            return 0;
         }
         return fNetFolder[i];
      }
   }
   Error("GetNetFolder", "Netfolder '%s' is not defined", name);
   return 0;
};

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
      PrintText("can not make socket connection for ");
      PrintText(fNetFolderName[i].Data());
      PrintLine(".");
      PrintLine("program sleeps for 5s and tries again.");
      gSystem->Sleep(5000);
      fNetFolderSocket[i] = new TSocket (fNetFolderHost[i].Data(), fNetFolderPort[i]);
   }
   fNetFolder[i] = new ROMENetFolder(fNetFolderRoot[i].Data(), fNetFolderName[i].Data(), fNetFolderSocket[i], fNetFolderReconnect[i]);
   ROMEString errMessage;
   if (!fNetFolder[i]->GetPointer()) {
      errMessage.SetFormatted("%s failed to connect to %s folder of %s.", fNetFolderName[i].Data(), fNetFolderRoot[i].Data(), fNetFolderHost[i].Data());
      Warning("ConnectNetFolder", errMessage);
   }

   return kTRUE;
}


Bool_t ROMEAnalyzer::ConnectSocketToROME()
{
   if (!IsSocketToROMEActive())
      return false;
   if (fSocketToROME!=NULL) {
      if (fSocketToROME->IsValid()) {
         return true;
      }
   }
   ROMEString port;
   port.SetFormatted("%d",fSocketToROMEPort);
   if (fSocketToROME==NULL)
      fSocketToROME = new TSocket (fSocketToROMEHost.Data(), fSocketToROMEPort);
   while (!fSocketToROME->IsValid()) {
      delete fSocketToROME;
      PrintText("can not make socket connection to the ROME analyzer on host '");
      PrintText(fSocketToROMEHost.Data());
      PrintText("' through port ");
      PrintText(port);
      PrintLine(".");
      PrintLine("program sleeps for 5s and tries again.");
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
