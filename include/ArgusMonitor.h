/********************************************************************
  ArgusMonitor.h, R.Sawada

  $Id$

********************************************************************/
//#define USE_TRINT

#ifndef ArgusMonitor_H
#define ArgusMonitor_H
#include <ROMEAnalyzer.h>
#include <ROMEConfig.h>
#include <TNetFolder.h>

class ArgusMonitor : public ROMEAnalyzer
{
protected:
   TApplication*    fApplication;                  //! Application Handle

   ROMETreeInfo*    fTreeInfo;                     //! Tree Info Object

   Int_t            fNumberOfNetFolders;           //! Number of net folders
   // Window
   Float_t          fWindowScale;                  //! Window scale
   
   // NetFolder
   TNetFolder**     fNetFolder;                    //! netfoldr handle
   Bool_t*          fNetFolderActive;              //! active flag
   Bool_t*          fNetFolderReconnect;           //! reconnect flag
   TSocket**        fNetFolderSocket;              //! socket connection handle
   Int_t*           fNetFolderPort;                //! port number
   ROMEString*      fNetFolderName;                //! name
   ROMEString*      fNetFolderHost;                //! server host name
   ROMEString*      fNetFolderRoot;                //! root directory name
   
   // virtual methods
   virtual Bool_t   StartWindow() = 0;
   
public:
   ArgusMonitor(){};
   ArgusMonitor(TApplication *app);
   ~ArgusMonitor();

   void            startSplashScreen(){;}
   void            consoleStartScreen(){;}
   void            redirectOutput(){;}
   TApplication*   GetApplication() { return fApplication; };

   // NetFolder
   Bool_t          IsNetFolderActive(const Char_t *name);
   TNetFolder*     GetNetFolder(const Char_t* name);
   Char_t*         GetNetFolderName(Int_t i) { return (Char_t*)fNetFolderName[i].Data(); }
   Char_t*         GetNetFolderHost(Int_t i) { return (Char_t*)fNetFolderHost[i].Data(); }
   Int_t           GetNetFolderPort(Int_t i) { return fNetFolderPort[i]; }
   Char_t*         GetNetFolderRoot(Int_t i) { return (Char_t*)fNetFolderRoot[i].Data(); }
   Bool_t          GetNetFolderActive(Int_t i) { return fNetFolderActive[i]; }
   Bool_t          GetNetFolderReconnect(Int_t i) { return fNetFolderReconnect[i]; }
   void            SetNetFolderName(Int_t i,const Char_t* name) { fNetFolderName[i] = name; }
   void            SetNetFolderName(Int_t i,ROMEString& name) { fNetFolderName[i] = name; }
   void            SetNetFolderHost(Int_t i,const Char_t* host) { fNetFolderHost[i] = host; }
   void            SetNetFolderHost(Int_t i,ROMEString& host) { fNetFolderHost[i] = host; }
   void            SetNetFolderPort(Int_t i,Int_t port) { fNetFolderPort[i] = port; }
   void            SetNetFolderPort(Int_t i,const Char_t* port) { Char_t* cstop; fNetFolderPort[i] = strtol(port,&cstop,10); }
   void            SetNetFolderPort(Int_t i,ROMEString& port) { SetNetFolderPort(i,(Char_t*)port.Data()); }
   void            SetNetFolderRoot(Int_t i,const Char_t* root) { fNetFolderRoot[i] = root; }
   void            SetNetFolderRoot(Int_t i,ROMEString& root) { fNetFolderRoot[i] = root; }
   void            SetNetFolderActive(Int_t i,Bool_t active) { fNetFolderActive[i] = active; }
   void            SetNetFolderReconnect(Int_t i,Bool_t reconnect) { fNetFolderReconnect[i] = reconnect; }
   Int_t           GetNumberOfNetFolders() { return fNumberOfNetFolders; }

   virtual void InitSingleFolders() = 0;
   virtual void InitArrayFolders() = 0;
   virtual void CleanUpFolders() = 0;
   virtual void ResetFolders() = 0;

   Bool_t DAQInit();
   Bool_t DAQBeginOfRun();
   Bool_t DAQEvent(Int_t event);
   Bool_t DAQEndOfRun();
   Bool_t DAQTerminate();

   // Window
   Float_t         GetWindowScale() { return fWindowScale; }
   void            SetWindowScale(Float_t scale) { fWindowScale = scale; }
   void            SetWindowScale(Char_t* scale) { Char_t* cstop; fWindowScale = (Float_t)strtod(scale,&cstop); }
   void            SetWindowScale(ROMEString& scale) { SetWindowScale((Char_t*)scale.Data()); }
   
   // Start Method
   Bool_t          Start(int argc=0, char **argv=NULL);
   
   // Start Monitor
   Bool_t          StartMonitor();

   // NetFolder connection
   Bool_t          ConnectNetFolder(const Char_t* name);
   Bool_t          ConnectNetFolder(Int_t i);
   Bool_t          DisconnectNetFolder(const Char_t* name);
   Bool_t          DisconnectNetFolder(Int_t i);
   Bool_t          ConnectNetFolders();
   Bool_t          DisconnectNetFolders();
   void            InitNetFolders(Int_t number);
   
protected:      
   Bool_t          ReadParameters(Int_t argc, Char_t *argv[]);
   virtual Bool_t  ReadUserParameter(const Char_t* opt, const Char_t* value, Int_t& i) { return false; }
   void            ParameterUsage();
   virtual void    UserParameterUsage(){}
   virtual void    InitTrees(){}
   
   ClassDef(ArgusMonitor,0)
};

extern ArgusMonitor *gArgus;  // global ArgusMonitor Handle
extern void *gPassToArgus;    // void ArgusMonitor Handle

#endif   // ArgusMonitor_H
