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


#include <sys/stat.h>
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
   int i;
   const int workDirLen = 1000;
   char workDir[workDirLen];
   getcwd(workDir,workDirLen);
   sprintf(workDir,"%s/",workDir);
   this->SetDataBaseDir(workDir);
   this->SetInputDir(workDir);
   this->SetOutputDir(workDir);

   char configFile[100] = "romeConfig.xml";

   if (argc==1) {
      cout << "  -i       Configuration file (default ROMEConfig.xml)" << endl;
      cout << "  -b       Batch Mode (no Argument)" << endl;
      cout << "  -m       Analysing Mode : (online/[offline])" << endl;
      cout << "  -f       Input Data Format : ([midas]/root)" << endl;
      cout << "  -r       Runnumbers" << endl;
      cout << "  -e       Eventnumbers" << endl;
      cout << "  -docu    Generates a Root-Html-Documentation (no Argument)" << endl;
      return false;
   }
   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-docu")) {
         THtml html;
         html.MakeAll(true);
         return false;
      }
      if (!strcmp(argv[i],"-i")&&i<argc-1) {
         strcpy(configFile,argv[i+1]);
         i++;
      }
   }
   char answer[10];
   struct stat buf;
   if( stat( configFile, &buf )) {
      cout << "Configuration file '" << configFile << "' not found." << endl;
      cout << "Do you like the framework to generate a new configuration file ([y]/n) ? ";
      cin >> answer;
      if (strstr(answer,"n")==NULL) {
         cout << "\nThe framework generated a new configuration file." << endl;
         cout << "Please edit this file and restart the program.\n" << endl;
         if (!WriteROMEConfigXML(configFile)) {
            cout << "\nTerminate program.\n" << endl;
            return false;
         }
      }
      else {
         cout << "\nTerminate program.\n" << endl;
      }
      return false;
   }
   if (!ReadROMEConfigXML(configFile)) {
      cout << "\nTerminate program.\n" << endl;
      return false;
   }
   if (!WriteROMEConfigXML(configFile)) {
      cout << "\nTerminate program.\n" << endl;
      return false;
   }

   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-b")) {
         fBatchMode = true;
      }
      if (!strcmp(argv[i],"-m")) {
         if (!strcmp(argv[i+1],"online")) fOnLineMode = true;
         else fOnLineMode = false;
         i++;
      }
      if (!strcmp(argv[i],"-f")) {
         if (!strcmp(argv[i+1],"root")) fRootMode = true;
         else fRootMode = false;
         i++;
      }
      if (!strcmp(argv[i],"-r")&&i<argc-1) {
         fRunNumberString = argv[i+1];
         fRunNumber = ROMEStatic::decodeRunNumbers(argv[i+1]);
         i++;
      }
      if (!strcmp(argv[i],"-e")&&i<argc-1) {
         fEventNumberString = argv[i+1];
         fEventNumber = ROMEStatic::decodeRunNumbers(argv[i+1]);
         i++;
      }
   }

   return true;
}
/*
   const int workDirLen = 1000;
   char workDir[workDirLen];
   getcwd(workDir,workDirLen);
   sprintf(workDir,"%s/",workDir);

void ROMEAnalyzer::WriteROMEConfigXML(char *configFile) {
}
bool ROMEAnalyzer::ReadROMEConfigXML(char *configFile) {
// Read the romeConfig.xml file

   int nres = 1;
   int type;
   const xmlChar *name,*value;

   xmlTextReaderPtr reader;
   reader = xmlReaderForFile(configFile, NULL, 0);
   if (reader != NULL) {
      while (xmlTextReaderRead(reader)) {
         type = xmlTextReaderNodeType(reader);
         name = xmlTextReaderConstName(reader);
         if (type == 1 && !strcmp((const char*)name,"Configuration")) {
            while (xmlTextReaderRead(reader)) {
               type = xmlTextReaderNodeType(reader);
               name = xmlTextReaderConstName(reader);
               if (type == 1) {
                  nres++;
                  xmlTextReaderRead(reader);
                  type = xmlTextReaderNodeType(reader);
                  value = xmlTextReaderConstValue(reader);
                  if (value!=NULL && type==3) {
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
      cout << "Unable to open " << configFile << endl;
      return false;
   }
   return true;
}
*/

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


void ROMEAnalyzer::intToNDigitsChar(char *buffer,int numberOfDigits,int number)
{
   for (int i=0;i<numberOfDigits;i++) 
      buffer[i] = 48+(int)((number % (int)pow(10,numberOfDigits-i)) / (int)pow(10,numberOfDigits-i-1));
   
   buffer[numberOfDigits] = 0;
}

