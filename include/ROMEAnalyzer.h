/********************************************************************
  ROMEAnalyzer.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEAnalyzer_H
#define ROMEAnalyzer_H

#include <time.h>
#include <TRint.h>
#include <TFile.h>
#include <TArrayI.h>
#include <TArrayL.h>
#include <TArrayL64.h>
#include <TList.h>
#include <TTask.h>
#include <TTree.h>
#include <TROOT.h>
#include <TFolder.h>
#include "ROME.h"
#include <ROMEConfig.h>
#include <ROMETree.h>
#include <ROMETask.h>
#include <ROMETreeInfo.h>
#include <ROMEDataBase.h>
#include <ROMEString.h>
#include <ROMEDAQSystem.h>
#include <ROMENetFolder.h>
#if defined ( HAVE_SQL )
#   include <ROMESQL.h>
#endif
#if defined ( HAVE_MIDAS )
#   include <midas.h>
#else
typedef Int_t HNDLE;
#endif

typedef struct {
   Double_t processedEvents;   //! Processed Events
   Double_t eventsPerSecond;   //! Events per Second
   Double_t writtenEvents;     //! Written Events
} Statistics;

class ROMEAnalyzer;
extern ROMEAnalyzer *gROME;  // global ROMEAnalyzer Handle

class ROMEAnalyzer : public TObject
{
friend class ArgusWindow;
private:
   // Analysis Mode
   enum {
      kAnalyzeOffline,
      kAnalyzeOnline
   };

public:
   // IO type
   enum {
      kNotBased,
      kRunNumberBased,
      kRunNumberAndFileNameBased,
      kFileNameBased
   };

   // Program Mode
   enum {
      kStandAloneROME,
      kStandAloneARGUS,
      kROMEAndARGUS
   };

protected:

   // Program Mode
   Int_t          fProgramMode;

   // Window Closed
   Bool_t         fWindowClosed;                 //! Window closed flag

   // Application
   TApplication  *fApplication;                  //! Application Handle

   // Cint
   ROMEString     fCintInitialisation;           //! Initialization String for the Cint

   // Active DAQ System
   ROMEDAQSystem *fActiveDAQ;                    //! Handle to the active DAQ system

   // Modes
   Int_t          fAnalysisMode;                 //! Analysis mode flag
   Bool_t         fBatchMode;                    //! Batch mode flag
   Bool_t         fQuitMode;                     //! Quit mode flag
   Bool_t         fSplashScreen;                 //! Splash screen flag

   Bool_t         fDontReadNextEvent;            //! Don't read the next event from file/buffer

   // Directories
   ROMEString     fInputDir;                     //! General Input Directory
   ROMEString     fOutputDir;                    //! General Output Directory
   ROMEString    *fDataBaseDir;                  //! Data Base File Directories
   ROMEString     fConfigDir;                    //! Configuration File Directory

   // Run Numbers
   Long64_t       fCurrentRunNumber;             //! Currently Analyzed Run Number
   TArrayL64      fRunNumber;                    //! Run Numbers to Analyze
   ROMEString     fRunNumberString;              //! Run Numbers in Input String Format

   // Event Numbers
   Long64_t       fCurrentEventNumber;           //! Currently Analyzed Event Number
   TArrayL64      fEventNumber;                  //! Event Numbers to Analyze
   ROMEString     fEventNumberString;            //! Event Numbers in Input String Format

   // Input File Names
   ROMEString     fCurrentInputFileName;         //! Name of Currently Analyzed Input File
   ROMEStrArray   fInputFileNames;               //! Input File Names to Analyze
   ROMEString     fInputFileNamesString;         //! Original Input File Names in Input String Format

   // IO type
   Int_t          fIOType;                       //! IO type

   // User Events
   Bool_t         fUserEvent;                    //! General User Event Flag
   Bool_t         fUserEventQ;                   //! Flag for User Event of Type Q
   Bool_t         fUserEventE;                   //! Flag for User Event of Type E
   Bool_t         fUserEventS;                   //! Flag for User Event of Type S
   Bool_t         fUserEventR;                   //! Flag for User Event of Type R
   Bool_t         fUserEventO;                   //! Flag for User Event of Type O
   Bool_t         fUserEventC;                   //! Flag for User Event of Type C
   Bool_t         fUserEventG;                   //! Flag for User Event of Type G
   Long64_t       fUserEventGRunNumber;          //! Run Number for User Event of Type G
   Long64_t       fUserEventGEventNumber;        //! Event Number for User Event of Type G
   Bool_t         fUserEventI;                   //! Flag for User Event of Type I

   // Event ID
   int            fEventID;                      //! Event ID of current Event

   // Flags
   Bool_t         fTerminate;                    //! Termination flag
   Bool_t         fFillEvent;                    //! Fill Event Flag

   // Object Handles
   TObjArray     *fTaskObjects;                  //! Handle to Task Objects
   TTask         *fMainTask;                     //! Handle to Main Task
   TFolder       *fMainFolder;                   //! Handle to Main Folder
   TFile         *fHistoFiles;                   //! Handle to Histogram Files

   // Trees
   TObjArray     *fTreeObjects;                  //! Handle to Tree Objects
   Bool_t         fTreeAccumulation;             //! Accumulation of runs in Trees

   // Histogram Folders
   TFolder       *fMainHistoFolder;              //! Handle to Main Histogram Folder (histos)
   TObjArray     *fHistoFolders;                 //! Handle to Histogram Folder Objects
   Bool_t         fHistoRead;                    //! Histogram read flag
   Int_t          fHistoRun;                     //! Read Histograms of this Run

   // Program name
   ROMEString     fProgramName;                  //! Name of this Program

   // Online
   ROMEString     fOnlineHost;                   //! Name of the Online Host
   ROMEString     fOnlineExperiment;             //! Name of the Online Experiment

   // Socket
   Int_t          fPortNumber;                   //! Port Number for TSocket
   Bool_t         fSocketOffline;                //! Socket available offline

   // SocketToROME
   TSocket       *fSocketToROME;                 //! Handle to socket connection to ROME
   ROMENetFolder *fSocketToROMENetFolder;        //! Handle to the ROMENetFolder of the socket connection to ROME
   Bool_t         fSocketToROMEActive;           //! Socket connection to ROME active flag
   ROMEString     fSocketToROMEHost;             //! Socket connection to ROME host
   Int_t          fSocketToROMEPort;             //! Socket connection to ROME port

   // Statistics
   Statistics     fTriggerStatistics;            //! Trigger Statistics
   Statistics     fScalerStatistics;             //! Scaler Statistics

   // Data base
   ROMEDataBase **fDataBaseHandle;               //! DataBase Handles
   ROMEString    *fDataBaseConnection;           //! DataBase connection strings
   Int_t          fNumberOfDataBases;            //! Number of DataBases available

   // Configuration
   ROMEConfig    *fConfiguration;                //! Configuration Handle

   // Run Stat
   Bool_t         fShowRunStat;                  //! Show Run Statistics 

   // Event Based Data Base
   Bool_t         fEventBasedDataBase;           //! Flag for Event Based Data Base

   // NetFolder
   Int_t          fNumberOfNetFolders;           //! Number of net folders
   ROMENetFolder **fNetFolder;                    //! netfolder handle
   Bool_t        *fNetFolderActive;              //! active flag
   Bool_t        *fNetFolderReconnect;           //! reconnect flag
   TSocket      **fNetFolderSocket;              //! socket connection handle
   Int_t         *fNetFolderPort;                //! port number
   ROMEString    *fNetFolderName;                //! name
   ROMEString    *fNetFolderHost;                //! server host name
   ROMEString    *fNetFolderRoot;                //! root directory name

   // Monitor
   ArgusWindow   *fWindow;                       //! Handle to Argus Window
   Bool_t         fEventFilled;                  //! Event filled flag

   // Midas
   HNDLE          fMidasOnlineDataBase;          //! Handle to the Midas Online Data Base

#ifndef __CINT__
   // stream
   streambuf     *fOldbuf;                       //! original buffer of stdout
#endif // __CINT__

public:
   ROMEAnalyzer() {};
   ROMEAnalyzer(TApplication *app);
   ~ROMEAnalyzer();

   // Program Mode
   Bool_t          IsStandAloneROME() { return fProgramMode==kStandAloneROME; };
   Bool_t          IsStandAloneARGUS() { return fProgramMode==kStandAloneARGUS; };
   Bool_t          IsROMEAndARGUS() { return fProgramMode==kROMEAndARGUS; };
   void            SetStandAloneROME() { fProgramMode=kStandAloneROME; };
   void            SetStandAloneARGUS() { fProgramMode=kStandAloneARGUS; };
   void            SetROMEAndARGUS() { fProgramMode=kROMEAndARGUS; };

   // Window Closed
   Bool_t          IsWindowClosed() { return fWindowClosed; }
   void            WindowClosed() { fWindowClosed = true; }

   // Output
   void            PrintText(char text);
   void            PrintText(const char *text="");
   void            PrintLine(const char *text="");
   void            PrintFlush(const char *text="");

   // Application Handle
   TApplication   *GetApplication() { return fApplication; };

   // Cint
   const char     *GetCintInitialisation() { return fCintInitialisation.Data(); };
   void            SetCintInitialisation(const char *str) { fCintInitialisation = str; };

   // Active DAQ System
   const char     *GetNameOfActiveDAQ() { if (fActiveDAQ==NULL) return "none"; return fActiveDAQ->GetName(); };
   ROMEDAQSystem  *GetActiveDAQ() {
                       if (fActiveDAQ!=NULL)
                          return fActiveDAQ;
                       this->PrintLine("\nYou have tried to access the active DAQ system but none is active .\nPlease select a DAQ system in the ROME configuration file under:\n<Modes>\n   <DAQSystem>\n\nShutting down the program.\n");
                       ((TRint*)fApplication)->Terminate(1);
                       return NULL;
                   }
   Bool_t          isActiveDAQSet() { return fActiveDAQ!=NULL; };
   void            SetActiveDAQ(ROMEDAQSystem *handle) { fActiveDAQ = handle; };

   // Data Base Handle
   const char     *GetDataBaseConnection(Int_t i) { return fDataBaseConnection[i].Data(); };
   void            SetDataBaseConnection(Int_t i,const char *connection) { fDataBaseConnection[i] = connection; };
   ROMEDataBase   *GetDataBase(Int_t i) {
                      if(i<fNumberOfDataBases && fDataBaseHandle[i]!=NULL)
                         return fDataBaseHandle[i];
                      this->PrintLine("\nYou have tried to access a database without initialisation.\nTo use the databases you have to add it to the list of databases in the\nROME configuration file under <DataBases>.\n\nShutting down the program.\n");
                      ((TRint*)fApplication)->Terminate(1);
                      return NULL;
                   }
   ROMEDataBase   *GetDataBase(const char *name) {
                      for (Int_t i=0;i<fNumberOfDataBases;i++)
                         if (!stricmp(fDataBaseHandle[i]->GetName(),name))
                            return fDataBaseHandle[i];
                      ROMEString str;
                      str.SetFormatted("\nYou have tried to access the %s database without initialisation.\nTo use the %s database you have to add it to the list of databases in the\nROME configuration file under <DataBases>.\n\nShutting down the program.\n",name,name);
                      this->PrintLine(str.Data());
                      ((TRint*)fApplication)->Terminate(1);
                      return NULL;
                   }
   Bool_t          isDataBaseActive(const char *name) {
                      for (Int_t i=0;i<fNumberOfDataBases;i++)
                         if (!stricmp(fDataBaseHandle[i]->GetName(),name))
                            return true;
                      return false;
                   };
   void            SetDataBase(Int_t i,ROMEDataBase *dataBase) { fDataBaseHandle[i] = dataBase; };
   Int_t           GetNumberOfDataBases() { return fNumberOfDataBases; };
   void            InitDataBases(Int_t number) {
                      fDataBaseHandle = new ROMEDataBase*[number];
                      fDataBaseConnection = new ROMEString[number];
                      fDataBaseDir = new ROMEString[number];
                      fNumberOfDataBases = number;
                   }

   // modes
   Bool_t          isSplashScreen() { return fSplashScreen; };
   Bool_t          isBatchMode() { return fBatchMode; };
   Bool_t          isQuitMode() { return fQuitMode; };

   void            SetSplashScreen(Bool_t flag=true)  { fSplashScreen = flag; };
   void            SetBatchMode(Bool_t flag=true) { fBatchMode = flag; };
   void            SetQuitMode(Bool_t flag=true) { fQuitMode = flag; };

   // Analysis Mode
   Bool_t          isOnline() { return fAnalysisMode==kAnalyzeOnline; };
   Bool_t          isOffline() { return fAnalysisMode==kAnalyzeOffline; };

   void            SetOnline()  { fAnalysisMode = kAnalyzeOnline; };
   void            SetOffline() { fAnalysisMode = kAnalyzeOffline; };

   // Directories
   const char     *GetInputDir() { return fInputDir.Data(); }
   const char     *GetOutputDir() { return fOutputDir.Data(); }
   const char     *GetDataBaseDir(Int_t i) { return fDataBaseDir[i].Data(); }
   const char     *GetConfigDir() { return fConfigDir.Data(); }

   void            SetInputDir(const char *dir) { fInputDir = dir; }
   void            SetInputDir(ROMEString &dir) { fInputDir = dir; }
   void            SetOutputDir(const char *dir) { fOutputDir = dir; }
   void            SetOutputDir(ROMEString &dir) { fOutputDir = dir; }
   void            SetDataBaseDir(Int_t i,const char *dir) { fDataBaseDir[i] = dir; }
   void            SetDataBaseDir(Int_t i,ROMEString &dir) { fDataBaseDir[i] = dir; }
   void            SetConfigDir(const char *dir) { fConfigDir = dir; }
   void            SetConfigDir(ROMEString &dir) { fConfigDir = dir; }

   // Fill Event Flag
   Bool_t          isFillEvent() { return fFillEvent; };
   void            SetFillEvent(Bool_t fillEvent = true) { fFillEvent = fillEvent; }

   // Termination Flag
   Bool_t          isTerminationFlag() { return fTerminate; };
   void            SetTerminationFlag() { fTerminate = true; };

   // Event Read Flag
   Bool_t          IsDontReadNextEvent() { return fDontReadNextEvent; };
   void            SetDontReadNextEvent(Bool_t flag = true) { fDontReadNextEvent = flag; };

   // Tree IO
   Bool_t          isTreeAccumulation()  { return fTreeAccumulation;  };
   void            SetTreeAccumulation(Bool_t flag = true) { fTreeAccumulation = flag;  };

   // Tasks
   void            AddTask(TTask *task) { fTaskObjects->AddLast(task); };
   ROMETask       *GetTaskObjectAt(Int_t index) { return (ROMETask*)fTaskObjects->At(index); };
   Int_t           GetTaskObjectEntries() { return fTaskObjects->GetEntries(); };

   // Trees
   void            AddTree(TTree *tree) { fTreeObjects->Add(new ROMETree(tree)); };
   ROMETree       *GetTreeObjectAt(Int_t index) { return (ROMETree*)fTreeObjects->At(index); };
   Int_t           GetTreeObjectEntries() { return fTreeObjects->GetEntries(); };

   // Histogram Folders
   TFolder        *GetMainHistoFolder() { return fMainHistoFolder; };
   TFolder        *GetHistoFolderAt(int index) { return (TFolder*)fHistoFolders->At(index); };
   Bool_t          IsHistosRead() { return fHistoRead; };
   Int_t           GetHistosRun() { return fHistoRun; };

   void            SetHistosRead(Bool_t flag) { fHistoRead = flag; };
   void            SetHistosRun(Int_t runNumber) { fHistoRun = runNumber; };

   // Run Number
   void            GetRunNumberStringAt(ROMEString &buffer,Int_t i) {
                      if (i>=fRunNumber.GetSize())
                         buffer.SetFormatted("%0*d",5,0);
                      else
#if defined( R__VISUAL_CPLUSPLUS )
                         buffer.SetFormatted("%0*I64d",5,fRunNumber.At(i));
#else
                         buffer.SetFormatted("%0*lld",5,fRunNumber.At(i));
#endif
                   }
   Long64_t        GetRunNumberAt(Int_t i) { if (i>=fRunNumber.GetSize()) return 0; return fRunNumber.At(i); }
   void            GetCurrentRunNumberString(ROMEString &buffer) {
#if defined( R__VISUAL_CPLUSPLUS )
                      buffer.SetFormatted("%0*I64d",5,fCurrentRunNumber);
#else
                      buffer.SetFormatted("%0*lld",5,fCurrentRunNumber);
#endif
                   }
   Long64_t        GetCurrentRunNumber() { return fCurrentRunNumber; }
   Int_t           GetNumberOfRunNumbers() { return fRunNumber.GetSize(); }
   const char     *GetRunNumberStringOriginal() { return fRunNumberString.Data(); }

   void            SetCurrentRunNumber(Long64_t runNumber) { fCurrentRunNumber = runNumber; }
   void            SetRunNumbers(ROMEString &numbers) {
                   fRunNumberString = numbers;
                   DecodeNumbers(numbers,fRunNumber); }
   void            SetRunNumbers(const char *numbers) {
                   fRunNumberString = numbers;
                   DecodeNumbers(fRunNumberString,fRunNumber); }
   Long64_t        GetNextRunNumber(const Long64_t runNumber);

   // Event Number
   Long64_t        GetCurrentEventNumber() { return fCurrentEventNumber; }
   const char     *GetEventNumberStringOriginal() { return fEventNumberString.Data(); }

   void            SetCurrentEventNumber(Long64_t eventNumber) { fCurrentEventNumber = eventNumber; }
   void            SetEventNumbers(ROMEString &numbers) {
                   fEventNumberString = numbers;
                   DecodeNumbers(numbers,fEventNumber); }
   void            SetEventNumbers(const char *numbers) {
                      fEventNumberString = numbers;
                      DecodeNumbers(fEventNumberString,fEventNumber);
                   }


   // Input File Names
   ROMEString      GetInputFileNameAt(Int_t i) { return fInputFileNames.At(i); }
   ROMEString      GetCurrentInputFileName() { return fCurrentInputFileName; }
   Int_t           GetNumberOfInputFileNames() { return fInputFileNames.GetEntriesFast(); }
   const char     *GetInputFileNamesStringOriginal() { return fInputFileNamesString.Data(); }

   void            SetCurrentInputFileName(ROMEString &inputFileName) { fCurrentInputFileName = inputFileName; }
   void            SetCurrentInputFileName(const char *inputFileName) { fCurrentInputFileName = inputFileName; }
   void            SetInputFileNames(ROMEString &names) {
                      fInputFileNamesString = names;
                      DecodeInputFileNames(names,fInputFileNames);
                   }
   void            SetInputFileNames(const char *numbers) {
                      fInputFileNamesString = numbers;
                      DecodeInputFileNames(fInputFileNamesString,fInputFileNames);
                   }

   // IO type
   Bool_t          IsRunNumberBasedIO() { return (fIOType==kRunNumberBased); };
   Bool_t          IsRunNumberAndFileNameBasedIO() { return (fIOType==kRunNumberAndFileNameBased); };
   Bool_t          IsFileNameBasedIO() { return (fIOType==kFileNameBased); };

   void            SetIOType(Int_t type) { fIOType = type; };

   // User Events
   void            SetUserEventQ() { fUserEvent = true; fUserEventQ = true; };
   void            SetUserEventE() { fUserEvent = true; fUserEventE = true; };
   void            SetUserEventS() { fUserEvent = true; fUserEventS = true; };
   void            SetUserEventR() { fUserEvent = true; fUserEventR = true; };
   void            SetUserEventO() { fUserEvent = true; fUserEventO = true; };
   void            SetUserEventC() { fUserEvent = true; fUserEventC = true; };
   void            SetUserEventG(Long64_t runNumber,Long64_t eventNumber) {
                      fUserEvent = true;
                      fUserEventG = true;
                      fUserEventGRunNumber = runNumber;
                      fUserEventGEventNumber = eventNumber;
                   }
   void            SetUserEventI() { fUserEvent = true; fUserEventI = true; };

   Bool_t          HasUserEvent() { return fUserEvent; };
   void            DeleteUserEvent() {
                      fUserEvent = false;
                      fUserEventQ = false;
                      fUserEventE = false;
                      fUserEventS = false;
                      fUserEventR = false;
                      fUserEventO = false;
                      fUserEventC = false;
                      fUserEventG = false;
                      fUserEventI = false;
                   }
   Bool_t          IsUserEventQ() { return fUserEventQ; };
   Bool_t          IsUserEventE() { return fUserEventE; };
   Bool_t          IsUserEventS() { return fUserEventS; };
   Bool_t          IsUserEventR() { return fUserEventR; };
   Bool_t          IsUserEventO() { return fUserEventO; };
   Bool_t          IsUserEventC() { return fUserEventC; };
   Bool_t          IsUserEventG() { return fUserEventG; };
   Long64_t        GetUserEventGRunNumber() { return fUserEventGRunNumber; };
   Long64_t        GetUserEventGEventNumber() { return fUserEventGEventNumber; };
   Bool_t          IsUserEventI() { return fUserEventI; };

   // Event ID
   Int_t           GetEventID() { return fEventID; }
   void            SetEventID(Int_t eventID) { fEventID = eventID; }

  // Processed Events
   Double_t        GetProcessedEvents() { return fTriggerStatistics.processedEvents; }

   // main objects
   TFolder        *GetMainFolder() { return fMainFolder; }
   TTask          *GetMainTask() { return fMainTask; }
   TFile          *GetHistoFileHandle() { return fHistoFiles; };

   void            SetHistoFileHandle(TFile *file) { fHistoFiles = file; };

   const char     *GetProgramName() { return fProgramName.Data(); };

   // Online
   const char     *GetOnlineHost() { return fOnlineHost.Data(); };
   const char     *GetOnlineExperiment() { return fOnlineExperiment.Data(); };

   void            SetOnlineHost(const char *host) { fOnlineHost = host; };
   void            SetOnlineExperiment(const char *experiment) { fOnlineExperiment = experiment; };

   // Socket
   Int_t           GetPortNumber() { return fPortNumber; };
   Bool_t          isSocketOffline() { return fSocketOffline; };

   void            SetPortNumber(Int_t portNumber) { fPortNumber = portNumber; };
   void            SetPortNumber(const char *portNumber) { char *cstop; fPortNumber = strtol(portNumber,&cstop,10); };
   void            SetSocketOffline(Bool_t flag=true) { fSocketOffline = flag; };

   // SocketToROME
   Bool_t          IsSocketToROMEActive() { return fSocketToROMEActive; };
   const char     *GetSocketToROMEHost() { return fSocketToROMEHost.Data(); };
   Int_t           GetSocketToROMEPort() { return fSocketToROMEPort; };
   ROMENetFolder  *GetSocketToROMENetFolder() { ConnectSocketToROME(); return fSocketToROMENetFolder; };

   void            SetSocketToROMEActive(Bool_t flag) { fSocketToROMEActive = flag; };
   void            SetSocketToROMEHost(const char *host) { fSocketToROMEHost = host; };
   void            SetSocketToROMEPort(Int_t portNumber) { fSocketToROMEPort = portNumber; };
   void            SetSocketToROMEPort(const char *portNumber) { char *cstop; fSocketToROMEPort = strtol(portNumber,&cstop,10); };
   
   // Midas ODB
   HNDLE        GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
   HNDLE*       GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; }

   // Configuration
   ROMEConfig     *GetConfiguration() { return fConfiguration; };

   // Statistics
   Statistics     *GetTriggerStatistics() { return &fTriggerStatistics; };
   Statistics     *GetScalerStatistics() { return &fScalerStatistics; };

   // Start Method
   virtual Bool_t  Start(int argc=0, char **argv=NULL);

   // Run/Event Number Handling
   void            DecodeNumbers(ROMEString &str,TArrayL64 &arr);
   void            DecodeInputFileNames(ROMEString &str,ROMEStrArray &arr);
   Int_t           CheckEventNumber(Long64_t eventNumber);
   Int_t           CheckRunNumber(Long64_t runNumber);
   Int_t           CheckNumber(Long64_t eventNumber,TArrayL64 &numbers);


   // Run Stat
   void            ShowRunStat(Bool_t flag=true) { fShowRunStat = flag; };
   Bool_t          IsShowRunStat() { return fShowRunStat; };

   // Event Based Data Base
   void            SetEventBasedDataBase(Bool_t flag=true) { fEventBasedDataBase = flag; };
   Bool_t          IsEventBasedDataBase() { return fEventBasedDataBase; };

   // NetFolder
   Bool_t          IsNetFolderActive(const char *name);
   ROMENetFolder  *GetNetFolder(const char *name);
   char           *GetNetFolderName(Int_t i) { return (char*)fNetFolderName[i].Data(); }
   char           *GetNetFolderHost(Int_t i) { return (char*)fNetFolderHost[i].Data(); }
   Int_t           GetNetFolderPort(Int_t i) { return fNetFolderPort[i]; }
   char           *GetNetFolderRoot(Int_t i) { return (char*)fNetFolderRoot[i].Data(); }
   Bool_t          GetNetFolderActive(Int_t i) { return fNetFolderActive[i]; }
   Bool_t          GetNetFolderReconnect(Int_t i) { return fNetFolderReconnect[i]; }
   void            SetNetFolderName(Int_t i,const char *name) { fNetFolderName[i] = name; }
   void            SetNetFolderName(Int_t i,ROMEString &name) { fNetFolderName[i] = name; }
   void            SetNetFolderHost(Int_t i,const char *host) { fNetFolderHost[i] = host; }
   void            SetNetFolderHost(Int_t i,ROMEString &host) { fNetFolderHost[i] = host; }
   void            SetNetFolderPort(Int_t i,Int_t port) { fNetFolderPort[i] = port; }
   void            SetNetFolderPort(Int_t i,const char *port) { char *cstop; fNetFolderPort[i] = strtol(port,&cstop,10); }
   void            SetNetFolderPort(Int_t i,ROMEString &port) { SetNetFolderPort(i,(char*)port.Data()); }
   void            SetNetFolderRoot(Int_t i,const char *root) { fNetFolderRoot[i] = root; }
   void            SetNetFolderRoot(Int_t i,ROMEString &root) { fNetFolderRoot[i] = root; }
   void            SetNetFolderActive(Int_t i,Bool_t active) { fNetFolderActive[i] = active; }
   void            SetNetFolderReconnect(Int_t i,Bool_t reconnect) { fNetFolderReconnect[i] = reconnect; }
   Int_t           GetNumberOfNetFolders() { return fNumberOfNetFolders; }

   // NetFolder connection
   Bool_t          ConnectNetFolder(const char *name);
   Bool_t          ConnectNetFolder(Int_t i);
   Bool_t          DisconnectNetFolder(const char *name);
   Bool_t          DisconnectNetFolder(Int_t i);
   Bool_t          ConnectNetFolders();
   Bool_t          DisconnectNetFolders();
   void            InitNetFolders(Int_t number);

   // Folder dump and load
   virtual Bool_t  DumpFolders(const char* filename, Bool_t only_database) = 0;
   virtual Bool_t  LoadFolders(const char* filename, Bool_t only_database) = 0;

   // Monitor
   ArgusWindow    *GetWindow() { return fWindow; };
   Bool_t          StartWindow();
   Int_t           GetUpdateFrequency();
   void            SetUpdateFrequency(Int_t frequency);
   Bool_t          IsWindowBusy();
   Bool_t          IsEventFilled() { return fEventFilled; };
   void            SetEventFilled(Bool_t flag) { fEventFilled = flag; };

   virtual Bool_t  ReadSingleDataBaseFolders() = 0;
   virtual Bool_t  ReadArrayDataBaseFolders() = 0;

   Int_t           ss_getchar(UInt_t reset);
   UInt_t          ss_kbhit();
   UInt_t          ss_millitime();
   Bool_t          strtobool(const char *str);

   Int_t           stricmp(const char*,const char*);
   Bool_t          toBool(Int_t value);
   void            redirectOutput();
   void            restoreOutput();

protected:
   Bool_t          CreateHistoFolders(TList *,TFolder *);

   virtual Bool_t  ReadParameters(int argc, char *argv[]);
   virtual Bool_t  ReadUserParameter(const char *opt, const char *value, Int_t &i) { return false; }
   virtual void    ParameterUsage();
   virtual void    UserParameterUsage(){}

   virtual void    startSplashScreen() = 0;
   virtual void    consoleStartScreen() = 0;
   Bool_t          ConnectSocketToROME();
   virtual Bool_t  ConnectSocketToROMENetFolder() = 0;

   ClassDef(ROMEAnalyzer,0)
};

#endif   // ROMEAnalyzer_H
