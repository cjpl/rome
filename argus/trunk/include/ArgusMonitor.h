/********************************************************************
  ArgusMonitor.h, R.Sawada

  $Log$
  Revision 1.1  2005/01/29 22:45:08  sawada
  Initial revision


********************************************************************/
#ifndef ArgusMonitor_H
#define ArgusMonitor_H
#include <TRint.h>
#include <ROMEString.h>
#include <ROMEDataBase.h>
#include <ArgusConfig.h>

class ArgusMonitor : public TObject
{
protected:
   // Application
   TRint*        fApplication;                 //! Application Handle
   
   // Directories
   ROMEString    fDataBaseDir;                 //! Data Base File Directory
   ROMEString    fConfigDir;                   //! Configuration File Directory
   
   // Data base
   ROMEDataBase* fDataBaseHandle;               //! DataBase Handle
   ROMEString    fDataBaseConnection;           //! DataBase connection string
   
   // Object Handles
   TFolder*      fMainFolder;                   //! Handle to Main Folder
   
   // Program name
   ROMEString    fProgramName;                  //! Name of this Program
   
   // Midas
   int           fMidasOnlineDataBase;          //! Handle to the Midas Online Data Base
   
   // Online
   ROMEString    fOnlineHost;                   //! Name of the Online Host
   ROMEString    fOnlineExperiment;             //! Name of the Online Experiment
   
   // NetFolder
   int           fNetFolderPortNumber;          //! Port Number for TSocket
   ROMEString    fNetFolderHost;                //! Host for TSocket
   
   // Window
   float         fWindowSize;                   //! Window size
   ROMEString    fWindowStyle;                  //! Style
   
   // Configuration
   ArgusConfig*  fConfiguration;                //! Configuration Handle
   
   void CheckLineToProcess();
   
   // virtual methods
   virtual void InitSingleFolders() = 0;
   virtual void InitArrayFolders() = 0;
   virtual void CleanUpFolders() = 0;
   virtual void ResetFolders() = 0;
   virtual bool InitODB() = 0;
   
public:
   ArgusMonitor() {};
   ArgusMonitor(TRint *app);
   ~ArgusMonitor();
   
   // Application Handle
   TRint*        GetApplication() { return fApplication; };
   
   // Data Base Handle
   char*         GetDataBaseConnection() { return (char*)fDataBaseConnection.Data(); };
   void          SetDataBaseConnection(char* connection) { fDataBaseConnection = connection; };
   void          SetDataBaseConnection(ROMEString& connection) { fDataBaseConnection = connection; };
   ROMEDataBase* GetDataBase() { return fDataBaseHandle; };
   void          SetDataBase(ROMEDataBase* dataBase) { fDataBaseHandle = dataBase; };
   
   // Directories
   char*         GetDataBaseDir() { return (char*)fDataBaseDir.Data(); }
   char*         GetConfigDir() { return (char*)fConfigDir.Data(); }
   void          SetDataBaseDir(char* dir) { fDataBaseDir = dir; }
   void          SetDataBaseDir(ROMEString& dir) { fDataBaseDir = dir; }
   void          SetConfigDir(char* dir) { fConfigDir = dir; }
   void          SetConfigDir(ROMEString& dir) { fConfigDir = dir; }
   
   // Online
   char*         GetOnlineHost() { return (char*)fOnlineHost.Data(); };
   char*         GetOnlineExperiment() { return (char*)fOnlineExperiment.Data(); };   
   void          SetOnlineHost(char* host) { fOnlineHost = host; };
   void          SetOnlineHost(ROMEString& host) { fOnlineHost = host; };
   void          SetOnlineExperiment(char* experiment) { fOnlineExperiment = experiment; };
   void          SetOnlineExperiment(ROMEString& experiment) { fOnlineExperiment = experiment; };
   
   // NetFolder
   char*         GetNetFolderHost() { return (char*)fNetFolderHost.Data(); };
   int           GetNetFolderPortNumber() { return fNetFolderPortNumber; };
   void          SetNetFolderHost(char* host) { fNetFolderHost = host; };
   void          SetNetFolderHost(ROMEString& host) { fNetFolderHost = host; };
   void          SetNetFolderPortNumber(int port) { fNetFolderPortNumber = port; };
   void          SetNetFolderPortNumber(char* port) { char* cstop; fNetFolderPortNumber = strtol(port,&cstop,10); };
   void          SetNetFolderPortNumber(ROMEString& port) { SetNetFolderPortNumber((char*)port.Data()); };
   
   // Window
   float         GetWindowSize() { return fWindowSize; };
   char*         GetWindowStyle() { return (char*)fWindowStyle.Data(); };   
   void          SetWindowSize(float size) { fWindowSize = size; };
   void          SetWindowSize(char* size) { char* cstop; fWindowSize = strtod(size,&cstop); };
   void          SetWindowSize(ROMEString& size) { SetWindowSize((char*)size.Data()); };
   void          SetWindowStyle(char* style) { fWindowStyle = style; };
   void          SetWindowStyle(ROMEString& style) { fWindowStyle = style; };
   
   // Midas
   int           GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
   int*          GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; };
   
   // Configuration
   ArgusConfig*  GetConfiguration() { return fConfiguration; };

   // Program name
   char*         GetProgramName() { return (char*)fProgramName.Data(); };
   
   static const char* LineToProcess;
   
   // main objects
   TFolder*   GetMainFolder() { return fMainFolder; }
   
   // Start Method
   bool         Start(int argc=0, char **argv=NULL);
   
   // Start Monitor
   virtual bool StartMonitor() = 0 ;   
   virtual bool ReadSingleDataBaseFolders() = 0;
   virtual bool ReadArrayDataBaseFolders() = 0;
   
   int  ss_getchar(bool reset);
   bool ss_kbhit();
   int  ss_sleep(int millisec);
   
protected:      
   bool ReadParameters(int argc, char *argv[]);
   void ParameterUsage();
   
#ifndef HAVE_MIDAS
   bool bk_is32(void *event);
   int  bk_find(void* pbkh, const char *name, unsigned long * bklen, unsigned long * bktype,void *pdata);
#endif
   
   ClassDef(ArgusMonitor,0)
};

extern ArgusMonitor *gArgus;  // global ArgusMonitor Handle
extern void *gPassToArgus;    // void ArgusMonitor Handle

#endif   // ArgusMonitor_H
