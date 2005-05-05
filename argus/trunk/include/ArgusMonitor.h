/********************************************************************
  ArgusMonitor.h, R.Sawada

  $Log$
  Revision 1.9  2005/05/05 20:08:05  sawada
  code clean up.

  Revision 1.8  2005/04/12 17:51:04  sawada
  changes for mxml

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
   Int_t         fSocketInterfacePortNumber;    //! Port Number for TSocket
   ROMEString    fSocketInterfaceHost;          //! Host for TSocket
   Int_t         fRunNumber;                    //! Run number for ROMEDataBase

   // Window
   Float_t       fWindowScale;                  //! Window scale
   
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
   Char_t*       GetDataBaseConnection() { return (Char_t*)fDataBaseConnection.Data(); };
   void          SetDataBaseConnection(Char_t* connection) { fDataBaseConnection = connection; };
   void          SetDataBaseConnection(ROMEString& connection) { fDataBaseConnection = connection; };
   ROMEDataBase* GetDataBase() { return fDataBaseHandle; };
   void          SetDataBase(ROMEDataBase* dataBase) { fDataBaseHandle = dataBase; };
   
   // Directories
   Char_t*       GetDataBaseDir() { return (Char_t*)fDataBaseDir.Data(); }
   Char_t*       GetConfigDir() { return (Char_t*)fConfigDir.Data(); }
   void          SetDataBaseDir(Char_t* dir) { fDataBaseDir = dir; }
   void          SetDataBaseDir(ROMEString& dir) { fDataBaseDir = dir; }
   void          SetConfigDir(Char_t* dir) { fConfigDir = dir; }
   void          SetConfigDir(ROMEString& dir) { fConfigDir = dir; }
   
   // Online
   Char_t*       GetOnlineHost() { return (Char_t*)fOnlineHost.Data(); };
   Char_t*       GetOnlineExperiment() { return (Char_t*)fOnlineExperiment.Data(); };   
   void          SetOnlineHost(Char_t* host) { fOnlineHost = host; };
   void          SetOnlineHost(ROMEString& host) { fOnlineHost = host; };
   void          SetOnlineExperiment(Char_t* experiment) { fOnlineExperiment = experiment; };
   void          SetOnlineExperiment(ROMEString& experiment) { fOnlineExperiment = experiment; };
   
   // SocketInterface
   Char_t*       GetSocketInterfaceHost() { return (Char_t*)fSocketInterfaceHost.Data(); };
   Int_t         GetSocketInterfacePortNumber() { return fSocketInterfacePortNumber; };
   void          SetSocketInterfaceHost(Char_t* host) { fSocketInterfaceHost = host; };
   void          SetSocketInterfaceHost(ROMEString& host) { fSocketInterfaceHost = host; };
   void          SetSocketInterfacePortNumber(Int_t port) { fSocketInterfacePortNumber = port; };
   void          SetSocketInterfacePortNumber(Char_t* port) { Char_t* cstop; fSocketInterfacePortNumber = strtol(port,&cstop,10); };
   void          SetSocketInterfacePortNumber(ROMEString& port) { SetSocketInterfacePortNumber((Char_t*)port.Data()); };
   
   // Window
   Float_t       GetWindowScale() { return fWindowScale; };
   void          SetWindowScale(Float_t scale) { fWindowScale = scale; };
   void          SetWindowScale(Char_t* scale) { Char_t* cstop; fWindowScale = strtod(scale,&cstop); };
   void          SetWindowScale(ROMEString& scale) { SetWindowScale((Char_t*)scale.Data()); };
   
   // Midas
   Int_t         GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
   Int_t*        GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; };
   
   // Configuration
   ArgusConfig*  GetConfiguration() { return fConfiguration; };

   // Program name
   Char_t*       GetProgramName() { return (Char_t*)fProgramName.Data(); };
   
   // main objects
   TFolder*      GetMainFolder() { return fMainFolder; }
   
   // Start Method
   Bool_t          Start(Int_t argc=0, Char_t **argv=NULL);
   
   // Start Monitor
   virtual Bool_t  StartMonitor() = 0 ;   
   virtual Bool_t  ReadSingleDataBaseFolders() = 0;
   virtual Bool_t  ReadArrayDataBaseFolders() = 0;

   Bool_t strtobool(const Char_t* str);
   Bool_t toBool(Int_t value){ return value!=0; }
   
protected:      
   Bool_t ReadParameters(Int_t argc, Char_t *argv[]);
   void ParameterUsage();
   
   ClassDef(ArgusMonitor,0)
};

extern ArgusMonitor *gArgus;  // global ArgusMonitor Handle
extern void *gPassToArgus;    // void ArgusMonitor Handle

#endif   // ArgusMonitor_H
