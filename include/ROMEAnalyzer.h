/********************************************************************
  ROMEAnalyzer.h, M. Schneebeli PSI

  $Log$
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
#if defined HAVE_MIDAS
#include <midas.h>
#endif
#if defined HAVE_SQL
#include <ROMESQL.h>
#endif

typedef struct {
   double processedEvents;   //! Processed Events
   double eventsPerSecond;   //! Events per Second
   double writtenEvents;     //! Written Events
} Statistics;

class ROMEAnalyzer : public TObject
{
private:
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

protected:

   // Application
   TRint*     fApplication;                     //! Application Handle

   // Modes
   Int_t      fAnalysisMode;                    //! Analysis mode flag
   ROMEString fDAQSystem;                       //! DAQ System
   Bool_t     fBatchMode;                       //! Batch mode flag
   Bool_t     fSplashScreen;                    //! Splash screen flag


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
   Int_t      fLastEventNumberIndex;            //! Index of the last Analyzed Event Number

   // Event ID
   char       fEventID;                         //! Event ID of current Event

   // Midas
   char       fRawDataEvent[2][0x80000];        //! Midas Inputdata Stack for the current Event and the last Event
   int        fCurrentRawDataEvent;             //! Index of the current event buffer
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
   ROMEDataBase *fDataBaseHandle;                  //! DataBase Handle
   ROMEString    fDataBaseConnection;              //! DataBase connection string

   // Configuration
   ROMEConfig   *fConfiguration;                   //! Configuration Handle

public:
   static const char* LineToProcess;

public:
   ROMEAnalyzer() {};
   ROMEAnalyzer(TRint *app);
   ~ROMEAnalyzer();

   // Application Handle
   TRint*        GetApplication() { return fApplication; };

   // Data Base Handle
   char*         GetDataBaseConnection() { return (char*)fDataBaseConnection.Data(); };
   void          SetDataBaseConnection(char* connection) { fDataBaseConnection = connection; };
   void          SetDataBaseConnection(ROMEString& connection) { fDataBaseConnection = connection; };
   ROMEDataBase* GetDataBase() { return fDataBaseHandle; };
   void          SetDataBase(ROMEDataBase* dataBase) { fDataBaseHandle = dataBase; };

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

   // Data Format
   ROMEString &GetDAQ() { return fDAQSystem; };

   void       SetDAQ(ROMEString daqSystem)  { daqSystem.ToLower(); fDAQSystem = daqSystem; };

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
   Bool_t     isTerminationFlag() { return fTerminate; };
   void       SetTerminationFlag() { fTerminate = true; };


   // Event Read Flag
   bool       IsDontReadNextEvent() { return fDontReadNextEvent; };

   void       SetDontReadNextEvent(bool flag = true) { fDontReadNextEvent = flag; };

   // Tree IO
   Bool_t     isTreeAccumulation()  { return fTreeAccumulation;  };

   void       SetTreeAccumulation(bool flag = true) { fTreeAccumulation = flag;  };

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

   void       SetCurrentRunNumber(int runNumber) { fCurrentRunNumber = runNumber; }
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

   int        CheckEventNumber(int eventNumber);

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

   char*      GetProgramName() { return (char*)fProgramName.Data(); };

   // Online
   char*      GetOnlineHost() { return (char*)fOnlineHost.Data(); };
   char*      GetOnlineExperiment() { return (char*)fOnlineExperiment.Data(); };

   void       SetOnlineHost(char* host) { fOnlineHost = host; };
   void       SetOnlineExperiment(char* experiment) { fOnlineExperiment = experiment; };

   // Socket
   int        GetPortNumber() { return fPortNumber; };
   bool       isSocketOffline() { return fSocketOffline; };

   void       SetPortNumber(int portNumber) { fPortNumber = portNumber; };
   void       SetPortNumber(char* portNumber) { char* cstop; fPortNumber = strtol(portNumber,&cstop,10); };
   void       SetSocketOffline(bool flag=true) { fSocketOffline = flag; };

   // Midas ODB
   int        GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
   int*       GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; };

   // Raw Data
   void*      GetRawDataEvent() { return fRawDataEvent[fCurrentRawDataEvent]; };
   void*      GetLastRawDataEvent() { return fRawDataEvent[1-fCurrentRawDataEvent]; };
   int        GetRawDataEventSize() { return sizeof(fRawDataEvent[fCurrentRawDataEvent]); };
   void       SwitchRawDataBuffer() { fCurrentRawDataEvent = 1-fCurrentRawDataEvent; };

   // Configuration
   ROMEConfig *GetConfiguration() { return fConfiguration; };

   // Statistics
   Statistics* GetTriggerStatistics() { return &fTriggerStatistics; };
   Statistics* GetScalerStatistics() { return &fScalerStatistics; };

   // Start Method
   bool       Start(int argc=0, char **argv=NULL);

   static TArrayI decodeRunNumbers(ROMEString& str);


   virtual bool ReadSingleDataBaseFolders() = 0;
   virtual bool ReadArrayDataBaseFolders() = 0;
   virtual void InitMidasBanks() = 0;

   int ss_getchar(bool reset);
   Bool_t ss_kbhit();
   int ss_sleep(int millisec);
   long ss_millitime();

protected:

   bool CreateHistoFolders(TList *,TFolder *);

   bool ReadParameters(int argc, char *argv[]);
   void ParameterUsage();

   virtual void startSplashScreen() = 0;
   virtual void consoleStartScreen() = 0;
   virtual void redirectOutput() = 0;

#ifndef HAVE_MIDAS
   bool bk_is32(void *event);
   int bk_find(void* pbkh, const char *name, unsigned long * bklen, unsigned long * bktype,void *pdata);
#endif

   ClassDef(ROMEAnalyzer,0)
};

extern ROMEAnalyzer *gROME;  // global ROMEAnalyzer Handle
extern void *gPassToROME;  // void ROMEAnalyzer Handle

#endif   // ROMEAnalyzer_H
