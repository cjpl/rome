/********************************************************************
  ROMEAnalyzer.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEAnalyzer_H
#define ROMEAnalyzer_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TTask.h>
#include <TFolder.h>
#include <TFile.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROME.h"
#include "ROMEiostream.h"
#include "ROMEString.h"
#include "ROMEStrArray.h"
#include "ROMEDAQSystem.h"
#include "ROMETree.h"
#include "ROMERint.h"
#include "ROMEDataBase.h"
#include "ROMETask.h"
#include "ROMENetFolder.h"
#include "ROMEConfig.h"
#include "ROMEEventLoop.h"
#include "TArrayL64.h"
#if defined ( HAVE_MIDAS ) && !defined ( __MAKECINT__ )
#   include "midas.h"
#else
typedef Int_t HNDLE;
#endif

class TMutex;
class TObjArray;
class TSocket;
class TTree;
class ROMEDAQSystem;
class ROMEAnalyzer;
class ArgusWindow;
class ROMENetFolderServer;

typedef struct {
   Double_t processedEvents;   //! Processed Events
   Double_t eventsPerSecond;   //! Events per Second
   Double_t writtenEvents;     //! Written Events
} Statistics;

extern ROMEAnalyzer *gROME;  // global ROMEAnalyzer Handle

const Int_t kMaxSocketClients = 100;

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
      kROMEAndARGUS,
      kROMEMonitor
   };

   // Folder Storage Status
   enum {
      kStorageFree,
      kStorageWriting,
      kStorageReading
   };

protected:

   // Program Mode
   Int_t          fProgramMode;

   // Window Closed
   Bool_t         fWindowClosed;                 //! Window closed flag

   // Application
   ROMERint      *fApplication;                  //! Application Handle

   // Cint
   ROMEString     fCintInitialisation;           //! Initialization String for the Cint

   // Active DAQ System
   ROMEDAQSystem *fActiveDAQ;                    //! Handle to the active DAQ system

   // Modes
   Int_t          fAnalysisMode;                 //! Analysis mode flag
   Bool_t         fBatchMode;                    //! Batch mode flag
   Bool_t         fDaemonMode;                   //! Daemon mode flag
   Bool_t         fQuitMode;                     //! Quit mode flag
   Bool_t         fSplashScreen;                 //! Splash screen flag
   Bool_t         fGraphicalConfigEdit;          //! Configuration edit flag
   Bool_t         fPreserveConfig;              //! Flag if analyzer overwrites existing config file
   Bool_t         fNoGraphics;                   //! No graphics flag

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
   Bool_t         fUserEventJ;                   //! Flag for User Event of Type J
   Long64_t       fUserEventJEventNumber;        //! Event Number for User Event of Type J
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
   ROMENetFolderServer *fNetFolderServer;        //! Handle to NetFolder server

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
   ROMEString     fOnlineAnalyzerName;           //! The name of the analyzer in the midas environment
   ROMEString     fOnlineMemoryBuffer;           //! The name of the midas memory buffer

   // Socket Server
   Bool_t         fSocketServerActive;           //! Socket active
   Int_t          fSocketServerPortNumber;       //! Port Number for TSocket
   TMutex        *fSocketServerMutex;            //! Mutex to secure socket server
   TMutex        *fObjectStorageMutex;           //! Mutex to secure object storage
   TMutex        *fRunEventNumberMutex;          //! Mutex to secure run and event number access
   TMutex        *fUpdateObjectStorageMutex;     //! Mutex to secure object storage update flag
   Bool_t         fObjectStorageUpdated;         //! Object storage update flag

   // Socket Client
   TSocket       *fSocketClient;                 //! Handle to socket connection to ROME
   ROMENetFolder *fSocketClientNetFolder;        //! Handle to the ROMENetFolder of the socket connection to ROME
   ROMEString     fSocketClientHost;             //! Socket connection to ROME host
   Int_t          fSocketClientPort;             //! Socket connection to ROME port

   // Statistics
   Statistics     fTriggerStatistics;            //! Trigger Statistics
   Statistics     fScalerStatistics;             //! Scaler Statistics

   // Data base
   ROMEDataBase **fDataBaseHandle;               //! DataBase Handles
   ROMEString    *fDataBaseName;                 //! DataBase name
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
   Int_t          fWindowUpdateFrequency;        //! The update frequency of the Argus window

   // Midas
   HNDLE          fMidasOnlineDataBase;          //! Handle to the Midas Online Data Base

#ifndef __CINT__
   // stream
   streambuf     *fOldbuf;                       //! original buffer of stdout
   ofstream      *fRomeOutputFile;               //! Redirected output currently not used
#endif // __CINT__

public:
   ROMEAnalyzer() {}
   ROMEAnalyzer(ROMERint *app,Bool_t batch,Bool_t daemon,Bool_t nographics);
   virtual ~ROMEAnalyzer();

   // Program Mode
   Bool_t          IsStandAloneROME() { return fProgramMode==kStandAloneROME; }
   Bool_t          IsStandAloneARGUS() { return fProgramMode==kStandAloneARGUS; }
   Bool_t          IsROMEAndARGUS() { return fProgramMode==kROMEAndARGUS; }
   Bool_t          IsROMEMonitor() { return fProgramMode==kROMEMonitor; }
   void            SetStandAloneROME() { fProgramMode=kStandAloneROME; }
   void            SetStandAloneARGUS() { fProgramMode=kStandAloneARGUS; }
   void            SetROMEAndARGUS() { fProgramMode=kROMEAndARGUS; }
   void            SetROMEMonitor() { fProgramMode=kROMEMonitor; }

   // Window Closed
   Bool_t          IsWindowClosed() { return fWindowClosed; }
   void            WindowClosed() { fWindowClosed = true; }

   // Application Handle
   ROMERint       *GetApplication() { return fApplication; }

   // Cint
   const char     *GetCintInitialisation() { return fCintInitialisation.Data(); }
   void            SetCintInitialisation(const char *str) { fCintInitialisation = str; }

   // Active DAQ System
   const char     *GetNameOfActiveDAQ() { if (fActiveDAQ==NULL) return "none"; return fActiveDAQ->GetName(); }
   ROMEDAQSystem  *GetActiveDAQ();
   Bool_t          isActiveDAQSet() { return fActiveDAQ!=NULL; }
   void            SetActiveDAQ(ROMEDAQSystem *handle) { fActiveDAQ = handle; }

   // Data Base Handle
   const char     *GetDataBaseName(Int_t i) { return fDataBaseName[i].Data(); }
   void            SetDataBaseName(Int_t i,const char *name) { fDataBaseName[i] = name; }
   const char     *GetDataBaseConnection(Int_t i) { return fDataBaseConnection[i].Data(); }
   void            SetDataBaseConnection(Int_t i,const char *connection) { fDataBaseConnection[i] = connection; }
   ROMEDataBase   *GetDataBase(Int_t i);
   ROMEDataBase   *GetDataBase(const char *name);
   Bool_t          isDataBaseActive(const char *name);
   void            SetDataBase(Int_t i,ROMEDataBase *dataBase) { fDataBaseHandle[i] = dataBase; }
   Int_t           GetNumberOfDataBases() { return fNumberOfDataBases; }
   void            InitDataBases(Int_t number);

   // NetFolderServer
   ROMENetFolderServer *GetNetFolderServer() { return fNetFolderServer; }

   // modes
   Bool_t          isSplashScreen() { return fSplashScreen; }
   Bool_t          isBatchMode() { return fBatchMode; }
   Bool_t          isDaemonMode() { return fDaemonMode; }
   Bool_t          isQuitMode() { return fQuitMode; }
   Bool_t          isGraphicalConfigEdit() { return fGraphicalConfigEdit; }
   Bool_t          isPreserveConfig() { return fPreserveConfig; }
   Bool_t          isNoGraphics() { return fNoGraphics; }

   void            SetSplashScreen(Bool_t flag=true)  { fSplashScreen = flag; }
   void            SetBatchMode(Bool_t flag=true) { fBatchMode = flag; }
   void            SetDaemonMode(Bool_t flag=true) { fDaemonMode = flag; }
   void            SetQuitMode(Bool_t flag=true) { fQuitMode = flag; }
   void            SetGraphicalConfigEdit(Bool_t flag=true) { fGraphicalConfigEdit = flag; }
   void            SetPreserveConfig(Bool_t flag=true) { fPreserveConfig = flag; }
   void            SetNoGraphics(Bool_t flag=true) { fNoGraphics = flag; }

   // Analysis Mode
   Bool_t          isOnline() { return fAnalysisMode==kAnalyzeOnline; }
   Bool_t          isOffline() { return fAnalysisMode==kAnalyzeOffline; }

   void            SetOnline()  { fAnalysisMode = kAnalyzeOnline; }
   void            SetOffline() { fAnalysisMode = kAnalyzeOffline; }

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
   Bool_t          isFillEvent() { return fFillEvent; }
   void            SetFillEvent(Bool_t fillEvent = true) { fFillEvent = fillEvent; }

   // Termination Flag
   Bool_t          isTerminationFlag() { return fTerminate; }
   void            SetTerminationFlag() { fTerminate = true; }

   // Event Read Flag
   Bool_t          IsDontReadNextEvent() { return fDontReadNextEvent; }
   void            SetDontReadNextEvent(Bool_t flag = true) { fDontReadNextEvent = flag; }

   // Tree IO
   Bool_t          isTreeAccumulation()  { return fTreeAccumulation;  }
   void            SetTreeAccumulation(Bool_t flag = true) { fTreeAccumulation = flag;  }

   // Tasks
   void            AddTask(TTask *task) { fTaskObjects->AddLast(task); }
   ROMETask       *GetTaskObjectAt(Int_t index) { return (ROMETask*)fTaskObjects->At(index); }
   Int_t           GetTaskObjectEntries() { return fTaskObjects->GetEntries(); }

   // Trees
   void            AddTree(TTree *tree) { fTreeObjects->Add(new ROMETree(tree)); }
   ROMETree       *GetTreeObjectAt(Int_t index) { return (ROMETree*)fTreeObjects->At(index); }
   Int_t           GetTreeObjectEntries() { return fTreeObjects->GetEntries(); }

   // Histogram Folders
   TFolder        *GetMainHistoFolder() { return fMainHistoFolder; }
   TFolder        *GetHistoFolderAt(int index) { return (TFolder*)fHistoFolders->At(index); }
   Bool_t          IsHistosRead() { return fHistoRead; }
   Int_t           GetHistosRun() { return fHistoRun; }

   void            SetHistosRead(Bool_t flag) { fHistoRead = flag; }
   void            SetHistosRun(Int_t runNumber) { fHistoRun = runNumber; }

   void            ReplaceWithRunAndEventNumber(ROMEString &buffer);
   // Run Number
   void            GetRunNumberStringAt(ROMEString &buffer,Int_t i, const char* format = 0);
   Long64_t        GetRunNumberAt(Int_t i) { if (i>=fRunNumber.GetSize()) return 0; return fRunNumber.At(i); }
   void            GetCurrentRunNumberString(ROMEString &buffer, const char* format = 0);
   Long64_t        GetCurrentRunNumber();
   Int_t           GetNumberOfRunNumbers() { return fRunNumber.GetSize(); }
   const char     *GetRunNumberStringOriginal() { return fRunNumberString.Data(); }

   void            SetCurrentRunNumber(Long64_t runNumber);
   void            SetRunNumbers(ROMEString &numbers) {
                   fRunNumberString = numbers;
                   DecodeNumbers(numbers,fRunNumber); }
   void            SetRunNumbers(const char *numbers) {
                   fRunNumberString = numbers;
                   DecodeNumbers(fRunNumberString,fRunNumber); }
   Long64_t        GetNextRunNumber(const Long64_t runNumber);

   // Event Number
   Long64_t        GetCurrentEventNumber();
   const char     *GetEventNumberStringOriginal() { return fEventNumberString.Data(); }

   void            SetCurrentEventNumber(Long64_t eventNumber);
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
   Bool_t          IsRunNumberBasedIO() { return (fIOType==kRunNumberBased); }
   Bool_t          IsRunNumberAndFileNameBasedIO() { return (fIOType==kRunNumberAndFileNameBased); }
   Bool_t          IsFileNameBasedIO() { return (fIOType==kFileNameBased); }

   void            SetIOType(Int_t type) { fIOType = type; }

   // User Events
   void            SetUserEventQ() { fUserEvent = true; fUserEventQ = true; }
   void            SetUserEventE() { fUserEvent = true; fUserEventE = true; }
   void            SetUserEventS() { fUserEvent = true; fUserEventS = true; }
   void            SetUserEventR() { fUserEvent = true; fUserEventR = true; }
   void            SetUserEventO() { fUserEvent = true; fUserEventO = true; }
   void            SetUserEventC() { fUserEvent = true; fUserEventC = true; }
   void            SetUserEventJ(Long64_t eventNumber) {
                      fUserEvent = true;
                      fUserEventJ = true;
                      fUserEventJEventNumber = eventNumber;
                   }
   void            SetUserEventG(Long64_t runNumber,Long64_t eventNumber) {
                      fUserEvent = true;
                      fUserEventG = true;
                      fUserEventGRunNumber = runNumber;
                      fUserEventGEventNumber = eventNumber;
                   }
   void            SetUserEventI() { fUserEvent = true; fUserEventI = true; }

   Bool_t          HasUserEvent() { return fUserEvent; }
   void            DeleteUserEvent() {
                      fUserEvent = false;
                      fUserEventQ = false;
                      fUserEventE = false;
                      fUserEventS = false;
                      fUserEventR = false;
                      fUserEventO = false;
                      fUserEventC = false;
                      fUserEventJ = false;
                      fUserEventG = false;
                      fUserEventI = false;
                   }
   Bool_t          IsUserEventQ() { return fUserEventQ; }
   Bool_t          IsUserEventE() { return fUserEventE; }
   Bool_t          IsUserEventS() { return fUserEventS; }
   Bool_t          IsUserEventR() { return fUserEventR; }
   Bool_t          IsUserEventO() { return fUserEventO; }
   Bool_t          IsUserEventC() { return fUserEventC; }
   Bool_t          IsUserEventJ() { return fUserEventJ; }
   Long64_t        GetUserEventJEventNumber() { return fUserEventJEventNumber; }
   Bool_t          IsUserEventG() { return fUserEventG; }
   Long64_t        GetUserEventGRunNumber() { return fUserEventGRunNumber; }
   Long64_t        GetUserEventGEventNumber() { return fUserEventGEventNumber; }
   Bool_t          IsUserEventI() { return fUserEventI; }

   // Event ID
   Int_t           GetEventID() { return fEventID; }
   void            SetEventID(Int_t eventID) { fEventID = eventID; }

  // Processed Events
   Double_t        GetProcessedEvents() { return fTriggerStatistics.processedEvents; }

   // main objects
   TFolder        *GetMainFolder() { return fMainFolder; }
   TTask          *GetMainTask() { return fMainTask; }
   TFile          *GetHistoFileHandle() { return fHistoFiles; }

   void            SetHistoFileHandle(TFile *file) { fHistoFiles = file; }

   const char     *GetProgramName() { return fProgramName.Data(); }

   // Online
   const char     *GetOnlineHost() { return fOnlineHost.Data(); }
   const char     *GetOnlineExperiment() { return fOnlineExperiment.Data(); }
   const char     *GetOnlineAnalyzerName() { return fOnlineAnalyzerName.Data(); }
   const char     *GetOnlineMemoryBuffer() { return fOnlineMemoryBuffer.Data(); }

   void            SetOnlineHost(const char *host) { fOnlineHost = host; }
   void            SetOnlineExperiment(const char *experiment) { fOnlineExperiment = experiment; }
   void            SetOnlineAnalyzerName(const char *analyzerName) { fOnlineAnalyzerName = analyzerName; }
   void            SetOnlineMemoryBuffer(const char *memoryBufferName) { fOnlineMemoryBuffer = memoryBufferName; }

   // Socket Server
   Int_t           GetSocketServerPortNumber() { return fSocketServerPortNumber; }
   Bool_t          isSocketServerActive() { return fSocketServerActive; }
   TMutex         *GetSocketServerMutex() { return fSocketServerMutex; }
   TMutex         *GetObjectStorageMutex() { return fObjectStorageMutex; }
   TMutex         *GetRunEventNumberMutex() { return fRunEventNumberMutex; }
   TMutex         *GetUpdateObjectStorageMutex() { return fUpdateObjectStorageMutex; }

   void            SetSocketServerPortNumber(Int_t portNumber) { fSocketServerPortNumber = portNumber; }
   void            SetSocketServerPortNumber(const char *portNumber) { char *cstop; fSocketServerPortNumber = strtol(portNumber,&cstop,10); }
   void            SetSocketServerActive(Bool_t flag=true) { fSocketServerActive = flag; }
   static THREADTYPE FillObjectsInNetFolderServer(ROMEAnalyzer *localThis);
   virtual void    FillObjectStorage() = 0;
   void            UpdateObjectStorage();
   Bool_t          IsObjectStorageUpdated();
   void            SetObjectStorageUpdated();

   // Socket Client
   const char     *GetSocketClientHost() { return fSocketClientHost.Data(); }
   Int_t           GetSocketClientPort() { return fSocketClientPort; }
   ROMENetFolder  *GetSocketClientNetFolder() { ConnectSocketClient(); return fSocketClientNetFolder; }

   void            SetSocketClientHost(const char *host) { fSocketClientHost = host; }
   void            SetSocketClientPort(Int_t portNumber) { fSocketClientPort = portNumber; }
   void            SetSocketClientPort(const char *portNumber) { char *cstop; fSocketClientPort = strtol(portNumber,&cstop,10); }

   // Midas ODB
   HNDLE        GetMidasOnlineDataBase() { return fMidasOnlineDataBase; }
   HNDLE*       GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; }

   // Configuration
   ROMEConfig     *GetConfiguration() { return fConfiguration; }
   virtual Bool_t  ShowConfigurationFile() = 0;

   // Statistics
   Statistics     *GetTriggerStatistics() { return &fTriggerStatistics; }
   Statistics     *GetScalerStatistics() { return &fScalerStatistics; }

   // Start Method
   Bool_t  Start(int argc=0, char **argv=NULL);

   // Run/Event Number Handling
   void            DecodeNumbers(ROMEString &str,TArrayL64 &arr);
   void            DecodeInputFileNames(ROMEString &str,ROMEStrArray &arr);
   Int_t           CheckEventNumber(Long64_t eventNumber);
   Int_t           CheckRunNumber(Long64_t runNumber);
   Int_t           CheckNumber(Long64_t eventNumber,TArrayL64 &numbers);


   // Run Stat
   void            ShowRunStat(Bool_t flag=true) { fShowRunStat = flag; }
   Bool_t          IsShowRunStat() { return fShowRunStat; }

   // Event Based Data Base
   void            SetEventBasedDataBase(Bool_t flag=true) { fEventBasedDataBase = flag; }
   Bool_t          IsEventBasedDataBase() { return fEventBasedDataBase; }

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
   ArgusWindow    *GetWindow() { return fWindow; }
   Bool_t          StartWindow();
   Int_t           GetWindowUpdateFrequency() { return fWindowUpdateFrequency; };
   void            SetWindowUpdateFrequency(Int_t frequency) { fWindowUpdateFrequency = frequency; };
   Bool_t          IsWindowBusy();

   // Program Steering
   void            NextEvent();
   void            GotoEvent(Long64_t eventNumber);

   virtual Bool_t  ReadSingleDataBaseFolders() = 0;
   virtual Bool_t  ReadArrayDataBaseFolders() = 0;

   Int_t           ss_getchar(UInt_t reset);
   UInt_t          ss_kbhit();
   Int_t           ss_daemon_init(Bool_t keep_stdout);
   Int_t           ss_batch_init();
   Bool_t          strtobool(const char *str);

   Int_t           stricmp(const char*,const char*);
   Bool_t          toBool(Int_t value);
   void            redirectOutput();
   void            restoreOutput();

   void            CopyTObjectWithStreamer(TBuffer *buffer,TObject* source,TObject* destination);

protected:
   Bool_t          CreateHistoFolders(TList *,TFolder *);

   virtual Bool_t  ReadParameters(int argc, char *argv[]);
   virtual Bool_t  ReadUserParameter(const char* /*opt*/, const char* /*value*/, Int_t &/*i*/) { return false; }
   virtual void    ParameterUsage();
   virtual void    UserParameterUsage(){}

   virtual void    startSplashScreen() = 0;
   virtual void    consoleStartScreen() = 0;
   Bool_t          ConnectSocketClient();
   virtual Bool_t  ConnectSocketClientNetFolder() = 0;
   virtual void    StartNetFolderServer() = 0;

   ClassDef(ROMEAnalyzer,0) // Base analyzer class
};

#endif   // ROMEAnalyzer_H
