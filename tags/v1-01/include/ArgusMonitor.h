/********************************************************************
  ArgusMonitor.h, R.Sawada

  $Log$
  Revision 1.7  2005/02/24 15:04:03  sawada
  Reduced number of configuration to 1.
  Replaced ss_getchar to getchar().
  Removed LineToProcess.
  Removed bk_is32,bk_find.
  Improved help.
  Handling of midas host and experiment.

  Revision 1.6  2005/02/21 23:07:50  sawada
  several UNIX support

  Revision 1.5  2005/02/06 00:39:35  sawada
   Changed TRint to TApplication
   Stop method of thread function
   Name of method for start/stop

  Revision 1.4  2005/02/02 23:54:30  sawada
  link with midas library.

  Revision 1.3  2005/02/02 18:58:02  sawada
  small change.

  Revision 1.2  2005/02/01 14:44:33  sawada
  Resize frames in window.

  Revision 1.1.1.1  2005/01/29 22:45:08  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
//#define USE_TRINT

#ifndef ArgusMonitor_H
#define ArgusMonitor_H
#if defined ( USE_TRINT )
#include <TRint.h>
#else
#include <TApplication.h>
#endif
#include <ROMEString.h>
#include <ROMEDataBase.h>
#include <ArgusConfig.h>
#if defined( HAVE_MIDAS )
#include <midas.h>
#endif

class ArgusMonitor : public TObject
{
protected:
   // Application
#if defined( USE_TRINT )
   TRint*        fApplication;                 //! Application Handle
#else
   TApplication* fApplication;                 //! Application Handle
#endif
   
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
   
   // SocketInterface
   int           fSocketInterfacePortNumber;    //! Port Number for TSocket
   ROMEString    fSocketInterfaceHost;          //! Host for TSocket
   int           fRunNumber;                    //! Run number for ROMEDataBase

   // Window
   float         fWindowScale;                  //! Window scale
   
   // Configuration
   ArgusConfig*  fConfiguration;                //! Configuration Handle
   
   // virtual methods
   virtual void InitSingleFolders() = 0;
   virtual void InitArrayFolders() = 0;
   virtual void CleanUpFolders() = 0;
   virtual void ResetFolders() = 0;
   
public:
   ArgusMonitor() {};
#if defined( USE_TRINT )
   ArgusMonitor(TRint *app);
#else
   ArgusMonitor(TApplication *app);
#endif
   ~ArgusMonitor();
   
   // Application Handle
#if defined( USE_TRINT )
   TRint* GetApplication() { return fApplication; };
#else
   TApplication* GetApplication() { return fApplication; };
#endif
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
   
   // SocketInterface
   char*         GetSocketInterfaceHost() { return (char*)fSocketInterfaceHost.Data(); };
   int           GetSocketInterfacePortNumber() { return fSocketInterfacePortNumber; };
   void          SetSocketInterfaceHost(char* host) { fSocketInterfaceHost = host; };
   void          SetSocketInterfaceHost(ROMEString& host) { fSocketInterfaceHost = host; };
   void          SetSocketInterfacePortNumber(int port) { fSocketInterfacePortNumber = port; };
   void          SetSocketInterfacePortNumber(char* port) { char* cstop; fSocketInterfacePortNumber = strtol(port,&cstop,10); };
   void          SetSocketInterfacePortNumber(ROMEString& port) { SetSocketInterfacePortNumber((char*)port.Data()); };
   
   // Window
   float         GetWindowScale() { return fWindowScale; };
   void          SetWindowScale(float scale) { fWindowScale = scale; };
   void          SetWindowScale(char* scale) { char* cstop; fWindowScale = strtod(scale,&cstop); };
   void          SetWindowScale(ROMEString& scale) { SetWindowScale((char*)scale.Data()); };
   
   // Midas
   int           GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
   int*          GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; };
   
   // Configuration
   ArgusConfig*  GetConfiguration() { return fConfiguration; };

   // Program name
   char*         GetProgramName() { return (char*)fProgramName.Data(); };
   
   // main objects
   TFolder*      GetMainFolder() { return fMainFolder; }
   
   // Start Method
   bool          Start(int argc=0, char **argv=NULL);
   
   // Start Monitor
   virtual bool  StartMonitor() = 0 ;   
   virtual bool  ReadSingleDataBaseFolders() = 0;
   virtual bool  ReadArrayDataBaseFolders() = 0;
   
protected:      
   bool ReadParameters(int argc, char *argv[]);
   void ParameterUsage();
   
   ClassDef(ArgusMonitor,0)
};

extern ArgusMonitor *gArgus;  // global ArgusMonitor Handle
extern void *gPassToArgus;    // void ArgusMonitor Handle

#endif   // ArgusMonitor_H
