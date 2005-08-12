/********************************************************************
  ROMEAnalyzer.h, M. Schneebeli PSI

  $Log$
  Revision 1.58  2005/08/12 15:37:02  schneebeli_m
  added input file based IO

  Revision 1.57  2005/08/05 07:45:00  schneebeli_m
  added UserEvents

  Revision 1.56  2005/08/02 14:44:51  schneebeli_m
  correct taskHierarchy handling

  Revision 1.55  2005/07/12 06:42:21  sawada
  Bug fix. Matched the name of method (IsActiveID and IsActiveEventID)

  Revision 1.54  2005/05/18 09:49:32  schneebeli_m
  removed run & event number error, implemented FileRead in ROMEString

  Revision 1.53  2005/05/13 23:51:14  sawada
  code cleanup.

  Revision 1.52  2005/04/25 13:35:11  schneebeli_m
  .

  Revision 1.51  2005/04/22 16:52:57  sawada
  default return value of ReadUserParameter

  Revision 1.50  2005/04/22 16:25:15  sawada
  user defined command line option.

  Revision 1.49  2005/04/20 15:12:40  schneebeli_m
  histo and path in definition xml

  Revision 1.48  2005/04/15 16:44:26  schneebeli_m
  odb, zlib

  Revision 1.47  2005/04/15 14:30:24  sawada
  small bug fix.

  Revision 1.46  2005/04/15 14:22:47  sawada
  check for NULL pointer.

  Revision 1.45  2005/04/15 08:10:46  schneebeli_m
  removed config error

  Revision 1.44  2005/04/15 07:33:14  schneebeli_m
  GetActiveDAQ() NULL pointer test

  Revision 1.43  2005/04/14 07:56:46  schneebeli_m
  Implemented odb database (offline)

  Revision 1.42  2005/04/08 17:07:59  schneebeli_m
  TNetFolderServer changes

  Revision 1.41  2005/04/07 08:27:36  schneebeli_m
  config bug, db bug

  Revision 1.40  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.39  2005/03/23 09:06:11  schneebeli_m
  libxml replaced by mxml, Bool SP error

  Revision 1.38  2005/03/18 16:12:57  schneebeli_m
  Event request & Histo in romeConfig

  Revision 1.37  2005/02/21 21:29:07  sawada

  Changed OS specifying macros
  Support for DEC,Ultrix,FreeBSD,Solaris

  Revision 1.36  2005/01/27 16:21:06  schneebeli_m
  print method & no gROME in path

  Revision 1.35  2005/01/24 16:29:40  schneebeli_m
  last bank access

  Revision 1.34  2005/01/24 15:38:02  schneebeli_m
  ss_millitime

  Revision 1.33  2005/01/24 15:25:06  schneebeli_m
  Seperated DAQ classes

  Revision 1.32  2005/01/19 13:29:50  schneebeli_m
  geteventid

  Revision 1.31  2005/01/18 17:41:29  schneebeli_m
  Termination flag

  Revision 1.30  2004/12/06 09:20:50  schneebeli_m
  ss_getchar on linux

  Revision 1.29  2004/12/06 09:04:34  schneebeli_m
  minor changes

  Revision 1.28  2004/11/16 16:14:00  schneebeli_m
  implemented task hierarchy

  Revision 1.27  2004/11/11 12:55:27  schneebeli_m
  Implemented XML database with new path rules

  Revision 1.26  2004/10/14 09:53:41  schneebeli_m
  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag

  Revision 1.25  2004/10/05 13:30:32  schneebeli_m
  make -e, Port number

  Revision 1.24  2004/10/05 07:52:44  schneebeli_m
  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed

  Revision 1.23  2004/10/01 13:11:33  schneebeli_m
  Tree write error removed, Database Number Problem solved, Trees in Folder for TSocket

  Revision 1.22  2004/09/30 13:08:21  schneebeli_m
  ...

  Revision 1.21  2004/09/30 10:18:05  schneebeli_m
  gAnalyzer and gROME

  Revision 1.20  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROMEAnalyzer_H
#define ROMEAnalyzer_H

#include <time.h>
#include <TRint.h>
#include <TFile.h>
#include <TArrayI.h>
#include <ROMEString.h>
#include <TList.h>
#include <TTask.h>
#include <TTree.h>
#include <TROOT.h>
#include <TFolder.h>
#include "ROME.h"
#include <ROMEConfig.h>
#include <ROMETree.h>
#include <ROMETreeInfo.h>
#include <ROMEDataBase.h>
#include <ROMEString.h>
#include <ROMEDAQSystem.h>
#if defined ( HAVE_SQL )
#   include <ROMESQL.h>
#endif

typedef struct {
   double processedEvents;   //! Processed Events
   double eventsPerSecond;   //! Events per Second
   double writtenEvents;     //! Written Events
} Statistics;

class ROMEAnalyzer : public TObject
{
private:
   // Analysis Mode
   enum {
      kAnalyzeOffline,
      kAnalyzeOnline
   };

public:
   // IO type
   enum {
      kRunNumberBased,
      kRunNumberAndFileNameBased,
      kFileNameBased
   };

protected:

   // Application
   TRint*     fApplication;                     //! Application Handle

   // Active DAQ System
   ROMEDAQSystem *fActiveDAQ;                   //! Handle to the active DAQ system

   // Modes
   Int_t      fAnalysisMode;                    //! Analysis mode flag
   Bool_t     fBatchMode;                       //! Batch mode flag
   Bool_t     fSplashScreen;                    //! Splash screen flag


   Bool_t     fDontReadNextEvent;               //! Don't read the next event from file/buffer

   // Directories
   ROMEString fInputDir;                        //! General Input Directory
   ROMEString fOutputDir;                       //! General Output Directory
   ROMEString *fDataBaseDir;                    //! Data Base File Directories
   ROMEString fConfigDir;                       //! Configuration File Directory

   // Run Numbers
   Int_t      fCurrentRunNumber;                //! Currently Analyzed Run Number
   TArrayI    fRunNumber;                       //! Run Numbers to Analyze
   ROMEString fRunNumberString;                 //! Run Numbers in Input String Format

   // Event Numbers
   Int_t      fCurrentEventNumber;              //! Currently Analyzed Event Number
   TArrayI    fEventNumber;                     //! Event Numbers to Analyze
   ROMEString fEventNumberString;               //! Event Numbers in Input String Format
   Int_t      fLastEventNumberIndex;            //! Index of the last Analyzed Event Number

   // Input File Names
   ROMEString   fCurrentInputFileName;            //! Name of Currently Analyzed Input File
   ROMEStrArray fInputFileNames;                  //! Input File Names to Analyze
   ROMEString   fInputFileNamesString;            //! Original Input File Names in Input String Format

   // IO type
   int          fIOType;                          //! IO type

   // User Events
   bool       fUserEvent;                       //! General User Event Flag
   bool       fUserEventQ;                      //! Flag for User Event of Type Q
   bool       fUserEventE;                      //! Flag for User Event of Type E
   bool       fUserEventS;                      //! Flag for User Event of Type S
   bool       fUserEventR;                      //! Flag for User Event of Type R
   bool       fUserEventO;                      //! Flag for User Event of Type O
   bool       fUserEventC;                      //! Flag for User Event of Type C
   bool       fUserEventG;                      //! Flag for User Event of Type G
   int        fUserEventGRunNumber;             //! Run Number for User Event of Type G
   int        fUserEventGEventNumber;           //! Event Number for User Event of Type G
   bool       fUserEventI;                      //! Flag for User Event of Type I

   // Event ID
   char       fEventID;                         //! Event ID of current Event

   // Midas
   int        fMidasOnlineDataBase;             //! Handle to the Midas Online Data Base

   // Flags
   Bool_t     fTerminate;                       //! Termination flag
   Bool_t     fFillEvent;                       //! Fill Event Flag

   // Object Handles
   TTask*     fMainTask;                        //! Handle to Main Task
   TFolder*   fMainFolder;                      //! Handle to Main Folder
   TFile*     fHistoFiles;                      //! Handle to Histogram Files

   // Trees
   TObjArray*    fTreeObjects;                     //! Handle to Tree Objects
   bool          fTreeAccumulation;                //! Accumulation of runs in Trees

   // Histogram Folders
   TFolder*      fMainHistoFolder;                 //! Handle to Main Histogram Folder (histos)
   TObjArray*    fHistoFolders;                    //! Handle to Histogram Folder Objects

   // Program name
   ROMEString    fProgramName;                     //! Name of this Program

   // Online
   ROMEString    fOnlineHost;                      //! Name of the Online Host
   ROMEString    fOnlineExperiment;                //! Name of the Online Experiment

   // Socket
   int           fPortNumber;                      //! Port Number for TSocket
   bool          fSocketOffline;                   //! Socket available offline

   // Statistics
   Statistics    fTriggerStatistics;               //! Trigger Statistics
   Statistics    fScalerStatistics;                //! Scaler Statistics

   // Data base
   ROMEDataBase **fDataBaseHandle;                 //! DataBase Handles
   ROMEString    *fDataBaseConnection;             //! DataBase connection strings
   int            fNumberOfDataBases;              //! Number of DataBases available

   // Configuration
   ROMEConfig   *fConfiguration;                   //! Configuration Handle

public:
   ROMEAnalyzer() {};
   ROMEAnalyzer(TRint *app);
   ~ROMEAnalyzer();

   // Output
   void          Print(char text);
   void          Print(const char* text="");
   void          Println(const char* text="");
   void          Printfl(const char* text="");

   // Application Handle
   TRint*        GetApplication() { return fApplication; };

   // Active DAQ System
   const char*    GetNameOfActiveDAQ() { if (fActiveDAQ==NULL) return "none"; return fActiveDAQ->GetName(); };
   ROMEDAQSystem* GetActiveDAQ() {
      if (fActiveDAQ!=NULL)
         return fActiveDAQ;
      this->Println("\nYou have tried to access the active DAQ system but none is active .\nPlease select a DAQ system in the ROME configuration file under:\n<Modes>\n   <DAQSystem>\n\nShutting down the program.\n");
      fApplication->Terminate(1);
      return NULL;
   };
   bool           isActiveDAQSet() { return fActiveDAQ!=NULL; };
   void           SetActiveDAQ(ROMEDAQSystem* handle) { fActiveDAQ = handle; };

   // Data Base Handle
   const char*   GetDataBaseConnection(int i) { return fDataBaseConnection[i].Data(); };
   void          SetDataBaseConnection(int i,const char* connection) { fDataBaseConnection[i] = connection; };
   ROMEDataBase* GetDataBase(int i) {
      if(i<fNumberOfDataBases && fDataBaseHandle[i]!=NULL)
	 return fDataBaseHandle[i];
      this->Println("\nYou have tried to access a database without initialisation.\nTo use the databases you have to add it to the list of databases in the\nROME configuration file under <DataBases>.\n\nShutting down the program.\n");
      fApplication->Terminate(1);
      return NULL;
   };
   ROMEDataBase* GetDataBase(const char *name) {
      for (int i=0;i<fNumberOfDataBases;i++)
         if (!stricmp(fDataBaseHandle[i]->GetName(),name))
            return fDataBaseHandle[i];
      ROMEString str;
      str.SetFormatted("\nYou have tried to access the %s database without initialisation.\nTo use the %s database you have to add it to the list of databases in the\nROME configuration file under <DataBases>.\n\nShutting down the program.\n",name,name);
      this->Println(str.Data());
      fApplication->Terminate(1);
      return NULL;
   };
   bool isDataBaseActive(const char *name) {
      for (int i=0;i<fNumberOfDataBases;i++)
         if (!stricmp(fDataBaseHandle[i]->GetName(),name))
            return true;
      return false;
   };
   void          SetDataBase(int i,ROMEDataBase* dataBase) { fDataBaseHandle[i] = dataBase; };
   int           GetNumberOfDataBases() { return fNumberOfDataBases; };
   void          InitDataBases(int number) { fDataBaseHandle = new ROMEDataBase*[number];
                                             fDataBaseConnection = new ROMEString[number];
                                             fDataBaseDir = new ROMEString[number];
                                             fNumberOfDataBases = number; };

   // modes
   Bool_t     isSplashScreen() { return fSplashScreen; };
   Bool_t     isBatchMode() { return fBatchMode; };

   void       SetSplashScreen(bool flag=true)  { fSplashScreen = flag; };
   void       SetBatchMode(bool flag=true) { fBatchMode = flag; };

   // Analysis Mode
   Bool_t     isOnline() { return fAnalysisMode==kAnalyzeOnline; };
   Bool_t     isOffline() { return fAnalysisMode==kAnalyzeOffline; };

   void       SetOnline()  { fAnalysisMode = kAnalyzeOnline; };
   void       SetOffline() { fAnalysisMode = kAnalyzeOffline; };

   // Directories
   const char*      GetInputDir() { return fInputDir.Data(); }
   const char*      GetOutputDir() { return fOutputDir.Data(); }
   const char*      GetDataBaseDir(int i) { return fDataBaseDir[i].Data(); }
   const char*      GetConfigDir() { return fConfigDir.Data(); }

   void       SetInputDir(const char* dir) { fInputDir = dir; }
   void       SetInputDir(ROMEString& dir) { fInputDir = dir; }
   void       SetOutputDir(const char* dir) { fOutputDir = dir; }
   void       SetOutputDir(ROMEString& dir) { fOutputDir = dir; }
   void       SetDataBaseDir(int i,const char* dir) { fDataBaseDir[i] = dir; }
   void       SetDataBaseDir(int i,ROMEString& dir) { fDataBaseDir[i] = dir; }
   void       SetConfigDir(const char* dir) { fConfigDir = dir; }
   void       SetConfigDir(ROMEString& dir) { fConfigDir = dir; }

   // Fill Event Flag
   Bool_t     isFillEvent() { return fFillEvent; };
   void       SetFillEvent(Bool_t fillEvent = true) { fFillEvent = fillEvent; }

   // Termination Flag
   Bool_t     isTerminationFlag() { return fTerminate; };
   void       SetTerminationFlag() { fTerminate = true; };


   // Event Read Flag
   bool       IsDontReadNextEvent() { return fDontReadNextEvent; };

   void       SetDontReadNextEvent(bool flag = true) { fDontReadNextEvent = flag; };

   // Tree IO
   Bool_t     isTreeAccumulation()  { return fTreeAccumulation;  };

   void       SetTreeAccumulation(bool flag = true) { fTreeAccumulation = flag;  };

   // Trees
   void       AddTree(TTree *tree) { fTreeObjects->Add(new ROMETree(tree)); };
   ROMETree*  GetTreeObjectAt(int index) { return (ROMETree*)fTreeObjects->At(index); };
   int        GetTreeObjectEntries() { return fTreeObjects->GetEntries(); };

   // Histogram Folders
   TFolder*  GetMainHistoFolder() { return fMainHistoFolder; };
   TFolder*  GetHistoFolderAt(int index) { return (TFolder*)fHistoFolders->At(index); };

   // Run Number
   void       GetRunNumberStringAt(ROMEString& buffer,int i) { buffer.SetFormatted("%0*d",5,fRunNumber.At(i)); }
   int        GetRunNumberAt(int i) { return fRunNumber.At(i); }
   void       GetCurrentRunNumberString(ROMEString& buffer) { buffer.SetFormatted("%0*d",5,fCurrentRunNumber); }
   int        GetCurrentRunNumber() { return fCurrentRunNumber; }
   int        GetNumberOfRunNumbers() { return fRunNumber.GetSize(); }
   const char* GetRunNumberStringOriginal() { return fRunNumberString.Data(); }

   void       SetCurrentRunNumber(int runNumber) { fCurrentRunNumber = runNumber; }
   void       SetRunNumbers(ROMEString& numbers) {
                  fRunNumberString = numbers;
                  DecodeRunNumbers(numbers,fRunNumber); }
   void       SetRunNumbers(const char* numbers) {
                  fRunNumberString = numbers;
                  DecodeRunNumbers(fRunNumberString,fRunNumber); }

   // Event Number
   int        GetCurrentEventNumber() { return fCurrentEventNumber; }
   const char* GetEventNumberStringOriginal() { return fEventNumberString.Data(); }

   void       SetCurrentEventNumber(int eventNumber) { fCurrentEventNumber = eventNumber; }
   void       SetEventNumbers(ROMEString& numbers) {
                  fEventNumberString = numbers;
                  DecodeRunNumbers(numbers,fEventNumber); }
   void       SetEventNumbers(const char* numbers) {
                  fEventNumberString = numbers;
                  DecodeRunNumbers(fEventNumberString,fEventNumber); }

   int        CheckEventNumber(int eventNumber);
   void       InitCheckEventNumber() { fLastEventNumberIndex = 0; };

   // Input File Names
   ROMEString GetInputFileNameAt(int i) { return fInputFileNames.At(i); }
   ROMEString GetCurrentInputFileName() { return fCurrentInputFileName; }
   int        GetNumberOfInputFileNames() { return fInputFileNames.GetEntriesFast(); }
   const char* GetInputFileNamesStringOriginal() { return fInputFileNamesString.Data(); }

   void       SetCurrentInputFileName(ROMEString& inputFileName) { fCurrentInputFileName = inputFileName; }
   void       SetInputFileNames(ROMEString& names) {
                  fInputFileNamesString = names;
                  DecodeInputFileNames(names,fInputFileNames); }
   void       SetInputFileNames(const char* numbers) {
                  fInputFileNamesString = numbers;
                  DecodeInputFileNames(fInputFileNamesString,fInputFileNames); }

   // IO type
   bool       IsRunNumberBasedIO() { return (fIOType==kRunNumberBased); };
   bool       IsRunNumberAndFileNameBasedIO() { return (fIOType==kRunNumberAndFileNameBased); };
   bool       IsFileNameBasedIO() { return (fIOType==kFileNameBased); };

   void       SetIOType(int type) { fIOType = type; };

   // User Events
   void       SetUserEventQ() { fUserEvent = true; fUserEventQ = true; };
   void       SetUserEventE() { fUserEvent = true; fUserEventE = true; };
   void       SetUserEventS() { fUserEvent = true; fUserEventS = true; };
   void       SetUserEventR() { fUserEvent = true; fUserEventR = true; };
   void       SetUserEventO() { fUserEvent = true; fUserEventO = true; };
   void       SetUserEventC() { fUserEvent = true; fUserEventC = true; };
   void       SetUserEventG() { fUserEvent = true; fUserEventG = true; };
   void       SetUserEventI() { fUserEvent = true; fUserEventI = true; };

   bool       HasUserEvent() { return fUserEvent; };
   void       DeleteUserEvent() { fUserEvent = false; fUserEventQ = false; fUserEventE = false; fUserEventS = false; fUserEventR = false; fUserEventO = false; fUserEventC = false; fUserEventG = false; fUserEventI = false; };
   bool       IsUserEventQ() { return fUserEventQ; };
   bool       IsUserEventE() { return fUserEventE; };
   bool       IsUserEventS() { return fUserEventS; };
   bool       IsUserEventR() { return fUserEventR; };
   bool       IsUserEventO() { return fUserEventO; };
   bool       IsUserEventC() { return fUserEventC; };
   bool       IsUserEventG() { return fUserEventG; };
   int        GetUserEventGRunNumber() { return fUserEventGRunNumber; };
   int        GetUserEventGEventNumber() { return fUserEventGEventNumber; };
   bool       IsUserEventI() { return fUserEventI; };

   // Event ID
   int        GetEventID() { return fEventID-48; }
   char       GetEventIDChar() { return fEventID; }
   void       SetEventID(char eventID) { fEventID = eventID; }
   void       SetEventID(int eventID) { fEventID = 48+eventID; }

  // Processed Events
   double     GetProcessedEvents() { return fTriggerStatistics.processedEvents; }

   // main objects
   TFolder*   GetMainFolder() { return fMainFolder; }
   TTask*     GetMainTask() { return fMainTask; }
   TFile*     GetHistoFileHandle() { return fHistoFiles; };

   void       SetHistoFileHandle(TFile *file) { fHistoFiles = file; };

   const char* GetProgramName() { return fProgramName.Data(); };

   // Online
   const char* GetOnlineHost() { return fOnlineHost.Data(); };
   const char* GetOnlineExperiment() { return fOnlineExperiment.Data(); };

   void       SetOnlineHost(const char* host) { fOnlineHost = host; };
   void       SetOnlineExperiment(const char* experiment) { fOnlineExperiment = experiment; };

   // Socket
   int        GetPortNumber() { return fPortNumber; };
   bool       isSocketOffline() { return fSocketOffline; };

   void       SetPortNumber(int portNumber) { fPortNumber = portNumber; };
   void       SetPortNumber(const char* portNumber) { char* cstop; fPortNumber = strtol(portNumber,&cstop,10); };
   void       SetSocketOffline(bool flag=true) { fSocketOffline = flag; };

   // Midas ODB
   int        GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
   int*       GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; };

   // Configuration
   ROMEConfig *GetConfiguration() { return fConfiguration; };

   // Statistics
   Statistics* GetTriggerStatistics() { return &fTriggerStatistics; };
   Statistics* GetScalerStatistics() { return &fScalerStatistics; };

   // Start Method
   bool        Start(int argc=0, char **argv=NULL);

   // Decode Methods
   void        DecodeRunNumbers(ROMEString& str,TArrayI& arr);
   void        DecodeInputFileNames(ROMEString& str,ROMEStrArray& arr);


   virtual bool ReadSingleDataBaseFolders() = 0;
   virtual bool ReadArrayDataBaseFolders() = 0;

   int ss_getchar(bool reset);
   Bool_t ss_kbhit();
   int ss_sleep(int millisec);
   long ss_millitime();
   bool strtobool(const char* str);

   int  stricmp(const char*,const char*);
   bool toBool(int value);
protected:

   bool CreateHistoFolders(TList *,TFolder *);

   bool ReadParameters(int argc, char *argv[]);
   virtual bool ReadUserParameter(const char* opt, const char* value, int& i) { return false; }
   void ParameterUsage();
   virtual void UserParameterUsage(){}

   virtual void startSplashScreen() = 0;
   virtual void consoleStartScreen() = 0;
   virtual void redirectOutput() = 0;

   ClassDef(ROMEAnalyzer,0)
};

extern ROMEAnalyzer *gROME;  // global ROMEAnalyzer Handle
extern void *gPassToROME;  // void ROMEAnalyzer Handle

#endif   // ROMEAnalyzer_H
