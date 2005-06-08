/********************************************************************

  Builder.cpp, Ryu Sawada

  $Log$
  Revision 1.49  2005/06/08 14:05:35  schneebeli_m
  delete menu

  Revision 1.48  2005/06/02 20:40:11  sawada
  renamed TGROMEController as ArgusAnalyzerController

  Revision 1.47  2005/06/02 08:25:15  schneebeli_m
  RootCintHeader --> DictionaryHeader

  Revision 1.46  2005/06/01 11:08:39  schneebeli_m
  error

  Revision 1.45  2005/06/01 10:17:50  schneebeli_m
  RootCintHeaders & RootCintIncludes

  Revision 1.44  2005/05/26 14:26:54  sawada
  Lots of changes.
  Made ArgusBuilder an inheriting class of ROMEBuilder.
  Remove ROMEFolder and added NetFolers.
  Added ArgusWindow class.
  and so on.

  Revision 1.43  2005/05/16 16:46:57  sawada
  with gcc4, enumeration in class can not be constant in switch.

  Revision 1.42  2005/05/14 21:42:22  sawada
  Separated file writing function in builder.

  Revision 1.41  2005/05/08 00:28:53  sawada
  fixed mismathes of [Set,Append]Formatted in builder.
  added readme of examples.

  Revision 1.40  2005/05/06 22:41:09  sawada
  small bug fix.

  Revision 1.39  2005/05/06 18:26:03  sawada
  better Makefile.

  Revision 1.38  2005/05/05 20:08:04  sawada
  code clean up.

  Revision 1.37  2005/05/03 18:18:45  sawada
  enable to use Long64_t and ULong64_t as steering parameters.

  Revision 1.36  2005/04/28 16:26:26  sawada
  PostgreSQL support.
  Error message from GetMenuHandle.

  Revision 1.35  2005/04/27 20:17:42  sawada
  SQLite support.

  Revision 1.34  2005/04/22 12:58:24  schneebeli_m
  removed windows errors

  Revision 1.33  2005/04/12 17:50:34  sawada
  changes for mxml

  Revision 1.32  2005/04/01 12:31:19  sawada
  sub menu

  Revision 1.31  2005/03/28 10:54:37  sawada
  removed tab hierarchy.
  made ReadXMLMenu.

  Revision 1.30  2005/03/26 16:53:42  sawada
  libxml replaced by mxml.

  Revision 1.29  2005/03/20 18:26:20  sawada
  small bug fix.

  Revision 1.28  2005/03/20 18:19:55  sawada
  unified makefile for windows and unix.

  Revision 1.27  2005/03/17 17:35:52  sawada
  better document output.

  Revision 1.26  2005/03/13 08:40:43  sawada
  modified handling of recursiveDepth.
  removed unused variables.

  Revision 1.25  2005/03/12 22:56:39  sawada
  small change in Makefile

  Revision 1.24  2005/03/12 22:22:33  sawada
  Better output from builder.
  Menu of nested tab.

  Revision 1.23  2005/03/12 01:21:00  sawada
  Nested tab.

  Revision 1.22  2005/03/11 19:52:03  sawada
  Improved makefile.
  added new make target "build".

  Revision 1.21  2005/03/11 19:49:34  sawada
  *** empty log message ***

  Revision 1.20  2005/02/27 23:53:43  sawada
  Create placeholder of ROMEFolder at start.
  Environment variable in ROMEProjectPath.
  Bug fix of arrayed ROMEFolder.
  Bug fix of SetActive of tabs from monitor.
  Create menues of the first tab at start.

  Revision 1.19  2005/02/27 13:21:41  sawada
  recompile monitor and window when dictionary is changed.

  Revision 1.18  2005/02/25 16:14:00  sawada
  bug fix and example for ROMEFolder

  Revision 1.17  2005/02/25 13:51:02  sawada
  added folderShortCut and ROMEProjectPath

  Revision 1.16  2005/02/24 23:25:01  sawada
  Removed thread flag in builder.
  Added ProcessMessageThread.
  Changed arguments of constructor of tabs.

  Revision 1.15  2005/02/24 15:04:03  sawada
  Reduced number of configuration to 1.
  Replaced ss_getchar to getchar().
  Removed LineToProcess.
  Removed bk_is32,bk_find.
  Improved help.
  Handling of midas host and experiment.

  Revision 1.14  2005/02/21 23:07:50  sawada
  several UNIX support

  Revision 1.13  2005/02/21 19:06:55  sawada
  changed platform specifying macros

  Revision 1.12  2005/02/06 00:57:12  sawada
  bug fix

  Revision 1.11  2005/02/06 00:39:35  sawada
   Changed TRint to TApplication
   Stop method of thread function
   Name of method for start/stop

  Revision 1.10  2005/02/05 23:16:23  sawada
  Removed TROOT,VoidFuncPtr_t.
  small bug fix.

  Revision 1.9  2005/02/04 22:58:46  sawada
  ROMEFolder

  Revision 1.8  2005/02/03 11:44:54  sawada
  IO to MIDAS ODB

  Revision 1.7  2005/02/02 18:58:02  sawada
  small change.

  Revision 1.6  2005/02/02 18:29:10  sawada
  regenerate dictionary, when a header is modified. (linux/mac)

  Revision 1.5  2005/02/02 18:19:24  sawada
  small bug fix.

  Revision 1.4  2005/02/01 21:53:24  sawada
  implemented thread functions of tabs.

  Revision 1.3  2005/01/31 10:40:41  sawada
  small bug fix

  Revision 1.2  2005/01/31 10:21:29  sawada
   * make clean.
   * changed template of tabs.

  Revision 1.1.1.1  2005/01/29 22:45:07  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#include "ArgusBuilder.h"

Bool_t ArgusBuilder::WriteMain() {
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString mainprogname(mainProgName);
   cppFile.SetFormatted("%s/src/framework/main.cpp",outDir.Data());
   buffer.Resize(0);

   buffer.AppendFormatted("#include <TRint.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include \"include/framework/%sMonitor.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include <Riostream.h>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("int main(int argc, char *argv[])\n");
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int argn = 1;\n");
   buffer.AppendFormatted("   char arg[1][100];\n");
   buffer.AppendFormatted("   char *argp = &arg[0][0];\n");
   buffer.AppendFormatted("   strcpy(arg[0],argv[0]);\n");
   buffer.AppendFormatted("\n");

#if defined( USE_TRINT )
   buffer.AppendFormatted("   TRint *app = new TRint(\"App\", &argn, &argp,NULL,0,kTRUE);\n");
#else
   buffer.AppendFormatted("   TApplication *app = new TApplication(\"App\", &argn, &argp,NULL,0);\n");
#endif
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   // Monitor\n");
   buffer.AppendFormatted("   gMonitor = new %sMonitor(app);\n",shortCut.Data());
   buffer.AppendFormatted("   // Main window\n");
   buffer.AppendFormatted("   Char_t str[80];\n");
   buffer.AppendFormatted("   sprintf(str, \"%%s\", gMonitor->GetProgramName());\n");
   buffer.AppendFormatted("   gWindow = new %sWindow(gClient->GetRoot(), str);\n",shortCut.Data());   
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   TFolder *fMainFolder = gROOT->GetRootFolder()->AddFolder(\"Argus\",\"Argus Folder\");\n");
   buffer.AppendFormatted("   fMainFolder->Add(gMonitor);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (!gMonitor->Start(argc, argv)) {\n");
   buffer.AppendFormatted("      delete gMonitor;\n");
   buffer.AppendFormatted("      return 1;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   delete gMonitor;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return kTRUE;
}


void ArgusBuilder::startBuilder(Char_t* xmlFile) 
{
   xml = new ROMEXML();
   Char_t* name;
   Bool_t finished = kFALSE;
   Int_t type;
   Int_t i,j;   
   experimentName = "";
   shortCut = "";
   mainProgName = "";
   mainDescription = "";
   mainAuthor = "";
   mainInstitute = "";
   mainCollaboration = "";
   mainEmail = "";
   usedCLO = ROMECommandLineOptions;

   readExperiment = kFALSE;
   readAuthor = kFALSE;
   readFolders = kFALSE;
   readTabs = kFALSE;
   readGlobalSteeringParameters = kFALSE;

   numOfFolder = -1;
   numOfTab = numOfTask = numOfTaskHierarchy = -1;
   numOfNetFolder = -1;

   TString::MaxWaste(kTStringResizeIncrement-1);
   TString::ResizeIncrement(kTStringResizeIncrement);

   if (!xml->OpenFileForRead(xmlFile)) return;
   while (xml->NextLine()&&!finished) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp((const Char_t*)name,"ArgusMonitorDefinition")) {
         while (xml->NextLine()&&!finished) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 15 && !strcmp((const Char_t*)name,"ArgusMonitorDefinition")) {
               if (!readExperiment) {
                  cout << "Experiment tree missing!" << endl;
                  cout << "Terminating program." << endl;
                  return;
               }
               finished = kTRUE;
               break;
            }

            if (type == 1) {
               if (!strcmp((const Char_t*)name,"Experiment")) {
                  readExperiment = kTRUE;
                  while (xml->NextLine()&&!finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp((const Char_t*)name,"ExperimentName"))
                        xml->GetValue(experimentName,experimentName);
                     if (type == 1 && !strcmp((const Char_t*)name,"ExperimentShortCut"))
                        xml->GetValue(shortCut,shortCut);
                     if (type == 1 && !strcmp((const Char_t*)name,"ProgramName"))
                        xml->GetValue(mainProgName,mainProgName);
                     if (type == 1 && !strcmp((const Char_t*)name,"MonitorDescription"))
                        xml->GetValue(mainDescription,mainDescription);
                     if (type == 15 && !strcmp((const Char_t*)name,"Experiment")) {
                        if (shortCut=="") {
                           cout << "Experiment must have a shortcut!" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                        break;
                     }
                  }
                  continue;
               }

               if (!strcmp((const Char_t*)name,"Author")) {
                  readAuthor = kTRUE;
                  while (xml->NextLine()&&!finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp((const Char_t*)name,"AuthorName"))
                        xml->GetValue(mainAuthor,mainAuthor);
                     if (type == 1 && !strcmp((const Char_t*)name,"AuthorInstitute"))
                        xml->GetValue(mainInstitute,mainInstitute);
                     if (type == 1 && !strcmp((const Char_t*)name,"AuthorCollaboration"))
                        xml->GetValue(mainCollaboration,mainCollaboration);
                     if (type == 1 && !strcmp((const Char_t*)name,"AuthorEmail"))
                        xml->GetValue(mainEmail,mainEmail);
                     if (type == 15 && !strcmp((const Char_t*)name,"Author"))
                        break;
                  }
                  continue;
               }

               if (!strcmp((const Char_t*)name,"Folders")) {
                  // initialization
                  numOfFolder = -1;
                  parent[0] = "GetMainFolder()";
                  // output
                  if (makeOutput) cout << "Folders:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // folder
                     if (type == 1 && !strcmp((const Char_t*)name,"Folder")) {
                        recursiveDepth = 0;
                        if (!ReadXMLFolder()) return;
                     }
                     // folders end
                     if (type == 15 && !strcmp((const Char_t*)name,"Folders")) {
                        break;
                     }
                  }
                  // check input
                  for (i=0;i<numOfFolder;i++) {
                     for (j=i+1;j<numOfFolder;j++) {
                        if (folderName[i]==folderName[j]) {
                           cout << "\nFolder '" << folderName[i].Data() << "' is defined twice !" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                     }
                  }
                  // count folders
                  numOfFolder++;
               }

               if (!strcmp((const Char_t*)name,"Tabs")) {
                  // initialization
                  numOfTab = numOfTask = numOfTaskHierarchy = -1;
                  // output
                  if (makeOutput) cout << "\n\nTabs:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // tab
                     if (type == 1 && !strcmp((const Char_t*)name,"Tab")) {
                        tabParentIndex[numOfTab+1] = -1;
                        recursiveTabDepth = 0;
                        if (!ReadXMLTab()) return;
                     }
                     // tabs end
                     if (type == 15 && !strcmp((const Char_t*)name,"Tabs")) {
                        break;
                     }
                  }
                  // check input
                  for (i=0;i<numOfTab;i++) {
                     for (j=i+1;j<numOfTab;j++) {
                        if (tabName[i] == tabName[j]) {
                           cout << "\nTab '" << tabName[i].Data() << "' is defined twice !" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                     }
                  }
                  // count tabs
                  numOfTab++;
                  numOfTask++;
                  numOfTaskHierarchy++;
                  for (i=0;i<numOfTab;i++)
                     numOfThreadFunctions[i]++;
               }

               if (!strcmp((const Char_t*)name,"NetFolders")) {
                  // initialization
                  numOfNetFolder = -1;
                  // output
                  if (makeOutput) cout << "\n\nNetFolders:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // net object
                     if (type == 1 && !strcmp((const Char_t*)name,"NetFolder")) {
                        if (!ReadXMLNetFolder()) return;
                     }
                     // net folders end
                     if (type == 15 && !strcmp((const Char_t*)name,"NetFolders")) {
                        break;
                     }
                  }
                  // check input
                  for (i=0;i<numOfNetFolder;i++) {
                     for (j=i+1;j<numOfNetFolder;j++) {
                        if (netFolderName[i] == netFolderName[j]) {
                           cout << "\nNet folder '" << netFolderName[i].Data() << "' is defined twice !" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                     }
                  }
                  // count net object
                  numOfNetFolder++;
               }

               if (!strcmp((const Char_t*)name,"GlobalSteeringParameters")) {
                  // output
                  if (makeOutput) cout << "\n\nGlobal Steering Parameters:" << endl;
                  // initialisation
                  steerName[numOfTab][0] = "GlobalSteering";
                  actualSteerIndex = 0;
                  recursiveSteerDepth = 0;
                  steerParent[numOfTab][0] = -1;
                  numOfSteering[numOfTab] = -1;
                  if (!ReadXMLSteering(numOfTab)) return;
                  numOfSteering[numOfTab]++;
               }
            }
         }
      }
   }

   if (makeOutput) cout << "\n\nFramework:" << endl;
   if (makeOutput) cout << "\n\nMonitor :" << endl;
   if (!WriteMonitorCpp()) return;
   if (!WriteMonitorH()) return;
   if (makeOutput) cout << "\n\nWindow :" << endl;
   if (!WriteWindowCpp()) return;
   if (!WriteWindowH()) return;
   if (makeOutput) cout << "\n\nFolders:" << endl;
   if (!WriteFolderCpp()) return;
   if (!WriteFolderH()) return;
   if (makeOutput) cout << "\n\nTabs:" << endl;
   if (!WriteTabCpp()) return;
   if (!WriteTabH()) return;
   if (makeOutput) cout << "\n\nUser Database Interfaces:" << endl;
   if (!WriteDBCpp()) return;
   if (!WriteDBH()) return;
   if (!WriteSteering(numOfTab)) return;   
   if (!WriteConfigCpp()) return;
   if (!WriteConfigH()) return;
   if (!WriteMain()) return;

   delete xml;
   ROMEString buffer;
// Dictionary
   chdir(outDir.Data());
#if defined( R__VISUAL_CPLUSPLUS )
   if (makeOutput) cout << "\nExecuting 'rootcint' for Root-Dictionary generation." << endl;
   WriteDictionaryBat(buffer);
//   system(buffer);
#endif
// Linking
   if (makeOutput && !noLink) cout << "\nLinking " << shortCut.Data() << " Project." << endl;
   WriteMakefile(xmlFile);
   if (!noLink) {
#if defined( R__UNIX )
      system("make -e");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      const Int_t workDirLen = 1000;
      Char_t workDir[workDirLen];
      getcwd(workDir,workDirLen);
      cout << "working dir = " << workDir << endl;
      system("nmake -f Makefile.win");
#endif
   }

   // Documentation
   if (makeOutput) cout << "\nWrite HTML Documentation." << endl;
   WriteHTMLDoku();
}


void ArgusBuilder::WriteMakefile(Char_t* xmlFile) {
   // write a Makefile
   ROMEString buffer;
   ROMEString tempBuffer[2];
   ROMEString compileFormatFrame,compileFormatTabs,compileFormatBlank,compileFormatROME,compileFormatARGUS,compileFormatMXML;
   Int_t i;
   ROMEString shortcut(shortCut);
   shortcut.ToLower();
   ROMEString mainprogname(mainProgName);
   mainprogname.ToLower();   
   buffer.Resize(0);

// Libraries, Flags and Includes
// -----------------------------
#if defined( R__VISUAL_CPLUSPLUS )
//   buffer.AppendFormatted("rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libWin32gdk.lib $(ROOTSYS)/lib/libVMC.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libGeomPainter.lib $(ROOTSYS)/lib/libMLP.lib $(ROOTSYS)/lib/libProof.lib $(ROOTSYS)/lib/libProofGui.lib $(ROOTSYS)/lib/libRGL.lib $(ROOTSYS)/lib/libfreetype.lib\n");
   buffer.AppendFormatted("rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libWin32gdk.lib \n");
   buffer.AppendFormatted("sqllibs =");
   if (this->mysql)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libmySQL.lib $(ROMESYS)/lib_win/mysys.lib $(ROMESYS)/lib_win/mysqlclient.lib");
   if (this->pgsql)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libpq.lib");
   if (this->sqlite)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libsqlite.lib");
   if (this->sqlite3)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libsqlite3.lib");
   buffer.AppendFormatted("\n");
   if (this->midas)
      buffer.AppendFormatted("midaslibs = $(MIDASSYS)/nt/lib/midas.lib\n");
   else
      buffer.AppendFormatted("midaslibs = \n");
   buffer.AppendFormatted("clibs = wsock32.lib gdi32.lib user32.lib kernel32.lib\n");
   buffer.AppendFormatted("Libraries = $(rootlibs) $(clibs) $(sqllibs) $(midaslibs)\n");
   buffer.AppendFormatted("\n");
   // flags
   buffer.AppendFormatted("Flags = /GX /GR $(%suserflags)",shortcut.Data());
   if (this->midas) 
      buffer.AppendFormatted(" /DHAVE_MIDAS");
   if (this->sql)
      buffer.AppendFormatted(" /DHAVE_SQL");
   if (this->mysql)
      buffer.AppendFormatted(" /DHAVE_MYSQL");
   if (this->pgsql)
      buffer.AppendFormatted(" /DHAVE_PGSQL");
   if (this->sqlite)
      buffer.AppendFormatted(" /DHAVE_SQLITE");
   if (this->sqlite3)
      buffer.AppendFormatted(" /DHAVE_SQLITE3");
   buffer.AppendFormatted("\n");
   // includes
   buffer.AppendFormatted("Includes = /I$(ARGUSSYS)/include/ /I$(ROMESYS)/include/ /I$(ROOTSYS)/include/ /I. /Iinclude/ /Iinclude/tabs/ /Iinclude/framework/ ");
   if (this->midas) 
      buffer.AppendFormatted(" /I$(MIDASSYS)/include/");
   if (this->mysql) 
      buffer.AppendFormatted(" /I$(ROMESYS)/include/mysql/");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
#endif

#if defined( R__UNIX )
   buffer.AppendFormatted("rootlibs := $(shell root-config --libs)\n");
   buffer.AppendFormatted("rootglibs := $(shell root-config --glibs)\n");
   buffer.AppendFormatted("rootcflags := $(shell root-config --cflags)\n");
   buffer.AppendFormatted("rootthreadlibs := -lThread\n");
   buffer.AppendFormatted("sqllibs :=");
   if (this->mysql)
      buffer.AppendFormatted(" $(shell mysql_config --libs)");
   if (this->pgsql)
      buffer.AppendFormatted(" -L$(shell pg_config --libdir) -lpq");
   if (this->sqlite)
      buffer.AppendFormatted(" -lsqlite");
   if (this->sqlite3)
      buffer.AppendFormatted(" -lsqlite3");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("sqlcflags :=");
   if (this->sql)
      buffer.AppendFormatted(" -DHAVE_SQL");
   if (this->mysql)
      buffer.AppendFormatted(" $(shell mysql_config --cflags) -DHAVE_MYSQL");
   if (this->pgsql)
      buffer.AppendFormatted(" -I$(shell pg_config --includedir) -DHAVE_PGSQL");
   if (this->sqlite)
      buffer.AppendFormatted(" -DHAVE_SQLITE");
   if (this->sqlite3)
      buffer.AppendFormatted(" -DHAVE_SQLITE3");
   buffer.AppendFormatted("\n");
#if defined( R__ALPHA )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lc -lbsd\n");
#elif defined ( R__SGI )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs :=\n");
#elif defined ( R__FBSD )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lbsd -lcompat\n");
#elif defined ( R__MACOSX )
   buffer.AppendFormatted("FINK_DIR := $(shell which fink 2>&1 | sed -ne \"s/\\/bin\\/fink//p\")\n");
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function  $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)\n");
   buffer.AppendFormatted("oslibs := -lpthread -multiply_defined suppress $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib)\n");
#elif defined ( R__LINUX )
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function\n");
   buffer.AppendFormatted("oslibs := -lutil -lpthread\n");
#elif defined ( R__SOLARIS )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lsocket -lnsl\n");
#else
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs :=\n");
#endif

   if (this->midas) {
#if defined( R__ALPHA )
      buffer.AppendFormatted("midascflags := -DOSF1 -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/osf1/lib -lmidas\n");
#elif defined ( R__SGI )
      buffer.AppendFormatted("midascflags := -DOS_ULTRIX -DNO_PTY -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/ultrix/lib -lmidas\n");
#elif defined ( R__FBSD )
      buffer.AppendFormatted("midascflags := -DOS_FREEBSD -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/freeBSD/lib -lmidas\n");
#elif defined ( R__MACOSX )
      buffer.AppendFormatted("midascflags := -DOS_LINUX -DOS_DARWIN -DHAVE_STRLCPY -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/darwin/lib -lmidas\n");
#elif defined ( R__LINUX )
      buffer.AppendFormatted("midascflags := -DOS_LINUX -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/linux/lib -lmidas\n");
#elif defined ( R__SOLARIS )
      buffer.AppendFormatted("midascflags := -DOS_SOLARIS -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/solaris/lib -lmidas\n");
#else
      buffer.AppendFormatted("midascflags := -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -lmidas\n");
#endif
   }
   else{
      buffer.AppendFormatted("midaslibs := \n");
      buffer.AppendFormatted("midascflags := \n");
   }
   buffer.AppendFormatted("clibs :=-lHtml $(SYSLIBS)");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   // libs
   buffer.AppendFormatted("Libraries := $(oslibs) $(rootglibs) $(rootthreadlibs) $(clibs) $(sqllibs) $(midaslibs)\n");
   // flags
   buffer.AppendFormatted("Flags := $(%suserflags) $(oscflags) $(rootcflags) $(sqlcflags) $(midascflags)\n",shortcut.Data());
   // includes
   buffer.AppendFormatted("Includes := -I$(ARGUSSYS)/include/ -I$(ROMESYS)/include/ -I. -Iinclude/ -Iinclude/tabs/ -Iinclude/framework/");
   buffer.AppendFormatted("\n");
#endif

// Dependencies
// ------------
   // folders
   buffer.AppendFormatted("FolderIncludes %s",EqualSign());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0)
         buffer.AppendFormatted(" include/framework/%s%s.h",shortCut.Data(),folderName[i].Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("BaseFolderIncludes %s",EqualSign());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0 && folderUserCode[i])
         buffer.AppendFormatted(" include/framework/%s%s_Base.h",shortCut.Data(),folderName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // tabs
   buffer.AppendFormatted("TabIncludes %s",EqualSign());
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted(" include/tabs/%sT%s.h",shortCut.Data(),tabName[i].Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("BaseTabIncludes %s",EqualSign());
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted(" include/tabs/%sT%s_Base.h",shortCut.Data(),tabName[i].Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("TabObjects %s",EqualSign());
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted(" obj/%sT%s.obj",shortCut.Data(),tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // root cint headers
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("DictionaryHeaders %s",EqualSign());
   // root cint includes
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("DictionaryIncludes %s",EqualSign());

   // tab dependences
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("%sT%sDep %s\n",shortCut.Data(),tabName[i].Data(),EqualSign());
   }
   buffer.AppendFormatted("\n");

// Objects
// -------
   buffer.AppendFormatted("objects %s",EqualSign());
   for (i=0;i<numOfFolder;i++) {
      if (!folderUserCode[i]) continue;
      buffer.AppendFormatted(" obj/%s%s.obj",shortCut.Data(),folderName[i].Data());
   }
   buffer.AppendFormatted(" $(TabObjects)");
   buffer.AppendFormatted(" obj/%sMonitor.obj obj/%sWindow.obj obj/%sConfig.obj obj/main.obj",shortCut.Data(),shortCut.Data(),shortCut.Data());
   if (this->mysql)
      buffer.AppendFormatted(" obj/ROMEMySQL.obj");
   if (this->pgsql)
      buffer.AppendFormatted(" obj/ROMEPgSQL.obj");
   if (this->sqlite)
      buffer.AppendFormatted(" obj/ROMESQLite.obj");
   if (this->sqlite3)
      buffer.AppendFormatted(" obj/ROMESQLite3.obj");
   if (this->sql)
      buffer.AppendFormatted(" obj/ROMESQL.obj obj/ROMESQLDataBase.obj");
   buffer.AppendFormatted(" obj/ArgusMonitor.obj  obj/ArgusWindow.obj obj/ArgusTextDialog.obj obj/ArgusAnalyzerController.obj obj/TNetFolder.obj obj/ROMEXML.obj obj/ROMEString.obj obj/ROMEStrArray.obj obj/ROMEStr2DArray.obj obj/ROMEPath.obj obj/ROMEXMLDataBase.obj obj/ROMEODBOnlineDataBase.obj obj/%sDict.obj obj/mxml.obj obj/strlcpy.obj\n\n",shortCut.Data());

   // all
   buffer.AppendFormatted("all:obj %s%s",shortcut.Data(),mainprogname.Data());
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted(".exe\n");
#endif
   buffer.AppendFormatted("\n");

   // user makefile
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("!INCLUDE Makefile.usr\n");
#else
   buffer.AppendFormatted("-include Makefile.usr\n");
#endif
   buffer.AppendFormatted("\n");
   
   // make obj
   buffer.AppendFormatted("obj:\n");
   buffer.AppendFormatted("\t@if [ ! -d  obj ] ; then \\\n");
   buffer.AppendFormatted("\t\techo \"Making directory obj\" ; \\\n");
   buffer.AppendFormatted("\t\tmkdir obj; \\\n");
   buffer.AppendFormatted("\tfi;\n");

// Link Statement
// --------------
   buffer.AppendFormatted("%s%s",shortcut.Data(),mainprogname.Data());
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted(".exe");
#endif
   buffer.AppendFormatted(": $(objects)\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("	cl /Fe%s%s.exe $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("	g++ $(Flags) -o $@ $(objects) $(Libraries)\n\n");
#endif

// Compile Statements
// ------------------
#if defined( R__UNIX )
   compileFormatFrame.SetFormatted("	g++ -c $(Flags) $(Includes) src/framework/%s%%s.cpp -o obj/%s%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatTabs.SetFormatted("	g++ -c $(Flags) $(Includes) src/tabs/%sT%%s.cpp -o obj/%sT%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatBlank.SetFormatted("	g++ -c $(Flags) $(Includes) %%s.cpp -o obj/%%s.obj\n");
   compileFormatROME.SetFormatted ("	g++ -c $(Flags) $(Includes) $(ROMESYS)/src/ROME%%s.cpp -o obj/ROME%%s.obj\n");
   compileFormatARGUS.SetFormatted ("	g++ -c $(Flags) $(Includes) $(ARGUSSYS)/src/Argus%%s.cpp -o obj/Argus%%s.obj\n");
   compileFormatMXML.SetFormatted ("	g++ -c $(Flags) $(Includes) $(ROMESYS)/src/%%s.c -o obj/%%s.obj\n");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   compileFormatFrame.SetFormatted("	cl /c $(Flags) $(Includes) src/framework/%s%%s.cpp /Foobj/%s%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatTabs.SetFormatted("	cl /c $(Flags) $(Includes) src/tabs/%sT%%s.cpp /Foobj/%sT%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatBlank.SetFormatted("	cl /c $(Flags) $(Includes) %%s.cpp /Foobj/%%s.obj\n");
   compileFormatROME.SetFormatted ("	cl /c $(Flags) $(Includes) $(ROMESYS)/src/ROME%%s.cpp /Foobj/ROME%%s.obj\n");
   compileFormatARGUS.SetFormatted ("	cl /c $(Flags) $(Includes) $(ARGUSSYS)/src/Argus%%s.cpp /Foobj/Argus%%s.obj\n");
   compileFormatMXML.SetFormatted ("	cl /c $(Flags) $(Includes) $(ROMESYS)/src/%%s.c /Foobj/%%s.obj\n");
#endif
   for (i=0;i<numOfFolder;i++) {
      if (folderUserCode[i]) {
         buffer.AppendFormatted("obj/%s%s.obj: src/framework/%s%s.cpp include/framework/%s%s.h include/framework/%s%s_Base.h",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted((Char_t*)compileFormatFrame.Data(),folderName[i].Data(),folderName[i].Data());
      }
   }
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("obj/%sT%s.obj: src/tabs/%sT%s.cpp include/tabs/%sT%s.h include/tabs/%sT%s_Base.h $(%sT%sDep)\n",shortCut.Data(),tabName[i].Data(),shortCut.Data(),tabName[i].Data(),shortCut.Data(),tabName[i].Data(),shortCut.Data(),tabName[i].Data(),shortCut.Data(),tabName[i].Data());
      buffer.AppendFormatted((Char_t*)compileFormatTabs.Data(),tabName[i].Data(),tabName[i].Data());
   }
   buffer.AppendFormatted("obj/%sWindow.obj: src/framework/%sWindow.cpp include/framework/%sWindow.h $(ARGUSSYS)/include/ArgusTextDialog.h $(ARGUSSYS)/include/ArgusAnalyzerController.h include/framework/%sMonitor.h obj/ArgusWindow.obj",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(" $(TabIncludes)");
   buffer.AppendFormatted(" $(BaseTabIncludes)");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted((Char_t*)compileFormatFrame.Data(),"Window","Window");
   buffer.AppendFormatted("obj/%sMonitor.obj: src/framework/%sMonitor.cpp include/framework/%sMonitor.h include/framework/%sWindow.h include/framework/%sConfig.h $(ROMESYS)/include/ROME.h",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(" $(TabIncludes)");
   buffer.AppendFormatted(" $(BaseTabIncludes)");
   buffer.AppendFormatted(" $(FolderIncludes)");
   buffer.AppendFormatted(" $(BaseFolderIncludes)");
   buffer.AppendFormatted(" $(NetFolderIncludes)");
   buffer.AppendFormatted(" $(ROMEBaseFolderIncludes)");
   if (numOfSteering[numOfTab]>0) {
      buffer.AppendFormatted(" include/framework/%sGlobalSteering.h",shortCut.Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted((Char_t*)compileFormatFrame.Data(),"Monitor","Monitor");
   buffer.AppendFormatted("obj/%sConfig.obj: src/framework/%sConfig.cpp include/framework/%sConfig.h include/framework/%sMonitor.h include/framework/%sWindow.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted((Char_t*)compileFormatFrame.Data(),"Config","Config");
   buffer.AppendFormatted("obj/main.obj: src/framework/main.cpp include/framework/%sMonitor.h include/framework/%sWindow.h\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted((Char_t*)compileFormatBlank.Data(),"src/framework/main","main");
   buffer.AppendFormatted("obj/ArgusMonitor.obj: $(ARGUSSYS)/src/ArgusMonitor.cpp $(ARGUSSYS)/include/ArgusMonitor.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatARGUS.Data(),"Monitor","Monitor");
   buffer.AppendFormatted("obj/ArgusWindow.obj: $(ARGUSSYS)/src/ArgusWindow.cpp $(ARGUSSYS)/include/ArgusWindow.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatARGUS.Data(),"Window","Window");
   buffer.AppendFormatted("obj/ArgusTextDialog.obj: $(ARGUSSYS)/src/ArgusTextDialog.cpp $(ARGUSSYS)/include/ArgusTextDialog.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatARGUS.Data(),"TextDialog","TextDialog");
   buffer.AppendFormatted("obj/ArgusAnalyzerController.obj: $(ARGUSSYS)/src/ArgusAnalyzerController.cpp $(ARGUSSYS)/include/ArgusAnalyzerController.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatARGUS.Data(),"AnalyzerController","AnalyzerController");
   buffer.AppendFormatted("obj/TNetFolder.obj: $(ARGUSSYS)/src/TNetFolder.cpp $(ARGUSSYS)/include/TNetFolder.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatBlank.Data(),"$(ARGUSSYS)/src/TNetFolder","TNetFolder");
   buffer.AppendFormatted("obj/ROMEXML.obj: $(ROMESYS)/src/ROMEXML.cpp $(ROMESYS)/include/ROMEXML.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatROME.Data(),"XML","XML");
   buffer.AppendFormatted("obj/mxml.obj: $(ROMESYS)/src/mxml.c $(ROMESYS)/include/mxml.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatMXML.Data(),"mxml","mxml");
   buffer.AppendFormatted("obj/strlcpy.obj: $(ROMESYS)/src/strlcpy.c $(ROMESYS)/include/strlcpy.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatMXML.Data(),"strlcpy","strlcpy");
   buffer.AppendFormatted("obj/ROMEString.obj: $(ROMESYS)/src/ROMEString.cpp $(ROMESYS)/include/ROMEString.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatROME.Data(),"String","String");
   buffer.AppendFormatted("obj/ROMEStrArray.obj: $(ROMESYS)/src/ROMEStrArray.cpp $(ROMESYS)/include/ROMEStrArray.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatROME.Data(),"StrArray","StrArray");
   buffer.AppendFormatted("obj/ROMEStr2DArray.obj: $(ROMESYS)/src/ROMEStr2DArray.cpp $(ROMESYS)/include/ROMEStr2DArray.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatROME.Data(),"Str2DArray","Str2DArray");
   buffer.AppendFormatted("obj/ROMEPath.obj: $(ROMESYS)/src/ROMEPath.cpp $(ROMESYS)/include/ROMEPath.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatROME.Data(),"Path","Path");
   buffer.AppendFormatted("obj/ROMEXMLDataBase.obj: $(ROMESYS)/src/ROMEXMLDataBase.cpp $(ROMESYS)/include/ROMEXMLDataBase.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatROME.Data(),"XMLDataBase","XMLDataBase");
   buffer.AppendFormatted("obj/ROMEODBOnlineDataBase.obj: $(ROMESYS)/src/ROMEODBOnlineDataBase.cpp $(ROMESYS)/include/ROMEODBOnlineDataBase.h\n");
   buffer.AppendFormatted((Char_t*)compileFormatROME.Data(),"ODBOnlineDataBase","ODBOnlineDataBase");
   if (this->mysql) {
      buffer.AppendFormatted("obj/ROMEMySQL.obj: $(ROMESYS)/src/ROMEMySQL.cpp $(ROMESYS)/include/ROMEMySQL.h obj/ROMESQL.obj\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"MySQL","MySQL");
   }
   if (this->sqlite) {
      buffer.AppendFormatted("obj/ROMESQLite.obj: $(ROMESYS)/src/ROMESQLite.cpp $(ROMESYS)/include/ROMESQLite.h obj/ROMESQL.obj\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"SQLite","SQLite");
   }
   if (this->pgsql) {
      buffer.AppendFormatted("obj/ROMEPgSQL.obj: $(ROMESYS)/src/ROMEPgSQL.cpp $(ROMESYS)/include/ROMEPgSQL.h obj/ROMESQL.obj\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"PgSQL","PgSQL");
   }
   if (this->sqlite3) {
      buffer.AppendFormatted("obj/ROMESQLite3.obj: $(ROMESYS)/src/ROMESQLite3.cpp $(ROMESYS)/include/ROMESQLite3.h obj/ROMESQL.obj\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"SQLite3","SQLite3");
   }
   if (this->sql) {
      buffer.AppendFormatted("obj/ROMESQLDataBase.obj: $(ROMESYS)/src/ROMESQLDataBase.cpp $(ROMESYS)/include/ROMESQLDataBase.h\n");
      buffer.AppendFormatted((Char_t*)compileFormatROME.Data(),"SQLDataBase","SQLDataBase");
      buffer.AppendFormatted("obj/ROMESQL.obj: $(ROMESYS)/src/ROMESQL.cpp $(ROMESYS)/include/ROMESQL.h\n");
      buffer.AppendFormatted((Char_t*)compileFormatROME.Data(),"SQL","SQL");
   }

// Dictionary
   buffer.AppendFormatted("obj/%sDict.obj: src/framework/%sDict.h src/framework/%sDict.cpp\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   tempBuffer[0].SetFormatted("src/framework/%sDict",shortCut.Data());
   tempBuffer[1].SetFormatted("%sDict",shortCut.Data());
   buffer.AppendFormatted((Char_t*)compileFormatBlank.Data(),tempBuffer[0].Data(),tempBuffer[1].Data());
   buffer.AppendFormatted("\n");
   ROMEString dictionarybat;
   WriteDictionaryBat(dictionarybat);
   dictionarybat.ReplaceAll("$ROOTSYS","$(ROOTSYS)");
   dictionarybat.ReplaceAll("$ROMESYS","$(ROMESYS)");
   dictionarybat.ReplaceAll("$ARGUSSYS","$(ARGUSSYS)");
#if defined( R__MACOSX )
   buffer.AppendFormatted("DYLD_LIBRARY_PATH=$(ROOTSYS)/lib\n");
#else
   buffer.AppendFormatted("LD_LIBRARY_PATH=$(ROOTSYS)/lib\n");
#endif
   buffer.AppendFormatted("src/framework/%sDict.h src/framework/%sDict.cpp:",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(" $(TabIncludes)");
   buffer.AppendFormatted(" $(BaseTabIncludes)");
   buffer.AppendFormatted(" $(FolderIncludes)");
   buffer.AppendFormatted(" $(BaseFolderIncludes)");
   buffer.AppendFormatted(" $(ARGUSSYS)/include/ArgusMonitor.h $(ARGUSSYS)/include/ArgusWindow.h include/framework/%sMonitor.h  include/framework/%sWindow.h $(DictionaryHeaders)\n",shortCut.Data(),shortCut.Data());
   dictionarybat.Remove(dictionarybat.Length()-1);
   buffer.AppendFormatted("	%s $(DictionaryHeaders)\n",dictionarybat.Data());
   buffer.AppendFormatted("\n");

// Clean and build
   buffer.AppendFormatted("clean:\n");
   buffer.AppendFormatted("	rm -f obj/*.obj src/framework/%sDict.cpp src/framework/%sDict.h\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");
   Int_t pdnameend = 0;
   Int_t pbnamestart = 0;
   ROMEString xmlfilename = xmlFile;
   while((pdnameend = xmlfilename.Index("/",1,pbnamestart,TString::kExact))!=-1)
      pbnamestart = pdnameend+1;
   ROMEString xmlbasename = xmlfilename(pbnamestart,xmlfilename.Length());
   buffer.AppendFormatted("build:\n");
   buffer.AppendFormatted("	$(ARGUSSYS)/bin/argusbuilder");
#if defined ( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted(".exe");
#endif
   buffer.AppendFormatted(" -i %s -o .",xmlbasename.Data());
   if (makeOutput)
      buffer.AppendFormatted(" -v");
   if(noLink)
      buffer.AppendFormatted(" -nl");
   if(midas)
      buffer.AppendFormatted(" -midas");
   if(mysql)
      buffer.AppendFormatted(" -mysql");
   if(pgsql)
      buffer.AppendFormatted(" -pgsql");
   if(sqlite)
      buffer.AppendFormatted(" -sqlite");
   if(sqlite3)
      buffer.AppendFormatted(" -sqlite3");
   buffer.AppendFormatted("\n");

   ROMEString makeFile;
#if defined( R__UNIX )
   makeFile = "Makefile";
#endif
#if defined ( R__VISUAL_CPLUSPLUS )
   makeFile = "Makefile.win";
#endif
   WriteFile(makeFile.Data(),buffer.Data(),0);

// Write Makefile.usr
   struct stat buf;
   makeFile = "Makefile.usr";
   ROMEString usrBuffer;
   if( stat( makeFile.Data(), &buf )) {
      usrBuffer.SetFormatted("# User editable Makefile for the %s%s\n",shortcut.Data(),mainprogname.Data());
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("# Description:\n");
      usrBuffer.AppendFormatted("# 1) Add compile(link) options to Flags(Libraries), e.g. Flags += -g -O2 -Wall\n");
      usrBuffer.AppendFormatted("# 2) Add mySource.obj to the list of objects, e.g. objects += mySource.obj\n");
      usrBuffer.AppendFormatted("# 3) Add compile statment, e.g.\n");
      usrBuffer.AppendFormatted("#       obj/mySource.obj: mySource.cpp\n");
      usrBuffer.AppendFormatted("#          g++ -c $(Flags) $(Includes) mySource.cpp -o obj/mySource.obj\n");
      usrBuffer.AppendFormatted("# 4) Add include paths for the rootcint, e.g. DictionaryIncludes += -ImyPath\n");
      usrBuffer.AppendFormatted("# 5) Add header files for the rootcint, e.g. DictionaryHeaders += myHeader.h/\n");
      usrBuffer.AppendFormatted("#\n");
      WriteFile(makeFile.Data(),usrBuffer.Data(),0);
   }
}


void ArgusBuilder::WriteDictionaryBat(ROMEString& buffer) 
{
   // writes a script file that executes rootcint
   ROMEString noInclude;
   Int_t i;
   buffer.Resize(0);

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("rootcint -f src/framework/%sDict.cpp -c ",shortCut.Data());
   buffer.AppendFormatted("-I%%ARGUSSYS%%/include ");
   buffer.AppendFormatted("-I%%ROMESYS%%/include ");
   buffer.AppendFormatted("-I%%ROOTSYS%%/include ");
   buffer.AppendFormatted("$(DictionaryIncludes) ");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint -f src/framework/%sDict.cpp -c ",shortCut.Data());
   buffer.AppendFormatted("-I$ARGUSSYS/include ");
   buffer.AppendFormatted("-I$ROMESYS/include ");
   buffer.AppendFormatted("-I$ROOTSYS/include ");
   buffer.AppendFormatted("$(DictionaryIncludes) ");
#endif
   buffer.AppendFormatted("-Iinclude -Iinclude/tabs -Iinclude/framework ");
   buffer.AppendFormatted("ArgusMonitor.h ArgusWindow.h ArgusTextDialog.h ArgusAnalyzerController.h TNetFolder.h include/framework/%sMonitor.h include/framework/%sWindow.h ",shortCut.Data(),shortCut.Data());

   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderUserCode[i])
            buffer.AppendFormatted("include/framework/%s%s_Base.h ",shortCut.Data(),folderName[i].Data());
         else
            buffer.AppendFormatted("include/framework/%s%s.h ",shortCut.Data(),folderName[i].Data());
      }
   }

   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("include/tabs/%sT%s_Base.h ",shortCut.Data(),tabName[i].Data());
      buffer.AppendFormatted("include/tabs/%sT%s.h ",shortCut.Data(),tabName[i].Data());
   }
   buffer.Append("\0");

#if defined( R__VISUAL_CPLUSPLUS )
   ROMEString batFile;
   batFile.SetFormatted("%smakeDictionary.bat",outDir.Data());
   WriteFile(batFile.Data(),buffer.Data(),0);
#endif
}


void ArgusBuilder::WriteHTMLDoku() {
   Int_t i=0,j=0,k=0;
   ROMEString buffer;
   ROMEString parentt;
   Int_t depthold=0;
   Int_t depth=0;

   // Header
   buffer.Resize(0);
   buffer.AppendFormatted("<HTML>\n");
   buffer.AppendFormatted("<HEAD>\n");
   buffer.AppendFormatted("<TITLE>%s%s Manual</TITLE>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("</HEAD>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\">\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<H1>%s%s Manual</H1>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");

   // Table of Contents
   buffer.AppendFormatted("<H2>Table of Contents</H2>\n");
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#introduction\">Introduction</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#objects\">Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#tabobjects\">Tabs</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#folderobjects\">Folders</a></li>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<li><a href=\"#accessmethods\">Access Methods to Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"%s/htmldoc/ClassIndex.html\">Class Overview</A></li>\n",outDir.Data());
   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"%s/%sUserHTML.html\">Additional Info</A></li>\n",outDir.Data(),shortCut.Data());
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");

   // Introduction
   buffer.AppendFormatted("<H2><a name=introduction>Introduction</a> </H2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%s\n",mainDescription.Data());
   buffer.AppendFormatted("<p>\n");
   // Objects
   buffer.AppendFormatted("<H2><a name=objects>Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("All <a href=\"#tabobjects\">Tabs</a>, <a href=\"#folderobjects\">Folders</a> are described here.\n");

   // Tabs
   buffer.AppendFormatted("<h3><a name=tabobjects>Tabs</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s has the following tab hierarchy :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   Int_t index;
   for (i=0;i<numOfTab;i++) {
      index = i;
      depth=0;
      while (index!=-1) {
         depth++;
         index = tabParentIndex[index];
      }
      if (depth<depthold) buffer.AppendFormatted("</ul>\n");
      if (depth>depthold) buffer.AppendFormatted("<ul>\n");
      buffer.AppendFormatted("<li type=\"circle\"><h4><a href=\"#%s\">%sT%s</a></h4></li>\n",tabName[i].Data(),shortCut.Data(),tabName[i].Data());
      depthold = depth;
   }
   for (i=0;i<depth;i++) buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<p><b>Note</b> : The user should write code into the Init() methods of the tabs methods file (.cpp). But the user may not add code to or alter code in the class file (_Base.h). The base class file (_Base.h) is generated by the builder. Manual changes to this file will allways be overwritten.\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("In the following all tabs will be described.\n");
   buffer.AppendFormatted("<p>\n");
   ROMEString cppFile;
   ROMEString str;
   fstream *fileStream;
   ROMEString fileBuffer;

   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("<h4><a name=%s><u>%s</u></a></h4>\n",tabName[i].Data(),tabName[i].Data());
      buffer.AppendFormatted("%s<p>\n",tabDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      cppFile.SetFormatted("%s/src/tabs/%sT%s.cpp",outDir.Data(),shortCut.Data(),tabName[i].Data());
      fileStream = new fstream(cppFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      buffer.AppendFormatted("%s accesses data from the following folders :\n",tabName[i].Data());
      buffer.AppendFormatted("<ul>\n");
      for (j=0;j<numOfFolder;j++) {
         str = "Get";
         str.Append(folderName[j]);
         if (folderArray[j]=="1")
            str += "(";
         else
            str += "At(";
         if (fileBuffer.Contains(str)) {
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[j].Data());
         }
      }
      buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("For more information take a look at the <A TARGET=_top HREF=\"%s/htmldoc/%sT%s.html\">class file</a>\n",outDir.Data(),shortCut.Data(),tabName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("\n");
   }
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Folders
   buffer.AppendFormatted("<h3><a name=folderobjects>Folders</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following folders :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   depthold=0;
   depth=0;
   for (i=0;i<numOfFolder;i++) {
      depth=0;
      if (folderParentName[i]!="GetMainFolder()") {
         depth++;
         parentt = folderParentName[i];
         for (j=0;j<100;j++) {
            for (k=0;k<numOfFolder;k++) {
               if (parentt==folderName[k]) {
                  parentt = folderParentName[k];
                  break;
               }
            }
            if (k>=numOfFolder) {
               cout << "Invalid folder structure." << endl;
               return;
            }
            if (folderParentName[k]=="GetMainFolder()")
               break;
            depth++;
         }
      }
      if (depth<depthold) buffer.AppendFormatted("</ul>\n");
      if (depth>depthold) buffer.AppendFormatted("<ul>\n");
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted("<b>\n");
         buffer.AppendFormatted("<li type=\"circle\"><a href=\"#%s\">%s</a></li>\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("</b>\n");
      }
      else {
         buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[i].Data());
      }
      depthold = depth;
   }
   for (i=0;i<depth;i++) buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("The bold folders are data folders. The others are only used to structure the framework.\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("In the following all folders will be described.\n");
   buffer.AppendFormatted("<p>\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] <= 0) continue;
      buffer.AppendFormatted("<h4><a name=%s><u>%s</u></a></h4>\n",folderName[i].Data(),folderName[i].Data());
      buffer.AppendFormatted("%s\n",folderDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<u>Fields</u>\n");
      buffer.AppendFormatted("<table border=\"1\">\n");
      buffer.AppendFormatted("<tr><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      for (j=0;j<numOfValue[i];j++) {
         ROMEString comment = valueComment[i][j];
         if (valueComment[i][j].Length()>3) {
            if (valueComment[i][j][0]=='/') {
               comment = valueComment[i][j](3,valueComment[i][j].Length()-3);
            }
         }
         buffer.AppendFormatted("<tr><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",valueName[i][j].Data(),valueType[i][j].Data(),comment.Data());
      }
      buffer.AppendFormatted("</table>\n");
   }
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Access Methods
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<H2><a name=accessmethods>Access Methods to Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("For a description of all access methods to all types of objects in the %s%s please visit the\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<A TARGET=_top HREF=\"http://midas.psi.ch/argus/user_guide/index.html\">users guide on the ARGUS homepage</A>.\n");
   buffer.AppendFormatted("</p>\n");
   // Footer
   buffer.AppendFormatted("<HR>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ADDRESS>\n");
   buffer.AppendFormatted("<u> Contact person from the %s Experiment</u></br>\n",shortCut.Data());
   buffer.AppendFormatted("%s</br>\n",mainAuthor.Data());
   buffer.AppendFormatted("%s</br>\n",mainInstitute.Data());
   buffer.AppendFormatted("%s</br>\n",mainCollaboration.Data());
   buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail.Data(),mainEmail.Data());
   buffer.AppendFormatted("<u> Contact person from Argus</u></br>\n");
   buffer.AppendFormatted("<a href=\"http://www.icepp.s.u-tokyo.ac.jp/%%E7sawada\">Ryu Sawada</a> (ICEPP)</p>\n");
   buffer.AppendFormatted("</ADDRESS>\n");
   buffer.AppendFormatted("</BODY>\n");
   buffer.AppendFormatted("</HTML>\n");

   // Write documentation
   ROMEString htmlFile;
   htmlFile.SetFormatted("%s%s%s.html",outDir.Data(),shortCut.Data(),mainProgName.Data());
   WriteFile(htmlFile.Data(),buffer.Data(),0);

   // Write UserHTML
   struct stat buf;
   htmlFile.SetFormatted("%s%sUserHTML.html",outDir.Data(),shortCut.Data());
   if( stat( htmlFile.Data(), &buf )) {
      buffer.Resize(0);
      buffer.AppendFormatted("<html>\n");
      buffer.AppendFormatted("<head>\n");
      buffer.AppendFormatted("  <title>%s%s Additional Info</title>\n",shortCut.Data(),mainProgName.Data());
      buffer.AppendFormatted("</head>\n");
      buffer.AppendFormatted("<body>\n");
      buffer.AppendFormatted("  <h1>Additional Info</h1>\n");
      buffer.AppendFormatted("  <a href=\"%s%s.html\">Main page</a>\n",shortCut.Data(),mainProgName.Data());
      buffer.AppendFormatted("  <hr width=\"100%%\">\n");
      buffer.AppendFormatted("\n\n\n\n");
      buffer.AppendFormatted("  <hr width=\"100%%\">\n");
      buffer.AppendFormatted("<address>\n");
      buffer.AppendFormatted("<u> Contact person from the %s Experiment</u></br>\n",shortCut.Data());
      buffer.AppendFormatted("%s</br>\n",mainAuthor.Data());
      buffer.AppendFormatted("%s</br>\n",mainInstitute.Data());
      buffer.AppendFormatted("%s</br>\n",mainCollaboration.Data());
      buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail.Data(),mainEmail.Data());
      buffer.AppendFormatted("<u> Contact person from ARGUS</u></br>\n");
      buffer.AppendFormatted("Ryu Sawada</br>\n");
      buffer.AppendFormatted("<a href=\"http://www.icepp.s.u-tokyo.ac.jp/%%E7sawada\">Ryu Sawada</a> (ICEPP)</p>\n");
      buffer.AppendFormatted("</address>\n");
      buffer.AppendFormatted("</body>\n");
      buffer.AppendFormatted("</html>");                             
      WriteFile(htmlFile.Data(),buffer.Data(),0);
   }
}


void ArgusBuilder::GetMidasTID(ROMEString* buf,Char_t *type) 
{
   buf->Resize(0);
   if (!strcmp(type,"Byte_t") ||
       !strcmp(type,"BYTE"))
      buf->Append("TID_BYTE");     // < unsigned byte         0       255   
   else if (!strcmp(type,"Char_t") ||
            !strcmp(type,"Text_t") ||
            !strcmp(type,"char"))
      buf->Append("TID_SBYTE");    //< signed byte         -128      127   
   else if (!strcmp(type,"UChar_t") ||
            !strcmp(type,"unsigned char"))
      buf->Append("TID_CHAR");     //< single character      0       255   
   else if (!strcmp(type,"UShort_t") ||
            !strcmp(type,"WORD") ||
            !strcmp(type,"unsigned short") ||
            !strcmp(type,"unsigned short int"))
      buf->Append("TID_WORD");     //< two bytes             0      65535   
   else if (!strcmp(type,"Short_t") ||
            !strcmp(type,"Version_t") ||
            !strcmp(type,"short") ||
            !strcmp(type,"short int"))
      buf->Append("TID_SHORT");    //< signed word        -32768    32767   
   else if (!strcmp(type,"UInt_t") ||
            !strcmp(type,"DWORD") ||
            !strcmp(type,"unsigned int") ||
            !strcmp(type,"unsigned long") ||
            !strcmp(type,"unsigned long int"))
      buf->Append("TID_DWORD");    //< four bytes            0      2^32-1   
   else if (!strcmp(type,"Int_t") ||
            !strcmp(type,"INT") ||
            !strcmp(type,"Ssize_t") ||
            !strcmp(type,"int") ||
            !strcmp(type,"long") ||
            !strcmp(type,"long int"))
      buf->Append("TID_INT");      //< signed dword        -2^31    2^31-1   
   else if (!strcmp(type,"Bool_t") ||
            !strcmp(type,"BOOL") ||
            !strcmp(type,"bool"))
      buf->Append("TID_BOOL");     //< four bytes bool       0        1   
   else if (!strcmp(type,"Float_t") ||
            !strcmp(type,"Real_t") ||
            !strcmp(type,"float"))
      buf->Append("TID_FLOAT");    //< 4 Byte float format   
   else if (!strcmp(type,"Double_t") ||
            !strcmp(type,"Double32_t") ||
            !strcmp(type,"double"))
      buf->Append("TID_DOUBLE");   //< 8 Byte float format   
#if defined ( R__B64 ) // Note: Long_t and ULong_t are currently not portable types
   else if (!strcmp(type,"Long_t") ||
            !strcmp(type,"ULong_t")){
      cout<<"WARNING: Long_t and ULong_t is not supported on this architecture."<<endl;
      buf->Append("TID_DOUBLE");   //< 8 Byte float format
   }
#else
   else if (!strcmp(type,"Long_t"))
      buf->Append("TID_INT");   
   else if (!strcmp(type,"ULong_t"))
      buf->Append("TID_DWORD");
#endif
   else
      buf->Append("TID_STRING");   //< zero terminated string
}


Bool_t ArgusBuilder::ReplaceHeader(const char* filename,const char* header,const char* content,int nspace) {
   ROMEString hd = header;
   ROMEString ct = content;

   ROME2Argus(hd);
   ROME2Argus(ct);

   return ROMEBuilder::ReplaceHeader(filename,hd.Data(),ct.Data(),nspace);
}


Bool_t ArgusBuilder::WriteFile(const char* filename,const char* content,int nspace) {
   ROMEString ct = content;

   ROME2Argus(ct);

   return ROMEBuilder::WriteFile(filename,ct.Data(),nspace);
}


void ArgusBuilder::ROME2Argus(ROMEString &buffer) {
   ROMEString rome;
   ROMEString argus;

   rome.SetFormatted("%sAnalyzer",shortCut.Data());
   argus.SetFormatted("%sMonitor",shortCut.Data());
   buffer.ReplaceAll(rome,argus);

   buffer.ReplaceAll("ROMEBuilder","ArgusBuilder");
   buffer.ReplaceAll("gAnalyzer","gMonitor");
}
