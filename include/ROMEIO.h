#ifndef ROMEIO_H
#define ROMEIO_H

#include <time.h>
#include <TFile.h>
#include <TString.h>
#include <TArrayI.h>
#include <TObjArray.h>
#include <ROMETree.h>
#include <ROMETreeInfo.h>
#include <ROMEStatic.h>
#include <ROME.h>
#if defined HAVE_MIDAS
#include <midas.h>
#endif
#if defined HAVE_SQL
#ifndef __CINT__
  #include <ROMESQL.h>
#endif
#endif
#include <Riostream.h>

class ROMEIO {

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

   // Modes
   Int_t      fAnalysisMode;                    //! Analysis mode flag
   Int_t      fDataFormat;                      //! Root mode flag
   Int_t      fDataBase;                        //! Data Base flag

   // Status
   Int_t      fRunStatus;                       //! Run Status flag
   Int_t      fEventStatus;                     //! Event Status flag

   // Directories
   TString    fInputDir;                        //! General Input Directory
   TString    fOutputDir;                       //! General Output Directory
   TString    fDataBaseDir;                     //! Data Base File Directory

   // Run & Event Numbers
   Int_t      fCurrentRunNumber;                //! Currently Analyzed Run Number
   TArrayI    fRunNumber;                       //! Run Numbers to Analyze
   TString    fRunNumberString;                 //! Run Numbers in Input String Format
   Int_t      fCurrentEventNumber;              //! Currently Analyzed Event Number
   TArrayI    fEventNumber;                     //! Event Numbers to Analyze
   TString    fEventNumberString;               //! Event Numbers in Input String Format

   // Event ID
   TString    fEventID;                         //! Event ID of current Event

   Int_t      fMidasFileHandle;                 //! Handle to Midas Inputfile
   int        fMidasBuffer;                     //! Midas Online Buffer
   char       fMidasEvent[1000000];             //! Midas Inputdata Stack for an Event

   TFile**    fRootFiles;                       //! Root files

   // Progress Display
   int        fProgressDelta;                   //! Maximal time difference
   time_t     fProgressLast;                    //! Last time
   int        fProgressLastEvent;               //! Last Event
   bool       fProgressWrite;                   //! Write flag

public:

   static bool fTaskSwitchesChanged;            //! Flag Task Switches Changes

protected:
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

   TString       fProgramName;                     //! Name of this Program

   // Statistics
   Statistics    fTriggerStatistics;               //! Trigger Statistics
   TString       fTriggerStatisticsString;         //! String describing Trigger Statistics
   Statistics    fScalerStatistics;                //! Scaler Statistics
   TString       fScalerStatisticsString;          //! String describing Scaler Statistics
   int           fTimeOfLastEvent;                 //! Time of last Event
   double        fLastEvent;                       //! Last Event

#if defined HAVE_MIDAS
   HNDLE         fMidasOnlineDataBase;             //! Handle to the Midas Online Data Base
#endif

#if defined HAVE_SQL
#ifndef __CINT__
   ROMESQL*      fSQL;                             //! Handle to SQL Class
#endif
#endif

public:
   ROMEIO();
   ~ROMEIO(); 

   // methods

   // inline methods
   bool Init();
   bool Connect(Int_t runNumber);
   bool ReadEvent(Int_t event);
   bool UserInput();
   bool WriteEvent();
   bool Update();
   bool Disconnect();
   bool Terminate();

   EVENT_HEADER* GetEventHeader() { return (EVENT_HEADER*)fMidasEvent; };

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

   // Tree IO
   Bool_t     isTreeAccumulation()  { return fTreeAccumulation;  };

   void       SetTreeAccumulation() { fTreeAccumulation = true;  };

   // Trees
   void       AddTree(TTree *tree) { fTreeObjects->Add(new ROMETree(tree,0,0)); };
   ROMETree*  GetTreeObjectAt(int index) { return (ROMETree*)fTreeObjects->At(index); };
   int        GetTreeObjectEntries() { return fTreeObjects->GetEntries(); };

   // Directories
   char*      GetInputDir() { return (char*)fInputDir.Data(); }
   char*      GetOutputDir() { return (char*)fOutputDir.Data(); }
   char*      GetDataBaseDir() { return (char*)fDataBaseDir.Data(); }

   void       SetInputDir(char* dir) { fInputDir = dir; }
   void       SetOutputDir(char* dir) { fOutputDir = dir; }
   void       SetDataBaseDir(char* dir) { fDataBaseDir = dir; }

   // Run Number
   void       GetRunNumberStringAt(char* buffer,int i) { sprintf(buffer,"%05d",fRunNumber.At(i)); }
   int        GetRunNumberAt(int i) { return fRunNumber.At(i); }
   void       GetCurrentRunNumberString(char* buffer) { sprintf(buffer,"%05d",fCurrentRunNumber); }
   int        GetCurrentRunNumber() { return fCurrentRunNumber; }
   int        GetNumberOfRunNumbers() { return fRunNumber.GetSize(); }
   char*      GetRunNumberStringOriginal() { return (char*)fRunNumberString.Data(); }

   void       SetRunNumbers(char* numbers) { 
                  fRunNumberString = numbers;
                  fRunNumber = ROMEStatic::decodeRunNumbers(numbers); }

   int        GetCurrentEventNumber() { return fCurrentEventNumber; }
   char*      GetEventNumberStringOriginal() { return (char*)fEventNumberString.Data(); }

   void       SetEventNumbers(char* numbers) { 
                  fEventNumberString = numbers;
                  fEventNumber = ROMEStatic::decodeRunNumbers(numbers); }

   // Event ID
   char*      GetEventID() { return (char*)fEventID.Data(); }
   void       SetEventID(char* eventID) { fEventID = eventID; }
   void       SetEventID(int eventID) { char tmp[10]; sprintf(tmp,"%d",eventID); fEventID = tmp; }

  // Processed Events
   double     GetProcessedEvents() { return fTriggerStatistics.processedEvents; }

private:
   virtual bool InitODB() = 0;

   virtual void ConnectTrees() = 0;
   virtual void FillTrees() = 0;
   virtual void ClearFolders() = 0;

   virtual bool ReadXMLDataBase() = 0;

   virtual bool InitSQLDataBase() = 0;
   virtual bool ReadSQLDataBase() = 0;

   virtual void InitMidasBanks() = 0;
};

#endif   // ROMEIO_H
