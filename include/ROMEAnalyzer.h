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
#include <ROMEStatic.h>
#include <ROMETree.h>
#include <ROMETreeInfo.h>
#if defined HAVE_MIDAS
#include <midas.h>
#endif
#if defined HAVE_SQL
#include <ROMESQL.h>
#endif

class ROMEAnalyzer : public TObject
{
private:
   typedef struct {
      double processedEvents;   //! Processed Events
      double eventsPerSecond;   //! Events per Second
      double writtenEvents;     //! Written Events
   } Statistics;

   // Run Status
   enum {
      kRunning   = 0,
      kStopped   = 1
   };
   // Event Status
   enum {
      kAnalyze   = 0,
      kContinue  = 1,
      kEndOfRun  = 2,
      kTerminate = 3
   };
   // Data Format
   enum {
      kRoot  = 0,
      kMidas = 1
   };
   // Analysis Mode
   enum {
      kAnalyzeOffline = 0,
      kAnalyzeOnline  = 1
   };
   // Data Base
   enum {
      kDataBaseNone = 0,
      kDataBaseSQL  = 1,
      kDataBaseXML  = 2
   };

protected:
   // Application
   TRint*     fApplication;                     //! Application Handle

   // Modes
   Int_t      fAnalysisMode;                    //! Analysis mode flag
   Int_t      fDataFormat;                      //! Root mode flag
   Int_t      fDataBase;                        //! Data Base flag
   Bool_t     fBatchMode;                       //! Batch mode flag
   Bool_t     fSplashScreen;                    //! Splash screen flag

   // Status
   Int_t      fRunStatus;                       //! Run Status flag
   Int_t      fEventStatus;                     //! Event Status flag

   Bool_t     fDontReadNextEvent;               //! Don't read the next event from file/buffer

   // Directories
   ROMEString fInputDir;                        //! General Input Directory
   ROMEString fOutputDir;                       //! General Output Directory
   ROMEString fDataBaseDir;                     //! Data Base File Directory
   ROMEString fConfigDir;                       //! Configuration File Directory

   // Run & Event Numbers
   Int_t      fCurrentRunNumber;                //! Currently Analyzed Run Number
   TArrayI    fRunNumber;                       //! Run Numbers to Analyze
   ROMEString fRunNumberString;                 //! Run Numbers in Input String Format
   Int_t      fCurrentEventNumber;              //! Currently Analyzed Event Number
   TArrayI    fEventNumber;                     //! Event Numbers to Analyze
   ROMEString fEventNumberString;               //! Event Numbers in Input String Format

   // Event ID
   ROMEString fEventID;                         //! Event ID of current Event

   Int_t      fMidasFileHandle;                 //! Handle to Midas Inputfile
   int        fMidasBuffer;                     //! Midas Online Buffer
   char       fMidasEvent[110000];              //! Midas Inputdata Stack for an Event

   TFile**    fRootFiles;                       //! Root files

   // Progress Display
   int        fProgressDelta;                   //! Maximal time difference
   time_t     fProgressLast;                    //! Last time
   int        fProgressLastEvent;               //! Last Event
   bool       fProgressWrite;                   //! Write flag

   // Flags
   Bool_t     fTerminate;                       //! Termination flag
   Bool_t     fFillEvent;                       //! Fill Event Flag

   // Object Handles
   TTask*     fMainTask;                        //! Handle to Main Task
   TFolder*   fMainFolder;                      //! Handle to Main Folder
   TFile*     fHistoFiles;                      //! Handle to Histogram Files

   // Trees Files
   TFile**       treeFiles;                        //! File Handles for Tree Objects

   // Trees
   TObjArray*    fTreeObjects;                     //! Handle to Tree Objects
   bool          fTreeAccumulation;                //! Accumulation of runs in Trees

   // Tree Info
   int           fSequentialNumber;                //! Sequential Number
   ROMETreeInfo* fTreeInfo;                        //! Tree Info Object
   int*          fTreePosition;                    //! Array with tree read positions
   int*          fTreeNextSeqNumber;               //! Array with the trees next sequential number

   // Program name
   ROMEString    fProgramName;                     //! Name of this Program
   // Online Host Name
   ROMEString    fOnlineHost;                      //! Name of the Online Host
   ROMEString    fOnlineExperiment;                //! Name of the Online Experiment

   // Statistics
   Statistics    fTriggerStatistics;               //! Trigger Statistics
   ROMEString    fTriggerStatisticsString;         //! String describing Trigger Statistics
   Statistics    fScalerStatistics;                //! Scaler Statistics
   ROMEString    fScalerStatisticsString;          //! String describing Scaler Statistics
   int           fTimeOfLastEvent;                 //! Time of last Event
   double        fLastEvent;                       //! Last Event

   // ODB Handle
#if defined HAVE_MIDAS
   HNDLE         fMidasOnlineDataBase;             //! Handle to the Midas Online Data Base
#endif

   // SQL Handle
#if defined HAVE_SQL
#ifndef __CINT__
   ROMESQL*      fSQL;                             //! Handle to SQL Class
#endif
#endif

   bool          fContinuous;                      //! Continuous Mode

public:
   // Static Task Switches Changes Flag
   static bool fTaskSwitchesChanged;            //! Flag Task Switches Changes

public:
   ROMEAnalyzer();
   ~ROMEAnalyzer();

   // modes
   Bool_t     isSplashScreen() { return fSplashScreen; };
   Bool_t     isBatchMode() { return fBatchMode; };

   // Run Status
   Bool_t     isRunning()  { return fRunStatus==kRunning; };
   Bool_t     isStopped()  { return fRunStatus==kStopped; };

   void       SetRunning()  { fRunStatus = kRunning; };
   void       SetStopped()  { fRunStatus = kStopped; };

   // Event Status
   Bool_t     isAnalyze()   { return fEventStatus==kAnalyze; };
   Bool_t     isContinue()  { return fEventStatus==kContinue; };
   Bool_t     isEndOfRun()  { return fEventStatus==kEndOfRun; };
   Bool_t     isTerminate() { return fEventStatus==kTerminate; };

   void       SetAnalyze()   { fEventStatus = kAnalyze; };
   void       SetContinue()  { fEventStatus = kContinue; };
   void       SetEndOfRun()  { fEventStatus = kEndOfRun; };
   void       SetTerminate() { fEventStatus = kTerminate; };

   // Analysis Mode
   Bool_t     isOnline() { return fAnalysisMode==kAnalyzeOnline; };
   Bool_t     isOffline() { return fAnalysisMode==kAnalyzeOffline; };

   void       SetOnline()  { fAnalysisMode = kAnalyzeOnline; };
   void       SetOffline() { fAnalysisMode = kAnalyzeOffline; };

   // Data Format
   Bool_t     isRoot() { return fDataFormat==kRoot; };
   Bool_t     isMidas() { return fDataFormat==kMidas; };

   void       SetRoot()  { fDataFormat = kRoot; };
   void       SetMidas() { fDataFormat = kMidas; };

   // Data Base
   Bool_t     isSQLDataBase() { return fDataBase==kDataBaseSQL; };
   Bool_t     isXMLDataBase() { return fDataBase==kDataBaseXML; };

   void       SetNoDataBase()  { fDataBase = kDataBaseNone; };
   void       SetSQLDataBase() { fDataBase = kDataBaseSQL; };
   void       SetXMLDataBase() { fDataBase = kDataBaseXML; };

   // Directories
   char*      GetInputDir() { return (char*)fInputDir.Data(); }
   char*      GetOutputDir() { return (char*)fOutputDir.Data(); }
   char*      GetDataBaseDir() { return (char*)fDataBaseDir.Data(); }
   char*      GetConfigDir() { return (char*)fConfigDir.Data(); }

   void       SetInputDir(char* dir) { fInputDir = dir; }
   void       SetInputDir(ROMEString& dir) { fInputDir = dir; }
   void       SetOutputDir(char* dir) { fOutputDir = dir; }
   void       SetOutputDir(ROMEString& dir) { fOutputDir = dir; }
   void       SetDataBaseDir(char* dir) { fDataBaseDir = dir; }
   void       SetDataBaseDir(ROMEString& dir) { fDataBaseDir = dir; }
   void       SetConfigDir(char* dir) { fConfigDir = dir; }
   void       SetConfigDir(ROMEString& dir) { fConfigDir = dir; }

   // Fill Event Flag
   Bool_t     isFillEvent() { return fFillEvent; };
   void       SetFillEvent(Bool_t fillEvent = true) { fFillEvent = fillEvent; }

   // Termination Flag
   void       SetTerminationFlag() { fTerminate = true; };


   // Event Read Flag
   bool       IsDontReadNextEvent() { return fDontReadNextEvent; };

   void       SetDontReadNextEvent() { fDontReadNextEvent = true; };

   // midass event header
   EVENT_HEADER* GetEventHeader() { return (EVENT_HEADER*)fMidasEvent; };

   // Tree IO
   Bool_t     isTreeAccumulation()  { return fTreeAccumulation;  };

   void       SetTreeAccumulation() { fTreeAccumulation = true;  };

   // Trees
   void       AddTree(TTree *tree) { fTreeObjects->Add(new ROMETree(tree,0,0)); };
   ROMETree*  GetTreeObjectAt(int index) { return (ROMETree*)fTreeObjects->At(index); };
   int        GetTreeObjectEntries() { return fTreeObjects->GetEntries(); };

   // Run Number
   void       GetRunNumberStringAt(ROMEString& buffer,int i) { buffer.SetFormatted("%0*d",5,fRunNumber.At(i)); }
   int        GetRunNumberAt(int i) { return fRunNumber.At(i); }
   void       GetCurrentRunNumberString(ROMEString& buffer) { buffer.SetFormatted("%0*d",5,fCurrentRunNumber); }
   int        GetCurrentRunNumber() { return fCurrentRunNumber; }
   int        GetNumberOfRunNumbers() { return fRunNumber.GetSize(); }
   char*      GetRunNumberStringOriginal() { return (char*)fRunNumberString.Data(); }

   void       SetRunNumbers(ROMEString& numbers) { 
                  fRunNumberString = numbers;
                  fRunNumber = decodeRunNumbers(numbers); }
   void       SetRunNumbers(char* numbers) { 
                  fRunNumberString = numbers;
                  fRunNumber = decodeRunNumbers(fRunNumberString); }

   int        GetCurrentEventNumber() { return fCurrentEventNumber; }
   char*      GetEventNumberStringOriginal() { return (char*)fEventNumberString.Data(); }

   void       SetCurrentEventNumber(int eventNumber) { fCurrentEventNumber = eventNumber; }
   void       SetEventNumbers(ROMEString& numbers) { 
                  fEventNumberString = numbers;
                  fEventNumber = decodeRunNumbers(numbers); }
   void       SetEventNumbers(char* numbers) { 
                  fEventNumberString = numbers;
                  fEventNumber = decodeRunNumbers(fEventNumberString); }

   // Event ID
   char*      GetEventID() { return (char*)fEventID.Data(); }
   void       SetEventID(char* eventID) { fEventID = eventID; }
   void       SetEventID(int eventID) { fEventID.SetFormatted("%0*d",1,eventID); }

  // Processed Events
   double     GetProcessedEvents() { return fTriggerStatistics.processedEvents; }

   // main objects
   TFolder*   GetMainFolder() { return fMainFolder; }
   TTask*     GetMainTask() { return fMainTask; }
   TFile*     GetHistoFileHandle() { return fHistoFiles; };

   void       SetHistoFileHandle(TFile *file) { fHistoFiles = file; };

   char*      GetProgramName() { return (char*)fProgramName.Data(); };

   // Start Method
   bool       Start(int argc=0, char **argv=NULL);

   // event methods
   bool Init();
   bool Connect(Int_t runNumber);
   bool ReadEvent(Int_t event);
   bool UserInput();
   bool WriteEvent();
   bool Update();
   bool Disconnect();
   bool Terminate();

   virtual void InitFolders() = 0;
   virtual void InitTaskSwitches() = 0;
   virtual void UpdateTaskSwitches() = 0;

   static TArrayI decodeRunNumbers(ROMEString& str);

private:
   void CreateHistoFolders();

   bool ReadParameters(int argc, char *argv[]);
   void ParameterUsage();

   virtual bool ReadROMEConfigXML(char *configFile) = 0;
   virtual bool WriteROMEConfigXML(char *configFile) = 0;

   virtual void startSplashScreen() = 0;
   virtual void consoleStartScreen() = 0;

   virtual bool InitODB() = 0;

   virtual void ConnectTrees() = 0;
   virtual void FillTrees() = 0;
   virtual void ClearFolders() = 0;

   virtual bool ReadXMLDataBase() = 0;

   virtual bool InitSQLDataBase() = 0;
   virtual bool ReadSQLDataBase() = 0;

   virtual void InitMidasBanks() = 0;

   ClassDef(ROMEAnalyzer,0)
};

#endif   // ROMEAnalyzer_H
