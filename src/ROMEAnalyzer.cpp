// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEAnalyzer                                                         //
//                                                                      //
//  Basic class in the Data Analysis Tool. This class is the base-class
//  for the analysis dependent main class, which should implement the 
//  Containers, Trees and Task definitions.
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#if defined( _MSC_VER )
#include <io.h>
#include <direct.h>
#endif
#if defined ( __linux__ )
#include <unistd.h>
#include <stdlib.h>
#include <sys/io.h>
#include <TThread.h>
#endif


#include <libxml/xmlreader.h>
#include <fcntl.h>
#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <THtml.h>
#include <TTask.h>
#include <TTree.h>
#include <TFolder.h>
#include <TList.h>
#include <TObjString.h>
#include <TROOT.h>
#include "ROMEAnalyzer.h"
#include "ROMEEventLoop.h"
#include "ROMETree.h"
#include "ROMETask.h"
#include "ROMEStatic.h"
#include <Riostream.h>

ClassImp(ROMEAnalyzer)

ROMEAnalyzer::ROMEAnalyzer()
{
// Folder,Task and Tree initialisation

   int i=0;
   fBatchMode = false;
   fRunStatus = false;

   fTreeObjects = new TObjArray(0);
}

bool ROMEAnalyzer::Start(int argc, char **argv) 
{
// Starts the Data Analysis Tool

   consoleStartScreen();

   if (!ReadParameters(argc,argv)) return false;

   startSplashScreen();

   CreateHistoFolders();

#if defined ( __linux__ )
   /* start socket server */
   TThread *th1 = new TThread("root_server_loop", root_server_loop, NULL);
   th1->Run();
#endif

   fMainTask->ExecuteTask();

   return true;
}

void ROMEAnalyzer::CreateHistoFolders() 
{
   // Creates a Folder for each active Task
   int i,j;
   char name[1000];
   char title[1000];
   TFolder *fHistoFolder = fMainFolder->AddFolder("histos","Histogram Folder");
   TList *taskList = fMainTask->GetListOfTasks();
   for (i=0;i<taskList->GetSize();i++) {
      TTask *task = (TTask*)taskList->At(i);
      if (!task->IsActive()) continue;
      sprintf(name,"%sHistos",task->GetName());
      sprintf(title,"Histograms of Task '%s'",task->GetName());
      TFolder *folder = fHistoFolder->AddFolder(name,title);
      TList *subTaskList = task->GetListOfTasks();
      for (j=0;j<subTaskList->GetSize();j++) {
         TTask *task = (TTask*)subTaskList->At(j);
         if (!task->IsActive()) continue;
         sprintf(name,"%sHistos",task->GetName());
         sprintf(title,"Histograms of Task '%s'",task->GetName());
         folder->AddFolder(name,title);
      }
   }
}

bool ROMEAnalyzer::ReadParameters(int argc, char *argv[]) 
{
   // Reads the Inputlineparameters
   int i,j,k;
   char str[1000];
   const int workDirLen = 1000;
   char workDir[workDirLen];
   getcwd(workDir,workDirLen);

   sprintf(str,"%s/",workDir);
   SetConfigDir(str);
   SetDataBaseDir(str);
   SetInputDir(str);
   sprintf(str,"%s/%s",workDir,"Output/");
   SetOutputDir(str);

   if (argc==1) {
      cout << "  -c       Configuration-File path" << endl;
      cout << "  -d       The Inputparameters are read from the config.xml file (Argument path to the config.xml file)" << endl;
      cout << "  -i       Inputfile path" << endl;
      cout << "  -o       Outputfile path" << endl;
      cout << "  -b       Batch Mode (no Argument)" << endl;
      cout << "  -m       Analysing Mode : (online/[offline])" << endl;
      cout << "  -df      Input Data Format : ([midas]/root)" << endl;
      cout << "  -r       Runnumbers" << endl;
      cout << "  -e       Eventnumbers" << endl;
      cout << "  -ta      Taskselection" << endl;
      cout << "             Set tasks activate/inactivate in the order of their" << endl;
      cout << "             appearance in the list of tasks." << endl;
      cout << "             e.g. -ta 1,0,1,0,0,..." << endl;
      cout << "  -th      Histoselection" << endl;
      cout << "             Set Histogram Accumulationflag for each task in" << endl;
      cout << "             the order of their appearance in the list of tasks." << endl;
      cout << "             e.g. -th 1,0,1,0,0,..." << endl;
      cout << "  -tasks   Displays a List of all Tasks (no Argument)" << endl;
      cout << "  -tr      Treeselection" << endl;
      cout << "             Set read-, write-, accumulation and fillstate of the" << endl;
      cout << "             trees in the order of their appearance in the list of" << endl;
      cout << "             trees. [r,w,f,a;r,w,f,a;...]" << endl;
      cout << "             e.g. -tr 1,0,1,0;1,1,1,1;..." << endl;
      cout << "  -trees   Displays a List of all Trees (no Argument)" << endl;
      cout << "  -docu    Generates a Root-Html-Documentation (no Argument)" << endl;
      return false;
   }
   if (!strcmp(argv[1],"-tasks")) {
      TList *taskList = fMainTask->GetListOfTasks();
      for (i=0;i<taskList->GetSize();i++) {
         TTask *task = (TTask*)taskList->At(i);
         cout << "   " << task->GetName() << endl;
         TList *subTaskList = task->GetListOfTasks();
         for (j=0;j<subTaskList->GetSize();j++) {
            TTask *task = (TTask*)subTaskList->At(j);
            cout << "      " << task->GetName() << endl;
         }
      }
      return false;
   }
   if (!strcmp(argv[1],"-trees")) {
      TTree *tree;
      for (i=0;i<fTreeObjects->GetEntries();i++) {
         tree = this->GetTreeObjectAt(i)->GetTree();
         cout << "   " << tree->GetName() << endl;
      }
      return false;
   }
   char arg[100][200];
   if (!strcmp(argv[1],"-d")) {
      if (argc>2) {
         sprintf(str,argv[2]);
         if (argv[2][0]!='/'&&strlen(argv[2])>1&&argv[2][1]!=':') {
            sprintf(str,"%s/%s",workDir,argv[2]);
         }
         SetConfigDir(str);
      }
      argc = ReadROMEConfigXML(arg);
      if (argc==-1) return false;
   }
   else {
      for (i=0;i<argc;i++) strcpy(arg[i],argv[i]);
   }

   for (i=1;i<argc;i++) {
      if (!strcmp(arg[i],"-docu")) {
         THtml html;
         html.MakeAll(true);
         return false;
      }
      if (!strcmp(arg[i],"-b")) {
         fBatchMode = true;
      }
      if (!strcmp(arg[i],"-m")) {
         if (!strcmp(arg[i+1],"online")) fOnLineMode = true;
         else fOnLineMode = false;
         i++;
      }
      if (!strcmp(arg[i],"-df")) {
         if (!strcmp(arg[i+1],"root")) fRootMode = true;
         else fRootMode = false;
         i++;
      }
      if (!strcmp(arg[i],"-c")&&i<argc-1) {
         sprintf(str,arg[i+1]);
         if (arg[i+1][0]!='/'&&strlen(arg[i+1])>1&&arg[i+1][1]!=':') {
            sprintf(str,"%s/%s",workDir,arg[i+1]);
         }
         strcat(str,"/");
         SetConfigDir(str);
         i++;
      }
      if (!strcmp(arg[i],"-db")&&i<argc-1) {
         sprintf(str,arg[i+1]);
         if (arg[i+1][0]!='/'&&strlen(arg[i+1])>1&&arg[i+1][1]!=':') {
            sprintf(str,"%s/%s",workDir,arg[i+1]);
         }
         strcat(str,"/");
         SetDataBaseDir(str);
         i++;
      }
      if (!strcmp(arg[i],"-i")&&i<argc-1) {
         sprintf(str,arg[i+1]);
         if (arg[i+1][0]!='/'&&strlen(arg[i+1])>1&&arg[i+1][1]!=':') {
            sprintf(str,"%s/%s",workDir,arg[i+1]);
         }
         strcat(str,"/");
         SetInputDir(str);
         i++;
      }
      if (!strcmp(arg[i],"-o")&&i<argc-1) {
         sprintf(str,arg[i+1]);
         if (arg[i+1][0]!='/'&&strlen(arg[i+1])>1&&arg[i+1][1]!=':') {
            sprintf(str,"%s/%s",workDir,arg[i+1]);
         }
         strcat(str,"/");
         SetOutputDir(str);
         i++;
      }
      if (!strcmp(arg[i],"-r")&&i<argc-1) {
         fRunNumber = ROMEStatic::decodeRunNumbers(arg[i+1]);
         i++;
      }
      if (!strcmp(arg[i],"-e")&&i<argc-1) {
         fEventNumber = ROMEStatic::decodeRunNumbers(arg[i+1]);
         i++;
      }
      if (!strcmp(arg[i],"-ta")&&i<argc-1) {
         int ntask=0;
         TArrayI flags = ROMEStatic::decodeRunNumbers(arg[i+1]);
         int nflags = flags.GetSize();
         TList *taskList = fMainTask->GetListOfTasks();
         for (j=0;j<taskList->GetSize();j++) {
            TTask *task = (TTask*)taskList->At(j);
            if (!flags.At(ntask)) task->SetActive(false);
            ntask++;
            if (ntask>=nflags) break;
            TList *subTaskList = task->GetListOfTasks();
            for (k=0;k<subTaskList->GetSize();k++) {
               TTask *task = (TTask*)subTaskList->At(k);
               if (!flags.At(ntask)) task->SetActive(false);
               ntask++;
               if (ntask>=nflags) break;
            }
            if (ntask>=nflags) break;
         }
         i++;
      }
      if (!strcmp(arg[i],"-th")&&i<argc-1) {
         int ntask=0;
         TArrayI flags = ROMEStatic::decodeRunNumbers(arg[i+1]);
         int nflags = flags.GetSize();
         TList *taskList = fMainTask->GetListOfTasks();
         for (j=0;j<taskList->GetSize();j++) {
            ROMETask *task = (ROMETask*)taskList->At(j);
            if (flags.At(ntask)) task->SetHistoAccumulation(true);
            else task->SetHistoAccumulation(false);
            ntask++;
            if (ntask>=nflags) break;
            TList *subTaskList = task->GetListOfTasks();
            for (k=0;k<subTaskList->GetSize();k++) {
               ROMETask *task = (ROMETask*)subTaskList->At(k);
               if (flags.At(ntask)) task->SetHistoAccumulation(true);
               else task->SetHistoAccumulation(false);
               ntask++;
               if (ntask>=nflags) break;
            }
            if (ntask>=nflags) break;
         }
         i++;
      }
      if (!strcmp(arg[i],"-tr")&&i<argc-1) {
         TArrayI flags = ROMEStatic::decodeRunNumbers(arg[i+1]);
         ROMETree *datTree;
         for (j=0;j<fTreeObjects->GetEntries();j++) {
            datTree = this->GetTreeObjectAt(j);
            if (flags.GetSize()>j*4) datTree->SetRead(toBool(flags.At(j*4)));
            if (flags.GetSize()>j*4+1) datTree->SetWrite(toBool(flags.At(j*4+1)));
            if (flags.GetSize()>j*4+2) datTree->SetFill(toBool(flags.At(j*4+2)));
            if (flags.GetSize()>j*4+3) datTree->SetAccumulation(toBool(flags.At(j*4+3)));
         }
         i++;
      }
   }
   return true;
}


int ROMEAnalyzer::ReadROMEConfigXML(char buffer[][200]) {
// Read the romeConfig.xml file
   int nres = 1;
   strcpy(buffer[0]," ");
   int type;
   const xmlChar *name,*value;
   xmlTextReaderPtr reader;
   char filename[gFileNameLength];
   sprintf(filename,"%sromeConfig.xml",GetConfigDir());
   reader = xmlReaderForFile(filename, NULL, 0);
   if (reader != NULL) {
      while (xmlTextReaderRead(reader)) {
         type = xmlTextReaderNodeType(reader);
         name = xmlTextReaderConstName(reader);
         if (type == 1 && !strcmp((const char*)name,"Configuration")) {
            while (xmlTextReaderRead(reader)) {
               type = xmlTextReaderNodeType(reader);
               name = xmlTextReaderConstName(reader);
               if (type == 1) {
                  sprintf(buffer[nres],"-%s",(const char*)name);
                  nres++;
                  xmlTextReaderRead(reader);
                  type = xmlTextReaderNodeType(reader);
                  value = xmlTextReaderConstValue(reader);
                  if (value!=NULL && type==3) {
                     strcpy(buffer[nres],(const char*)value);
                     nres++;
                  }
               }
               if (type == 15 && !strcmp((const char*)name,"Configuration")) break;
            }
            break;
         }
      }
      xmlFreeTextReader(reader);
   } else {
      cout << "Unable to open " << filename << endl;
      return -1;
   }
   return nres;
}


void *server_thread(void *arg)
//  Server histograms to remove clients
{
   int i;
   char str[32];

   TSocket *s = (TSocket *) arg;

   do {
      if (s->Recv(str, sizeof(str)) <= 0) {
         printf("Closed connection from %s\n", s->GetInetAddress().GetHostName());
         s->Close();
         delete s;
         return NULL;
      } else {
         printf("Received \"%s\"\n", str);

         TMessage *mess = new TMessage(kMESS_OBJECT);

         if (strcmp(str, "LIST") == 0) {
            // build name array 

            TObjArray *names = new TObjArray(100);

            cout << "histos" << endl;
            TCollection *histos = ((TFolder*)gROOT->FindObjectAny("histos"))->GetListOfFolders();
            TIterator *iter = histos->MakeIterator();
            while (TObject *obj = iter->Next()) {
               cout << obj->GetName() << endl;
               names->Add(new TObjString(obj->GetName()));
            }

            // send "names" array 
            mess->Reset();
            mess->WriteObject(names);
            s->Send(*mess);

            for (i = 0; i < names->GetLast() + 1; i++)
               delete(TObjString *) names->At(i);

            delete names;
         }

         else if (strncmp(str, "GET", 3) == 0) {
            // search histo 
            TObject *obj;

            TCollection *histos = ((TFolder*)gROOT->FindObjectAny("histos"))->GetListOfFolders();
            TIterator *iter = histos->MakeIterator();
            while (obj = iter->Next()) {
               if (strcmp(str + 4, obj->GetName()) == 0)
                  break;
            }

            if (!obj) {
               s->Send("Error");
            } else {
               // send single histo 
               mess->Reset();
               mess->WriteObject(obj);
               s->Send(*mess);
            }
         }

         else if (strncmp(str, "CLEAR", 5) == 0) {
            // search histo 
            TObject *obj;

            TCollection *histos = ((TFolder*)gROOT->FindObjectAny("histos"))->GetListOfFolders();
            TIterator *iter = histos->MakeIterator();
            while (obj = iter->Next()) {
               if (strcmp(str + 6, obj->GetName()) == 0)
                  break;
            }

            if (!obj) {
               s->Send("Error");
            } else {
               // clear histo 
#if defined ( __linux__ )
               TThread::Lock();
               ((TH1 *) obj)->Reset();
               TThread::UnLock();
#endif

               // send single histo 
               s->Send("OK");
            }
         }

         delete mess;
      }

   } while (1);
}

void *root_server_loop(void *arg)
// Server loop listening for incoming network connections on port
// specified by command line option -s. Starts a searver_thread for 
// each connection.
{
   int port = 9090;
   printf("Root server listening on port %d...\n", port);
   TServerSocket *lsock = new TServerSocket(port, kTRUE);

   do {
      TSocket *s = lsock->Accept();
      s->Send("RMSERV 1.0");
      printf("New connection from %s\n", s->GetInetAddress().GetHostName());

      // start a new server thread 
#if defined ( __linux__ )
      TThread *th = new TThread("server_thread", server_thread, s);
      th->Run();
#endif
   } while (1);
}

