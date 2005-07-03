/********************************************************************
  ArgusMonitor.h, R.Sawada

  $Log$
  Revision 1.12  2005/05/26 14:26:55  sawada
  Lots of changes.
  Made ArgusBuilder an inheriting class of ROMEBuilder.
  Remove ROMEFolder and added NetFolers.
  Added ArgusWindow class.
  and so on.

  Revision 1.11  2005/05/06 14:37:41  schneebeli_m
  small change

  Revision 1.10  2005/05/05 20:13:30  sawada
  code clean up.

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
#include <TSocket.h>
#include <TNetFolder.h>

class ArgusMonitor : public TObject
{
protected:
   // Application
#if defined( USE_TRINT )
   TRint*           fApplication;                 //! Application Handle
#else
   TApplication*    fApplication;                 //! Application Handle
#endif
   
   // Directories
   ROMEString*      fDataBaseDir;                 //! Data Base File Directory
   ROMEString       fConfigDir;                   //! Configuration File Directory
   
   // Data base
   ROMEDataBase**   fDataBaseHandle;               //! DataBase Handle
   ROMEString*      fDataBaseConnection;           //! DataBase connection string
   Int_t            fNumberOfDataBases;            //! Number of DataBases available

   // Object Handles
   TFolder*         fMainFolder;                   //! Handle to Main Folder
   
   // Program name
   ROMEString       fProgramName;                  //! Name of this Program
   
   // Midas
   int              fMidasOnlineDataBase;          //! Handle to the Midas Online Data Base
   
   // Online
   ROMEString       fOnlineHost;                   //! Name of the Online Host
   ROMEString       fOnlineExperiment;             //! Name of the Online Experiment
   
   Int_t            fNumberOfNetFolders;           //! Number of net folders
   Int_t            fRunNumber;                    //! Run number for ROMEDataBase

   // Window
   Float_t          fWindowScale;                  //! Window scale
   
   // NetFolder
   TNetFolder**     fNetFolder;                    //! netfoldr handle
   Bool_t*          fNetFolderActive;              //! active flag
   TSocket**        fNetFolderSocket;              //! socket connection handle
   Int_t*           fNetFolderPort;                //! port number
   ROMEString*      fNetFolderName;                //! name
   ROMEString*      fNetFolderTitle;               //! title
   ROMEString*      fNetFolderHost;                //! server host name
   ROMEString*      fNetFolderRoot;                //! root directory name

   // Configuration
   ArgusConfig*     fConfiguration;                //! Configuration Handle
   
   // virtual methods
   virtual Bool_t   StartWindow() = 0;
   virtual void     InitSingleFolders() = 0;
   virtual void     InitArrayFolders() = 0;
   virtual void     CleanUpFolders() = 0;
   virtual void     ResetFolders() = 0;
   
public:
   ArgusMonitor() {}
#if defined( USE_TRINT )
   ArgusMonitor(TRint *app);
#else
   ArgusMonitor(TApplication *app);
#endif
   ~ArgusMonitor();

   // Output
   void            Print(Char_t text);
   void            Print(const Char_t* text="");
   void            Println(const Char_t* text="");
   void            Printfl(const Char_t* text="");

   
   // Application Handle
#if defined( USE_TRINT )
   TRint*          GetApplication() { return fApplication; }
#else
   TApplication*   GetApplication() { return fApplication; }
#endif
   Int_t           GetCurrentRunNumber() { return fRunNumber; }
   void            SetCurrentRunNumber(Int_t run) { fRunNumber = run; }

   // Data Base Handle
   const Char_t*   GetDataBaseConnection(Int_t i) { return fDataBaseConnection[i].Data(); }
   void            SetDataBaseConnection(Int_t i,const Char_t* connection) { fDataBaseConnection[i] = connection; }
   ROMEDataBase*   GetDataBase(Int_t i);
   ROMEDataBase*   GetDataBase(const Char_t *name);
   Bool_t          isDataBaseActive(const Char_t *name);
   void            SetDataBase(Int_t i,ROMEDataBase* dataBase) { fDataBaseHandle[i] = dataBase; }
   Int_t           GetNumberOfDataBases() { return fNumberOfDataBases; }
   void            InitDataBases(Int_t number);
   
   // Directories
   Char_t*         GetDataBaseDir(Int_t i) { return (Char_t*) fDataBaseDir[i].Data(); }
   void            SetDataBaseDir(Int_t i,const Char_t* dir) { fDataBaseDir[i] = dir; }
   void            SetDataBaseDir(Int_t i,ROMEString& dir) { fDataBaseDir[i] = dir; }
   Char_t*         GetConfigDir() { return (Char_t*)fConfigDir.Data(); }
   void            SetConfigDir(Char_t* dir) { fConfigDir = dir; }
   void            SetConfigDir(ROMEString& dir) { fConfigDir = dir; }
   
   // Online
   Char_t*         GetOnlineHost() { return (Char_t*)fOnlineHost.Data(); }
   Char_t*         GetOnlineExperiment() { return (Char_t*)fOnlineExperiment.Data(); }
   void            SetOnlineHost(Char_t* host) { fOnlineHost = host; }
   void            SetOnlineHost(ROMEString& host) { fOnlineHost = host; }
   void            SetOnlineExperiment(Char_t* experiment) { fOnlineExperiment = experiment; }
   void            SetOnlineExperiment(ROMEString& experiment) { fOnlineExperiment = experiment; }
   
   // NetFolder
   TNetFolder*     GetNetFolder(const Char_t* name);
   Char_t*         GetNetFolderName(Int_t i) { return (Char_t*)fNetFolderName[i].Data(); }
   Char_t*         GetNetFolderTitle(Int_t i) { return (Char_t*)fNetFolderTitle[i].Data(); }
   Char_t*         GetNetFolderHost(Int_t i) { return (Char_t*)fNetFolderHost[i].Data(); }
   Int_t           GetNetFolderPort(Int_t i) { return fNetFolderPort[i]; }
   Char_t*         GetNetFolderRoot(Int_t i) { return (Char_t*)fNetFolderRoot[i].Data(); }
   Bool_t          GetNetFolderActive(Int_t i) { return fNetFolderActive[i]; }
   void            SetNetFolderName(Int_t i,const Char_t* name) { fNetFolderName[i] = name; }
   void            SetNetFolderName(Int_t i,ROMEString& name) { fNetFolderName[i] = name; }
   void            SetNetFolderTitle(Int_t i,const Char_t* title) { fNetFolderTitle[i] = title; }
   void            SetNetFolderTitle(Int_t i,ROMEString& title) { fNetFolderTitle[i] = title; }
   void            SetNetFolderHost(Int_t i,const Char_t* host) { fNetFolderHost[i] = host; }
   void            SetNetFolderHost(Int_t i,ROMEString& host) { fNetFolderHost[i] = host; }
   void            SetNetFolderPort(Int_t i,Int_t port) { fNetFolderPort[i] = port; }
   void            SetNetFolderPort(Int_t i,const Char_t* port) { Char_t* cstop; fNetFolderPort[i] = strtol(port,&cstop,10); }
   void            SetNetFolderPort(Int_t i,ROMEString& port) { SetNetFolderPort(i,(Char_t*)port.Data()); }
   void            SetNetFolderRoot(Int_t i,const Char_t* root) { fNetFolderRoot[i] = root; }
   void            SetNetFolderRoot(Int_t i,ROMEString& root) { fNetFolderRoot[i] = root; }
   void            SetNetFolderActive(Int_t i,Bool_t active) { fNetFolderActive[i] = active; }
   Int_t           GetNumberOfNetFolders() { return fNumberOfNetFolders; }

   // Window
   Float_t         GetWindowScale() { return fWindowScale; }
   void            SetWindowScale(Float_t scale) { fWindowScale = scale; }
   void            SetWindowScale(Char_t* scale) { Char_t* cstop; fWindowScale = (Float_t)strtod(scale,&cstop); }
   void            SetWindowScale(ROMEString& scale) { SetWindowScale((Char_t*)scale.Data()); }
   
   // Midas
   Int_t           GetMidasOnlineDataBase() { return fMidasOnlineDataBase; }
   Int_t*          GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; }
   
   // Configuration
   ArgusConfig*    GetConfiguration() { return fConfiguration; }

   // Program name
   Char_t*         GetProgramName() { return (Char_t*)fProgramName.Data(); }
   
   // main objects
   TFolder*        GetMainFolder() { return fMainFolder; }
   
   // Start Method
   Bool_t          Start(int argc=0, char **argv=NULL);
   
   // Start Monitor
   Bool_t          StartMonitor();
   virtual Bool_t  ReadSingleDataBaseFolders() = 0;
   virtual Bool_t  ReadArrayDataBaseFolders() = 0;

   // NetFolder connection
   Bool_t          ConnectNetFolder(const Char_t* name);
   Bool_t          ConnectNetFolder(Int_t i);
   Bool_t          DisconnectNetFolder(const Char_t* name);
   Bool_t          DisconnectNetFolder(Int_t i);
   Bool_t          ConnectNetFolders();
   Bool_t          DisconnectNetFolders();
   void            InitNetFolders(Int_t number);

   Int_t           stricmp(const Char_t*,const Char_t*);
   Bool_t          strtobool(const Char_t* str);
   Bool_t          toBool(Int_t value){ return value!=0; }
   
protected:      
   Bool_t          ReadParameters(Int_t argc, Char_t *argv[]);
   virtual Bool_t  ReadUserParameter(const Char_t* opt, const Char_t* value, Int_t& i) { return false; }
   void            ParameterUsage();
   virtual void    UserParameterUsage(){}
   
   ClassDef(ArgusMonitor,0)
};

extern ArgusMonitor *gArgus;  // global ArgusMonitor Handle
extern void *gPassToArgus;    // void ArgusMonitor Handle

#endif   // ArgusMonitor_H
