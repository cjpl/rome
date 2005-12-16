/********************************************************************************

  ArgusBuilder.cpp, Ryu Sawada

  $Id$

********************************************************************************/
#include "ArgusBuilder.h"

//______________________________________________________________________________
// ArgusBuilder


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteMain()
{
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString mainprogname(mainProgName);
   cppFile.SetFormatted("%s/src/framework/main.cpp", outDir.Data());
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//          *** This file will be overwritten by the ArgusBuilder ***         //\n");
   buffer.AppendFormatted("//          ***      Don't make manual changes to this file       ***         //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   buffer.AppendFormatted("#include <TRint.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include \"include/framework/%sMonitor.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sWindow.h\"\n", shortCut.Data());
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

   buffer.AppendFormatted("   TApplication *app = new TApplication(\"App\", &argn, &argp,NULL,0);\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   // Monitor\n");
   buffer.AppendFormatted("   gMonitor = new %sMonitor(app);\n", shortCut.Data());
   buffer.AppendFormatted("   // Main window\n");
   buffer.AppendFormatted("   Char_t str[80];\n");
   buffer.AppendFormatted("   sprintf(str, \"%%s\", gMonitor->GetProgramName());\n");
   buffer.AppendFormatted("   gWindow = new %sWindow(gClient->GetRoot(), str);\n", shortCut.Data());
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
   WriteFile(cppFile.Data(), buffer.Data(), 6);

   return kTRUE;
}


//______________________________________________________________________________
void ArgusBuilder::StartBuilder()
{
   bool inputok = false;

   xml = new ROMEXML();
   Char_t *name;
   Bool_t finished = kFALSE;
   Int_t type;
   Int_t i, j;
   experimentName = "";
   styleSheet = "";
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
   readTrees = kFALSE;
   readGlobalSteeringParameters = kFALSE;
   readMidasBanks = kFALSE;

   numOfFolder = -1;
   numOfTab = numOfTask = numOfTaskHierarchy = -1;
   numOfNetFolder = -1;
   numOfTree = -1;
   numOfEvent = -1;

   TString::MaxWaste(kTStringResizeIncrement - 1);
   TString::ResizeIncrement(kTStringResizeIncrement);

   if (!xml->OpenFileForRead(xmlFile.Data()))
      return;
   while (xml->NextLine() && !finished) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp(name, "ArgusMonitorDefinition")) {
         inputok = true;
         while (xml->NextLine() && !finished) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 15 && !strcmp(name, "ArgusMonitorDefinition")) {
               if (!readExperiment) {
                  cout << "Experiment tree missing!" << endl;
                  cout << "Terminating program." << endl;
                  return;
               }
               finished = kTRUE;
               break;
            }

            if (type == 1) {
               if (!strcmp(name, "Experiment")) {
                  readExperiment = kTRUE;
                  while (xml->NextLine() && !finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp(name, "ExperimentName"))
                        xml->GetValue(experimentName, experimentName);
                     if (type == 1 && !strcmp(name, "ExperimentShortCut"))
                        xml->GetValue(shortCut, shortCut);
                     if (type == 1 && !strcmp(name, "ProgramName"))
                        xml->GetValue(mainProgName, mainProgName);
                     if (type == 1 && !strcmp(name, "MonitorDescription"))
                        xml->GetValue(mainDescription, mainDescription);
                     if (type == 1 && !strcmp(name, "DocumentStyleSheet"))
                        xml->GetValue(styleSheet, styleSheet);
                     if (type == 15 && !strcmp(name, "Experiment")) {
                        if (shortCut == "") {
                           cout << "Experiment must have a shortcut!" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                        break;
                     }
                  }
                  continue;
               }

               if (!strcmp(name, "Author")) {
                  readAuthor = kTRUE;
                  while (xml->NextLine() && !finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp(name, "AuthorName"))
                        xml->GetValue(mainAuthor, mainAuthor);
                     if (type == 1 && !strcmp(name, "AuthorInstitute"))
                        xml->GetValue(mainInstitute, mainInstitute);
                     if (type == 1 && !strcmp(name, "AuthorCollaboration"))
                        xml->GetValue(mainCollaboration, mainCollaboration);
                     if (type == 1 && !strcmp(name, "AuthorEmail"))
                        xml->GetValue(mainEmail, mainEmail);
                     if (type == 15 && !strcmp(name, "Author"))
                        break;
                  }
                  continue;
               }

               if (!strcmp(name, "Folders")) {
                  // initialization
                  numOfFolder = -1;
                  parent[0] = "GetMainFolder()";
                  // output
                  if (makeOutput)
                     cout << "Folders:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // folder
                     if (type == 1 && !strcmp(name, "Folder")) {
                        folderSupport[numOfFolder + 1] = false;
                        recursiveDepth = 0;
                        if (!ReadXMLFolder())
                           return;
                     }
                     // folders end
                     if (type == 15 && !strcmp(name, "Folders")) {
                        break;
                     }
                  }
                  // check input
                  for (i = 0; i < numOfFolder; i++) {
                     for (j = i + 1; j < numOfFolder; j++) {
                        if (folderName[i] == folderName[j]) {
                           cout << "\nFolder '" << folderName[i].Data() << "' is defined twice !" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                     }
                  }
                  // count folders
                  numOfFolder++;
               }

               if (!strcmp(name, "Tabs")) {
                  // initialization
                  numOfTab = numOfTask = numOfTaskHierarchy = -1;
                  // output
                  if (makeOutput)
                     cout << "\n\nTabs:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // tab
                     if (type == 1 && !strcmp(name, "Tab")) {
                        tabParentIndex[numOfTab + 1] = taskHierarchyParentIndex[numOfTab + 1] = -1;
                        recursiveTabDepth = 0;
                        if (!ReadXMLTab())
                           return;
                     }
                     // tabs end
                     if (type == 15 && !strcmp(name, "Tabs")) {
                        break;
                     }
                  }
                  // check input
                  for (i = 0; i < numOfTab; i++) {
                     for (j = i + 1; j < numOfTab; j++) {
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
                  for (i = 0; i < numOfTab; i++)
                     numOfThreadFunctions[i]++;
               }

               if (!strcmp(name, "NetFolders")) {
                  // initialization
                  numOfNetFolder = -1;
                  // output
                  if (makeOutput)
                     cout << "\n\nNetFolders:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // net object
                     if (type == 1 && !strcmp(name, "NetFolder")) {
                        if (!ReadXMLNetFolder())
                           return;
                     }
                     // net folders end
                     if (type == 15 && !strcmp(name, "NetFolders")) {
                        break;
                     }
                  }
                  // check input
                  for (i = 0; i < numOfNetFolder; i++) {
                     for (j = i + 1; j < numOfNetFolder; j++) {
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

               if (!strcmp(name, "GlobalSteeringParameters")) {
                  // output
                  if (makeOutput)
                     cout << "\n\nGlobal Steering Parameters:" << endl;
                  // initialisation
                  steerName[numOfTab][0] = "GlobalSteering";
                  actualSteerIndex = 0;
                  recursiveSteerDepth = 0;
                  steerParent[numOfTab][0] = -1;
                  numOfSteering[numOfTab] = -1;
                  if (!ReadXMLSteering(numOfTab))
                     return;
                  numOfSteering[numOfTab]++;
               }

               if (!strcmp(name, "Trees")) {
                  numOfTree = -1;
                  if (!ReadXMLTree())
                     return;
               }

               if (!strcmp(name, "UserDAQSystems")) {
                  numOfDAQ = -1;
                  if (!ReadXMLDAQ())
                     return;
               }

               if (!strcmp(name, "UserDatabases")) {
                  numOfDB = -1;
                  if (!ReadXMLDB())
                     return;
               }

               if (!strcmp(name, "MidasBanks")) {
                  numOfEvent = -1;
                  if (!ReadXMLMidasBanks())
                     return;
               }

               if (!strcmp(name, "UserMakeFile")) {
                  if (!ReadXMLUserMakefile())
                     return;
               }
            }
         }
      }
   }

   if (!inputok) {
      cout << "\nCould not find the <ArgusMonitorDefinition> tag in the inputfile '"<< xmlFile.Data() << "'." << endl;
      cout << "Terminating the ArgusBuilder." << endl;
      return;
   }
   if (makeOutput)
      cout << "\n\nFramework:" << endl;
   if (makeOutput)
      cout << "\n\nMonitor :" << endl;
   if (!WriteMonitorCpp())
      return;
   if (!WriteMonitorH())
      return;
   if (makeOutput)
      cout << "\n\nWindow :" << endl;
   if (!WriteWindowCpp())
      return;
   if (!WriteWindowH())
      return;
   if (makeOutput)
      cout << "\n\nFolders:" << endl;
   if (!WriteFolderCpp())
      return;
   if (!WriteFolderH())
      return;
   if (makeOutput)
      cout << "\n\nTabs:" << endl;
   if (!WriteTabCpp())
      return;
   if (!WriteTabH())
      return;
   if (makeOutput)
      cout << "\n\nUser DAQ Systems:" << endl;
   if (!WriteDAQCpp())
      return;
   if (!WriteDAQH())
      return;
   if (makeOutput)
      cout << "\n\nUser Database Interfaces:" << endl;
   if (!WriteDBCpp())
      return;
   if (!WriteDBH())
      return;
   if (!WriteSteering(numOfTab))
      return;
   if (!WriteConfigCpp())
      return;
   if (!WriteConfigH())
      return;
   if (!WriteMidasDAQCpp())
      return;
   if (!WriteMidasDAQH())
      return;
   if (!WriteRomeDAQCpp())
      return;
   if (!WriteRomeDAQH())
      return;
   if (!WriteDataBaseDAQCpp()) 
      return;
   if (!WriteDataBaseDAQH()) 
      return;
   if (!WriteMain())
      return;

   delete xml;
   ROMEString buffer;

   chdir(outDir.Data());

   // Linking
   if (makeOutput && !noLink)
      cout << "\nLinking " << shortCut.Data() << " Project." << endl;
   WriteMakefile();
   if (noLink) {
      ROMEString tempStr;
#if defined( R__UNIX )
      tempStr.SetFormatted("make -e dict/ROMEDict.cpp dict/ARGUSDict.cpp dict/%sFrameworkDict.cpp dict/%sFolderDict.cpp dict/%sTabDict.cpp dict/%sUserDict.cpp", shortCut.Data(), shortCut.Data(), shortCut.Data(), shortCut.Data());
      system(tempStr.Data());
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      const int workDirLen = 1000;
      char workDir[workDirLen];
      getcwd(workDir,workDirLen);
      tempStr.SetFormatted("nmake -f Makefile.win dict/ROMEDict.cpp dict/ARGUSDict.cpp dict/%sFrameworkDict.cpp dict/%sFolderDict.cpp dict/%sTabDict.cpp dict/%sUserDict.cpp", shortCut.Data(), shortCut.Data(), shortCut.Data(), shortCut.Data());
      system(tempStr.Data());
#endif
   }
   else {
#if defined( R__UNIX )
      system("make -e");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      const int workDirLen = 1000;
      char workDir[workDirLen];
      getcwd(workDir,workDirLen);
      system("nmake -f Makefile.win");
#endif
   }
   // Documentation
   if (makeOutput)
      cout << "\nWrite HTML Documentation." << endl;
   WriteHTMLDoku();
}


//______________________________________________________________________________
void ArgusBuilder::WriteMakefileHeader(ROMEString& buffer) {
   ROMEString tmp;
   buffer.AppendFormatted("################################################################################\n");
   buffer.AppendFormatted("##          *** This file will be overwritten by the ArgusBuilder ***         ##\n");
   buffer.AppendFormatted("##          ***      Don't make manual changes to this file       ***         ##\n");
   buffer.AppendFormatted("################################################################################\n");
   buffer.AppendFormatted("#\n");
   buffer.AppendFormatted("# make        : compile executable\n");
   buffer.AppendFormatted("# make build  : execute argusbuilder\n");
   buffer.AppendFormatted("# make clean  : remove intermediate files\n");
   buffer.AppendFormatted("# make %sclean: remove %s specific intermediate files\n", shortCut.ToLower(tmp), shortCut.Data());
   buffer.AppendFormatted("\n");
}


//______________________________________________________________________________
void ArgusBuilder::WriteMakefileLibsAndFlags(ROMEString& buffer) {
   int i;
   ROMEString tmp;
#if defined( R__VISUAL_CPLUSPLUS )
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
   buffer.AppendFormatted("Libraries = $(rootlibs) $(clibs) $(sqllibs) $(midaslibs)");
   for (i = 0; i < numOfMFWinLibs; i++)
      buffer.AppendFormatted(" %s", mfWinLibName[i].Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   // flags
   buffer.AppendFormatted("Flags = /GX /GR $(%suserflags)", shortCut.ToLower(tmp));
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
   for (i = 0; i < this->flags.GetEntriesFast(); i++)
      buffer.AppendFormatted(" /D%s", this->flags.At(i).Data());
   for (i = 0; i < numOfMFPreDefs; i++)
      buffer.AppendFormatted(" /D%s", mfPreDefName[i].Data());
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
#elif defined( R__SGI )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs :=\n");
#elif defined( R__FBSD )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lbsd -lcompat\n");
#elif defined( R__MACOSX )
   buffer.AppendFormatted("FINK_DIR := $(shell which fink 2>&1 | sed -ne \"s/\\/bin\\/fink//p\")\n");
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function  $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)\n");
   buffer.AppendFormatted("oslibs := -lpthread -multiply_defined suppress $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib)\n");
#elif defined( R__LINUX )
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function\n");
   buffer.AppendFormatted("oslibs := -lutil -lpthread\n");
#elif defined( R__SOLARIS )
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
#elif defined( R__SGI )
      buffer.AppendFormatted("midascflags := -DOS_ULTRIX -DNO_PTY -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/ultrix/lib -lmidas\n");
#elif defined( R__FBSD )
      buffer.AppendFormatted("midascflags := -DOS_FREEBSD -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/freeBSD/lib -lmidas\n");
#elif defined( R__MACOSX )
      buffer.AppendFormatted("midascflags := -DOS_LINUX -DOS_DARWIN -DHAVE_STRLCPY -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/darwin/lib -lmidas\n");
#elif defined( R__LINUX )
      buffer.AppendFormatted("midascflags := -DOS_LINUX -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/linux/lib -lmidas\n");
#elif defined( R__SOLARIS )
      buffer.AppendFormatted("midascflags := -DOS_SOLARIS -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/solaris/lib -lmidas\n");
#else
      buffer.AppendFormatted("midascflags := -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -lmidas\n");
#endif
   }
   else {
      buffer.AppendFormatted("midaslibs := \n");
      buffer.AppendFormatted("midascflags := \n");
   }
   buffer.AppendFormatted("clibs :=-lHtml $(SYSLIBS)");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   // libs
   buffer.AppendFormatted("Libraries := $(oslibs) $(rootglibs) $(rootthreadlibs) $(clibs) $(sqllibs) $(midaslibs)");
   for (i = 0; i < numOfMFUnixLibs; i++)
      buffer.AppendFormatted(" -l%s", mfUnixLibName[i].Data());
   buffer.AppendFormatted("\n");

   // flags
   buffer.AppendFormatted("Flags := $(%suserflags) $(oscflags) $(rootcflags) $(sqlcflags) $(midascflags)",shortCut.ToLower(tmp));
   for (i = 0; i < numOfMFPreDefs; i++)
      buffer.AppendFormatted(" -D%s", mfPreDefName[i].Data());
   buffer.AppendFormatted("\n");
#endif
}


//______________________________________________________________________________
void ArgusBuilder::AddArgusHeaders() {
   argusHeaders = new ROMEStrArray(6);
   argusHeaders->Add("$(ARGUSSYS)/include/ArgusAnalyzerController.h");
   argusHeaders->Add("$(ARGUSSYS)/include/ArgusMonitor.h");
   argusHeaders->Add("$(ARGUSSYS)/include/ArgusTextDialog.h");
//   argusHeaders->Add("$(ARGUSSYS)/include/ArgusVersion.h");
   argusHeaders->Add("$(ARGUSSYS)/include/ArgusWindow.h");
   argusHeaders->Add("$(ARGUSSYS)/include/TNetFolder.h");
}


//______________________________________________________________________________
void ArgusBuilder::AddArgusSources(){
   argusSources = new ROMEStrArray(6);
   argusSources->AddFormatted("$(ARGUSSYS)/src/ArgusAnalyzerController.cpp");
   argusSources->AddFormatted("$(ARGUSSYS)/src/ArgusMonitor.cpp");
   argusSources->AddFormatted("$(ARGUSSYS)/src/ArgusTextDialog.cpp");
   argusSources->AddFormatted("$(ARGUSSYS)/src/ArgusWindow.cpp");
   argusSources->AddFormatted("$(ARGUSSYS)/src/TNetFolder.cpp");
   if (argusHeaders->GetEntriesFast()>0)
      argusSources->AddFormatted("%sdict/ARGUSDict.cpp",outDir.Data(),shortCut.Data());
}


//______________________________________________________________________________
void ArgusBuilder::AddTabHeaders() {
   int i;
   tabHeaders = new ROMEStrArray(numOfTab*2);
   for (i=0;i<numOfTab;i++) {
      tabHeaders->AddFormatted("%sinclude/tabs/%sT%s.h",outDir.Data(),shortCut.Data(),tabName[i].Data());
      tabHeaders->AddFormatted("%sinclude/tabs/%sT%s_Base.h",outDir.Data(),shortCut.Data(),tabName[i].Data());
   }
}


//______________________________________________________________________________
void ArgusBuilder::AddTabSources(){
   int i;
   tabSources = new ROMEStrArray(numOfTab+1);
   for (i=0;i<numOfTab;i++) {
      tabSources->AddFormatted("%ssrc/tabs/%sT%s.cpp",outDir.Data(),shortCut.Data(),tabName[i].Data());
   }
   if (tabHeaders->GetEntriesFast()>0)
      tabSources->AddFormatted("%sdict/%sTabDict.cpp",outDir.Data(),shortCut.Data());
}


//______________________________________________________________________________
void ArgusBuilder::AddFrameworkHeaders() {
   frameworkHeaders = new ROMEStrArray(4);
   frameworkHeaders->AddFormatted("%sinclude/framework/%sConfig.h",outDir.Data(),shortCut.Data());
   frameworkHeaders->AddFormatted("%sinclude/framework/%sGlobalSteering.h",outDir.Data(),shortCut.Data());
   frameworkHeaders->AddFormatted("%sinclude/framework/%sMonitor.h",outDir.Data(),shortCut.Data());
   frameworkHeaders->AddFormatted("%sinclude/framework/%sWindow.h",outDir.Data(),shortCut.Data());
   frameworkHeaders->AddFormatted("%sinclude/framework/%sMidasDAQ.h",outDir.Data(),shortCut.Data());
   frameworkHeaders->AddFormatted("%sinclude/framework/%sRomeDAQ.h",outDir.Data(),shortCut.Data());
   frameworkHeaders->AddFormatted("%sinclude/framework/%sDataBaseDAQ.h",outDir.Data(),shortCut.Data());
}


//______________________________________________________________________________
void ArgusBuilder::AddFrameworkDictHeaders() {
   frameworkDictHeaders = new ROMEStrArray(4);
   frameworkDictHeaders->AddFormatted("%sinclude/framework/%sConfig.h",outDir.Data(),shortCut.Data());
   frameworkDictHeaders->AddFormatted("%sinclude/framework/%sGlobalSteering.h",outDir.Data(),shortCut.Data());
   frameworkDictHeaders->AddFormatted("%sinclude/framework/%sMonitor.h",outDir.Data(),shortCut.Data());
   frameworkDictHeaders->AddFormatted("%sinclude/framework/%sWindow.h",outDir.Data(),shortCut.Data());
}


//______________________________________________________________________________
void ArgusBuilder::AddFrameworkSources(){
   frameworkSources = new ROMEStrArray(5);
   frameworkSources->AddFormatted("%ssrc/framework/main.cpp",outDir.Data());
   frameworkSources->AddFormatted("%ssrc/framework/%sConfig.cpp",outDir.Data(),shortCut.Data());
   frameworkSources->AddFormatted("%ssrc/framework/%sMonitor.cpp",outDir.Data(),shortCut.Data());
   frameworkSources->AddFormatted("%ssrc/framework/%sWindow.cpp",outDir.Data(),shortCut.Data());
   frameworkSources->AddFormatted("%ssrc/framework/%sMidasDAQ.cpp",outDir.Data(),shortCut.Data());
   frameworkSources->AddFormatted("%ssrc/framework/%sRomeDAQ.cpp",outDir.Data(),shortCut.Data());
   frameworkSources->AddFormatted("%ssrc/framework/%sDataBaseDAQ.cpp",outDir.Data(),shortCut.Data());
   if (frameworkDictHeaders->GetEntriesFast()>0)
      frameworkSources->AddFormatted("%sdict/%sFrameworkDict.cpp",outDir.Data(),shortCut.Data());
}


//______________________________________________________________________________
void ArgusBuilder::WriteMakefile()
{
   // write a Makefile
   ROMEString buffer;
   ROMEString tmp;

   AddInludeDirectories();
   includeDirectories->AddFormatted("$(ARGUSSYS)/include/");
   AddRomeHeaders();
   AddRomeDictHeaders();
   AddArgusHeaders();
   AddFrameworkHeaders();
   AddFrameworkDictHeaders();
   AddFolderHeaders();
   AddTabHeaders();
   AddDAQHeaders();
   AddDatabaseHeaders();
   AddRomeSources();
   AddArgusSources();
   AddFrameworkSources();
   AddFolderSources();
   AddTabSources();
   AddDAQSources();
   AddDatabaseSources();

   WriteMakefileHeader(buffer);
   WriteMakefileLibsAndFlags(buffer);
   WriteMakefileIncludes(buffer);

   // root cint headers
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("DictionaryHeaders %s", kEqualSign);
   buffer.AppendFormatted("\n");

   // root cint includes
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("DictionaryIncludes %s", kEqualSign);
   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("LD_LIBRARY_PATH=$(ROOTSYS)/lib\n");
#endif

   // all
   buffer.AppendFormatted("all:obj dict blank.d %s%s", shortCut.ToLower(tmp), mainProgName.ToLower(tmp));
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted(".exe\n");
#endif
   buffer.AppendFormatted("\n");

// Objects
// -------
   WriteMakefileObjects(buffer,romeSources);
   WriteMakefileObjects(buffer,argusSources);
   WriteMakefileObjects(buffer,folderSources);
   WriteMakefileObjects(buffer,tabSources);
   WriteMakefileObjects(buffer,frameworkSources);
   WriteMakefileObjects(buffer,daqSources);
   WriteMakefileObjects(buffer,databaseSources);
   WriteMakefileAdditionalSourceFilesObjects(buffer);
   // user makefile
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("!INCLUDE Makefile.winusr\n");
#else
   buffer.AppendFormatted("-include Makefile.usr\n");
#endif
   buffer.AppendFormatted("\n");
   WriteMakefileUserDictObject(buffer);

   // make obj
   buffer.AppendFormatted("obj:\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("\t@mkdir obj\n\n");
#else
   buffer.AppendFormatted("\t@if [ ! -d  obj ] ; then \\\n");
   buffer.AppendFormatted("\t\techo \"Making directory obj\" ; \\\n");
   buffer.AppendFormatted("\t\tmkdir obj; \\\n");
   buffer.AppendFormatted("\tfi;\n");
#endif

   // make dict
   buffer.AppendFormatted("dict:\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("\t@mkdir dict\n\n");
#else
   buffer.AppendFormatted("\t@if [ ! -d  dict ] ; then \\\n");
   buffer.AppendFormatted("\t\techo \"Making directory dict\" ; \\\n");
   buffer.AppendFormatted("\t\tmkdir dict; \\\n");
   buffer.AppendFormatted("\tfi;\n");
#endif

// make blank
   buffer.AppendFormatted("blank.d:\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("\tcopy nul blank.d\n\n");
#endif // R__VISUAL_CPLUSPLUS

// Dictionary
   WriteMakefileDictionary(buffer,"ROMEDict",romeDictHeaders);
   WriteMakefileDictionary(buffer,"ARGUSDict",argusHeaders);
   WriteMakefileDictionary(buffer,shortCut+"FrameworkDict",frameworkDictHeaders);
   WriteMakefileDictionary(buffer,shortCut+"FolderDict",folderHeaders);
   WriteMakefileDictionary(buffer,shortCut+"TabDict",tabHeaders);
   WriteMakefileUserDictionary(buffer);


// Link Statement
// --------------
   buffer.AppendFormatted("%s%s", shortCut.ToLower(tmp), mainProgName.ToLower(tmp));
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted(".exe");
#endif
   buffer.AppendFormatted(": $(objects)\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("\tcl /Fe%s%s.exe $(objects) $(Libraries)\n\n", shortCut.Data(), mainProgName.Data());
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("\tg++ $(Flags) -o $@ $(objects) $(Libraries)\n\n");
#endif


   WriteMakefileCompileStatements(buffer,romeSources);
   WriteMakefileCompileStatements(buffer,argusSources);
   WriteMakefileCompileStatements(buffer,frameworkSources);
   WriteMakefileCompileStatements(buffer,tabSources);
   WriteMakefileCompileStatements(buffer,folderSources);
   WriteMakefileCompileStatements(buffer,daqSources);
   WriteMakefileCompileStatements(buffer,databaseSources);
   WriteMakefileAdditionalSourceFilesCompileStatments(buffer);
   ROMEStrArray *tempArray = new ROMEStrArray(1);
   tempArray->AddFormatted("dict/%sUserDict.cpp",shortCut.Data());
   WriteMakefileCompileStatements(buffer,tempArray);
   delete tempArray;

   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
//ng_a   buffer.AppendFormatted("!INCLUDE obj/*.d\n");
#else
   buffer.AppendFormatted("-include obj/*.d\n");
#endif

   // Clean and build
   buffer.AppendFormatted("clean: userclean\n");
   buffer.AppendFormatted("\t-rm -f obj/*.obj obj/*.d dict/*.h dict/*.cpp\n");
   buffer.AppendFormatted("%sclean:\n", shortCut.ToLower(tmp));
   buffer.AppendFormatted("\t-rm -f obj/%s*.obj obj/%s*.d dict/%s%.h dict/%s.cpp\n", shortCut.Data(), shortCut.Data(), shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
   WriteMakefileBuildRule(buffer,"$(ARGUSSYS)\\bin\\argusbuilder");
#else
   WriteMakefileBuildRule(buffer,"$(ARGUSSYS)/bin/argusbuilder");
#endif // R__VISUAL_CPLUSPLUS

   ROMEString makeFile;
#if defined( R__UNIX )
   makeFile = "Makefile";
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   makeFile = "Makefile.win";
#endif
   WriteFile(makeFile.Data(), buffer.Data(), 0);

   // Write Makefile.usr
   WriteUserMakeFile();

   // Write Visual C++ Projects
#if defined( R__VISUAL_CPLUSPLUS )
   if (!noVP)
      WriteVisualProjects(7,10);
#endif // R__VISUAL_CPLUSPLUS
}


//______________________________________________________________________________
void ArgusBuilder::WriteVisualProjects(int version,int subVersion)
{
   ROMEString projectGUID = "12345678-1234-1234-1234-123456789012";
   WriteVisualProjectSln(version,projectGUID);

   // VisualStudioProject
   ROMEXML *xml = new ROMEXML();
   ROMEString fileName;
   fileName.SetFormatted("%s%s%d.vcproj",shortCut.Data(),mainProgName.Data(),version);
   xml->OpenFileForWrite(fileName.Data());

   WriteVisualProjectProjSettings(xml,version,subVersion,projectGUID);

   // Files
   xml->StartElement("Files");

   // Source Files
   xml->StartElement("Filter");
   xml->WriteAttribute("Name","Source Files");
   xml->WriteAttribute("Filter","cpp;c;cxx");
   xml->WriteAttribute("UniqueIdentifier","{4FC737F1-C7A5-4376-A066-2A32D752A2FF}");

   WriteVisualProjectProjSources(xml,tabSources,"Tabs");
   WriteVisualProjectProjSources(xml,daqSources,"User DAQs");
   WriteVisualProjectProjSources(xml,databaseSources,"User Databases");
   WriteVisualProjectProjSources(xml,folderSources,"Folders");
   WriteVisualProjectProjUserSources(xml);
   WriteVisualProjectProjSources(xml,frameworkSources,"Generated");
   WriteVisualProjectProjSources(xml,argusSources,"ARGUS");
   WriteVisualProjectProjSources(xml,romeSources,"ROME");

   // End Source Files
   xml->EndElement();

   // End Files
   xml->EndElement();

   // Globals
   xml->StartElement("Globals");
   xml->EndElement();

   xml->EndDocument();
   delete xml;
}


//______________________________________________________________________________
void ArgusBuilder::WriteARGUSDictionary(ROMEString& buffer)
{
   // writes a script file that executes rootcint for the ARGUS headers
   buffer.AppendFormatted("dict/ARGUSDict.h dict/ARGUSDict.cpp:", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted(" $(ARGUSSYS)/include/ArgusMonitor.h $(ARGUSSYS)/include/ArgusWindow.h $(ARGUSSYS)/include/ArgusTextDialog.h $(ARGUSSYS)/include/ArgusAnalyzerController.h $(ARGUSSYS)/include/TNetFolder.h\n");
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("%%ROOTSYS%%\\bin\\rootcint -f dict/ARGUSDict.cpp -c -p", shortCut.Data());
   buffer.AppendFormatted(" -I%%ROMESYS%%/include");
   buffer.AppendFormatted(" -I%%ROOTSYS%%/include");
   buffer.AppendFormatted(" -I%%ARGUSSYS%%/include");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint -f dict/ARGUSDict.cpp -c -p", shortCut.Data());
   buffer.AppendFormatted(" -I$(ROMESYS)/include");
   buffer.AppendFormatted(" -I$(shell $(ROOTSYS)/bin/root-config --incdir)");
   buffer.AppendFormatted(" -I$(ARGUSSYS)/include");
#endif
   buffer.AppendFormatted(" ArgusMonitor.h ArgusWindow.h ArgusTextDialog.h ArgusAnalyzerController.h TNetFolder.h");
   buffer.Append("\n\n");
}


//______________________________________________________________________________
void ArgusBuilder::WriteFrameworkDictionary(ROMEString& buffer)
{
   // writes a script file that executes rootcint for the framework headers
   buffer.AppendFormatted("dict/%sFrameworkDict.h dict/%sFrameworkDict.cpp:", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted(" include/framework/%sMonitor.h  include/framework/%sWindow.h\n", shortCut.Data(), shortCut.Data());
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("%%ROOTSYS%%\\bin\\rootcint -f dict/%sFrameworkDict.cpp -c -p", shortCut.Data());
   buffer.AppendFormatted(" -I%%ROMESYS%%/include");
   buffer.AppendFormatted(" -I%%ROOTSYS%%/include");
   buffer.AppendFormatted(" -I%%ARGUSSYS%%/include");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint -f dict/%sFrameworkDict.cpp -c -p", shortCut.Data());
   buffer.AppendFormatted(" -I$(ROMESYS)/include");
   buffer.AppendFormatted(" -I$(shell $(ROOTSYS)/bin/root-config --incdir)");
   buffer.AppendFormatted(" -I$(ARGUSSYS)/include");
#endif
   buffer.AppendFormatted(" -I. -Iinclude -Iinclude/framework");
   buffer.AppendFormatted(" include/framework/%sMonitor.h include/framework/%sWindow.h", shortCut.Data(), shortCut.Data());
   buffer.Append("\n\n");
}


//______________________________________________________________________________
void ArgusBuilder::WriteTabDictionary(ROMEString& buffer)
{
   // writes a script file that executes rootcint for the tab headers
   int i;
   buffer.AppendFormatted("dict/%sTabDict.h dict/%sTabDict.cpp:", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted(" $(TabIncludes)");
   buffer.AppendFormatted(" $(BaseTabIncludes)");
   buffer.AppendFormatted("\n");
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("%%ROOTSYS%%\\bin\\rootcint -f dict/%sTabDict.cpp -c -p", shortCut.Data());
   buffer.AppendFormatted(" -I%%ROMESYS%%/include");
   buffer.AppendFormatted(" -I%%ROOTSYS%%/include");
   buffer.AppendFormatted(" -I%%ARGUSSYS%%/include");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint -f dict/%sTabDict.cpp -c -p", shortCut.Data());
   buffer.AppendFormatted(" -I$(ROMESYS)/include");
   buffer.AppendFormatted(" -I$(shell $(ROOTSYS)/bin/root-config --incdir)");
   buffer.AppendFormatted(" -I$(ARGUSSYS)/include");
#endif
   buffer.AppendFormatted(" -I. -Iinclude -Iinclude/framework");
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted(" include/tabs/%sT%s_Base.h", shortCut.Data(), tabName[i].Data());
      buffer.AppendFormatted(" include/tabs/%sT%s.h", shortCut.Data(), tabName[i].Data());
   }
   buffer.Append("\n\n");
}


//______________________________________________________________________________
void ArgusBuilder::WriteHTMLDoku()
{
   Int_t i = 0, j = 0, k = 0;
   ROMEString buffer;
   ROMEString parentt;
   Int_t depthold = 0;
   Int_t depth = 0;
   Bool_t trodd = kTRUE;

   // Header
   buffer.Resize(0);
   buffer.AppendFormatted("<!--\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//          *** This file will be overwritten by the ArgusBuilder ***         //\n");
   buffer.AppendFormatted("//          ***      Don't make manual changes to this file       ***         //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("-->\n\n");
   buffer.AppendFormatted("<HTML>\n");
   buffer.AppendFormatted("<HEAD>\n");
   buffer.AppendFormatted("<TITLE>%s%s Manual</TITLE>\n", shortCut.Data(), mainProgName.Data());
   if (styleSheet.Length()) {
      buffer.AppendFormatted("<LINK rel=\"stylesheet\" type=\"text/css\" href=\"%s/%s\">\n", outDir.Data(), styleSheet.Data());
   }
   else {
      buffer.AppendFormatted("<STYLE>\n");
      WriteHTMLStyle(buffer);
      buffer.AppendFormatted("</STYLE>\n");
   }
   buffer.AppendFormatted("</HEAD>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\">\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<H1>%s%s Manual</H1>\n", shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("\n");

   // Table of Contents
   buffer.AppendFormatted("<H2>Table of Contents</H2>\n");
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#introduction\">Introduction</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#steers\">Steering Parameters</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#objects\">Objects in the %s%s</a></li>\n", shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#tabobjects\">Tabs</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#folderobjects\">Folders</a></li>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<li><a href=\"#accessmethods\">Access Methods to Objects in the %s%s</a></li>\n", shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"%s/htmldoc/ClassIndex.html\">Class Overview</A></li>\n", outDir.Data());
//   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"%s/%sUserHTML.html\">Additional Info</A></li>\n", outDir.Data(), shortCut.Data());
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");

   // Introduction
   buffer.AppendFormatted("<H2><a name=introduction>Introduction</a> </H2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%s\n", mainDescription.Data());
   buffer.AppendFormatted("<p>\n");

   // Steering parameters
   buffer.AppendFormatted("<H2><a name=steers>Steering Parameters</a> </H2>\n");
   buffer.AppendFormatted("\n");
   for (i = 0; i <= numOfTab; i++) {
      if (numOfSteering[i] < 1)
         continue;
      if (i < numOfTab)
         buffer.AppendFormatted("<a class=\"object\">%s</a><br>\n", tabName[i].Data());
      else
         buffer.AppendFormatted("<a class=\"object\">Global Steering Parameters</a><br>\n");
      buffer.AppendFormatted("<table>\n");
      buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      trodd = !trodd;
      WriteHTMLSteering(buffer, 0, i, "");
      buffer.AppendFormatted("</table><br>\n");
   }
   buffer.AppendFormatted("\n");
   // Objects
   buffer.AppendFormatted("<H2><a name=objects>Objects in the %s%s</a> </H2>\n", shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("All <a href=\"#tabobjects\">Tabs</a>, <a href=\"#folderobjects\">Folders</a> are described here.\n");

   // Tabs
   buffer.AppendFormatted("<h3><a name=tabobjects>Tabs</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s has the following tab hierarchy :\n", shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("\n");
   Int_t index;
   Int_t ddelta;
   buffer.AppendFormatted("<ul>\n");
   for (i = 0; i < numOfTab; i++) {
      index = i;
      depth = 0;
      while (index != -1) {
         depth++;
         index = tabParentIndex[index];
      }
      depth--;
      ddelta = depth - depthold;
      if (ddelta > 0)
         for (k = 0; k < ddelta; k++)
            buffer.AppendFormatted("<ul>\n");
      if (ddelta < 0)
         for (k = 0; k < -ddelta; k++)
            buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<li><a href=\"#%s\">%sT%s</a></li>\n", tabName[i].Data(), shortCut.Data(), tabName[i].Data());
      depthold = depth;
   }
   for (i = 0; i < depth; i++)
      buffer.AppendFormatted("</ul>\n");
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

   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("<h4><a name=%s><u>%s</u></a></h4>\n", tabName[i].Data(), tabName[i].Data());
      buffer.AppendFormatted("%s<p>\n", tabDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      cppFile.SetFormatted("%s/src/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[i].Data());
      fileStream = new fstream(cppFile.Data(), ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      buffer.AppendFormatted("%s accesses data from the following folders :\n", tabName[i].Data());
      buffer.AppendFormatted("<ul>\n");
      for (j = 0; j < numOfFolder; j++) {
         if (accessFolder(fileBuffer, j))
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n", folderName[j].Data());
      }
      buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("For more information take a look at the <A TARGET=_top HREF=\"%s/htmldoc/%sT%s.html\">class file</a>\n", outDir.Data(), shortCut.Data(), tabName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("\n");
   }
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Folders
   buffer.AppendFormatted("<h3><a name=folderobjects>Folders</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following folders :\n", shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   depthold = 0;
   depth = 0;
   for (i = 0; i < numOfFolder; i++) {
      depth = 0;
      if (folderParentName[i] != "GetMainFolder()") {
         depth++;
         parentt = folderParentName[i];
         for (j = 0; j < maxNumberOfFolders; j++) {
            for (k = 0; k < numOfFolder; k++) {
               if (parentt == folderName[k]) {
                  parentt = folderParentName[k];
                  break;
               }
            }
            if (k >= numOfFolder) {
               cout << "Invalid folder structure." << endl;
               return;
            }
            if (folderParentName[k] == "GetMainFolder()")
               break;
            depth++;
         }
      }
      ddelta = depth - depthold;
      if (ddelta > 0)
         for (k = 0; k < ddelta; k++)
            buffer.AppendFormatted("<ul>\n");
      if (ddelta < 0)
         for (k = 0; k < -ddelta; k++)
            buffer.AppendFormatted("</ul>\n");
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted("<b>\n");
         buffer.AppendFormatted("<li type=\"circle\"><a href=\"#%s\">%s</a></li>\n", folderName[i].Data(), folderName[i].Data());
         buffer.AppendFormatted("</b>\n");
      }
      else {
         buffer.AppendFormatted("<li type=\"circle\">%s</li>\n", folderName[i].Data());
      }
      depthold = depth;
   }
   for (i = 0; i < depth; i++)
      buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("The bold folders are data folders. The others are only used to structure the framework.\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("In the following all folders will be described.\n");
   buffer.AppendFormatted("<p>\n");
   for (i = 0; i < numOfFolder; i++) {
      if (numOfValue[i] <= 0)
         continue;
      buffer.AppendFormatted("<a name=%s class=\"object\">%s</a><br>\n", folderName[i].Data(), folderName[i].Data());
      buffer.AppendFormatted("%s\n", folderDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<u>Fields</u>\n");
      buffer.AppendFormatted("<table>\n");
      buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      for (j = 0; j < numOfValue[i]; j++) {
         ROMEString comment = valueComment[i][j];
         if (valueComment[i][j].Length() > 3) {
            if (valueComment[i][j][0] == '/') {
               comment = valueComment[i][j] (3, valueComment[i][j].Length() - 3);
            }
         }
         buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n", trodd ? "odd" : "even", valueName[i][j].Data(), valueType[i][j].Data(), comment.Data());
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
   buffer.AppendFormatted("<H2><a name=accessmethods>Access Methods to Objects in the %s%s</a> </H2>\n", shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("For a description of all access methods to all types of objects in the %s%s please visit the\n", shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("<A TARGET=_top HREF=\"http://midas.psi.ch/argus/user_guide/index.html\">users guide on the ARGUS homepage</A>.\n");
   buffer.AppendFormatted("</p>\n");

   // Footer
   buffer.AppendFormatted("<HR>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ADDRESS>\n");
   buffer.AppendFormatted("<u> Contact person from the %s Experiment</u></br>\n", shortCut.Data());
   buffer.AppendFormatted("%s</br>\n", mainAuthor.Data());
   buffer.AppendFormatted("%s</br>\n", mainInstitute.Data());
   buffer.AppendFormatted("%s</br>\n", mainCollaboration.Data());
   buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n", mainEmail.Data(), mainEmail.Data());
   buffer.AppendFormatted("<u> Contact person from Argus</u></br>\n");
   buffer.AppendFormatted("<a href=\"http://www.icepp.s.u-tokyo.ac.jp/%%E7sawada\">Ryu Sawada</a> (ICEPP)</p>\n");
   buffer.AppendFormatted("</ADDRESS>\n");
   buffer.AppendFormatted("</BODY>\n");
   buffer.AppendFormatted("</HTML>\n");

   // Write HTML
   ROMEString htmlFile;
   htmlFile.SetFormatted("%s/%s%s.html", outDir.Data(), shortCut.Data(), mainProgName.Data());
   WriteFile(htmlFile.Data(), buffer.Data(), 0);

   struct stat buf;
   // Write style sheet
   ROMEString css;
   if (styleSheet.Length()) {
      css.SetFormatted("%s/%s", outDir.Data(), styleSheet.Data());
      if (stat(css.Data(), &buf)) {
         buffer.Resize(0);
         WriteHTMLStyle(buffer);
         WriteFile(css.Data(), buffer.Data(), 0);
      }
   }
   // Write UserHTML
/*
   ROMEString userHtmlFile;
   userHtmlFile.SetFormatted("%s%sUserHTML.html", outDir.Data(), shortCut.Data());
   if (stat(userHtmlFile.Data(), &buf)) {
      buffer.Resize(0);
      buffer.AppendFormatted("<html>\n");
      buffer.AppendFormatted("<head>\n");
      buffer.AppendFormatted("  <title>%s%s Additional Info</title>\n", shortCut.Data(), mainProgName.Data());
      if (styleSheet.Length()) {
         buffer.AppendFormatted("<LINK rel=\"stylesheet\" type=\"text/css\" href=\"%s/%s\">\n", outDir.Data(), styleSheet.Data());
      }
      else {
         buffer.AppendFormatted("<STYLE>\n");
         WriteHTMLStyle(buffer);
         buffer.AppendFormatted("</STYLE>\n");
      }
      buffer.AppendFormatted("</head>\n");
      buffer.AppendFormatted("<body>\n");
      buffer.AppendFormatted("  <h1>Additional Info</h1>\n");
      buffer.AppendFormatted("  <a href=\"%s%s.html\">Main page</a>\n", shortCut.Data(), mainProgName.Data());
      buffer.AppendFormatted("  <hr width=\"100%%\">\n");
      buffer.AppendFormatted("\n\n\n\n");
      buffer.AppendFormatted("  <hr width=\"100%%\">\n");
      buffer.AppendFormatted("<address>\n");
      buffer.AppendFormatted("<u> Contact person from the %s Experiment</u></br>\n", shortCut.Data());
      buffer.AppendFormatted("%s</br>\n", mainAuthor.Data());
      buffer.AppendFormatted("%s</br>\n", mainInstitute.Data());
      buffer.AppendFormatted("%s</br>\n", mainCollaboration.Data());
      buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n", mainEmail.Data(), mainEmail.Data());
      buffer.AppendFormatted("<u> Contact person from ARGUS</u></br>\n");
      buffer.AppendFormatted("Ryu Sawada</br>\n");
      buffer.AppendFormatted("<a href=\"http://www.icepp.s.u-tokyo.ac.jp/%%E7sawada\">Ryu Sawada</a> (ICEPP)</p>\n");
      buffer.AppendFormatted("</address>\n");
      buffer.AppendFormatted("</body>\n");
      buffer.AppendFormatted("</html>");
      WriteFile(userHtmlFile.Data(), buffer.Data(), 0);
   }
*/
}


//______________________________________________________________________________
void ArgusBuilder::GetMidasTID(ROMEString *buf, Char_t *type)
{
   buf->Resize(0);
   if (!strcmp(type, "Byte_t") || !strcmp(type, "BYTE"))
      buf->Append("TID_BYTE");  // < unsigned byte         0       255
   else if (!strcmp(type, "Char_t") || !strcmp(type, "Text_t") || !strcmp(type, "char"))
      buf->Append("TID_SBYTE"); //< signed byte         -128      127
   else if (!strcmp(type, "UChar_t") || !strcmp(type, "unsigned char"))
      buf->Append("TID_CHAR");  //< single character      0       255
   else if (!strcmp(type, "UShort_t") || !strcmp(type, "WORD") || !strcmp(type, "unsigned short") || !strcmp(type, "unsigned short int"))
      buf->Append("TID_WORD");  //< two bytes             0      65535
   else if (!strcmp(type, "Short_t") || !strcmp(type, "Version_t") || !strcmp(type, "short") || !strcmp(type, "short int"))
      buf->Append("TID_SHORT"); //< signed word        -32768    32767
   else if (!strcmp(type, "UInt_t") || !strcmp(type, "DWORD") || !strcmp(type, "unsigned int") || !strcmp(type, "unsigned long") || !strcmp(type, "unsigned long int"))
      buf->Append("TID_DWORD"); //< four bytes            0      2^32-1
   else if (!strcmp(type, "Int_t") || !strcmp(type, "INT") || !strcmp(type, "Ssize_t") || !strcmp(type, "int") || !strcmp(type, "long") || !strcmp(type, "long int"))
      buf->Append("TID_INT");   //< signed dword        -2^31    2^31-1
   else if (!strcmp(type, "Bool_t") || !strcmp(type, "BOOL") || !strcmp(type, "bool"))
      buf->Append("TID_BOOL");  //< four bytes bool       0        1
   else if (!strcmp(type, "Float_t") || !strcmp(type, "Real_t") || !strcmp(type, "float"))
      buf->Append("TID_FLOAT"); //< 4 Byte float format
   else if (!strcmp(type, "Double_t") || !strcmp(type, "Double32_t") || !strcmp(type, "double"))
      buf->Append("TID_DOUBLE");        //< 8 Byte float format
#if defined( R__B64 )          // Note: Long_t and ULong_t are currently not portable types
   else if (!strcmp(type, "Long_t") || !strcmp(type, "ULong_t")) {
      cout << "WARNING: Long_t and ULong_t is not supported on this architecture." << endl;
      buf->Append("TID_DOUBLE");        //< 8 Byte float format
   }
#else
   else if (!strcmp(type, "Long_t"))
      buf->Append("TID_INT");
   else if (!strcmp(type, "ULong_t"))
      buf->Append("TID_DWORD");
#endif
   else
      buf->Append("TID_STRING");        //< zero terminated string
}


//______________________________________________________________________________
Bool_t ArgusBuilder::ReplaceHeader(const Char_t *filename, const Char_t *header, const Char_t *content, Int_t nspace)
{
   ROMEString hd = header;
   ROMEString ct = content;

   ROME2Argus(hd);
   ROME2Argus(ct);

   return ROMEBuilder::ReplaceHeader(filename, hd.Data(), ct.Data(), nspace);
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteFile(const Char_t *filename, const Char_t *content, Int_t nspace)
{
   ROMEString ct = content;

   ROME2Argus(ct);

   return ROMEBuilder::WriteFile(filename, ct.Data(), nspace);
}


//______________________________________________________________________________
void ArgusBuilder::ROME2Argus(ROMEString &buffer)
{
   ROMEString rome;
   ROMEString argus;

   rome.SetFormatted("%sAnalyzer", shortCut.Data());
   argus.SetFormatted("%sMonitor", shortCut.Data());
   buffer.ReplaceAll(rome, argus);

   buffer.ReplaceAll("ROMEBuilder", "ArgusBuilder");
   buffer.ReplaceAll("gAnalyzer", "gMonitor");
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteMonitorCpp()
{
   Int_t i;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString parentt;
   ROMEString buf;
   Int_t j, k;
   Int_t iFold;
   ROMEString classDescription;
   classDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all folders.", shortCut.Data(), mainProgName.Data());
   ROMEString tmp;
   ROMEString format;

   Int_t nameLen = -1;
   Int_t scl = shortCut.Length();
   for (i = 0; i < numOfFolder; i++) {
      if (!folderSupport[i]) {
         nameLen = TMath::Max(nameLen, static_cast<Int_t>(folderName[i].Length() + scl + strlen("Folder")));
      }
   }

   // File name
   cppFile.SetFormatted("%s/src/framework/%sMonitor.cpp", outDir.Data(), shortCut.Data());
   // Description
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//          *** This file will be overwritten by the ArgusBuilder ***         //\n");
   buffer.AppendFormatted("//          ***      Don't make manual changes to this file       ***         //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#include \"include/framework/%sMonitor.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sWindow.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sConfig.h\"\n", shortCut.Data());
   if (numOfSteering[numOfTab] > 0)
      buffer.AppendFormatted("#include \"include/framework/%sGlobalSteering.h\"\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ClassImp(%sMonitor);\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sMonitor *gMonitor;  // global Monitor Handle\n", shortCut.Data());
   buffer.AppendFormatted("\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <windows.h>\n");
#endif
   buffer.AppendFormatted("\n");

   // Constructor
   buffer.AppendFormatted("%sMonitor::%sMonitor(TApplication *app) : ArgusMonitor(app) {\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   gPassToArgus = static_cast<void*>(this); // Pass the handle to the monitor\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fConfiguration = new %sConfig();\n", shortCut.Data());
   buffer.AppendFormatted("\n");

   // Steering
   if (numOfSteering[numOfTab] > 0) {
      buffer.AppendFormatted("   fGlobalSteeringParameters = new %sGlobalSteering();\n", shortCut.Data());
   }
   // Folder
   buffer.AppendFormatted("   // Folder initialisation\n");
   buffer.AppendFormatted("   fMainFolder = gROOT->GetRootFolder()->AddFolder(\"%s\",\"Root Folder of %s%s\");\n", shortCut.Data(), shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("   gROOT->GetListOfBrowsables()->Add(fMainFolder,\"%s\");\n\n", shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (!folderSupport[i]) {
         if (folderParentName[i] == "GetMainFolder()")
            parentt = folderParentName[i];
         else
            parentt.SetFormatted("%sFolder", folderParentName[i].Data());
         format.SetFormatted("   TFolder* %%sFolder%%%ds = %%s->AddFolder(\"%%s\",\"%%s\");\n", nameLen - folderName[i].Length() - scl - strlen("Folder"));
         buffer.AppendFormatted(const_cast<Char_t*>(format.Data()), folderName[i].Data(), "", parentt.Data(), folderName[i].Data(), folderTitle[i].Data());
      }
   }
   for (i = 0; i < numOfFolder; i++) {
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            if (folderArray[i] == "1") {
               buffer.AppendFormatted("\n   f%sFolder = new %s%s();\n", folderName[i].Data(), shortCut.Data(), folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolder);\n", folderName[i].Data(), folderName[i].Data());
            }
            else {
               buffer.AppendFormatted("\n   f%sFolders = new TClonesArray(\"%s%s\");\n", folderName[i].Data(), shortCut.Data(), folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolders);\n", folderName[i].Data(), folderName[i].Data());
            }
         }
      }
   }

   buffer.AppendFormatted("   fProgramName = \"%s%s\";\n", shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("   InitNetFolders(%d);\n", numOfNetFolder);

   // NetFolder
   buffer.AppendFormatted("   fNumberOfNetFolders = %d;\n", numOfNetFolder);
   for (i = 0; i < numOfNetFolder; i++) {
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   // %s\n", netFolderName[i].Data());
      buffer.AppendFormatted("   fNetFolderName[%d]   = \"%s\";\n", i, netFolderName[i].Data());
      buffer.AppendFormatted("   fNetFolderActive[%d] = kFALSE;\n", i);
      buffer.AppendFormatted("   fNetFolderReconnect[%d] = kTRUE;\n", i);
      buffer.AppendFormatted("   fNetFolder[%d]       = 0;\n", i);
      buffer.AppendFormatted("   fNetFolderSocket[%d] = 0;\n", i);
      if (netFolderHost[i].Length())
         buffer.AppendFormatted("   fNetFolderHost[%d]   = \"%s\";\n", i, netFolderHost[i].Data());
      else
         buffer.AppendFormatted("   fNetFolderHost[%d]   = \"localhost\";\n", i);
      if (netFolderPort[i].Length())
         buffer.AppendFormatted("   fNetFolderPort[%d]   = %s;\n", i, netFolderPort[i].Data());
      else
         buffer.AppendFormatted("   fNetFolderPort[%d]   = 9090;\n", i);
      if (netFolderRoot[i].Length())
         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"%s\";\n", i, netFolderRoot[i].Data());
      else
//         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"histos\";\n",i);
         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"%s\";\n", i, shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   // Database Folder Fields
   buffer.AppendFormatted("   // Database Folder Fields\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            buffer.AppendFormatted("   f%s_%sDBName = \"\";\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("   f%s_%sDBPath = \"\";\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("   f%s_%sDBCode = new TArrayI(%d);\n", folderName[i].Data(), valueName[i][j].Data(), maxNumberOfPathObjectInterpreterCodes);
            buffer.AppendFormatted("   for (i=0;i<%d;i++)\n", maxNumberOfPathObjectInterpreterCodes);
            buffer.AppendFormatted("      f%s_%sDBCode->AddAt(-1,i);\n", folderName[i].Data(), valueName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("}\n\n");

   // Tree Initialization
   buffer.AppendFormatted("// Tree initialization\n");
   buffer.AppendFormatted("void %sMonitor::InitTrees()\n{\n", shortCut.Data());
   buffer.AppendFormatted("   TFolder *treeFolder = fMainFolder->AddFolder(\"Trees\",\"Trees of the %s framework\");\n", shortCut.Data());
   buffer.AppendFormatted("   TTree *tree;\n");
   buffer.AppendFormatted("   ROMEString fileName;\n\n");
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("   tree = new TTree(\"%s\",\"%s\");\n", treeName[i].Data(), treeTitle[i].Data());
      buffer.AppendFormatted("   tree->Branch(\"Info\",\"ROMETreeInfo\",&fTreeInfo,32000,99);\n");
      for (j = 0; j < numOfBranch[i]; j++) {
         iFold = -1;
         for (k = 0; k < numOfFolder; k++) {
            if (branchFolder[i][j] == folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (iFold < 0)
            continue;
         if (folderArray[iFold] == "1") {
            buffer.AppendFormatted("   tree->Branch(\"%s\",\"%s%s\",gAnalyzer->Get%sAddress(),%s,%s);\n", branchName[i][j].Data(), shortCut.Data(), folderName[iFold].Data(), branchFolder[i][j].Data(), branchBufferSize[i][j].Data(), branchSplitLevel[i][j].Data());
         }
         else {
            buffer.AppendFormatted("   tree->Branch(\"%s\",\"TClonesArray\",gAnalyzer->Get%sAddress(),%s,%s);\n", branchName[i][j].Data(), branchFolder[i][j].Data(), branchBufferSize[i][j].Data(), branchSplitLevel[i][j].Data());
         }
      }
      buffer.AppendFormatted("   AddTree(tree);\n");
      buffer.AppendFormatted("   treeFolder->Add(tree);\n\n");
   }
   buffer.AppendFormatted("}\n\n");

   Int_t ndb = 0;
   for (i = 0; i < numOfFolder; i++)
      if (folderDataBase[i])
         ndb++;

   // ReadSingleDataBaseFolders
   buffer.AppendFormatted("Bool_t %sMonitor::ReadSingleDataBaseFolders() {\n", shortCut.Data());
   if (ndb > 0) {
      for (i = 0; i < numOfFolder; i++) {
         if (folderDataBase[i] && folderArray[i] == "1" && !folderSupport[i]) {
            buffer.AppendFormatted("   if(!Read%s())\n", folderName[i].Data());
            buffer.AppendFormatted("      return kFALSE;\n");
         }
      }
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");

   // ReadArrayDataBaseFolders
   buffer.AppendFormatted("\nBool_t %sMonitor::ReadArrayDataBaseFolders() {\n", shortCut.Data());
   if (ndb > 0) {
      for (i = 0; i < numOfFolder; i++) {
         if (folderDataBase[i] && folderArray[i] != "1" && !folderSupport[i])
            buffer.AppendFormatted("   if(!Read%s()) return kFALSE;\n", folderName[i].Data());
      }
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");

   // clean up folders
   buffer.AppendFormatted("\n// Delete Unused Folders\n");
   buffer.AppendFormatted("void %sMonitor::CleanUpFolders() {\n", shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (numOfValue[i] > 0 && !folderDataBase[i] && folderArray[i] == "variable") {
         buffer.AppendFormatted("   Int_t i;\n");
         break;
      }
   }
   for (i = 0; i < numOfFolder; i++) {
      if (numOfValue[i] > 0 && !folderDataBase[i] && !folderSupport[i]) {
         if (folderArray[i] == "variable") {
            buffer.AppendFormatted("   for (i=gMonitor->Get%ss()->GetEntriesFast()-1;i>=0;i--) {\n", folderName[i].Data());
            buffer.AppendFormatted("      if (dynamic_cast<%s%s*>(gMonitor->Get%sAt(i))->isModified())\n", shortCut.Data(), folderName[i].Data(), folderName[i].Data());
            buffer.AppendFormatted("         break;\n");
            buffer.AppendFormatted("      gMonitor->Get%ss()->RemoveAt(i);\n", folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // reset folders
   buffer.AppendFormatted("// Reset Folders\n");
   buffer.AppendFormatted("\nvoid %sMonitor::ResetFolders() {\n", shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (numOfValue[i] > 0 && !folderDataBase[i] && folderArray[i] != "1" && !folderSupport[i]) {
         buffer.AppendFormatted("   Int_t i;\n");
         break;
      }
   }
   for (i = 0; i < numOfFolder; i++) {
      if (numOfValue[i] > 0 && !folderDataBase[i]) {
         if (folderArray[i] == "1") {
            buffer.AppendFormatted("   gMonitor->Get%s()->Reset();\n", folderName[i].Data());
         }
         else if (folderArray[i] == "variable") {
            buffer.AppendFormatted("   for (i=0;i<gMonitor->Get%ss()->GetEntriesFast();i++) {\n", folderName[i].Data());
            buffer.AppendFormatted("      dynamic_cast<%s%s*>(gMonitor->Get%sAt(i))->Reset();\n", shortCut.Data(), folderName[i].Data(), folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
         else {
            buffer.AppendFormatted("   for (i=0;i<gMonitor->Get%ss()->GetEntriesFast();i++) {\n", folderName[i].Data());
            buffer.AppendFormatted("      ((%s%s*)gMonitor->Get%sAt(i))->SetModified(kFALSE);\n", shortCut.Data(), folderName[i].Data(), folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Read each folders
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("Bool_t %sMonitor::Read%s() {\n", shortCut.Data(), folderName[i].Data());
         WriteReadDataBaseFolder(buffer, i, folderArray[i] == "1" ? 1 : 2);
         buffer.AppendFormatted("   return kTRUE;\n");
         buffer.AppendFormatted("}\n\n");
      }
   }

/*
      if (!folderDataBase[i] || numOfValue[i]==0 )
         continue;
      buffer.AppendFormatted("\nBool_t %sMonitor::Read%s() {\n",shortCut.Data(),folderName[i].Data());
      // ROMEFolder
      if(folderDefinedInROME[i]){
         buffer.AppendFormatted("   if(!fNetFolder){\n");
         buffer.AppendFormatted("      cout<<\"Warning: \"<<gMonitor->GetProgramName()<<\" is not connected to ROOT server.\"<<endl;\n");
         buffer.AppendFormatted("      cout<<\"%s is not updated.\"<<endl;\n",folderName[i].Data());
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   }\n");
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   if(f%sFolder)\n",folderName[i].Data());
            buffer.AppendFormatted("      delete f%sFolder;\n",folderName[i].Data());
            buffer.AppendFormatted("      f%sFolder = (%s%s*)((TFolder*) fNetFolder->FindObjectAny(\"%s\"))->FindObject(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("   if (!f%sFolder){\n",folderName[i].Data());
            buffer.AppendFormatted("      cout<<\"%s not available\"<<endl;\n",folderName[i].Data());
            buffer.AppendFormatted("      return kFALSE;\n");
            buffer.AppendFormatted("   }\n");
         }
         else {
            buffer.AppendFormatted("   if(f%sFolders)\n",folderName[i].Data());
            buffer.AppendFormatted("      delete f%sFolders;\n",folderName[i].Data());
            buffer.AppendFormatted("      f%sFolders = (TClonesArray*)((TFolder*) fNetFolder->FindObjectAny(\"%s\"))->FindObject(\"%s%ss\");\n",folderName[i].Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("   if (!f%sFolders){\n",folderName[i].Data());
            buffer.AppendFormatted("      cout<<\"%s not available\"<<endl;\n",folderName[i].Data());
            buffer.AppendFormatted("      return kTRUE;\n");
            buffer.AppendFormatted("   }\n");
         }
      }
      // ODB folder
      else if(folderConnectionType[i] == "ODB"){
         buffer.AppendFormatted("   INT   size;\n");
         ROMEString tid;
         for (j=0;j<numOfValue[i];j++) {
            if (valueArray[i][j]!="1") {
               buffer.AppendFormatted("   HNDLE hkey;\n");
               buffer.AppendFormatted("   Int_t i;\n");
               break;
            }
         }
         for (j=0;j<numOfValue[i];j++) {
            GetMidasTID(&tid, (Char_t*)valueType[i][j].Data());
            if (valueArray[i][j]=="1") {
               buffer.AppendFormatted("   %s %s_%s;\n",valueType[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("   size = sizeof(%s_%s);\n",folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("   db_get_value(fMidasOnlineDataBase, 0, \"%s\", &%s_%s, &size, %s, FALSE);\n",valueDataBasePath[i][j].Data(),folderName[i].Data(),valueName[i][j].Data(),tid.Data());
               buffer.AppendFormatted("   f%sFolder->Set%s(%s_%s);\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
            }
            else {
               buffer.AppendFormatted("   %s %s_%s[%s];\n",valueType[i][j].Data(),folderName[i].Data(),valueName[i][j].Data(),valueArray[i][j].Data());
               buffer.AppendFormatted("   size = sizeof(%s_%s);\n",folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("   db_find_key(fMidasOnlineDataBase, 0, %s, &hkey);\n",valueDataBasePath[i][j].Data());
               buffer.AppendFormatted("   db_get_data(fMidasOnlineDataBase, hkey, %s_%s, &size, %s);\n",folderName[i].Data(),valueName[i][j].Data(),tid.Data());
               buffer.AppendFormatted("   for (i=0;i<%s;i++) {\n",valueArray[i][j].Data());
               buffer.AppendFormatted("      f%sFolder->Set%sAt(i,(%s)%s_%s[i]);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("   }\n");
            }
         }
      }
      else if(folderConnectionType[i] == "ROMEDataBase"){
         // ROMEDataBase Folder
         buffer.AppendFormatted("   ROMEString path;\n");
         buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
         if(folderArray[i]!="1")
         buffer.AppendFormatted("   Int_t i;\n");
         for (j=0;j<numOfValue[i];j++) {
         if (valueArray[i][j]!="1") {
               buffer.AppendFormatted("   Int_t j;\n");
               break;
            }
         }
         for (j=0;j<numOfValue[i];j++) {
            if (isNumber(valueType[i][j].Data())){
               buffer.AppendFormatted("   Char_t *cstop;\n");
               break;
            }
         }
         if (folderArray[i] == "1") {
            for (j=0;j<numOfValue[i];j++) {
               if (folderArray[i]=="1") {
                  buffer.AppendFormatted("   values->RemoveAll();\n");
                  buffer.AppendFormatted("   path.SetFormatted(%s);\n",valueDataBasePath[i][j].Data());
                  buffer.AppendFormatted("   if (!this->GetDataBase()->Read(values,path,fRunNumber))\n");
                  buffer.AppendFormatted("      return kFALSE;\n");
                  if (valueArray[i][j]=="1") {
                     buffer.AppendFormatted("   if (values->At(0,0).Length()!=0)\n");
                     setValue(&buf,(Char_t*)valueName[i][j].Data(),"values->At(0,0).Data()",(Char_t*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("      f%sFolder->Set%s((%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("   else\n");
                     buffer.AppendFormatted("      f%sFolder->Set%s(%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                  }
                  else {
                     buffer.AppendFormatted("   for (j=0;j<%s;j++) {\n",valueArray[i][j].Data());
                     buffer.AppendFormatted("      if (values->At(0,j).Length()!=0)\n");
                     setValue(&buf,(Char_t*)valueName[i][j].Data(),"values->At(0,j).Data()",(Char_t*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("         f%sFolder->Set%sAt(j,(%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("      else\n");
                     buffer.AppendFormatted("         f%sFolder->Set%sAt(j,%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                     buffer.AppendFormatted("   }\n");
                  }
               }
            }
         }
         else{
            for (j=0;j<numOfValue[i];j++) {
               buffer.AppendFormatted("   values->RemoveAll();\n");
               buffer.AppendFormatted("   path.SetFormatted(%s);\n",valueDataBasePath[i][j].Data());
               buffer.AppendFormatted("   if (!this->GetDataBase()->Read(values,path,fRunNumber))\n");
               buffer.AppendFormatted("      return kFALSE;\n");
               buffer.AppendFormatted("   for (i=0;i<f%sFolders->GetEntries();i++) {\n",folderName[i].Data());
               if (valueArray[i][j]=="1") {
                  buffer.AppendFormatted("      if (values->At(i,0).Length()!=0)\n");
                  setValue(&buf,(Char_t*)valueName[i][j].Data(),"values->At(i,0).Data()",(Char_t*)valueType[i][j].Data(),1);
                  buffer.AppendFormatted("         ((%s%s*)f%sFolders->At(i))->Set%s((%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                  buffer.AppendFormatted("      else\n");
                  buffer.AppendFormatted("         ((%s%s*)f%sFolders->At(i))->Set%s(%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("      for (j=0;j<%s;j++) {\n",valueArray[i][j].Data());
                  buffer.AppendFormatted("         if (values->At(i,j).Length()!=0)\n");
                  setValue(&buf,(Char_t*)valueName[i][j].Data(),"values->At(i,j).Data()",(Char_t*)valueType[i][j].Data(),1);
                  buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%sAt(j,(%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                  buffer.AppendFormatted("         else\n");
                  buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%sAt(j,%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                  buffer.AppendFormatted("      }\n");
               }
               buffer.AppendFormatted("   }\n");
            }
         }
         buffer.AppendFormatted("   values->RemoveAll();\n");
         buffer.AppendFormatted("   delete values;\n");
      }
      buffer.AppendFormatted("   return kTRUE;\n");
      buffer.AppendFormatted("};\n");
   }
*/

   // Get Object Interpreter Code
   Int_t codeNumber = 0;
   buffer.AppendFormatted("Int_t %sMonitor::GetObjectInterpreterCode(const Char_t* objectPath) {\n", shortCut.Data());
   buffer.AppendFormatted("   ROMEString path = objectPath;\n");
   buffer.AppendFormatted("   if (path.Index(\"/\")!=0)\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   if (path.Index(\"/GSP\")==0) {\n");
   ROMEString path = "/GSP";
   codeNumber = WriteSteeringInterpreterCode(buffer, codeNumber, 0, numOfTaskHierarchy, path, 1);
   buffer.AppendFormatted("      gMonitor->PrintLine(\"\\nWrong path to a steering parameter in configuration file.\\n\");\n");
   buffer.AppendFormatted("      gMonitor->PrintLine(\"   \"+path+\"\\n\");\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   }\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderArray[i] == "1" && !folderSupport[i]) {
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] == 0) {
               buffer.AppendFormatted("   if (path==\"/%s/%s\")\n", folderName[i].Data(), valueName[i][j].Data());
               buffer.AppendFormatted("      return %d;\n", codeNumber);
               codeNumber++;
            }
         }
      }
   }
   buffer.AppendFormatted("   gMonitor->PrintLine(\"\\nWrong path in configuration file.\\n\");\n");
   buffer.AppendFormatted("   gMonitor->PrintLine(\"   \"+path+\"\\n\");\n");
   buffer.AppendFormatted("   return -1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   fNumberOfInterpreterCodes = codeNumber;

   // Get Object Interpreter Values
   WriteObjectInterpreterValue(buffer, "int", "Int");
   WriteObjectInterpreterValue(buffer, "double", "Double");
   WriteObjectInterpreterValue(buffer, "ROMEString&", "Char");

   buffer.AppendFormatted("\nBool_t %sMonitor::StartWindow() {\n", shortCut.Data());
   buffer.AppendFormatted("   return gWindow->Start();\n");
   buffer.AppendFormatted("}\n");

   // Initialize Single Folders
   buffer.AppendFormatted("\nvoid %sMonitor::InitSingleFolders() {\n", shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i] == "1") {
            buffer.AppendFormatted("   new(gMonitor->Get%s()) %s%s( ", folderName[i].Data(), shortCut.Data(), folderName[i].Data());
            for (j = 0; j < numOfValue[i]; j++) {
               if (isFolder(valueType[i][j].Data()))
                  continue;
               if (valueDimension[i][j] == 0)
                  buffer.AppendFormatted("%s,", valueInit[i][j].Data());
            }
            buffer.Resize(buffer.Length() - 1);
            buffer.AppendFormatted(" );\n");
         }
      }
   }
   buffer.AppendFormatted("};\n\n");

   // Initialize Array Folders
   buffer.AppendFormatted("\nvoid %sMonitor::InitArrayFolders() {\n", shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i] != "1" && folderArray[i] != "variable")
            buffer.AppendFormatted("   gMonitor->Set%sSize(%s);\n", folderName[i].Data(), folderArray[i].Data());
      }
   }
   buffer.AppendFormatted("};\n\n");

   // ReadUserParameter
   buffer.AppendFormatted("Bool_t %sMonitor::ReadUserParameter(const Char_t* opt, const Char_t* value, Int_t& i) {\n", shortCut.Data());

   // Global Steering Parameter
   buffer.AppendFormatted("   ROMEString option = opt;\n");
   buffer.AppendFormatted("   Char_t *cstop;\n");
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTab] > 0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gMonitor->GetGSP()");
      WriteSteeringReadParameters(buffer, 0, numOfTab, pointerT, steerPointerT);
   }
   // Tab steering parameter
   ROMEString pointer;
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("   // %s tab\n", tabName[i].Data());
      Int_t index = i;
      pointer.Resize(0);
      while (index != -1) {
         pointer.InsertFormatted(0, "->f%sTab", tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (numOfSteering[i] > 0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering", pointer.Data());
         steerPointerT.SetFormatted("gWindow->Get%s%03dTab()->GetSP()", tabName[i].Data(), i);
         WriteSteeringReadParameters(buffer, 0, i, pointerT, steerPointerT);
      }
   }
   buffer.AppendFormatted("   return kFALSE;\n");
   buffer.AppendFormatted("}\n\n");

/*   // WriteDataBaseFolders
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("void %sMonitor::Write%sDataBase() {\n",shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted("   Int_t i,j;\n");
         buffer.AppendFormatted("   ROMEString path;\n");
         buffer.AppendFormatted("   ROMEString buffer[%d];\n",maxNumberOfPathObjectInterpreterCodes);
         buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
         for (j=0;j<numOfValue[i];j++) {
            buffer.AppendFormatted("   values->RemoveAll();\n");
            if (folderArray[i]=="1") {
               if (valueArray[i][j]=="1") {
                  buf = "buffer[0]";
                  str.SetFormatted("f%sFolder->Get%s()",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("   values->SetAt(%s,0,0);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
               else {
                  buf = "buffer[0]";
                  str.SetFormatted("f%sFolder->Get%sAt(j)",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("   for (j=0;j<%s;j++)\n",valueArray[i][j].Data());
                  buffer.AppendFormatted("      values->SetAt(%s,0,j);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
            }
            else {
               buffer.AppendFormatted("   for (i=0;i<f%sFolders->GetEntries();i++)\n",folderName[i].Data());
               if (valueArray[i][j]=="1") {
                  buf = "buffer[0]";
                  str.SetFormatted("((%s%s*)f%sFolders->At(i))->Get%s()",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      values->SetAt(%s,i,0);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
               else {
                  buf = "buffer[0]";
                  str.SetFormatted("((%s%s*)f%sFolders->At(i))->Get%sAt(j)",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      for (j=0;j<%s;j++)\n",valueArray[i][j].Data());
                  buffer.AppendFormatted("         values->SetAt(%s,i,j);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
            }
            buffer.AppendFormatted("   path.SetFormatted(gMonitor->Get%s_%sDBPath()",folderName[i].Data(),valueName[i][j].Data());
            for (k=0;k<maxNumberOfPathObjectInterpreterCodes;k++)
               buffer.AppendFormatted(",gMonitor->GetObjectInterpreterCharValue(gMonitor->Get%s_%sDBCodeAt(%d),buffer[%d],buffer[%d]).Data()",folderName[i].Data(),valueName[i][j].Data(),k,k,k);
            buffer.AppendFormatted(");\n");
            buffer.AppendFormatted("   if (!this->GetDataBase(gMonitor->Get%s_%sDBIndex())->Write(values,path,gMonitor->GetCurrentRunNumber())) {\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      gMonitor->PrintLine(\"   in Folder '%s' Value '%s'.\");\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      delete values;\n");
            buffer.AppendFormatted("      return;\n");
            buffer.AppendFormatted("   }\n");
         }
         buffer.AppendFormatted("   values->RemoveAll();\n");
         buffer.AppendFormatted("   delete values;\n");
         buffer.AppendFormatted("}\n");
      }
   }
*/

   // UserParameterUsage
   buffer.AppendFormatted("void %sMonitor::UserParameterUsage() {\n", shortCut.Data());
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTab] > 0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gMonitor->GetGSP()");
      WriteSteeringParameterUsage(buffer, 0, numOfTab, pointerT, steerPointerT);
   }
   // Tab steering parameter
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("   // %s tab\n", tabName[i].Data());
      Int_t index = i;
      pointer.Resize(0);
      while (index != -1) {
         pointer.InsertFormatted(0, "->f%sTab", tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (numOfSteering[i] > 0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering", pointer.Data());
         steerPointerT.SetFormatted("((%sT%s*)gMonitor->Get%s%03dTab())->GetSP()", shortCut.Data(), tabName[i].Data(), tabName[i].Data(), i);
         WriteSteeringParameterUsage(buffer, 0, i, pointerT, steerPointerT);
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Close cpp-File
   WriteFile(cppFile.Data(), buffer.Data(), 6);
   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteMonitorH()
{
   Int_t i, j;
   ROMEString hFile;
   ROMEString buffer;
   ROMEString bankname;
   ROMEString format;

   // max folder name length
   Int_t nameLen = -1;
   Int_t fieldLen = -1;
   Int_t typeLen = -1;
   Int_t scl = shortCut.Length();
   for (i = 0; i < numOfFolder; i++) {
      if (!folderSupport[i]) {
         typeLen = TMath::Max(typeLen, static_cast<Int_t>(folderName[i].Length() + scl + strlen("*")));
         typeLen = TMath::Max(typeLen, static_cast<Int_t>(folderName[i].Length() + scl + strlen("**")));
         nameLen = TMath::Max(nameLen, static_cast<Int_t>(folderName[i].Length()));
         if (folderDataBase[i]) {
            for (j = 0; j < numOfValue[i]; j++)
               fieldLen = TMath::Max(fieldLen, valueName[i][j].Length());
         }
      }
   }
   typeLen = TMath::Max(typeLen, static_cast<Int_t>(strlen("TClonesArray*")));
   typeLen = TMath::Max(typeLen, static_cast<Int_t>(strlen("TClonesArray**")));

   // max tree name length
   Int_t treeLen = 0;
   for (i = 0; i < numOfTree; i++) {
      if (treeLen < static_cast<Int_t>(treeName[i].Length()))
         treeLen = treeName[i].Length();
   }

   // File name
   hFile.SetFormatted("%s/include/framework/%sMonitor.h", outDir.Data(), shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//          *** This file will be overwritten by the ArgusBuilder ***         //\n");
   buffer.AppendFormatted("//          ***      Don't make manual changes to this file       ***         //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sMonitor_H\n", shortCut.Data());
   buffer.AppendFormatted("#define %sMonitor_H\n\n", shortCut.Data());
   buffer.AppendFormatted("#include <ROME.h>\n");
   buffer.AppendFormatted("#include <ArgusMonitor.h>\n");
   buffer.AppendFormatted("#include <TClonesArray.h>\n");
   buffer.AppendFormatted("#include <TNetFolder.h>\n");
   buffer.AppendFormatted("#include <TArrayI.h>\n", shortCut.Data());
   // DAQ includes
   buffer.AppendFormatted("#include <include/framework/%sMidasDAQ.h>\n", shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sRomeDAQ.h>\n", shortCut.Data());
   if (this->orca)
      buffer.AppendFormatted("#include <ROMEOrcaDAQ.h>\n", shortCut.Data());
   for (i = 0; i < numOfDAQ; i++)
      buffer.AppendFormatted("#include <include/framework/%s%s.h>\n", shortCut.Data(), daqName[i].Data());

   // Folder includes
   for (i = 0; i < numOfFolder; i++) {
      if (numOfValue[i] > 0 && !folderSupport[i])
         buffer.AppendFormatted("#include \"include/framework/%s%s.h\"\n", shortCut.Data(), folderName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("\nclass %sMonitor : public ArgusMonitor\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   // friends
   if (numOfSteering[numOfTab] > 0) {
      buffer.AppendFormatted("friend class %sGlobalSteering;\n", shortCut.Data());
   }

   // Fields
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("\n");

   // Folder Fields
   buffer.AppendFormatted("   // Folder fields\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            if (folderArray[i] == "1") {
               format.SetFormatted("   %%s%%s*%%%ds f%%sFolder; %%%ds // Handle to %%s%%s Folder\n", typeLen - folderName[i].Length() - scl - strlen("*"), nameLen - folderName[i].Length());
               buffer.AppendFormatted(const_cast<Char_t*>(format.Data()), shortCut.Data(), folderName[i].Data(), "", folderName[i].Data(), "", shortCut.Data(), folderName[i].Data());
            }
            else {
               format.SetFormatted("   TClonesArray*%%%ds f%%sFolders;%%%ds // Handle to %%s%%s Folders\n", typeLen - strlen("TClonesArray*"), nameLen - folderName[i].Length());
               buffer.AppendFormatted(const_cast<Char_t*>(format.Data()), "", folderName[i].Data(), "", shortCut.Data(), folderName[i].Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Database Folder Fields
   buffer.AppendFormatted("   // Database Folder Fields\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            format.SetFormatted("   ROMEString f%%s_%%sDBName; %%%ds // Name of the database from which to read the field %%s of the folder %%s\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", valueName[i][j].Data(), folderName[i].Data());
            format.SetFormatted("   ROMEString f%%s_%%sDBPath; %%%ds // Database path to the value of the field %%s of the folder %%s\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", valueName[i][j].Data(), folderName[i].Data());
            format.SetFormatted("   TArrayI   *f%%s_%%sDBCode; %%%ds // Object Interpreter codes for database path to the value of the field %%s of the folder %%s\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", valueName[i][j].Data(), folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Steering Fields
   if (numOfSteering[numOfTab] > 0) {
      buffer.AppendFormatted("   // Steering Parameter Fields\n");
      buffer.AppendFormatted("\n   %sGlobalSteering* fGlobalSteeringParameters; // Handle to the GlobalSteering Class\n", shortCut.Data());
   }
   // DAQ Handle
   buffer.AppendFormatted("   // DAQ Handle\n");
   buffer.AppendFormatted("   %sMidasDAQ* fMidasDAQ; // Handle to the Midas DAQ Class\n", shortCut.Data());
   buffer.AppendFormatted("   %sRomeDAQ*  fRomeDAQ; // Handle to the Rome DAQ Class\n", shortCut.Data());
   if (this->orca)
      buffer.AppendFormatted("   ROMEOrcaDAQ* fOrcaDAQ; // Handle to the Orca DAQ Class\n", shortCut.Data());
   for (i = 0; i < numOfDAQ; i++)
      buffer.AppendFormatted("   %s%s*  f%s; // Handle to the %s DAQ Class\n", shortCut.Data(), daqName[i].Data(), daqName[i].Data(), daqName[i].Data());
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("public:\n");

   // Constructor
   buffer.AppendFormatted("   %sMonitor(TApplication *app = 0);\n", shortCut.Data());

   // Folder Getters
   buffer.AppendFormatted("   // Folders\n");
   for (i = 0; i < numOfFolder; i++)
      WriteFolderGetter(buffer, i, scl, nameLen, typeLen);
   buffer.AppendFormatted("\n");

   // Set size
   for (i = 0; i < numOfFolder; i++) {
      if (!folderSupport[i] && numOfValue[i] > 0 && folderArray[i] != "1") {
         if (folderArray[i] != "variable")
            buffer.AppendFormatted("private:\n");
         buffer.AppendFormatted("   void Set%sSize(Int_t number) {\n", folderName[i].Data());
         buffer.AppendFormatted("      Int_t i;\n");
         buffer.AppendFormatted("      if(f%sFolders) f%sFolders->Delete();\n", folderName[i].Data(), folderName[i].Data());
         buffer.AppendFormatted("      for (i=0;i<number;i++) {\n");
         buffer.AppendFormatted("         new((*f%sFolders)[i]) %s%s( ", folderName[i].Data(), shortCut.Data(), folderName[i].Data());
         for (j = 0; j < numOfValue[i]; j++) {
            if (isFolder(valueType[i][j].Data()))
               continue;
            if (valueDimension[i][j] == 0)
               buffer.AppendFormatted("%s,", valueInit[i][j].Data());
         }
         buffer.Resize(buffer.Length() - 1);
         buffer.AppendFormatted(" );\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
         if (folderArray[i] != "variable")
            buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   Int_t Get%sSize() { return f%sFolders->GetEntries(); }\n", folderName[i].Data(), folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Tree Getters
   buffer.AppendFormatted("   // Trees\n");
   for (i = 0; i < numOfTree; i++) {
      format.SetFormatted("   TTree* Get%%sTree()%%%ds { return ((ROMETree*)fTreeObjects->At(%%d))->GetTree(); };\n", treeLen - treeName[i].Length());
      buffer.AppendFormatted(format.Data(), treeName[i].Data(), "", i);
   }
   buffer.AppendFormatted("\n");

   // Database Getters
   buffer.AppendFormatted("   // Database\n");
   buffer.AppendFormatted("   ROMEDataBase* GetXMLDataBase() { return GetDataBase(\"XML\"); };\n");
   buffer.AppendFormatted("   ROMEDataBase* GetTextDataBase() { return GetDataBase(\"TEXT\"); };\n");
   buffer.AppendFormatted("   ROMEDataBase* GetODBDataBase() { return GetDataBase(\"ODB\"); };\n");
   if (this->sql)
      buffer.AppendFormatted("   ROMEDataBase* GetSQLDataBase() { return GetDataBase(\"SQL\"); };\n");
   for (i = 0; i < numOfDB; i++) {
      buffer.AppendFormatted("   ROMEDataBase* Get%sDataBase() { return GetDataBase(\"%s\"); };\n", dbName[i].Data(), dbName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Database Folder Field Getters
   buffer.AppendFormatted("   // Database Folder Field Getters\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            format.SetFormatted("   const Char_t* Get%%s_%%sDBName()%%%ds          { return f%%s_%%sDBName; };\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", folderName[i].Data(), valueName[i][j].Data());
            format.SetFormatted("   const Char_t* Get%%s_%%sDBPath()%%%ds          { return f%%s_%%sDBPath; };\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", folderName[i].Data(), valueName[i][j].Data());
            format.SetFormatted("   Int_t         Get%%s_%%sDBNumberOfCode()%%%ds  { return f%%s_%%sDBCode->GetSize(); };\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", folderName[i].Data(), valueName[i][j].Data());
            format.SetFormatted("   Int_t         Get%%s_%%sDBCodeAt(Int_t i)%%%ds   { return f%%s_%%sDBCode->At(i); };\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", folderName[i].Data(), valueName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Database Folder Field Setters
   buffer.AppendFormatted("   // Database Folder Field Setters\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            format.SetFormatted("   void Set%%s_%%sDBName(const Char_t* name)%%%ds  { f%%s_%%sDBName = name; };\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", folderName[i].Data(), valueName[i][j].Data());
            format.SetFormatted("   void Set%%s_%%sDBPath(const Char_t* path)%%%ds  { f%%s_%%sDBPath = path; };\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", folderName[i].Data(), valueName[i][j].Data());
            format.SetFormatted("   void Set%%s_%%sDBNumberOfCode(Int_t num)%%%ds   { f%%s_%%sDBCode->Set(num); };\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", folderName[i].Data(), valueName[i][j].Data());
            format.SetFormatted("   void Set%%s_%%sDBCodeAt(Int_t i,Int_t code)%%%ds  { f%%s_%%sDBCode->AddAt(code,i); };\n", fieldLen + nameLen - folderName[i].Length() - valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(), folderName[i].Data(), valueName[i][j].Data(), "", folderName[i].Data(), valueName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Data Base
   buffer.AppendFormatted("   // DataBase Methodes\n");
   // Read each folders
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i])
         buffer.AppendFormatted("   Bool_t Read%s();\n", folderName[i].Data());
   }
   buffer.AppendFormatted("   Bool_t ReadSingleDataBaseFolders();\n");
   buffer.AppendFormatted("   Bool_t ReadArrayDataBaseFolders();\n");
   buffer.AppendFormatted("\n");

   // Object Interpreter
   buffer.AppendFormatted("   // Object Interpreter\n");
   buffer.AppendFormatted("   Int_t         GetObjectInterpreterCode(const Char_t* objectPath);\n");
   buffer.AppendFormatted("   Int_t         GetObjectInterpreterIntValue(Int_t code,Int_t defaultValue);\n");
   buffer.AppendFormatted("   double      GetObjectInterpreterDoubleValue(Int_t code,double defaultValue);\n");
   buffer.AppendFormatted("   ROMEString& GetObjectInterpreterCharValue(Int_t code,ROMEString& defaultValue,ROMEString& buffer);\n");
   buffer.AppendFormatted("\n");

   // Steering
   if (numOfSteering[numOfTab] > 0) {
      buffer.AppendFormatted("   // Steering Parameter Methodes\n");
      buffer.AppendFormatted("   %sGlobalSteering* GetGSP() { return fGlobalSteeringParameters; };\n", shortCut.Data());
      buffer.AppendFormatted("\n");
   }
   // DAQ Access Methods
   buffer.AppendFormatted("   // DAQ Access Methods\n");
   buffer.AppendFormatted("   %sMidasDAQ* GetMidasDAQ() {\n", shortCut.Data());
   buffer.AppendFormatted("      if (fMidasDAQ==NULL) {\n");
   buffer.AppendFormatted("         this->PrintLine(\"\\nYou have tried to access the midas DAQ system over a gMonitor->GetMidasDAQ()\\nhandle but the current DAQ system is not 'Midas'.\\n\\nShutting down the program.\\n\");\n");
   buffer.AppendFormatted("         fApplication->Terminate(1);\n");
   buffer.AppendFormatted("         return NULL;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      return fMidasDAQ;\n");
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("   void     SetMidasDAQ(%sMidasDAQ* handle) { fMidasDAQ = handle; };\n", shortCut.Data());
   buffer.AppendFormatted("   %sRomeDAQ*  GetRomeDAQ() {\n", shortCut.Data());
   buffer.AppendFormatted("      if (fRomeDAQ==NULL) {\n");
   buffer.AppendFormatted("         this->PrintLine(\"\\nYou have tried to access the root DAQ system over a gMonitor->GetRomeDAQ()\\nhandle but the current DAQ system is not 'Rome'.\\n\\nShutting down the program.\\n\");\n");
   buffer.AppendFormatted("         fApplication->Terminate(1);\n");
   buffer.AppendFormatted("         return NULL;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      return fRomeDAQ;\n");
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("   void     SetRomeDAQ (%sRomeDAQ*  handle) { fRomeDAQ  = handle; };\n", shortCut.Data());
   if (this->orca) {
      buffer.AppendFormatted("   ROMEOrcaDAQ*  GetOrcaDAQ() {\n");
      buffer.AppendFormatted("      if (fOrcaDAQ==NULL) {\n");
      buffer.AppendFormatted("         this->PrintLine(\"\\nYou have tried to access the orca DAQ system over a gMonitor->GetOrcaDAQ()\\nhandle but the current DAQ system is not 'Orca'.\\n\\nShutting down the program.\\n\");\n");
      buffer.AppendFormatted("         fApplication->Terminate(1);\n");
      buffer.AppendFormatted("         return NULL;\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("      return fOrcaDAQ;\n");
      buffer.AppendFormatted("   };\n");
      buffer.AppendFormatted("   void     SetOrcaDAQ (ROMEOrcaDAQ*  handle) { fOrcaDAQ  = handle; };\n");
   }
   for (i = 0; i < numOfDAQ; i++) {
      buffer.AppendFormatted("   %s%s*  Get%s()                 { return f%s;    };\n", shortCut.Data(), daqName[i].Data(), daqName[i].Data(), daqName[i].Data());
      buffer.AppendFormatted("   void     Set%s (%s%s*  handle) { f%s  = handle; };\n", daqName[i].Data(), shortCut.Data(), daqName[i].Data(), daqName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Private
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   Bool_t         StartWindow();\n");
   buffer.AppendFormatted("   void           InitSingleFolders();\n");
   buffer.AppendFormatted("   void           InitArrayFolders();\n");
   buffer.AppendFormatted("   void           ResetFolders();\n");
   buffer.AppendFormatted("   void           CleanUpFolders();\n");
   buffer.AppendFormatted("   Bool_t         ReadUserParameter(const Char_t* opt, const Char_t* value, Int_t& i);\n");
   buffer.AppendFormatted("   void           UserParameterUsage();\n");
   buffer.AppendFormatted("   void           InitTrees();\n", shortCut.Data());

   // Set size
   for (i = 0; i < numOfFolder; i++) {
      if (!folderSupport[i] && numOfValue[i] > 0 && folderArray[i] != "1" && folderArray[i] != "variable") {
         buffer.AppendFormatted("   void SetSize%s(Int_t number) {\n", folderName[i].Data());
         buffer.AppendFormatted("      Int_t i;\n");
         buffer.AppendFormatted("      if(f%sFolders) f%sFolders->Delete();\n", folderName[i].Data(), folderName[i].Data());
         buffer.AppendFormatted("      for (i=0;i<number;i++) {\n");
         buffer.AppendFormatted("         new((*f%sFolders)[i]) %s%s( ", folderName[i].Data(), shortCut.Data(), folderName[i].Data());
         for (j = 0; j < numOfValue[i]; j++) {
            if (isFolder(valueType[i][j].Data()))
               continue;
            if (valueDimension[i][j] == 0)
               buffer.AppendFormatted("%s,", valueInit[i][j].Data());
         }
         buffer.Resize(buffer.Length() - 1);
         buffer.AppendFormatted(" );\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
      }
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ClassDef(%sMonitor,0);\n", shortCut.Data());

   // Footer
   buffer.AppendFormatted("};\n\n");
   buffer.AppendFormatted("extern %sMonitor *gMonitor;  // global Monitor Handle\n", shortCut.Data());
   buffer.AppendFormatted("#endif   // %sMonitor_H\n", shortCut.Data());

   // Write File
   WriteFile(hFile.Data(), buffer.Data(), 6);
   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteWindowCpp()
{
   Int_t i, j;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString bufferTemp;
   ROMEString format;
   ROMEString menu_title;
   ROMEString buf;

   ROMEString classDescription;
   classDescription.SetFormatted("Main window class for the %s%s. This class creates main window and manages Tabs.", shortCut.Data(), mainProgName.Data());
   ROMEString tmp;

   // File name
   cppFile.SetFormatted("%s/src/framework/%sWindow.cpp", outDir.Data(), shortCut.Data());

   // Description
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//          *** This file will be overwritten by the ArgusBuilder ***         //\n");
   buffer.AppendFormatted("//          ***      Don't make manual changes to this file       ***         //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TGMsgBox.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <ArgusTextDialog.h>\n");
   buffer.AppendFormatted("#include \"include/framework/%sWindow.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sMonitor.h\"\n", shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("ClassImp(%sWindow);\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sWindow *gWindow;\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("%sWindow::%sWindow(const TGWindow* p, Char_t* title)\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fStatusBarSwitch = kTRUE;\n");

   for (i = 0; i < numOfTab; i++) {
      Int_t index = tabParentIndex[i];
      ROMEString switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted("   fTabSwitches.%s = kTRUE;\n", switchString.Data());
      for (j = 0; j < numOfMenu[i]; j++) {
         buffer.AppendFormatted("   f%sMenu[%d] = NULL;\n", tabName[i].Data(), j);
      }
   }

   for (i = 0; i < numOfTab; i++) {
      format.SetFormatted("   f%%s%%03dTab = new %%sT%%s();\n");
      buffer.AppendFormatted(const_cast<Char_t*>(format.Data()), tabName[i].Data(), i, shortCut.Data(), tabName[i].Data());
   }
   buffer.AppendFormatted("}\n\n");
   buffer.AppendFormatted("\n");

   // CreateTabs
   buffer.AppendFormatted("Bool_t %sWindow::CreateTabs()\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t tabID = 0;\n");
   for (i = 0; i < numOfTab; i++) {
      recursiveTabDepth = 0;
      if (!AddTab(buffer, i))
         return kFALSE;
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ProcessMessage
   buffer.AppendFormatted("Bool_t %sWindow::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   // Process messages coming from widgets associated with the dialog.  \n");
   buffer.AppendFormatted("   switch (GET_MSG(msg)) {\n");
   buffer.AppendFormatted("   case kC_COMMAND:    \n");
   buffer.AppendFormatted("      switch (GET_SUBMSG(msg)) {\n");
   buffer.AppendFormatted("      case kCM_MENU:\n");
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("         if(fCurrentTabID == f%sTabID) {\n", tabName[i].Data());
      for (j = 0; j < numOfMenu[i]; j++) {
         buffer.AppendFormatted("            if (f%sMenu[%d]->GetEntry(param1)!=0)\n", tabName[i].Data(), j);
         buffer.AppendFormatted("               f%s%03dTab->MenuClicked(f%sMenu[%d],param1);\n", tabName[i].Data(), i, tabName[i].Data(), j);
      }
      buffer.AppendFormatted("         }\n");
   }
   buffer.AppendFormatted("         switch (param1) {\n");
   buffer.AppendFormatted("         case M_FILE_EXIT:\n");
   buffer.AppendFormatted("            CloseWindow();\n");
   buffer.AppendFormatted("            break;\n");
   for (i = 0; i < numOfNetFolder; i++) {
      buffer.AppendFormatted("         case M_FILE_CONNECT_%s:\n", netFolderName[i].Data());
      buffer.AppendFormatted("            gMonitor->ConnectNetFolder(\"%s\");\n", netFolderName[i].Data());
      buffer.AppendFormatted("            break;\n");
   }
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      case kCM_BUTTON:\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      case kCM_LISTBOX:\n");
   buffer.AppendFormatted("         break;      \n");
   buffer.AppendFormatted("      case kCM_TAB:\n");

   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("         if(fCurrentTabID == f%sTabID && param1 != f%sTabID) {\n", tabName[i].Data(), tabName[i].Data());
      for (j = 0; j < numOfMenu[i]; j++) {
         menu_title = menuTitle[i][j];
         menu_title.ReplaceAll("&", "");
         buffer.AppendFormatted("            delete fMenuBar->RemovePopup(\"%s\");\n", menu_title.Data());
      }
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if (");
      Int_t index = i;
      do {
         buffer.AppendFormatted(" param1 == f%sTabID ||", tabName[index].Data());
         index = tabParentIndex[index];
      } while (index != -1);
      buffer.Remove(buffer.Length() - 2);       // remove the last "||"
      buffer.AppendFormatted(") {\n");
      buffer.AppendFormatted("            f%s%03dTab->SetActive(kTRUE);\n", tabName[i].Data(), i);
      for (j = 0; j < numOfMenu[i]; j++) {
         buffer.AppendFormatted("            f%sMenu[%d] = new TGPopupMenu(fClient->GetRoot());\n", tabName[i].Data(), j);
         buffer.AppendFormatted("            f%sMenu[%d]->Associate(this);\n", tabName[i].Data(), j);
      }
      for (j = 0; j < numOfMenu[i]; j++) {
         if (menuDepth[i][j] == 1) {
            if (!AddMenuItems(buffer, i, j))
               return kFALSE;
            buffer.AppendFormatted("            fMenuBar->AddPopup(\"%s\", f%sMenu[%d],\n", menuTitle[i][j].Data(), tabName[i].Data(), j);
            buffer.AppendFormatted("                               new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));\n");
         }
      }
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if(fCurrentTabID == f%sTabID && param1 != f%sTabID) {\n", tabName[i].Data(), tabName[i].Data());
      buffer.AppendFormatted("            f%s%03dTab->TabUnSelected();\n", tabName[i].Data(), i);
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         if(fCurrentTabID != f%sTabID && param1 == f%sTabID) {\n", tabName[i].Data(), tabName[i].Data());
      buffer.AppendFormatted("            fCurrentTabID = param1;\n");
      buffer.AppendFormatted("            f%s%03dTab->TabSelected();\n", tabName[i].Data(), i);
      buffer.AppendFormatted("         }\n");
   }
   buffer.AppendFormatted("         fCurrentTabID = param1;\n");

   buffer.AppendFormatted("         MapSubwindows();\n");
   buffer.AppendFormatted("         Resize(fWidth-1,fHeight-1);\n");
   buffer.AppendFormatted("         Resize(fWidth+1,fHeight+1);\n");
   buffer.AppendFormatted("         MapWindow();\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // GetMenuHandle
   buffer.AppendFormatted("TGPopupMenu* %sWindow::GetMenuHandle(const Char_t* menuName)\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("   if(fCurrentTabID == f%sTabID) {\n", tabName[i].Data());
      for (j = 0; j < numOfMenu[i]; j++) {
         buffer.AppendFormatted("      if (!strcmp(menuName,\"%s\"))\n", menuTitle[i][j].Data());
         buffer.AppendFormatted("         return f%sMenu[%d];\n", tabName[i].Data(), j);
      }
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   cout<<\"Error: A menu (\"<<menuName<<\") was not found.\"<<endl;\n");
   buffer.AppendFormatted("   return NULL;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddMenuNetFolder
   buffer.AppendFormatted("Bool_t %sWindow::AddMenuNetFolder(TGPopupMenu* menu)\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   if(gMonitor->GetNumberOfNetFolders() <= 0 )\n", i);
   buffer.AppendFormatted("      return kFALSE;\n", i);
   buffer.AppendFormatted("   Bool_t active = kFALSE;\n", i);
   for (i = 0; i < numOfNetFolder; i++) {
      buffer.AppendFormatted("   if(gMonitor->GetNetFolderActive(%d)){\n", i);
      buffer.AppendFormatted("      menu->AddEntry(\"%s\", M_FILE_CONNECT_%s);\n", netFolderName[i].Data(), netFolderName[i].Data());
      buffer.AppendFormatted("      active = kTRUE;\n");
      buffer.AppendFormatted("   }\n", i);
   }
   buffer.AppendFormatted("   return active;\n", i);
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(), buffer.Data(), 6);
   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteWindowH()
{
   Int_t i, j, k;
   ROMEString hFile;
   ROMEString buffer;
   ROMEString buf;
   ROMEString classDescription;
   classDescription.SetFormatted("Main window class for the %s%s.", shortCut.Data(), mainProgName.Data());
   ROMEString tmp;
   ROMEString format;

   Int_t nameLen = -1;
   Int_t typeLen = -1;
   Int_t scl = shortCut.Length();
   for (i = 0; i < numOfTab; i++) {
      nameLen = TMath::Max(nameLen, static_cast<Int_t>(tabName[i].Length() + strlen("f000Tab")));
      nameLen = TMath::Max(nameLen, static_cast<Int_t>(tabName[i].Length() + scl + strlen("tT")));
      nameLen = TMath::Max(nameLen, static_cast<Int_t>(tabName[i].Length() + strlen("Get000Tab()")));
      typeLen = TMath::Max(typeLen, static_cast<Int_t>(tabName[i].Length() + scl + strlen("T*")));
   }
   typeLen = TMath::Max(typeLen, static_cast<Int_t>(strlen("TGCompositeFrame*")));

   // max tab switch name length
   Int_t switchLen = -1;
   ROMEString switchString;
   for (i = 0; i < numOfTab; i++) {
      Int_t index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (switchLen < static_cast<Int_t>(switchString.Length()))
         switchLen = switchString.Length();
   }

   // File name
   hFile.SetFormatted("%s/include/framework/%sWindow.h", outDir.Data(), shortCut.Data());

   // Description
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//          *** This file will be overwritten by the ArgusBuilder ***         //\n");
   buffer.AppendFormatted("//          ***      Don't make manual changes to this file       ***         //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sWINDOW_H\n", shortCut.Data());
   buffer.AppendFormatted("#define %sWINDOW_H\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#include \"ArgusWindow.h\"\n");
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("#include \"include/tabs/%sT%s.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Tab Switches Structure
   buffer.AppendFormatted("// Tab Switches Structure\n");
   buffer.AppendFormatted("typedef struct{\n");

   for (i = 0; i < numOfTab; i++) {
      Int_t index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      format.SetFormatted("   Bool_t %%s;%%%ds  //! %%s Tab\n", switchLen - switchString.Length());
      buffer.AppendFormatted(const_cast<Char_t*>(format.Data()), switchString.Data(), "", switchString.Data());
//      buffer.AppendFormatted("   Int_t %s;   //! %s Tab\n",switchString.Data(),switchString.Data());
   }
   buffer.AppendFormatted("} TabSwitches;\n");
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("class %sWindow : public ArgusWindow {  \n", shortCut.Data());
   // Enumeration
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   enum CommandIdentifiers{\n");
   buffer.AppendFormatted("      M_FILE_CONNECT_ROOT = 10,\n");
   for (i = 0; i < numOfNetFolder; i++)
      buffer.AppendFormatted("      M_FILE_CONNECT_%s,\n", netFolderName[i].Data());
   buffer.Remove(buffer.Length() - 2, 1);       // remove the last ','
   buffer.AppendFormatted("   };\n");

   buffer.AppendFormatted("   enum MenuEnumeration {\n");
   buffer.AppendFormatted("      M_ROOT = 1000,\n");
   for (i = 0; i < numOfTab; i++) {
      for (j = 0; j < numOfMenu[i]; j++) {
         for (k = 0; k < numOfMenuItem[i][j]; k++) {
            if (menuItemEnumName[i][j][k].Length() > 0)
               buffer.AppendFormatted("      %s,\n", menuItemEnumName[i][j][k].Data());
         }
      }
   }
   buffer.Remove(buffer.Length() - 2, 1);       // remove the last ','
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("private:\n");
   for (i = 0; i < numOfTab; i++) {
      if (tabNumOfChildren[i])
         buffer.AppendFormatted("   TGTab               *f%s%03dTabSubTab;\n", tabName[i].Data(), i);
   }
   for (i = 0; i < numOfTab; i++) {
      if (numOfMenu[i] > 0)
         buffer.AppendFormatted("   TGPopupMenu         *f%sMenu[%d];\n", tabName[i].Data(), numOfMenu[i]);
      buffer.AppendFormatted("   Int_t               f%sTabID;\n", tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Tab Switches
   buffer.AppendFormatted("   // Tab Switches\n");
   buffer.AppendFormatted("   TabSwitches fTabSwitches;               //! Tab Switches\n");
   buffer.AppendFormatted("\n");

   // Tab Fields
   buffer.AppendFormatted("   // Tab Fields\n");
   for (i = 0; i < numOfTab; i++) {
      format.SetFormatted("   %%sT%%s*%%%ds f%%s%%03dTab;%%%ds  // Handle to %%s Tab\n", typeLen - tabName[i].Length() - shortCut.Length() - strlen("T*"), nameLen - tabName[i].Length() - strlen("f000Tab"));
      buffer.AppendFormatted(const_cast<Char_t*>(format.Data()), shortCut.Data(), tabName[i].Data(), "", tabName[i].Data(), i, "", tabName[i].Data());
      format.SetFormatted("   TGCompositeFrame*%%%ds t%%sT%%s;%%%ds  // Container of %%s Tab\n", typeLen - strlen("TGCompositeFrame*"), nameLen - tabName[i].Length() - scl - strlen("tT"));
      buffer.AppendFormatted(const_cast<Char_t*>(format.Data()), "", shortCut.Data(), tabName[i].Data(), "", tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Method
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sWindow(const TGWindow *p, Char_t *title);\n", shortCut.Data());
   buffer.AppendFormatted("   ~%sWindow(){}\n", shortCut.Data());
   buffer.AppendFormatted("   Bool_t CreateTabs();\n");
   buffer.AppendFormatted("   Bool_t AddMenuNetFolder(TGPopupMenu* menu);\n");
   buffer.AppendFormatted("\n");

   // Tab Switches
   buffer.AppendFormatted("   // Tab Switches\n");
   buffer.AppendFormatted("   TabSwitches* GetTabSwitches() { return &fTabSwitches; }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   // Tabs\n");
   for (i = 0; i < numOfTab; i++) {
      format.SetFormatted("   %%sT%%s*%%%ds Get%%s%%03dTab()%%%ds { return f%%s%%03dTab;%%%ds }\n", typeLen - tabName[i].Length() - shortCut.Length() - strlen("T*"), nameLen - tabName[i].Length() - strlen("Get000Tab()"), nameLen - tabName[i].Length() - strlen("f000Tab"));
      buffer.AppendFormatted(const_cast<Char_t*>(format.Data()), shortCut.Data(), tabName[i].Data(), "", tabName[i].Data(), i, "", tabName[i].Data(), i, "");
   }
   buffer.AppendFormatted("   Bool_t       ProcessMessage(Long_t msg, Long_t param1, Long_t param2);\n");
   buffer.AppendFormatted("   TGPopupMenu* GetMenuHandle(const Char_t* menuName);\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   ClassDef(%sWindow,1)\n", shortCut.Data());
   buffer.AppendFormatted("};\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("extern %sWindow *gWindow;\n", shortCut.Data());
   buffer.AppendFormatted("#endif\n");

   // Write File
   WriteFile(hFile.Data(), buffer.Data(), 6);
   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::AddTab(ROMEString &buffer, Int_t &i)
{
   Int_t j;
   ROMEString parentt;
   ROMEString format;
   Int_t index = tabParentIndex[i];

   ROMEString switchString = tabName[i].Data();
   Int_t depth;
   while (index != -1) {
      switchString.Insert(0, "_");
      switchString.Insert(0, tabName[index].Data());
      index = tabParentIndex[index];
   }
   if (tabParentIndex[i] == -1)
      parentt = "fTab";
   else
      parentt.SetFormatted("f%s%03dTabSubTab", tabName[tabParentIndex[i]].Data(), tabParentIndex[i]);

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("   if (fTabSwitches.%s){\n", switchString.Data());

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("      t%sT%s = %s->AddTab(\"%s\");\n", shortCut.Data(), tabName[i].Data(), parentt.Data(), tabTitle[i].Data());

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("      f%s%03dTab->ReparentWindow(t%sT%s, 60, 20);\n", tabName[i].Data(), i, shortCut.Data(), tabName[i].Data());

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("      f%s%03dTab->Init();\n", tabName[i].Data(), i);

   if (!tabNumOfChildren[i]) {
      for (depth = 0; depth < recursiveTabDepth; depth++)
         buffer += "   ";
      buffer.AppendFormatted("      t%sT%s->AddFrame(f%s%03dTab,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0));\n", shortCut.Data(), tabName[i].Data(), tabName[i].Data(), i);
   }

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";

   buffer.AppendFormatted("      f%sTabID = tabID++;\n", tabName[i].Data());

   if (tabNumOfChildren[i]) {
      for (depth = 0; depth < recursiveTabDepth; depth++)
         buffer += "   ";
      buffer.AppendFormatted("      f%s%03dTabSubTab = new TGTab(t%sT%s, (UInt_t)(600*gMonitor->GetWindowScale()), (UInt_t)(400*gMonitor->GetWindowScale()));\n", tabName[i].Data(), i, shortCut.Data(), tabName[i].Data());
   }
   recursiveTabDepth++;
   j = i;

   while (i < j + tabNumOfChildren[j]) {
      i++;
      if (!AddTab(buffer, i))
         return kFALSE;
   }

   recursiveTabDepth--;

   if (tabNumOfChildren[j]) {
      for (depth = 0; depth < recursiveTabDepth; depth++)
         buffer += "   ";
      buffer.AppendFormatted("      t%sT%s->AddFrame(f%s%03dTabSubTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));\n", shortCut.Data(), tabName[j].Data(), tabName[j].Data(), j);
   }

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("   }\n");

   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::AddMenuItems(ROMEString &buffer, Int_t i, Int_t j)
{
   Int_t k;

   for (k = 0; k < numOfMenuItem[i][j]; k++) {
      if (menuItemTitle[i][j][k] == LINE_TITLE) {
         buffer.AppendFormatted("            f%sMenu[%d]->AddSeparator();\n", tabName[i].Data(), j);
      }
      else if (menuItemChildMenuIndex[i][j][k]) {
         if (!AddMenuItems(buffer, i, menuItemChildMenuIndex[i][j][k]))
            return kFALSE;
         buffer.AppendFormatted("            f%sMenu[%d]->AddPopup(\"%s\", f%sMenu[%d]);\n", tabName[i].Data(), j, menuTitle[i][menuItemChildMenuIndex[i][j][k]].Data()
                                , tabName[i].Data(), menuItemChildMenuIndex[i][j][k]);
      }
      else {
         buffer.AppendFormatted("            f%sMenu[%d]->AddEntry(\"%s\", %s);\n", tabName[i].Data(), j, menuItemTitle[i][j][k].Data()
                                , menuItemEnumName[i][j][k].Data());
      }
   }

   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::ReadXMLMenu(Int_t currentNumberOfTabs)
{
   ROMEString tmp;
   Int_t type, i, j;
   Char_t *name;

   Int_t currentNumberOfMenus = numOfMenu[currentNumberOfTabs];
   menuDepth[currentNumberOfTabs][currentNumberOfMenus] = recursiveMenuDepth;
   // count menus
   numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] = -1;
   if (currentNumberOfMenus >= maxNumberOfMenus) {
      cout << "Maximal number of menus reached : " << maxNumberOfMenus << " !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   menuTitle[currentNumberOfTabs][currentNumberOfMenus] = "";

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();

      // end
      if (type == 15 && !strcmp(name, "Menu")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 2; i++)
               cout << "   ";
         if (makeOutput)
            menuTitle[currentNumberOfTabs][currentNumberOfMenus].WriteLine();
         numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
         for (j = 0; j < numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]; j++) {
            if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j] != LINE_TITLE) {
               if (makeOutput)
                  for (i = 0; i < recursiveTabDepth + 3; i++)
                     cout << "   ";
               if (makeOutput)
                  menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j].WriteLine();
            }
         }
         break;
      }
      if (type == 1 && !strcmp(name, "MenuTitle"))
         xml->GetValue(menuTitle[currentNumberOfTabs][currentNumberOfMenus], menuTitle[currentNumberOfTabs][currentNumberOfMenus]);

      // tab menu items
      if (type == 1 && !strcmp(name, "MenuItems")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();

            // end
            if (type == 15 && !strcmp(name, "MenuItems"))
               break;

            // menu
            if (type == 1 && !strcmp(name, "Menu")) {
               recursiveMenuDepth++;
               numOfMenu[currentNumberOfTabs]++;
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout << "Maximal number of menu items reached : " << maxNumberOfMenuItems << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               menuItemChildMenuIndex[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = numOfMenu[currentNumberOfTabs];
               if (!ReadXMLMenu(currentNumberOfTabs))
                  return kFALSE;
               recursiveMenuDepth--;
            }
            // Splitter
            if (type == 1 && !strcmp(name, "Line")) {
               // count menu items
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout << "Maximal number of menu items reached : " << maxNumberOfMenuItems << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = LINE_TITLE;
               menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = "";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // end
                  if (type == 15 && !strcmp(name, "Line"))
                     break;
               }
            }
            // menu item
            if (type == 1 && !strcmp(name, "MenuItem")) {
               // count menu items
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout << "Maximal number of menu items reached : " << maxNumberOfMenuItems << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = "";
               menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = "";
               menuItemChildMenuIndex[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();

                  // end
                  if (type == 15 && !strcmp(name, "MenuItem"))
                     break;
                  if (type == 1 && !strcmp(name, "MenuItemTitle"))
                     xml->GetValue(menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]], menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]);
                  if (type == 1 && !strcmp(name, "MenuItemEnumName")) {
                     xml->GetValue(menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]], menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]);
                  }
               }

               // check input
               if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] == "") {
                  cout << currentNumberOfTabs << endl;
                  cout << currentNumberOfMenus << endl;
                  cout << numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] << endl;
                  cout << "A menu item of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Title !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               for (j = 0; j < numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]; j++) {
                  if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j] != LINE_TITLE && menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j]
                      == menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]) {
                     cout << "Two menu items of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Title !" << endl;
                     cout << "Terminating program." << endl;
                     return kFALSE;
                  }
               }
            }
         }
      }
   }

   // check input
   if (menuTitle[currentNumberOfTabs][currentNumberOfMenus] == "") {
      cout << "A menu of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Title !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   for (j = 0; j < currentNumberOfMenus; j++) {
      if (menuTitle[currentNumberOfTabs][j] == menuTitle[currentNumberOfTabs][currentNumberOfMenus]) {
         cout << "Two menus of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Title !" << endl;
         cout << "Terminating program." << endl;
         return kFALSE;
      }
   }

   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::ReadXMLTab()
{
   // read the tab definitions out of the xml file
   ROMEString tmp;
   Char_t *name;
   Int_t type, i, j;
   ROMEString currentTabName = "";
   Int_t currentNumberOfTabs = 0;

   // count tabs
   numOfTab++;
   numOfTask++;
   numOfTaskHierarchy++;
   currentNumberOfTabs = numOfTab;
   if (currentNumberOfTabs >= maxNumberOfTabs) {
      cout << "Maximal number of tabs reached : " << maxNumberOfTabs << " !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   // initialisation
   tabName[currentNumberOfTabs] = "";
   tabTitle[currentNumberOfTabs] = "";
   tabAuthor[currentNumberOfTabs] = mainAuthor;
   tabVersion[currentNumberOfTabs] = "1";
   tabDescription[currentNumberOfTabs] = "";
   numOfSteering[currentNumberOfTabs] = -1;
   numOfThreadFunctions[currentNumberOfTabs] = -1;
   numOfMenu[currentNumberOfTabs] = -1;
   tabNumOfChildren[currentNumberOfTabs] = 0;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();

      // subtab
      if (type == 1 && !strcmp(name, "Tab")) {
         // set tab as parent for subsequent tabs
         recursiveTabDepth++;
         tabParentIndex[numOfTab + 1] = taskHierarchyParentIndex[numOfTab + 1] = currentNumberOfTabs;
         tabNumOfChildren[currentNumberOfTabs]++;

         // read subtab
         if (!ReadXMLTab())
            return kFALSE;
         continue;
      }
      // end tab
      if (type == 15 && !strcmp(name, "Tab")) {
         // check input
         if (currentTabName == "") {
            cout << "The " << (currentNumberOfTabs + 1) << ". Tab has no name !" << endl;
            cout << "Terminating program." << endl;
            return kFALSE;
         }
         recursiveTabDepth--;
         return kTRUE;
      }
      // tab name
      if (type == 1 && !strcmp(name, "TabName")) {
         xml->GetValue(tabName[currentNumberOfTabs], tabName[currentNumberOfTabs]);
         currentTabName = tabName[currentNumberOfTabs];
         // output
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth; i++)
               cout << "   ";
         if (makeOutput)
            tabName[currentNumberOfTabs].WriteLine();
      }
      // tab title
      if (type == 1 && !strcmp(name, "TabTitle"))
         xml->GetValue(tabTitle[currentNumberOfTabs], tabTitle[currentNumberOfTabs]);

      // tab author
      if (type == 1 && !strcmp(name, "Author"))
         xml->GetValue(tabAuthor[currentNumberOfTabs], tabAuthor[currentNumberOfTabs]);

      // tab version
      if (type == 1 && !strcmp(name, "TabVersion"))
         xml->GetValue(tabVersion[currentNumberOfTabs], tabVersion[currentNumberOfTabs]);

      // tab description
      if (type == 1 && !strcmp(name, "TabDescription"))
         xml->GetValue(tabDescription[currentNumberOfTabs], tabDescription[currentNumberOfTabs]);

      // tab steering parameters
      if (type == 1 && !strcmp(name, "SteeringParameters")) {
         // read steering parameter
         steerName[currentNumberOfTabs][0] = "Steering";
         steerParent[currentNumberOfTabs][0] = -1;
         actualSteerIndex = 0;
         recursiveSteerDepth = 0;
         if (!ReadXMLSteering(currentNumberOfTabs))
            return kFALSE;
         numOfSteering[currentNumberOfTabs]++;
      }
      // tab threadFunctions
      if (type == 1 && !strcmp(name, "ThreadFunctions")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 1; i++)
               cout << "   ";
         if (makeOutput)
            cout << "ThreadFunctions:" << endl;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // end
            if (type == 15 && !strcmp(name, "ThreadFunctions"))
               break;

            // thread function
            if (type == 1 && !strcmp(name, "ThreadFunction")) {
               // count thread functions
               numOfThreadFunctions[currentNumberOfTabs]++;
               if (numOfThreadFunctions[currentNumberOfTabs] >= maxNumberOfThreadFunctions) {
                  cout << "Maximal number of thread functions reached : " << maxNumberOfThreadFunctions << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] = "";
               numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();

                  // end
                  if (type == 15 && !strcmp(name, "ThreadFunction")) {
                     // output
                     if (makeOutput)
                        for (i = 0; i < recursiveTabDepth + 2; i++)
                           cout << "   ";
                     if (makeOutput)
                        threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]].WriteLine();
                     break;
                  }
                  if (type == 1 && !strcmp(name, "FunctionName"))
                     xml->GetValue(threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]], threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]);
                  if (type == 1 && !strcmp(name, "FunctionArgument")) {
                     xml->GetValue(threadFunctionArgument[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]][numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]], threadFunctionArgument[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]][numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]]);
                     numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]++;
                  }
               }

               // check input
               if (threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] == "") {
                  cout << "A thread function of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Name !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               for (j = 0; j < numOfThreadFunctions[currentNumberOfTabs]; j++) {
                  if (threadFunctionName[currentNumberOfTabs][j] == threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]) {
                     cout << "Two thread functions of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Name !" << endl;
                     cout << "Terminating program." << endl;
                     return kFALSE;
                  }
               }
            }
         }
      }
      // tab menu
      if (type == 1 && !strcmp(name, "Menus")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 1; i++)
               cout << "   ";
         if (makeOutput)
            cout << "Menus:" << endl;
         recursiveMenuDepth = 0;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();

            // end
            if (type == 15 && !strcmp(name, "Menus")) {
               numOfMenu[currentNumberOfTabs]++;
               break;
            }
            // menu
            if (type == 1 && !strcmp(name, "Menu")) {
               recursiveMenuDepth++;
               numOfMenu[currentNumberOfTabs]++;
               if (!ReadXMLMenu(currentNumberOfTabs))
                  return kFALSE;
               recursiveMenuDepth--;
            }
         }
      }

   }

   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteTabCpp()
{
   ROMEString cppFile;
   ROMEString header;
   ROMEString buffer;
   Int_t lenTot, ll, i;
   Char_t *pos;
   ROMEString format;
   ROMEString discript;
   ROMEString str;

   if (makeOutput)
      cout << "\n   Output Cpp-Files:" << endl;
   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      header.Resize(0);
      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%s/src/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());

      // Description
      header.AppendFormatted("//// Author: %s\n", tabAuthor[iTab].Data());
      header.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      header.AppendFormatted("//                                                                            //\n");
      ll = 73 - shortCut.Length();
      format.SetFormatted("// %%sT%%-%d.%ds //\n", ll, ll);
      header.AppendFormatted(const_cast<Char_t*>(format.Data()), shortCut.Data(), tabName[iTab].Data());
      header.AppendFormatted("//                                                                            //\n");
      discript = tabDescription[iTab].Data();
      pos = const_cast<Char_t*>(discript.Data());
      lenTot = discript.Length();
      while (pos - discript.Data() < lenTot) {
         if (lenTot + (discript.Data() - pos) < 74)
            i = lenTot + (discript.Data() - pos);
         else
            for (i = 74; pos[i] != 32 && i > 0; i--) {
            }
         if (i <= 0)
            i = TMath::Min(75, lenTot);
         pos[i] = 0;
         header.AppendFormatted("// %-74.74s   \n", pos);
         pos = pos + i + 1;
      }
      header.AppendFormatted("// \n");
      header.AppendFormatted("// \n");
      header.AppendFormatted("// This header has been generated by the ArgusBuilder.\n");
      header.AppendFormatted("//                                                                            //\n");

      // Thread
      if (numOfThreadFunctions[iTab] > 0) {
         header.AppendFormatted("// This tab has following thread functions.                                   //\n");
         for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
            format.SetFormatted("//    %%s%%%ds//\n", 72 - threadFunctionName[iTab][i].Length());
            header.AppendFormatted(const_cast<Char_t*>(format.Data()), threadFunctionName[iTab][i].Data(), "");
         }
         header.AppendFormatted("//                                                                            //\n");
      }
      header.AppendFormatted("//                                                                            //\n");
      header.AppendFormatted("/////////////////////////////////////----///////////////////////////////////////");
      buffer.Resize(0);

      // Header
      buffer.AppendFormatted("\n\n#include \"include/tabs/%sT%s.h\"\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("#include \"include/framework/%sWindow.h\"\n", shortCut.Data());
      buffer.AppendFormatted("\nClassImp(%sT%s)\n\n", shortCut.Data(), tabName[iTab].Data());

      // Functions
      buffer.AppendFormatted("void %sT%s::Init()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::MenuClicked(TGPopupMenu *menu,Long_t param)\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::TabSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");

      buffer.AppendFormatted("void %sT%s::TabUnSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("void %sT%s::%s()\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   gSystem->Sleep(10000);\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }

      // Write file
      ReplaceHeader(cppFile.Data(), header.Data(), buffer.Data(), 6);
   }
   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteTabH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   Int_t i, j;
   if (makeOutput)
      cout << "\n   Output H-Files:" << endl;

   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      // File name
      hFile.SetFormatted("%s/include/tabs/%sT%s_Base.h", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//          *** This file will be overwritten by the ArgusBuilder ***         //\n");
      buffer.AppendFormatted("//          ***      Don't make manual changes to this file       ***         //\n");
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

      // Header
      buffer.AppendFormatted("#ifndef %sT%s_Base_H\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("#define %sT%s_Base_H\n\n", shortCut.Data(), tabName[iTab].Data());
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include <TGFrame.h>\n");
      buffer.AppendFormatted("#include <TGMenu.h>\n");
      buffer.AppendFormatted("#include \"include/framework/%sMonitor.h\"\n", shortCut.Data());
      buffer.AppendFormatted("#include <TThread.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("struct %sArgs{\n", tabName[iTab].Data());
      buffer.AppendFormatted("   void*  inst;\n");
      buffer.AppendFormatted("   Long_t msg;\n");
      buffer.AppendFormatted("   Long_t param1;\n");
      buffer.AppendFormatted("   Long_t param2;\n");
      buffer.AppendFormatted("};\n");

      // Class
      buffer.AppendFormatted("\nclass %sT%s_Base : public TGCompositeFrame\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("protected:\n");

      // Fields
      if (numOfSteering[iTab] > 0) {
         WriteSteeringClass(buffer, 0, iTab, 1);
         buffer.AppendFormatted("\n");
      }
      if (numOfSteering[iTab] > 0) {
         buffer.AppendFormatted("   Steering* fSteering; //! Handle to Steering class\n\n");
      }
      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("   TThread* m%s; //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Bool_t   f%sActive; //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Int_t    f%sNumberOfLoops; //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Int_t    f%sInterval; //!\n", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("   %s f%sArgument_%d; //!\n", threadFunctionArgument[iTab][i][j].Data(), threadFunctionName[iTab][i].Data(), j);
      }
      buffer.AppendFormatted("   Int_t    fVersion; //! Version number\n");
      buffer.AppendFormatted("   Bool_t   fActive; //! is Active\n");

      // Methods
      buffer.AppendFormatted("public:\n");

      // Constructor
      buffer.AppendFormatted("   // Constructor\n");
      buffer.AppendFormatted("   %sT%s_Base():TGCompositeFrame(){\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("      fVersion = %s;\n", tabVersion[iTab].Data());
      buffer.AppendFormatted("      fActive  = kFALSE;\n");
      if (numOfSteering[iTab] > 0) {
         buffer.AppendFormatted("      fSteering = new Steering();\n");
      }
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("      m%s       = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   ~%sT%s_Base(){\n", shortCut.Data(), tabName[iTab].Data());
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("      Stop%s();\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");

      // Thread
      buffer.AppendFormatted("   virtual Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2){\n");
      buffer.AppendFormatted("      return RunProcessMessageThread(msg, param1, param2);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   virtual Bool_t ProcessMessageThread(Long_t msg, Long_t param1, Long_t param2){return kTRUE;}\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   static void ThreadProcessMessageThread(void* arg){\n");
      buffer.AppendFormatted("      ((%sT%s_Base*)((%sArgs*)arg)->inst)->ProcessMessageThread(((%sArgs*)arg)->msg, ((%sArgs*)arg)->param1, ((%sArgs*)arg)->param2);\n", shortCut.Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   Bool_t RunProcessMessageThread(Long_t msg, Long_t param1, Long_t param2){\n");
      buffer.AppendFormatted("      %sArgs* arg = new %sArgs();\n", tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("      arg->inst   = this;\n");
      buffer.AppendFormatted("      arg->msg    = msg;\n");
      buffer.AppendFormatted("      arg->param1 = param1;\n");
      buffer.AppendFormatted("      arg->param2 = param2;\n");
      buffer.AppendFormatted("      TThread* mProcessMessageThread = new TThread(\"processMessageThread\",(void(*) (void *))&ThreadProcessMessageThread,(void*) arg);\n");
      buffer.AppendFormatted("      mProcessMessageThread->Run();\n");
      buffer.AppendFormatted("      return kTRUE;\n");
      buffer.AppendFormatted("   }\n");
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   //%s\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   virtual void %s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("%s", threadFunctionArgument[iTab][i][j].Data());
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted(")\n");
         buffer.AppendFormatted("   {\n");
         buffer.AppendFormatted("      cout<<endl\n");
         buffer.AppendFormatted("          <<\" Thread function %s is not implemented.\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\" Please overwrite this function in derived class. For example,\"<<endl\n");
         buffer.AppendFormatted("          <<\" In %sT%s.h,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("          <<\"   void %s();\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\" In %sT%s.cpp,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("          <<\"   void %sT%s::%s()\"<<endl\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\"   {\"<<endl\n");
         buffer.AppendFormatted("          <<\"      cout<<\\\"Thread function %s is running.\\\"<<endl;\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\"   }\"<<endl<<endl;\n");
         buffer.AppendFormatted("      Stop%s();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   static void Thread%s(void* arg){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      TThread::SetCancelOn();\n");
         buffer.AppendFormatted("      TThread::SetCancelDeferred();\n");
         buffer.AppendFormatted("      %sT%s_Base* inst = (%sT%s_Base*) arg;\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("      Int_t iLoop = 0;\n");
//         buffer.AppendFormatted("      Int_t meid=TThread::SelfId(); // get pthread id\n");
         buffer.AppendFormatted("      while(inst->f%sActive){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         TThread::CancelPoint();\n");
         buffer.AppendFormatted("         inst->%s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("inst->f%sArgument_%d", threadFunctionName[iTab][i].Data(), j);
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted("); // call the user defined threaded function\n");
         buffer.AppendFormatted("         if(inst->f%sNumberOfLoops != 0 && ++iLoop>=inst->f%sNumberOfLoops) {\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            inst->f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            if(inst->m%s){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("               TThread::Delete(inst->m%s);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("               inst->m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         gSystem->Sleep(inst->f%sInterval);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t Start%s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("%s arg%d,", threadFunctionArgument[iTab][i][j].Data(), j);
         buffer.AppendFormatted("Int_t interval = 1000, Int_t nloop = 0){\n");

         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("      f%sArgument_%d = arg%d;\n", threadFunctionName[iTab][i].Data(), j, j);
         buffer.AppendFormatted("      f%sActive        = kTRUE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sNumberOfLoops = nloop;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sInterval      = interval;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      if(!m%s){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = new TThread(\"Thread%s\",(void(*) (void *))&Thread%s,(void*) this);\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s->Run();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t Stop%s(){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      gSystem->Sleep(1000); // wait a while for threads to halt\n");
         buffer.AppendFormatted("      if(m%s){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         TThread::Delete(m%s);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   } \n");
         buffer.AppendFormatted("   Bool_t Kill%s(){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      if(m%s){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s->Kill();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   } \n");

         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("\n");

      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTab] > 0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() { return fSteering; };\n");
         buffer.AppendFormatted("   Steering* GetSP() { return fSteering; };\n");
      }
      buffer.AppendFormatted("   Bool_t       GetActive() { return fActive; };\n");
      buffer.AppendFormatted("   void         SetActive(Bool_t active) { fActive = active; };\n");
      buffer.AppendFormatted("   virtual void TabSelected(){};\n");
      buffer.AppendFormatted("   virtual void TabUnSelected(){};\n");
      buffer.AppendFormatted("   virtual void MenuClicked(TGPopupMenu *menu,Long_t param)\n");
      buffer.AppendFormatted("   {\n");
      buffer.AppendFormatted("      cout<<endl\n");
      buffer.AppendFormatted("          <<\" Function MenuClicked is not implemented in %s.\"<<endl\n", tabName[iTab].Data());
      buffer.AppendFormatted("          <<\" Please overwrite this function in derived class. For example,\"<<endl\n");
      buffer.AppendFormatted("          <<\" In %sT%s.h,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("          <<\"   void MenuClicked(TGPopupMenu *menu,Long_t param);\"<<endl\n");
      buffer.AppendFormatted("          <<\" In %sT%s.cpp,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("          <<\"   void %sT%s::MenuClicked(TGPopupMenu *menu,Long_t param)\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("          <<\"   {\"<<endl\n");
      buffer.AppendFormatted("          <<\"      cout<<\\\"param = \\\"<< param <<endl;\"<<endl\n");
      buffer.AppendFormatted("          <<\"   }\"<<endl<<endl;\n");
      buffer.AppendFormatted("   }\n");

      // Footer
      buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s)\n", shortCut.Data(), tabName[iTab].Data(), tabVersion[iTab].Data());
      buffer.AppendFormatted("};\n\n");
      buffer.AppendFormatted("#endif   // %sT%s_Base_H\n", shortCut.Data(), tabName[iTab].Data());

      // Write File
      WriteFile(hFile.Data(), buffer.Data(), 6);

      // User H-File
      hFile.SetFormatted("%s/include/tabs/%sT%s.h", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      struct stat buf;
      if (stat(hFile.Data(), &buf)) {
         // Description
         buffer.Resize(0);
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("// Editable class file for %s tab.\n", tabName[iTab].Data());
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

         // Header
         buffer.AppendFormatted("#ifndef %sT%s_H\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("#define %sT%s_H\n\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("#include <include/tabs/%sT%s_Base.h>\n", shortCut.Data(), tabName[iTab].Data());

         // Class
         buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("protected:\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("public:\n");

         // Constructor
         buffer.AppendFormatted("   %sT%s():%sT%s_Base()\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("   {\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   ~%sT%s()\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("   {\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   void Init();\n");
         buffer.AppendFormatted("   void MenuClicked(TGPopupMenu *menu,Long_t param);\n");
         buffer.AppendFormatted("   void TabSelected();\n");
         buffer.AppendFormatted("   void TabUnSelected();\n");

         // Thread
         for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
            buffer.AppendFormatted("   void %s();\n", threadFunctionName[iTab][i].Data());
         }
         buffer.AppendFormatted("\n");

         // Fields
         buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)\n", shortCut.Data(), tabName[iTab].Data(), tabVersion[iTab].Data());
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %sT%s_H\n", shortCut.Data(), tabName[iTab].Data());

         // Write File
         WriteFile(hFile.Data(), buffer.Data(), 6);
      }
   }

   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteTabConfigWrite(ROMEString &buffer, Int_t parentIndex, ROMEString &pointer, Int_t tab)
{
   Int_t i;

   // max tab switch name length
   Int_t switchLen = -1;
   ROMEString switchString;
   for (i = 0; i < numOfTab; i++) {
      Int_t index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (switchLen < static_cast<Int_t>(switchString.Length()))
         switchLen = switchString.Length();
   }

   ROMEString blank = "";
   for (i = 0; i < tab; i++)
      blank.Append("   ");

   ROMEString pointerI;
   for (i = 0; i < numOfTab; i++) {
      if (tabParentIndex[i] != parentIndex)
         continue;
      Int_t index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      pointerI = pointer;
      pointerI.AppendFormatted("->f%sTab", tabName[i].Data());
      buffer.AppendFormatted("%s      if (fConfigData[index]%sModified || index==0) {\n", blank.Data(), pointerI.Data());
      buffer.AppendFormatted("%s         // %s\n", blank.Data(), pointerI.Data());
      buffer.AppendFormatted("%s         xml->StartElement(\"Tab\");\n", blank.Data());
      buffer.AppendFormatted("%s         xml->WriteElement(\"TabName\",\"%s\");\n", blank.Data(), tabName[i].Data());
      buffer.AppendFormatted("%s         if (index==0) {\n", blank.Data());
      buffer.AppendFormatted("%s            if (gWindow->GetTabSwitches()->%s)\n", blank.Data(), switchString.Data());
      buffer.AppendFormatted("%s               xml->WriteElement(\"Active\",\"true\");\n", blank.Data());
      buffer.AppendFormatted("%s            else\n", blank.Data());
      buffer.AppendFormatted("%s               xml->WriteElement(\"Active\",\"false\");\n", blank.Data());
      buffer.AppendFormatted("%s         }\n", blank.Data());
      buffer.AppendFormatted("%s         else if (fConfigData[index]%s->fActiveModified)\n", blank.Data(), pointerI.Data());
      buffer.AppendFormatted("%s            xml->WriteElement(\"Active\",const_cast<Char_t*>(fConfigData[index]%s->fActive.Data()));\n", blank.Data(), pointerI.Data());

      // Steering parameter
      if (numOfSteering[i] > 0) {
         buffer.AppendFormatted("%s         // steering parameters\n", blank.Data());
         buffer.AppendFormatted("%s         if (fConfigData[index]%s->fSteeringModified || index==0) {\n", blank.Data(), pointerI.Data());
         buffer.AppendFormatted("%s            ROMEString value;\n", blank.Data());
         ROMEString pointerT;
         ROMEString steerPointerT;
         Int_t indexCounter = 0;
         pointerT.SetFormatted("fConfigData[index]%s->fSteering", pointerI.Data());
         steerPointerT.SetFormatted("gWindow->Get%s%03dTab()->GetSP()", tabName[i].Data(), i);
         WriteSteeringConfigWrite(buffer, 0, i, pointerT, steerPointerT, 3 + tab, &indexCounter);
      }
      if (numOfSteering[i] > 0)
         buffer.AppendFormatted("%s         }\n", blank.Data());
      WriteTabConfigWrite(buffer, i, pointerI, tab + 1);
      buffer.AppendFormatted("%s         xml->EndElement();\n", blank.Data());
      buffer.AppendFormatted("%s      }\n", blank.Data());
   }

   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteTabConfigClass(ROMEString &buffer, Int_t parentIndex, Int_t tab)
{
   Int_t j, i;

   ROMEString blank = "";
   for (i = 0; i < tab; i++)
      blank.Append("   ");

   for (i = 0; i < numOfTab; i++) {
      if (tabParentIndex[i] != parentIndex)
         continue;
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("%s      class %sTab {\n", blank.Data(), tabName[i].Data());
      buffer.AppendFormatted("%s      public:\n", blank.Data());
      buffer.AppendFormatted("%s         ROMEString       fActive;\n", blank.Data());
      buffer.AppendFormatted("%s         Bool_t           fActiveModified;\n", blank.Data());
      if (numOfSteering[i] > 0) {
         buffer.AppendFormatted("%s         // steering parameters\n", blank.Data());
         buffer.AppendFormatted("%s         class Steering {\n", blank.Data());
         buffer.AppendFormatted("%s         public:\n", blank.Data());
         WriteSteeringConfigClass(buffer, 0, i, tab + 3);
         buffer.AppendFormatted("%s         };\n", blank.Data());
         buffer.AppendFormatted("%s         Steering*        fSteering;\n", blank.Data());
         buffer.AppendFormatted("%s         Bool_t           fSteeringModified;\n", blank.Data());
      }
      // Constructor
      buffer.AppendFormatted("%s      public:\n", blank.Data());
      buffer.AppendFormatted("%s         %sTab() {\n", blank.Data(), tabName[i].Data());
      if (numOfSteering[i] > 0) {
         buffer.AppendFormatted("%s         fSteeringModified = kFALSE;\n", blank.Data());
         buffer.AppendFormatted("%s         fSteering         = new Steering();\n", blank.Data());
      }
      for (j = 0; j < numOfTab; j++) {
         if (tabParentIndex[j] != i)
            continue;
         buffer.AppendFormatted("%s            f%sTab = new %sTab();\n", blank.Data(), tabName[j].Data(), tabName[j].Data());
      }
      buffer.AppendFormatted("%s         };\n", blank.Data());

      // Sub classes
      WriteTabConfigClass(buffer, i, tab + 1);
      buffer.AppendFormatted("%s      };\n", blank.Data());
      buffer.AppendFormatted("%s      %sTab*          f%sTab;\n", blank.Data(), tabName[i].Data(), tabName[i].Data());
      buffer.AppendFormatted("%s      Bool_t           f%sTabModified;\n", blank.Data(), tabName[i].Data());
   }
   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteConfigCpp()
{
   Int_t i, j;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString classDescription;

   classDescription.SetFormatted("This class handles the monitor configuration file (argusConfig.xml).");
   // File name
   cppFile.SetFormatted("%s/src/framework/%sConfig.cpp", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//          *** This file will be overwritten by the ArgusBuilder ***         //\n");
   buffer.AppendFormatted("//          ***      Don't make manual changes to this file       ***         //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#include <ROMEXMLDataBase.h>\n");
   buffer.AppendFormatted("#include <ROMETextDataBase.h>\n");
   buffer.AppendFormatted("#include <ROMENoDataBase.h>\n");
   buffer.AppendFormatted("#include <ROMEODBOfflineDataBase.h>\n");
   buffer.AppendFormatted("#include <ROMEODBOnlineDataBase.h>\n");
   if (this->sql)
      buffer.AppendFormatted("#include <ROMESQLDataBase.h>\n");
   buffer.AppendFormatted("#include \"include/framework/%sConfig.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sMonitor.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sWindow.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sMidasDAQ.h>\n", shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sRomeDAQ.h>\n", shortCut.Data());
   if (this->orca)
      buffer.AppendFormatted("#include <ROMEOrcaDAQ.h>\n");
   buffer.AppendFormatted("#include <ROMENoDAQSystem.h>\n");
   for (i = 0; i < numOfDAQ; i++)
      buffer.AppendFormatted("#include <include/framework/%s%s.h>\n", shortCut.Data(), daqName[i].Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sConfig::%sConfig() {\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("   fConfigData = new ConfigData*[1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("}\n\n");

   // Read Configuration File
   buffer.AppendFormatted("\n// Read Configuration File\n");
   buffer.AppendFormatted("Bool_t %sConfig::ReadConfigurationFile(const Char_t *file) {\n", shortCut.Data());
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   xml->OpenFileForPath(fXMLFile);\n");
   buffer.AppendFormatted("   delete [] fConfigData;\n");
   buffer.AppendFormatted("   fConfigData = new ConfigData*[1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("   ROMEString path = \"/Configuration\";\n");
   buffer.AppendFormatted("   ReadConfiguration(xml,path,0);\n");
   buffer.AppendFormatted("   if (!SetConfiguration())\n");
   buffer.AppendFormatted("      return kFALSE;\n");
   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");

   // Read Configuration
   buffer.AppendFormatted("\n// Read Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::ReadConfiguration(ROMEXML *xml,ROMEString& path,Int_t index) {\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   ROMEString buf = \"\";\n");

   // RunNumbers
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/RunNumbers\",fConfigData[index]->fRunNumbers,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fRunNumbers==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fRunNumbersModified = kFALSE;\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("      fConfigData[index]->fRunNumbersModified = kTRUE;\n");
   buffer.AppendFormatted("      fConfigData[index]->fLastRunNumberIndex = 0;\n");
   buffer.AppendFormatted("      gMonitor->DecodeRunNumbers(fConfigData[index]->fRunNumbers,fConfigData[index]->fRunNumberArray);\n");
   buffer.AppendFormatted("   }\n");

   // InputFileNames
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/InputFileNames\",fConfigData[index]->fInputFileNames,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fInputFileNames==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fInputFileNamesModified = kFALSE;\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("      fConfigData[index]->fInputFileNamesModified = kTRUE;\n");
   buffer.AppendFormatted("      fConfigData[index]->fLastInputFileNameIndex = 0;\n");
   buffer.AppendFormatted("      gMonitor->DecodeInputFileNames(fConfigData[index]->fInputFileNames,fConfigData[index]->fInputFileNameArray);\n");
   buffer.AppendFormatted("   }\n");

   // modes
   buffer.AppendFormatted("   // modes\n");
   // AnalyzingMode
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Modes/AnalyzingMode\",fConfigData[index]->fModes->fAnalyzingMode,\"\");\n");
   buffer.AppendFormatted("   fConfigData[index]->fModes->fAnalyzingMode.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fAnalyzingMode==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fAnalyzingModeModified = kFALSE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fAnalyzingModeModified = kTRUE;\n");
   // DAQSystem
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Modes/DAQSystem\",fConfigData[index]->fModes->fDAQSystem,\"\");\n");
   buffer.AppendFormatted("   fConfigData[index]->fModes->fDAQSystem.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fDAQSystem==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fDAQSystemModified = kFALSE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fDAQSystemModified = kTRUE;\n");
   // Check Modified
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fAnalyzingModeModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fModes->fDAQSystemModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fModesModified = kTRUE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModesModified = kFALSE;\n");

   // paths
   buffer.AppendFormatted("   // paths\n");
   // InputFilePath
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Paths/InputFilePath\",fConfigData[index]->fPaths->fInputFilePath,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fPaths->fInputFilePath==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fPaths->fInputFilePathModified = kFALSE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fPaths->fInputFilePathModified = kTRUE;\n");
   // Check Modified
   buffer.AppendFormatted("   if (fConfigData[index]->fPaths->fInputFilePathModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fPathsModified = kTRUE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fPathsModified = kFALSE;\n");

   // Window
   buffer.AppendFormatted("   // window\n");
   buffer.AppendFormatted("   fConfigData[index]->fWindow = new ConfigData::Window();\n");
   // Window/Scale
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Window/Scale\",fConfigData[index]->fWindow->fScale,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fWindow->fScale==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fWindow->fScaleModified = kFALSE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fWindow->fScaleModified = kTRUE;\n");
   // Window/StatusBar
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Window/StatusBar\",fConfigData[index]->fWindow->fStatusBar,\"\");\n");
   buffer.AppendFormatted("   fConfigData[index]->fWindow->fStatusBar.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fWindow->fStatusBar==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fWindow->fStatusBarModified = kFALSE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fWindow->fStatusBarModified = kTRUE;\n");
   // --Window
   buffer.AppendFormatted("   if (fConfigData[index]->fWindow->fScaleModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fWindow->fStatusBarModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fWindowModified = kTRUE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fWindowModified = kFALSE;\n");
   buffer.AppendFormatted("\n");

   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   if (index==0) {\n");
   buffer.AppendFormatted("      Int_t size = xml->NumberOfOccurrenceOfPath(path+\"/DataBases/DataBase\");\n");
   buffer.AppendFormatted("      gMonitor->InitDataBases(size);\n");
   buffer.AppendFormatted("      ROMEString dataBasePath;\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBase = new ConfigData::DataBase*[size];\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBaseModified = new Bool_t[size];\n");
   buffer.AppendFormatted("      for (i=0;i<size;i++) {\n");
   buffer.AppendFormatted("         fConfigData[index]->fDataBase[i] = new ConfigData::DataBase();\n");
   // Name
   buffer.AppendFormatted("         dataBasePath.SetFormatted(\"/DataBases/DataBase[%%d]/Name\",i+1);\n");
   buffer.AppendFormatted("         xml->GetPathValue(path+dataBasePath,fConfigData[index]->fDataBase[i]->fName,\"\");\n");
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBase[i]->fName==\"\")\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fNameModified = kFALSE;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fNameModified = kTRUE;\n");
   // Type
   buffer.AppendFormatted("         dataBasePath.SetFormatted(\"/DataBases/DataBase[%%d]/Type\",i+1);\n");
   buffer.AppendFormatted("         xml->GetPathValue(path+dataBasePath,fConfigData[index]->fDataBase[i]->fType,\"\");\n");
   buffer.AppendFormatted("         fConfigData[index]->fDataBase[i]->fType.ToLower();\n");
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBase[i]->fType==\"\")\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fTypeModified = kFALSE;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fTypeModified = kTRUE;\n");
   // Connection
   buffer.AppendFormatted("         dataBasePath.SetFormatted(\"/DataBases/DataBase[%%d]/Connection\",i+1);\n");
   buffer.AppendFormatted("         xml->GetPathValue(path+dataBasePath,fConfigData[index]->fDataBase[i]->fConnection,\"\");\n");
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBase[i]->fConnection==\"\")\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fConnectionModified = kFALSE;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fConnectionModified = kTRUE;\n");
   // --DataBase
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBase[i]->fNameModified ||\n");
   buffer.AppendFormatted("             fConfigData[index]->fDataBase[i]->fTypeModified ||\n");
   buffer.AppendFormatted("             fConfigData[index]->fDataBase[i]->fConnectionModified) {\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBaseModified[i] = kTRUE;\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBasesModified = kTRUE;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBaseModified[i] = kFALSE;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");

   // Online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   fConfigData[index]->fOnline = new ConfigData::Online();\n");
   // Online/Host
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Online/Host\",fConfigData[index]->fOnline->fHost,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fOnline->fHost==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnline->fHostModified = kFALSE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnline->fHostModified = kTRUE;\n");
   // Online/Experiment
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Online/Experiment\",fConfigData[index]->fOnline->fExperiment,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fOnline->fExperiment==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnline->fExperimentModified = kFALSE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnline->fExperimentModified = kTRUE;\n");
   // --Online
   buffer.AppendFormatted("   if (fConfigData[index]->fOnline->fHostModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnlineModified = kTRUE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnlineModified = kFALSE;\n");
   buffer.AppendFormatted("\n");

   // NetFolder
   if (numOfNetFolder > 0) {
      buffer.AppendFormatted("   // NetFolder\n");
      for (i = 0; i < numOfNetFolder; i++) {
         buffer.AppendFormatted("   fConfigData[index]->fNetFolder[%d] = new ConfigData::NetFolder();\n", i);
         // NetFolder/Active
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/NetFolders/%s/Active\",fConfigData[index]->fNetFolder[%d]->fActive,\"\");\n", netFolderName[i].Data(), i);
         buffer.AppendFormatted("   if (fConfigData[index]->fNetFolder[%d]->fActive==\"\")\n", i);
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fActiveModified = kFALSE;\n", i);
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fActiveModified = kTRUE;\n", i);
         // NetFolder/Reconnect
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/NetFolders/%s/Reconnect\",fConfigData[index]->fNetFolder[%d]->fReconnect,\"\");\n", netFolderName[i].Data(), i);
         buffer.AppendFormatted("   if (fConfigData[index]->fNetFolder[%d]->fReconnect==\"\")\n", i);
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fReconnectModified = kFALSE;\n", i);
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fReconnectModified = kTRUE;\n", i);
         // NetFolder/Host
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/NetFolders/%s/Host\",fConfigData[index]->fNetFolder[%d]->fHost,\"\");\n", netFolderName[i].Data(), i);
         buffer.AppendFormatted("   if (fConfigData[index]->fNetFolder[%d]->fHost==\"\")\n", i);
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fHostModified = kFALSE;\n", i);
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fHostModified = kTRUE;\n", i);
         // NetFolder/Port
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/NetFolders/%s/Port\",fConfigData[index]->fNetFolder[%d]->fPort,\"\");\n", netFolderName[i].Data(), i);
         buffer.AppendFormatted("   if (fConfigData[index]->fNetFolder[%d]->fPort==\"\")\n", i);
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fPortModified = kFALSE;\n", i);
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fPortModified = kTRUE;\n", i);
         // NetFolder/Root
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/NetFolders/%s/Root\",fConfigData[index]->fNetFolder[%d]->fRoot,\"\");\n", netFolderName[i].Data(), i);
         buffer.AppendFormatted("   if (fConfigData[index]->fNetFolder[%d]->fRoot==\"\")\n", i);
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fRootModified = kFALSE;\n", i);
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->fNetFolder[%d]->fRootModified = kTRUE;\n", i);
         // --NetFolder
         buffer.AppendFormatted("   if (fConfigData[index]->fNetFolder[%d]->fActiveModified    ||\n", i);
         buffer.AppendFormatted("       fConfigData[index]->fNetFolder[%d]->fReconnectModified ||\n", i);
         buffer.AppendFormatted("       fConfigData[index]->fNetFolder[%d]->fHostModified      ||\n", i);
         buffer.AppendFormatted("       fConfigData[index]->fNetFolder[%d]->fPortModified      ||\n", i);
         buffer.AppendFormatted("       fConfigData[index]->fNetFolder[%d]->fRootModified)\n", i);
         buffer.AppendFormatted("      fConfigData[index]->fNetFolderModified[%d] = kTRUE;\n", i);
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->fNetFolderModified[%d] = kFALSE;\n", i);
         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("      fConfigData[index]->fNetFoldersModified = kFALSE;\n");
      for (i = 0; i < numOfNetFolder; i++) {
         buffer.AppendFormatted("      if(fConfigData[index]->fNetFolderModified[%d])\n", i);
         buffer.AppendFormatted("         fConfigData[index]->fNetFoldersModified = kTRUE;\n");
      }
   }
   // Folders
   buffer.AppendFormatted("   // folders\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("   // %s Folder\n", folderName[i].Data());
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            buffer.AppendFormatted("   // %s Field\n", valueName[i][j].Data());
            // Name
            buffer.AppendFormatted("   xml->GetPathValue(path+\"/Folders/Folder[FolderName='%s']/Field[FieldName='%s']/DataBaseName\",fConfigData[index]->f%sFolder->f%sField->fName,\"\");\n", folderName[i].Data(), valueName[i][j].Data(), folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("   if (fConfigData[index]->f%sFolder->f%sField->fName==\"\")\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sField->fNameModified = kFALSE;\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sField->fNameModified = kTRUE;\n", folderName[i].Data(), valueName[i][j].Data());
            // Path
            buffer.AppendFormatted("   xml->GetPathValue(path+\"/Folders/Folder[FolderName='%s']/Field[FieldName='%s']/DataBasePath\",fConfigData[index]->f%sFolder->f%sField->fPath,\"\");\n", folderName[i].Data(), valueName[i][j].Data(), folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("   if (fConfigData[index]->f%sFolder->f%sField->fPath==\"\")\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sField->fPathModified = kFALSE;\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sField->fPathModified = kTRUE;\n", folderName[i].Data(), valueName[i][j].Data());
            // Check Modified
            buffer.AppendFormatted("   if (fConfigData[index]->f%sFolder->f%sField->fNameModified ||\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("       fConfigData[index]->f%sFolder->f%sField->fPathModified) {\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("      fConfigData[index]->fFoldersModified = kTRUE;\n");
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sFieldModified = kTRUE;\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("      fConfigData[index]->f%sFolderModified = kTRUE;\n", folderName[i].Data());
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sFieldModified = kFALSE;\n", folderName[i].Data(), valueName[i][j].Data());
         }
      }
   }

   // Tabs
   buffer.AppendFormatted("   // tabs\n");
   buffer.AppendFormatted("   fConfigData[index]->fTabsModified = kFALSE;\n");
   ROMEString pointer;
   ROMEString path;
   ROMEString classname;
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("   // %s Tab\n", tabName[i].Data());
      Int_t index = i;
      pointer.Resize(0);
      path.Resize(0);
      classname.Resize(0);
      while (index != -1) {
         pointer.InsertFormatted(0, "->f%sTab", tabName[index].Data());
         path.InsertFormatted(0, "/Tab[TabName='%s']", tabName[index].Data());
         classname.InsertFormatted(0, "::%sTab", tabName[index].Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted("   fConfigData[index]%s = new ConfigData%s();\n", pointer.Data(), classname.Data());
      // Active
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tabs%s/Active\",fConfigData[index]%s->fActive,\"\");\n", path.Data(), pointer.Data());
      buffer.AppendFormatted("   if (fConfigData[index]%s->fActive==\"\")\n", pointer.Data());
      buffer.AppendFormatted("      fConfigData[index]%s->fActiveModified = kFALSE;\n", pointer.Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]%s->fActiveModified = kTRUE;\n", pointer.Data());
      // Steering parameter
      if (numOfSteering[i] > 0) {
         buffer.AppendFormatted("   // steering parameters\n");
         buffer.AppendFormatted("   fConfigData[index]%s->fSteering = new ConfigData%s::Steering();\n", pointer.Data(), classname.Data());
         ROMEString pathT;
         ROMEString pointerT;
         ROMEString classT;
         ROMEString indexT;
         ROMEString blankT;
         Int_t indexCounter = 0;
         pathT.SetFormatted("path+\"/Tabs%s", path.Data());
         pointerT.SetFormatted("fConfigData[index]%s->fSteering", pointer.Data());
         classT.SetFormatted("ConfigData%s::Steering", classname.Data());
         WriteSteeringConfigRead(buffer, 0, i, xml, pathT, pointerT, classT, indexT, blankT, &indexCounter);
      }
      // all
      buffer.AppendFormatted("   if (fConfigData[index]%s->fActiveModified", pointer.Data());
      if (numOfSteering[i] > 0)
         buffer.AppendFormatted("\n    || fConfigData[index]%s->fSteeringModified", pointer.Data());
      buffer.AppendFormatted(") {\n");
      buffer.AppendFormatted("      fConfigData[index]->fTabsModified = kTRUE;\n");
      buffer.AppendFormatted("      fConfigData[index]%sModified = kTRUE;\n", pointer.Data());
      ROMEString tempPointer = pointer;
      while (kTRUE) {
         for (j = tempPointer.Length() - 1; tempPointer[j] != '>' && j > 0; j--) {
         }
         if (j <= 1)
            break;
         tempPointer = tempPointer(0, j - 1);
         buffer.AppendFormatted("      fConfigData[index]%sModified = kTRUE;\n", tempPointer.Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]%sModified = kFALSE;\n", pointer.Data());
   }

   // trees
   buffer.AppendFormatted("   // trees\n");
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("   // %s Tree\n", treeName[i].Data());
      // Read
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/Tree[TreeName='%s']/Read\",fConfigData[index]->f%sTree->fRead,\"\");\n", treeName[i].Data(), treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fRead==\"\")\n", treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fReadModified = kFALSE;\n", treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fReadModified = kTRUE;\n", treeName[i].Data());
      // FileName
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/Tree[TreeName='%s']/TreeOutputFileName\",fConfigData[index]->f%sTree->fFileName,\"\");\n", treeName[i].Data(), treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fFileName==\"\")\n", treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fFileNameModified = kFALSE;\n", treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fFileNameModified = kTRUE;\n", treeName[i].Data());
      // Check Modified
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fReadModified ||\n", treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fFileNameModified) {\n", treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->fTreesModified = kTRUE;\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTreeModified = kTRUE;\n", treeName[i].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTreeModified = kFALSE;\n", treeName[i].Data());
   }

   // Global Steering Parameters
   if (numOfSteering[numOfTab] > 0) {
      buffer.AppendFormatted("   // global steering parameters\n");
      buffer.AppendFormatted("   fConfigData[index]->fGlobalSteering = new ConfigData::GlobalSteering();\n");
      ROMEString pathT;
      ROMEString pointerT;
      ROMEString classT;
      ROMEString indexT;
      ROMEString blankT;
      Int_t indexCounter = 0;
      pathT.SetFormatted("path+\"/GlobalSteeringParameters");
      pointerT.SetFormatted("fConfigData[index]->fGlobalSteering");
      classT.SetFormatted("ConfigData::GlobalSteering");
      WriteSteeringConfigRead(buffer, 0, numOfTab, xml, pathT, pointerT, classT, indexT, blankT, &indexCounter);
   }
   // midas banks
   for (i = 0; i < numOfEvent; i++) {
      // Active
      buffer.AppendFormatted("   // %s Event\n", eventName[i].Data());
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Midas/Event[EventName='%s']/Active\",fConfigData[index]->f%sEvent->fActive,\"\");\n", eventName[i].Data(), eventName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sEvent->fActive==\"\")\n", eventName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sEvent->fActiveModified = kFALSE;\n", eventName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sEvent->fActiveModified = kTRUE;\n", eventName[i].Data());
      for (j = 0; j < numOfBank[i]; j++) {
         buffer.AppendFormatted("   // %s Bank\n", bankName[i][j].Data());
         // Read
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Midas/Event[EventName='%s']/Bank[BankName='%s']/Active\",fConfigData[index]->f%sEvent->f%sBank->fActive,\"\");\n", eventName[i].Data(), bankName[i][j].Data(), eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]->f%sEvent->f%sBank->fActive==\"\")\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("      fConfigData[index]->f%sEvent->f%sBank->fActiveModified = kFALSE;\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->f%sEvent->f%sBank->fActiveModified = kTRUE;\n", eventName[i].Data(), bankName[i][j].Data());
         // Check Modified
         buffer.AppendFormatted("   if (fConfigData[index]->f%sEvent->f%sBank->fActiveModified)\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("      fConfigData[index]->f%sEvent->f%sBankModified = kTRUE;\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->f%sEvent->f%sBankModified = kFALSE;\n", eventName[i].Data(), bankName[i][j].Data());
      }
      // Check Modified
      buffer.AppendFormatted("   if (\n");
      for (j = 0; j < numOfBank[i]; j++) {
         buffer.AppendFormatted("       fConfigData[index]->f%sEvent->f%sBankModified ||\n", eventName[i].Data(), bankName[i][j].Data());
      }
      buffer.AppendFormatted("       fConfigData[index]->f%sEvent->fActiveModified) {\n", eventName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sEventModified = kTRUE;\n", eventName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->fMidasModified = kTRUE;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sEventModified = kFALSE;\n", eventName[i].Data());
   }
   // end
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");

   // Check Configuration
   buffer.AppendFormatted("\n// Check Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::CheckConfiguration(Int_t runNumber) {\n", shortCut.Data());
   buffer.AppendFormatted("   Int_t j;\n");
   buffer.AppendFormatted("   if (fConfigData[0]->fRunNumbersModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[0]->fLastRunNumberIndex!=-1) {\n");
   buffer.AppendFormatted("         for (j=fConfigData[0]->fLastRunNumberIndex;j<fConfigData[0]->fRunNumberArray.GetSize();j++) {\n");
   buffer.AppendFormatted("            if (fConfigData[0]->fRunNumberArray.At(j)==runNumber) {\n");
   buffer.AppendFormatted("               if (!SetConfiguration(0,0))\n");
   buffer.AppendFormatted("                  return kFALSE;\n");
   buffer.AppendFormatted("               fConfigData[0]->fLastRunNumberIndex = j;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            else if (fConfigData[0]->fRunNumberArray.At(j)<runNumber) {\n");
   buffer.AppendFormatted("               fConfigData[0]->fLastRunNumberIndex = -1;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            else {\n");
   buffer.AppendFormatted("               fConfigData[0]->fLastRunNumberIndex = j;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");

   // Check Configuration
   buffer.AppendFormatted("\n// Check Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::CheckConfiguration(const Char_t* file) {\n", shortCut.Data());
   buffer.AppendFormatted("   ROMEString fileName = file;\n");
   buffer.AppendFormatted("   Int_t j;\n");
   buffer.AppendFormatted("   if (fConfigData[0]!=0) {\n");
   buffer.AppendFormatted("      if (!SetConfiguration(0,0))\n");
   buffer.AppendFormatted("         return kFALSE;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[0]->fInputFileNamesModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[0]->fLastInputFileNameIndex!=-1) {\n");
   buffer.AppendFormatted("         for (j=fConfigData[0]->fLastInputFileNameIndex;j<fConfigData[0]->fInputFileNameArray.GetEntriesFast();j++) {\n");
   buffer.AppendFormatted("            if (fConfigData[0]->fInputFileNameArray.At(j)==fileName) {\n");
   buffer.AppendFormatted("               if (!SetConfiguration(0,0))\n");
   buffer.AppendFormatted("                  return kFALSE;\n");
   buffer.AppendFormatted("               fConfigData[0]->fLastInputFileNameIndex = j;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            else if (fConfigData[0]->fInputFileNameArray.At(j)<fileName) {\n");
   buffer.AppendFormatted("               fConfigData[0]->fLastInputFileNameIndex = -1;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            else {\n");
   buffer.AppendFormatted("               fConfigData[0]->fLastInputFileNameIndex = j;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");

   // Set Configuration
   buffer.AppendFormatted("\n// Set Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::SetConfiguration(Int_t modIndex,Int_t index) {\n", shortCut.Data());
   buffer.AppendFormatted("   Int_t i;\n");
   Bool_t need_cstop = kTRUE;
/*
   Bool_t need_cstop = kFALSE;
   for (i=0;i<numOfTab;i++) {
      for (j=0;j<numOfSteering[i];j++) {
         for (k=0;k<numOfSteerFields[i][j];k++) {
            if(IsNumber(steerFieldType[i][j][k].Data()))
               need_cstop = kTRUE;
         }
      }
   }
*/

   if (need_cstop)
      buffer.AppendFormatted("   Char_t *cstop;\n");

   // Modes
   buffer.AppendFormatted("   // modes\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fModes->fAnalyzingModeModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fModes->fAnalyzingMode==\"online\")\n");
   buffer.AppendFormatted("         gMonitor->SetOnline();\n");
   buffer.AppendFormatted("      else\n");
   buffer.AppendFormatted("         gMonitor->SetOffline();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fModes->fDAQSystemModified) {\n");
   buffer.AppendFormatted("      if (!fConfigData[index]->fModes->fDAQSystem.CompareTo(\"midas\",TString::kIgnoreCase)) {\n");
   buffer.AppendFormatted("         gMonitor->SetMidasDAQ(new %sMidasDAQ());\n", shortCut.Data());
   buffer.AppendFormatted("         gMonitor->SetActiveDAQ(gMonitor->GetMidasDAQ());\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (!fConfigData[index]->fModes->fDAQSystem.CompareTo(\"rome\",TString::kIgnoreCase)) {\n");
   buffer.AppendFormatted("         gMonitor->SetRomeDAQ(new %sRomeDAQ());\n", shortCut.Data());
   buffer.AppendFormatted("         gMonitor->SetActiveDAQ(gMonitor->GetRomeDAQ());\n");
   buffer.AppendFormatted("      }\n");
   if (this->orca) {
      buffer.AppendFormatted("      else if (!fConfigData[index]->fModes->fDAQSystem.CompareTo(\"orca\",TString::kIgnoreCase)) {\n");
      buffer.AppendFormatted("         gMonitor->SetOrcaDAQ(new ROMEOrcaDAQ());\n", shortCut.Data());
      buffer.AppendFormatted("         gMonitor->SetActiveDAQ(gMonitor->GetOrcaDAQ());\n");
      buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("      else if (!fConfigData[index]->fModes->fDAQSystem.CompareTo(\"none\",TString::kIgnoreCase)) {\n");
   buffer.AppendFormatted("         gMonitor->SetActiveDAQ(new ROMENoDAQSystem());\n");
   buffer.AppendFormatted("      }\n");
   for (i = 0; i < numOfDAQ; i++) {
      buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fDAQSystem==\"%s\") {\n", daqName[i].Data());
      buffer.AppendFormatted("         gMonitor->Set%s(new %s%s());\n", daqName[i].Data(), shortCut.Data(), daqName[i].Data());
      buffer.AppendFormatted("         gMonitor->SetActiveDAQ(gMonitor->Get%s());\n", daqName[i].Data());
      buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("   }\n");

   // Paths
   buffer.AppendFormatted("   // paths\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fPaths->fInputFilePathModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fPaths->fInputFilePath[fConfigData[index]->fPaths->fInputFilePath.Length()-1]!='/' && fConfigData[index]->fPaths->fInputFilePath[fConfigData[index]->fPaths->fInputFilePath.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("         fConfigData[index]->fPaths->fInputFilePath.Append(\"/\");\n");
   buffer.AppendFormatted("      gMonitor->SetInputDir(fConfigData[index]->fPaths->fInputFilePath);\n");
   buffer.AppendFormatted("   }\n");

   // Window
   buffer.AppendFormatted("   if (fConfigData[index]->fWindow->fScaleModified) {\n");
   buffer.AppendFormatted("      gMonitor->SetWindowScale(static_cast<Float_t>(atof(fConfigData[index]->fWindow->fScale.Data())));\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fWindow->fStatusBarModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fWindow->fStatusBar==\"false\")\n");
   buffer.AppendFormatted("         gWindow->SetStatusBarSwitch(kFALSE);\n");
   buffer.AppendFormatted("      else\n");
   buffer.AppendFormatted("         gWindow->SetStatusBarSwitch(kTRUE);\n");
   buffer.AppendFormatted("   }\n");

   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   if (index==0) {\n");
   buffer.AppendFormatted("      for (i=0;i<gMonitor->GetNumberOfDataBases();i++) {\n");
   buffer.AppendFormatted("         if (fConfigData[modIndex]->fDataBase[i]->fConnectionModified) {\n");
   buffer.AppendFormatted("            gMonitor->SetDataBaseConnection(i,fConfigData[index]->fDataBase[i]->fConnection.Data());\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         if (fConfigData[modIndex]->fDataBase[i]->fTypeModified) {\n");
   buffer.AppendFormatted("            if (fConfigData[index]->fDataBase[i]->fType==\"sql\") {\n");
   if (this->sql) {
      buffer.AppendFormatted("               gMonitor->SetDataBase(i,new ROMESQLDataBase());\n");
      buffer.AppendFormatted("               if (!gMonitor->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),\"\",gMonitor->GetDataBaseConnection(i)))\n");
      buffer.AppendFormatted("                  return kFALSE;\n");
   }
   else {
      buffer.AppendFormatted("                  cout<<gMonitor->GetProgramName()<<\" is not linked with sql support.\"<<endl;\n");
      buffer.AppendFormatted("                  return kFALSE;\n");
   }
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            else if (fConfigData[index]->fDataBase[i]->fType==\"none\" ||\n");
   buffer.AppendFormatted("                     fConfigData[index]->fDataBase[i]->fType==\"\") {\n");
   buffer.AppendFormatted("               gMonitor->SetDataBase(i,new ROMENoDataBase());\n");
   buffer.AppendFormatted("               if (!gMonitor->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),\"\",\"\"))\n");
   buffer.AppendFormatted("                  return kFALSE;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            else if (fConfigData[index]->fDataBase[i]->fType==\"xml\") {\n");
   buffer.AppendFormatted("               gMonitor->SetDataBase(i,new ROMEXMLDataBase());\n");
   buffer.AppendFormatted("               ROMEString str = gMonitor->GetDataBaseConnection(i);\n");
   buffer.AppendFormatted("               Int_t ind;\n");
   buffer.AppendFormatted("               if ((ind=str.Index(\";\",1,0,TString::kExact))==-1) {\n");
   buffer.AppendFormatted("                  cout<<\"Invalid database connection\"<<endl;\n");
   buffer.AppendFormatted("                  return kFALSE;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               ROMEString path = str(0,ind);\n");
   buffer.AppendFormatted("               if (path[path.Length()-1]!='/' && path[path.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("                  path += \"/\";\n");
   buffer.AppendFormatted("               gMonitor->SetDataBaseDir(i,path.Data());\n");
   buffer.AppendFormatted("               if (!gMonitor->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),gMonitor->GetDataBaseDir(i),static_cast<TString>(str(ind+1,str.Length()-ind-1)).Data()))\n");
   buffer.AppendFormatted("                  return kFALSE;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            else if (fConfigData[index]->fDataBase[i]->fType==\"text\") {\n");
   buffer.AppendFormatted("               gMonitor->SetDataBase(i,new ROMETextDataBase());\n");
   buffer.AppendFormatted("               if (!gMonitor->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),gMonitor->GetDataBaseConnection(i),\"\"))\n");
   buffer.AppendFormatted("                  return kFALSE;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            if (fConfigData[index]->fDataBase[i]->fType==\"odb\") {\n");
/*
   buffer.AppendFormatted("               if (gMonitor->isOffline())\n");
   buffer.AppendFormatted("                  gMonitor->SetDataBase(i,new ROMEODBOfflineDataBase());\n");
   buffer.AppendFormatted("               else\n");
*/
   buffer.AppendFormatted("                  gMonitor->SetDataBase(i,new ROMEODBOnlineDataBase());\n");
   buffer.AppendFormatted("               if (!gMonitor->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),\"\",\"\"))\n");
   buffer.AppendFormatted("                  return kFALSE;\n");
   buffer.AppendFormatted("            }\n");
   for (i = 0; i < numOfDB; i++) {
      buffer.AppendFormatted("            else if (fConfigData[index]->fDataBase[i]->fType==\"%s\") {\n", dbName[i].Data());
      buffer.AppendFormatted("               gMonitor->SetDataBase(i,new %s%sDataBase());\n", shortCut.Data(), dbName[i].Data());
      buffer.AppendFormatted("               if (!gMonitor->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),\"\",gMonitor->GetDataBaseConnection(i)))\n");
      buffer.AppendFormatted("                  return kFALSE;\n");
      buffer.AppendFormatted("            }\n");
   }
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");

   // Online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fOnline->fHostModified)\n");
   buffer.AppendFormatted("      gMonitor->SetOnlineHost(const_cast<Char_t*>(fConfigData[index]->fOnline->fHost.Data()));\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("      gMonitor->SetOnlineExperiment(const_cast<Char_t*>(fConfigData[index]->fOnline->fExperiment.Data()));\n");

   // NetFolder
   buffer.AppendFormatted("   // net folders\n");
   for (i = 0; i < numOfNetFolder; i++) {
      buffer.AppendFormatted("   gMonitor->SetNetFolderName(%d,\"%s\");\n", i, netFolderName[i].Data());
      buffer.AppendFormatted("   gMonitor->SetNetFolderRoot(%d,\"%s\");\n", i, shortCut.Data());
   }
   if (numOfNetFolder > 0) {
      buffer.AppendFormatted("   for(i=0;i<gMonitor->GetNumberOfNetFolders();i++){\n", i);
      buffer.AppendFormatted("      if (fConfigData[index]->fNetFolderModified[i]){\n");
      buffer.AppendFormatted("         if (fConfigData[index]->fNetFolder[i]->fActiveModified){\n");
      buffer.AppendFormatted("            if(fConfigData[index]->fNetFolder[i]->fActive == \"true\")\n");
      buffer.AppendFormatted("               gMonitor->SetNetFolderActive(i,kTRUE);\n");
      buffer.AppendFormatted("            else\n");
      buffer.AppendFormatted("               gMonitor->SetNetFolderActive(i,kFALSE);\n");
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         if (fConfigData[index]->fNetFolder[i]->fReconnectModified){\n");
      buffer.AppendFormatted("            if(fConfigData[index]->fNetFolder[i]->fReconnect == \"true\")\n");
      buffer.AppendFormatted("               gMonitor->SetNetFolderReconnect(i,kTRUE);\n");
      buffer.AppendFormatted("            else\n");
      buffer.AppendFormatted("               gMonitor->SetNetFolderReconnect(i,kFALSE);\n");
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         if (fConfigData[index]->fNetFolder[i]->fHostModified)\n");
      buffer.AppendFormatted("            gMonitor->SetNetFolderHost(i,const_cast<Char_t*>(fConfigData[index]->fNetFolder[i]->fHost.Data()));\n");
      buffer.AppendFormatted("         if (fConfigData[index]->fNetFolder[i]->fPortModified)\n");
      buffer.AppendFormatted("            gMonitor->SetNetFolderPort(i,const_cast<Char_t*>(fConfigData[index]->fNetFolder[i]->fPort.Data()));\n");
      buffer.AppendFormatted("         if (fConfigData[index]->fNetFolder[i]->fRootModified)\n");
      buffer.AppendFormatted("            gMonitor->SetNetFolderRoot(i,const_cast<Char_t*>(fConfigData[index]->fNetFolder[i]->fRoot.Data()));\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   // Folders
   buffer.AppendFormatted("   // folders\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i]) {
         buffer.AppendFormatted("   ROMEString path;\n");
         buffer.AppendFormatted("   ROMEString subPath;\n");
         break;
      }
   }
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("   // %s folder\n", folderName[i].Data());
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            buffer.AppendFormatted("   // %s field\n", valueName[i][j].Data());
            // Name
            buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sFolder->f%sField->fNameModified)\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("      gMonitor->Set%s_%sDBName(fConfigData[modIndex]->f%sFolder->f%sField->fName.Data());\n", folderName[i].Data(), valueName[i][j].Data(), folderName[i].Data(), valueName[i][j].Data());
            // Path
            buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sFolder->f%sField->fPathModified) {\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("      path = fConfigData[modIndex]->f%sFolder->f%sField->fPath.Data();\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("      Int_t ind = path.Last('\"');\n");
            buffer.AppendFormatted("      if (ind==-1 || ind==path.Length()-1) {\n");
            buffer.AppendFormatted("         subPath = path.ReplaceAll(\"\\\"\",\"\");\n");
            buffer.AppendFormatted("         gMonitor->Set%s_%sDBPath(subPath.Data());\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("      else {\n");
            buffer.AppendFormatted("         subPath = path(1,ind-1);\n");
            buffer.AppendFormatted("         gMonitor->Set%s_%sDBPath(subPath.Data());\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("         path = path(ind+2,path.Length()-ind+2);\n");
            buffer.AppendFormatted("         Int_t num = path.CountChar(',')+1;\n");
            buffer.AppendFormatted("         for (i=0;i<num;i++) {\n");
            buffer.AppendFormatted("            if (i<num-1)\n");
            buffer.AppendFormatted("               ind = path.First(',');\n");
            buffer.AppendFormatted("            else\n");
            buffer.AppendFormatted("               ind = path.Length();\n");
            buffer.AppendFormatted("            subPath = path(0,ind);\n");
            buffer.AppendFormatted("            gMonitor->Set%s_%sDBCodeAt(i,gMonitor->GetObjectInterpreterCode(subPath.Data()));\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("            path = path(ind+1,path.Length()-ind+1);\n");
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
         }
      }
   }

   // Tabs
   buffer.AppendFormatted("   // tabs\n");
   for (i = 0; i < numOfTab; i++) {
      buffer.AppendFormatted("   // %s tab\n", tabName[i].Data());
      Int_t index = i;
      pointer.Resize(0);
      while (index != -1) {
         pointer.InsertFormatted(0, "->f%sTab", tabName[index].Data());
         index = tabParentIndex[index];
      }
      ROMEString switchString = tabName[i].Data();
      index = tabParentIndex[i];
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted("   if (fConfigData[index]%s->fActiveModified) {\n", pointer.Data());
      buffer.AppendFormatted("      if (fConfigData[index]%s->fActive==\"false\")\n", pointer.Data());
      buffer.AppendFormatted("         gWindow->GetTabSwitches()->%s = kFALSE;\n", switchString.Data());
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         gWindow->GetTabSwitches()->%s = kTRUE;\n", switchString.Data());
      buffer.AppendFormatted("   }\n");
      // Steering parameter
      if (numOfSteering[i] > 0) {
         buffer.AppendFormatted("   // steering parameters\n");
         ROMEString pointerT;
         ROMEString steerPointerT;
         ROMEString blankT;
         Int_t indexCounter = 0;
         pointerT.SetFormatted("%s->fSteering", pointer.Data());
         steerPointerT.SetFormatted("gWindow->Get%s%03dTab()->GetSP()", tabName[i].Data(), i);
         WriteSteeringConfigSet(buffer, 0, i, pointerT, steerPointerT, blankT, &indexCounter);
      }
   }
   for (i = 0; i < numOfTab; i++) {
      Int_t index = tabParentIndex[i];
      ROMEString switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted("   // %s tab enabled hierarchy\n", tabName[i].Data());
      buffer.AppendFormatted("   gWindow->GetTabSwitches()->%s = gWindow->GetTabSwitches()->%s ", switchString.Data(), switchString.Data());
      index = tabParentIndex[i];
      while (index != -1) {
         Int_t index2 = tabParentIndex[index];
         ROMEString switchString2 = tabName[index].Data();
         while (index2 != -1) {
            switchString2.Insert(0, "_");
            switchString2.Insert(0, tabName[index2].Data());
            index2 = tabParentIndex[index2];
         }
         buffer.AppendFormatted(" * gWindow->GetTabSwitches()->%s", switchString2.Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted(";\n");
   }

   // Trees
   buffer.AppendFormatted("   // trees\n");
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("   // %s tree\n", treeName[i].Data());
      // Read
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sTree->fReadModified) {\n", treeName[i].Data());
      buffer.AppendFormatted("      if (fConfigData[index]->f%sTree->fRead==\"true\")\n", treeName[i].Data());
      buffer.AppendFormatted("         gMonitor->GetTreeObjectAt(%d)->SetRead(kTRUE);\n", i);
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         gMonitor->GetTreeObjectAt(%d)->SetRead(kFALSE);\n", i);
      buffer.AppendFormatted("   }\n");
      // FileName
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sTree->fFileNameModified) {\n", treeName[i].Data());
      buffer.AppendFormatted("      gMonitor->GetTreeObjectAt(%d)->SetConfigFileName(fConfigData[index]->f%sTree->fFileName);\n", i, treeName[i].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else {\n");
      buffer.AppendFormatted("      gMonitor->GetTreeObjectAt(%d)->SetConfigFileName(fConfigData[0]->f%sTree->fFileName);\n", i, treeName[i].Data());
      buffer.AppendFormatted("   }\n");
   }

   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTab] > 0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      ROMEString blankT;
      Int_t indexCounter = 0;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gMonitor->GetGSP()");
      WriteSteeringConfigSet(buffer, 0, numOfTab, pointerT, steerPointerT, blankT, &indexCounter);
   }
   // midas banks
   buffer.AppendFormatted("   // midas banks\n");
   for (i = 0; i < numOfEvent; i++) {
      buffer.AppendFormatted("   // %s event\n", eventName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sEventModified) {\n", eventName[i].Data());
      // Active
      buffer.AppendFormatted("      if (fConfigData[modIndex]->f%sEvent->fActiveModified && !strcmp(gMonitor->GetNameOfActiveDAQ(),\"midas\")) {\n", eventName[i].Data());
      buffer.AppendFormatted("         if (fConfigData[index]->f%sEvent->fActive==\"true\")\n", eventName[i].Data());
      buffer.AppendFormatted("            gMonitor->GetMidasDAQ()->Set%sEventActive(kTRUE);\n", eventName[i].Data());
      buffer.AppendFormatted("         else\n");
      buffer.AppendFormatted("            gMonitor->GetMidasDAQ()->Set%sEventActive(kFALSE);\n", eventName[i].Data());
      buffer.AppendFormatted("      }\n");
      // Banks
      for (j = 0; j < numOfBank[i]; j++) {
         buffer.AppendFormatted("      // %s bank\n", bankName[i][j].Data());
         buffer.AppendFormatted("      if (fConfigData[modIndex]->f%sEvent->f%sBankModified) {\n", eventName[i].Data(), bankName[i][j].Data());
         // Active
         buffer.AppendFormatted("         if (fConfigData[modIndex]->f%sEvent->f%sBank->fActiveModified && !strcmp(gMonitor->GetNameOfActiveDAQ(),\"midas\")) {\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("            if (fConfigData[index]->f%sEvent->f%sBank->fActive==\"true\")\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("               gMonitor->GetMidasDAQ()->Set%sBankActive(kTRUE);\n", bankName[i][j].Data());
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               gMonitor->GetMidasDAQ()->Set%sBankActive(kFALSE);\n", bankName[i][j].Data());
         buffer.AppendFormatted("         }\n");
         // end
         buffer.AppendFormatted("      }\n");
      }
      buffer.AppendFormatted("   }\n");
   }
   // end
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");

   // Write Configuration File
   buffer.AppendFormatted("\n// Write Configuration File\n");
   buffer.AppendFormatted("Bool_t %sConfig::WriteConfigurationFile(const Char_t *file) {\n", shortCut.Data());
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   if (!xml->OpenFileForWrite(fXMLFile))\n");
   buffer.AppendFormatted("      return kFALSE;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   xml->StartElement(\"Configuration\");\n");
   buffer.AppendFormatted("   WriteConfiguration(xml,0);\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("   xml->EndDocument();\n");
   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");

   // Write Configuration
   buffer.AppendFormatted("\n// Write Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::WriteConfiguration(ROMEXML *xml,Int_t index) {\n", shortCut.Data());
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   ROMEString str = \"\";\n");

   // run numbers
   buffer.AppendFormatted("   if (index==0)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"RunNumbers\",gMonitor->GetRunNumberStringOriginal());\n");
   buffer.AppendFormatted("   else if (fConfigData[index]->fRunNumbersModified)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"RunNumbers\",fConfigData[index]->fRunNumbers.Data());\n");

   // input file names
   buffer.AppendFormatted("   if (index==0)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"InputFileNames\",gMonitor->GetInputFileNamesStringOriginal());\n");
   buffer.AppendFormatted("   else if (fConfigData[index]->fInputFileNamesModified)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"InputFileNames\",fConfigData[index]->fInputFileNames.Data());\n");

   // modes
   buffer.AppendFormatted("   // modes\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModesModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Modes\");\n");
   // AnalyzingMode
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gMonitor->isOnline())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"AnalyzingMode\",\"online\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"AnalyzingMode\",\"offline\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fAnalyzingModeModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"AnalyzingMode\",fConfigData[index]->fModes->fAnalyzingMode.Data());\n");

   // DAQSystem
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gMonitor->isActiveDAQSet())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"DAQSystem\",gMonitor->GetActiveDAQ()->GetName());\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"DAQSystem\",\"none\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fDAQSystemModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"DAQSystem\",fConfigData[index]->fModes->fDAQSystem.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");

   // Paths
   buffer.AppendFormatted("   // paths\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fPathsModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Paths\");\n");
   // InputFilePath
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"InputFilePath\",gMonitor->GetInputDir());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fPaths->fInputFilePathModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"InputFilePath\",fConfigData[index]->fPaths->fInputFilePath.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");

   // Window
   buffer.AppendFormatted("   // window\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fWindowModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Window\");\n");
   // Window/Scale
   buffer.AppendFormatted("      if (index==0){\n");
   buffer.AppendFormatted("         str.SetFormatted(\"%%2.1f\",gMonitor->GetWindowScale());\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Scale\",const_cast<Char_t*>(str.Data()));\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fWindow->fScaleModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Scale\",const_cast<Char_t*>(fConfigData[index]->fWindow->fScale.Data()));\n");
   // Window/StatusBar
   buffer.AppendFormatted("      if (index==0){\n");
   buffer.AppendFormatted("         if(gWindow->GetStatusBarSwitch())\n");
   buffer.AppendFormatted("            str.SetFormatted(\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            str.SetFormatted(\"false\");\n");
   buffer.AppendFormatted("         xml->WriteElement(\"StatusBar\",const_cast<Char_t*>(str.Data()));\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fWindow->fStatusBarModified){\n");
   buffer.AppendFormatted("         if(gWindow->GetStatusBarSwitch())\n");
   buffer.AppendFormatted("            str.SetFormatted(\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            str.SetFormatted(\"false\");\n");
   buffer.AppendFormatted("         xml->WriteElement(\"StatusBar\",const_cast<Char_t*>(str.Data()));\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");

   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fDataBasesModified && index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"DataBases\");\n");
   buffer.AppendFormatted("      for (i=0;i<gMonitor->GetNumberOfDataBases();i++) {\n");
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBaseModified[i]) {\n");
   buffer.AppendFormatted("            xml->StartElement(\"DataBase\");\n");
   // Name
   buffer.AppendFormatted("            xml->WriteElement(\"Name\",gMonitor->GetDataBase(i)->GetName());\n");
   // Type
   buffer.AppendFormatted("            xml->WriteElement(\"Type\",gMonitor->GetDataBase(i)->GetType());\n");
   // Connection
   buffer.AppendFormatted("            xml->WriteElement(\"Connection\",gMonitor->GetDataBaseConnection(i));\n");
   buffer.AppendFormatted("            xml->EndElement();\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");

   // Online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fOnlineModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Online\");\n");
   // Online/Host
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Host\",gMonitor->GetOnlineHost());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fOnline->fHostModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Host\",const_cast<Char_t*>(fConfigData[index]->fOnline->fHost.Data()));\n");
   // Online/Experiment
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Experiment\",gMonitor->GetOnlineExperiment());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Experiment\",const_cast<Char_t*>(fConfigData[index]->fOnline->fExperiment.Data()));\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");

   // NetFolder
   if (numOfNetFolder > 0) {
      buffer.AppendFormatted("   // NetFolder\n");
      buffer.AppendFormatted("   if (fConfigData[index]->fNetFoldersModified || index==0){\n");
      buffer.AppendFormatted("      xml->StartElement(\"NetFolders\");\n");
      for (i = 0; i < numOfNetFolder; i++) {
         buffer.AppendFormatted("      if (fConfigData[index]->fNetFolderModified[%d] || index==0){\n", i);
         buffer.AppendFormatted("         xml->StartElement(\"%s\");\n", netFolderName[i].Data());
         // NetFolder/Active
         buffer.AppendFormatted("         if (index==0){\n");
         buffer.AppendFormatted("            if(gMonitor->GetNetFolderActive(%d))\n", i);
         buffer.AppendFormatted("               xml->WriteElement(\"Active\",\"true\");\n");
         buffer.AppendFormatted("            else\n", i);
         buffer.AppendFormatted("               xml->WriteElement(\"Active\",\"false\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->fNetFolder[%d]->fActiveModified)\n", i);
         buffer.AppendFormatted("            xml->WriteElement(\"Active\",const_cast<Char_t*>(fConfigData[index]->fNetFolder[%d]->fActive.Data()));\n", i);
         // NetFolder/Reconnect
         buffer.AppendFormatted("         if (index==0){\n");
         buffer.AppendFormatted("            if(gMonitor->GetNetFolderReconnect(%d))\n", i);
         buffer.AppendFormatted("               xml->WriteElement(\"Reconnect\",\"true\");\n");
         buffer.AppendFormatted("            else\n", i);
         buffer.AppendFormatted("               xml->WriteElement(\"Reconnect\",\"false\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->fNetFolder[%d]->fReconnectModified)\n", i);
         buffer.AppendFormatted("            xml->WriteElement(\"Reconnect\",const_cast<Char_t*>(fConfigData[index]->fNetFolder[%d]->fReconnect.Data()));\n", i);
         // NetFolder/Host
         buffer.AppendFormatted("         if (index==0)\n");
         buffer.AppendFormatted("            xml->WriteElement(\"Host\",gMonitor->GetNetFolderHost(%d));\n", i);
         buffer.AppendFormatted("         else if (fConfigData[index]->fNetFolder[%d]->fHostModified)\n", i);
         buffer.AppendFormatted("            xml->WriteElement(\"Host\",const_cast<Char_t*>(fConfigData[index]->fNetFolder[%d]->fHost.Data()));\n", i);
         // NetFolder/Port
         buffer.AppendFormatted("         if (index==0){\n");
         buffer.AppendFormatted("            str.SetFormatted(\"%%d\",gMonitor->GetNetFolderPort(%d));\n", i);
         buffer.AppendFormatted("            xml->WriteElement(\"Port\",const_cast<Char_t*>(str.Data()));\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->fNetFolder[%d]->fPortModified)\n", i);
         buffer.AppendFormatted("            xml->WriteElement(\"Port\",const_cast<Char_t*>(fConfigData[index]->fNetFolder[%d]->fPort.Data()));\n", i);
         // NetFolder/Root
         buffer.AppendFormatted("         if (index==0)\n");
         buffer.AppendFormatted("            xml->WriteElement(\"Root\",gMonitor->GetNetFolderRoot(%d));\n", i);
         buffer.AppendFormatted("         else if (fConfigData[index]->fNetFolder[%d]->fRootModified)\n", i);
         buffer.AppendFormatted("            xml->WriteElement(\"Root\",const_cast<Char_t*>(fConfigData[index]->fNetFolder[%d]->fRoot.Data()));\n", i);
         buffer.AppendFormatted("         xml->EndElement();\n");
         buffer.AppendFormatted("      }\n");
      }
      buffer.AppendFormatted("      xml->EndElement();\n");
      buffer.AppendFormatted("   }\n");
   }
   // Folders
   buffer.AppendFormatted("   // folders\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fFoldersModified) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Folders\");\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("      if (fConfigData[index]->f%sFolderModified) {\n", folderName[i].Data());
         buffer.AppendFormatted("         // %s folder\n", folderName[i].Data());
         buffer.AppendFormatted("         xml->StartElement(\"Folder\");\n");
         buffer.AppendFormatted("         xml->WriteElement(\"FolderName\",\"%s\");\n", folderName[i].Data());
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            buffer.AppendFormatted("         if (fConfigData[index]->f%sFolder->f%sFieldModified) {\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("            // %s field\n", valueName[i][j].Data());
            buffer.AppendFormatted("            xml->StartElement(\"Field\");\n");
            buffer.AppendFormatted("            xml->WriteElement(\"FieldName\",\"%s\");\n", valueName[i][j].Data());
            // name
            buffer.AppendFormatted("            if (fConfigData[index]->f%sFolder->f%sField->fNameModified)\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("               xml->WriteElement(\"DataBaseName\",fConfigData[index]->f%sFolder->f%sField->fName.Data());\n", folderName[i].Data(), valueName[i][j].Data());
            // path
            buffer.AppendFormatted("            if (fConfigData[index]->f%sFolder->f%sField->fNameModified)\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("               xml->WriteElement(\"DataBasePath\",fConfigData[index]->f%sFolder->f%sField->fPath.Data());\n", folderName[i].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("            xml->EndElement();\n");
            buffer.AppendFormatted("         }\n");
         }
         buffer.AppendFormatted("         xml->EndElement();\n");
         buffer.AppendFormatted("      }\n");
      }
   }
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");

   // Tabs
   buffer.AppendFormatted("   // tabs\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fTabsModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Tabs\");\n");
   pointer.Resize(0);
   WriteTabConfigWrite(buffer, -1, pointer, 0);
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");

   // Trees
   buffer.AppendFormatted("   // trees\n");
   if (numOfTree > 0) {
      buffer.AppendFormatted("   if (fConfigData[index]->fTreesModified || index==0) {\n");
      buffer.AppendFormatted("      xml->StartElement(\"Trees\");\n");
      buffer.AppendFormatted("      if (index==0) {\n");
      buffer.AppendFormatted("         if (gMonitor->isTreeAccumulation())\n");
      buffer.AppendFormatted("            xml->WriteElement(\"Accumulate\",\"true\");\n");
      buffer.AppendFormatted("         else\n");
      buffer.AppendFormatted("            xml->WriteElement(\"Accumulate\",\"false\");\n");
      buffer.AppendFormatted("      }\n");
      for (i = 0; i < numOfTree; i++) {
         buffer.AppendFormatted("      if (fConfigData[index]->f%sTreeModified || index==0) {\n", treeName[i].Data());
         buffer.AppendFormatted("         // %s tree\n", treeName[i].Data());
         buffer.AppendFormatted("         xml->StartElement(\"Tree\");\n");
         buffer.AppendFormatted("         xml->WriteElement(\"TreeName\",\"%s\");\n", treeName[i].Data());
         // read
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            if (gMonitor->GetTreeObjectAt(%d)->isRead())\n", i);
         buffer.AppendFormatted("               xml->WriteElement(\"Read\",\"true\");\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               xml->WriteElement(\"Read\",\"false\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fReadModified)\n", treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"Read\",fConfigData[index]->f%sTree->fRead.Data());\n", treeName[i].Data());
         // FileName
         buffer.AppendFormatted("         if (index==0)\n");
         buffer.AppendFormatted("            xml->WriteElement(\"TreeOutputFileName\",gMonitor->GetTreeObjectAt(%d)->GetConfigFileName().Data());\n", i);
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fFileNameModified)\n", treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"TreeOutputFileName\",fConfigData[index]->f%sTree->fFileName.Data());\n", treeName[i].Data());
         buffer.AppendFormatted("         xml->EndElement();\n");
         buffer.AppendFormatted("      }\n");
      }
      buffer.AppendFormatted("      xml->EndElement();\n");
      buffer.AppendFormatted("   }\n");
   }
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTab] > 0) {
      buffer.AppendFormatted("   if (fConfigData[index]->fGlobalSteeringModified || index==0) {\n");
      buffer.AppendFormatted("      ROMEString value;\n");
      buffer.AppendFormatted("      xml->StartElement(\"GlobalSteeringParameters\");\n");
      ROMEString pointerT;
      ROMEString steerPointerT;
      Int_t indexCounter = 0;
      pointerT.SetFormatted("fConfigData[index]->fGlobalSteering");
      steerPointerT.SetFormatted("gMonitor->GetGSP()");
      WriteSteeringConfigWrite(buffer, 0, numOfTab, pointerT, steerPointerT, 1, &indexCounter);
   }
   if (numOfSteering[numOfTab] > 0) {
      buffer.AppendFormatted("      xml->EndElement();\n");
      buffer.AppendFormatted("   }\n");
   }
   // Midas banks
   buffer.AppendFormatted("   // midas banks\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fMidasModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Midas\");\n");
   for (i = 0; i < numOfEvent; i++) {
      buffer.AppendFormatted("      if (fConfigData[index]->f%sEventModified || index==0) {\n", eventName[i].Data());
      buffer.AppendFormatted("         xml->StartElement(\"Event\");\n");
      buffer.AppendFormatted("         xml->WriteElement(\"EventName\",\"%s\");\n", eventName[i].Data());
      // active
      buffer.AppendFormatted("         if (index==0) {\n");
      buffer.AppendFormatted("            if (!strcmp(gMonitor->GetNameOfActiveDAQ(),\"midas\")) {\n");
      buffer.AppendFormatted("               if (gMonitor->GetMidasDAQ()->is%sEventActive())\n", eventName[i].Data());
      buffer.AppendFormatted("                  xml->WriteElement(\"Active\",\"true\");\n");
      buffer.AppendFormatted("               else\n");
      buffer.AppendFormatted("                  xml->WriteElement(\"Active\",\"false\");\n");
      buffer.AppendFormatted("            }\n");
      buffer.AppendFormatted("            else {\n");
      buffer.AppendFormatted("               if (fConfigData[index]->f%sEvent->fActiveModified)\n", eventName[i].Data());
      buffer.AppendFormatted("                  xml->WriteElement(\"Active\",fConfigData[index]->f%sEvent->fActive.Data());\n", eventName[i].Data());
      buffer.AppendFormatted("               else\n");
      buffer.AppendFormatted("                  xml->WriteElement(\"Active\",\"false\");\n");
      buffer.AppendFormatted("            }\n");
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         else if (fConfigData[index]->f%sEvent->fActiveModified)\n", eventName[i].Data());
      buffer.AppendFormatted("            xml->WriteElement(\"Active\",fConfigData[index]->f%sEvent->fActive.Data());\n", eventName[i].Data());
      for (j = 0; j < numOfBank[i]; j++) {
         buffer.AppendFormatted("         if (fConfigData[index]->f%sEvent->f%sBankModified || index==0) {\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("            // %s bank\n", bankName[i][j].Data());
         buffer.AppendFormatted("            xml->StartElement(\"Bank\");\n");
         buffer.AppendFormatted("            xml->WriteElement(\"BankName\",\"%s\");\n", bankName[i][j].Data());
         // active
         buffer.AppendFormatted("            if (index==0) {\n");
         buffer.AppendFormatted("               if (!strcmp(gMonitor->GetNameOfActiveDAQ(),\"midas\")) {\n");
         buffer.AppendFormatted("                  if (gMonitor->GetMidasDAQ()->is%sBankActive())\n", bankName[i][j].Data());
         buffer.AppendFormatted("                     xml->WriteElement(\"Active\",\"true\");\n");
         buffer.AppendFormatted("                  else\n");
         buffer.AppendFormatted("                     xml->WriteElement(\"Active\",\"false\");\n");
         buffer.AppendFormatted("               }\n");
         buffer.AppendFormatted("               else {\n");
         buffer.AppendFormatted("                  if (fConfigData[index]->f%sEvent->f%sBank->fActiveModified)\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("                     xml->WriteElement(\"Active\",fConfigData[index]->f%sEvent->f%sBank->fActive.Data());\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("                  else\n");
         buffer.AppendFormatted("                     xml->WriteElement(\"Active\",\"false\");\n");
         buffer.AppendFormatted("               }\n");
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("            else if (fConfigData[index]->f%sEvent->f%sBank->fActiveModified)\n", eventName[i].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("               xml->WriteElement(\"Active\",fConfigData[index]->f%sEvent->f%sBank->fActive.Data());\n", eventName[i].Data(), bankName[i][j].Data());
         // end
         buffer.AppendFormatted("            xml->EndElement();\n");
         buffer.AppendFormatted("         }\n");
      }
      buffer.AppendFormatted("         xml->EndElement();\n");
      buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");

   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   // Close cpp-File
   WriteFile(cppFile.Data(), buffer.Data(), 6);
   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::WriteConfigH()
{
   Int_t i, j;
   ROMEString hFile;
   ROMEString buffer;
   // File name
   hFile.SetFormatted("%s/include/framework/%sConfig.h", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//          *** This file will be overwritten by the ArgusBuilder ***         //\n");
   buffer.AppendFormatted("//          ***      Don't make manual changes to this file       ***         //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sConfig_H\n", shortCut.Data());
   buffer.AppendFormatted("#define %sConfig_H\n\n", shortCut.Data());
   buffer.AppendFormatted("#include <ROMEString.h>\n");
   buffer.AppendFormatted("#ifndef __CINT__\n");
   buffer.AppendFormatted("#include <ROMEXML.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <ROMEConfig.h>\n");
   if (numOfSteering[numOfTab] > 0)
      buffer.AppendFormatted("#include \"include/framework/%sGlobalSteering.h\"\n", shortCut.Data());

   // Class
   buffer.AppendFormatted("\nclass %sConfig : public ROMEConfig\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("#ifndef __CINT__\n");

   // sub classes
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   class ConfigData\n");
   buffer.AppendFormatted("   {\n");
   buffer.AppendFormatted("   public:\n");
   buffer.AppendFormatted("      ROMEString    fRunNumbers;\n");
   buffer.AppendFormatted("      Bool_t        fRunNumbersModified;\n");
   buffer.AppendFormatted("      Int_t         fLastRunNumberIndex;\n");
   buffer.AppendFormatted("      TArrayI       fRunNumberArray;\n");
   buffer.AppendFormatted("      ROMEString    fInputFileNames;\n");
   buffer.AppendFormatted("      Bool_t        fInputFileNamesModified;\n");
   buffer.AppendFormatted("      Int_t         fLastInputFileNameIndex;\n");
   buffer.AppendFormatted("      ROMEStrArray  fInputFileNameArray;\n");
   buffer.AppendFormatted("\n");

   // modes
   buffer.AppendFormatted("      // modes;\n");
   buffer.AppendFormatted("      class Modes {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         Bool_t      fRunNumbersModified;\n");
   buffer.AppendFormatted("         ROMEString  fAnalyzingMode;\n");
   buffer.AppendFormatted("         Bool_t      fAnalyzingModeModified;\n");
   buffer.AppendFormatted("         ROMEString  fDAQSystem;\n");
   buffer.AppendFormatted("         Bool_t      fDAQSystemModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      Modes *fModes;\n");
   buffer.AppendFormatted("      Bool_t   fModesModified;\n");

   // window
   buffer.AppendFormatted("      // window;\n");
   buffer.AppendFormatted("      class Window {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString       fScale;\n");
   buffer.AppendFormatted("         Bool_t           fScaleModified;\n");
   buffer.AppendFormatted("         ROMEString       fStatusBar;\n");
   buffer.AppendFormatted("         Bool_t           fStatusBarModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      Window*          fWindow;\n");
   buffer.AppendFormatted("      Bool_t           fWindowModified;\n");

   // database
   buffer.AppendFormatted("      // database;\n");
   buffer.AppendFormatted("      class DataBase {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString    fName;\n");
   buffer.AppendFormatted("         Bool_t        fNameModified;\n");
   buffer.AppendFormatted("         ROMEString    fType;\n");
   buffer.AppendFormatted("         Bool_t        fTypeModified;\n");
   buffer.AppendFormatted("         ROMEString    fConnection;\n");
   buffer.AppendFormatted("         Bool_t        fConnectionModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      DataBase       **fDataBase;\n");
   buffer.AppendFormatted("      Bool_t          *fDataBaseModified;\n");
   buffer.AppendFormatted("      Bool_t           fDataBasesModified;\n");

   // online
   buffer.AppendFormatted("      // online;\n");
   buffer.AppendFormatted("      class Online {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString       fHost;\n");
   buffer.AppendFormatted("         Bool_t           fHostModified;\n");
   buffer.AppendFormatted("         ROMEString       fExperiment;\n");
   buffer.AppendFormatted("         Bool_t           fExperimentModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      Online*          fOnline;\n");
   buffer.AppendFormatted("      Bool_t           fOnlineModified;\n");
   buffer.AppendFormatted("\n");

   // NetFolder
   buffer.AppendFormatted("      // NetFolder;\n");
   buffer.AppendFormatted("      class NetFolder {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString       fActive;\n");
   buffer.AppendFormatted("         Bool_t           fActiveModified;\n");
   buffer.AppendFormatted("         ROMEString       fReconnect;\n");
   buffer.AppendFormatted("         Bool_t           fReconnectModified;\n");
   buffer.AppendFormatted("         ROMEString       fRoot;\n");
   buffer.AppendFormatted("         Bool_t           fRootModified;\n");
   buffer.AppendFormatted("         ROMEString       fHost;\n");
   buffer.AppendFormatted("         Bool_t           fHostModified;\n");
   buffer.AppendFormatted("         ROMEString       fPort;\n");
   buffer.AppendFormatted("         Bool_t           fPortModified;\n");
   buffer.AppendFormatted("      };\n");
   if (numOfNetFolder > 0) {
      buffer.AppendFormatted("      NetFolder*       fNetFolder[%d];\n", numOfNetFolder);
      buffer.AppendFormatted("      Bool_t           fNetFolderModified[%d];\n", numOfNetFolder);
      buffer.AppendFormatted("      Bool_t           fNetFoldersModified;\n");
   }
   buffer.AppendFormatted(" \n");

   // tabs
   buffer.AppendFormatted("      // tabs\n");
   WriteTabConfigClass(buffer, -1, 0);
   buffer.AppendFormatted("      Bool_t           fTabsModified;\n");
   buffer.AppendFormatted("\n");

   // folders
   buffer.AppendFormatted("      // folders\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("      class %sFolder {\n", folderName[i].Data());
         buffer.AppendFormatted("      public:\n");
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            buffer.AppendFormatted("         class %sField {\n", valueName[i][j].Data());
            buffer.AppendFormatted("         public:\n");
            buffer.AppendFormatted("            ROMEString  fName;\n");
            buffer.AppendFormatted("            Bool_t      fNameModified;\n");
            buffer.AppendFormatted("            ROMEString  fPath;\n");
            buffer.AppendFormatted("            Bool_t      fPathModified;\n");
            buffer.AppendFormatted("         };\n");
            buffer.AppendFormatted("         %sField *f%sField;\n", valueName[i][j].Data(), valueName[i][j].Data());
            buffer.AppendFormatted("         Bool_t   f%sFieldModified;\n", valueName[i][j].Data());
         }
         buffer.AppendFormatted("         %sFolder() {\n", folderName[i].Data());
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            buffer.AppendFormatted("            f%sFieldModified = kFALSE;\n", valueName[i][j].Data());
            buffer.AppendFormatted("            f%sField = new %sField();\n", valueName[i][j].Data(), valueName[i][j].Data());
         }
         buffer.AppendFormatted("         };\n");
         buffer.AppendFormatted("      };\n");
         buffer.AppendFormatted("      %sFolder *f%sFolder;\n", folderName[i].Data(), folderName[i].Data());
         buffer.AppendFormatted("      Bool_t   f%sFolderModified;\n", folderName[i].Data());
      }
   }
   buffer.AppendFormatted("      Bool_t   fFoldersModified;\n");

   // paths
   buffer.AppendFormatted("      // paths;\n");
   buffer.AppendFormatted("      class Paths {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fInputFilePath;\n");
   buffer.AppendFormatted("         Bool_t      fInputFilePathModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      Paths *fPaths;\n");
   buffer.AppendFormatted("      Bool_t fPathsModified;\n");

   // trees
   buffer.AppendFormatted("      // trees\n");
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("      class %sTree {\n", treeName[i].Data());
      buffer.AppendFormatted("      public:\n");
      buffer.AppendFormatted("         ROMEString  fRead;\n");
      buffer.AppendFormatted("         Bool_t      fReadModified;\n");
      buffer.AppendFormatted("         ROMEString  fFileName;\n");
      buffer.AppendFormatted("         Bool_t      fFileNameModified;\n");
      buffer.AppendFormatted("      };\n");
      buffer.AppendFormatted("      %sTree *f%sTree;\n", treeName[i].Data(), treeName[i].Data());
      buffer.AppendFormatted("      Bool_t  f%sTreeModified;\n", treeName[i].Data());
   }
   buffer.AppendFormatted("      Bool_t   fTreesModified;\n");

   // steering parameters
   buffer.AppendFormatted("      // global steering parameters\n");
   buffer.AppendFormatted("      class GlobalSteering {\n");
   buffer.AppendFormatted("      public:\n");
   if (numOfSteering[numOfTab] > 0) {
      WriteSteeringConfigClass(buffer, 0, numOfTab, 2);
   }
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      GlobalSteering*  fGlobalSteering;\n");
   buffer.AppendFormatted("      Bool_t           fGlobalSteeringModified;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   public:\n");

   // midas banks
   buffer.AppendFormatted("      // midas banks\n");
   for (i = 0; i < numOfEvent; i++) {
      buffer.AppendFormatted("      // %s event\n", eventName[i].Data());
      buffer.AppendFormatted("      class %sEvent {\n", eventName[i].Data());
      buffer.AppendFormatted("      public:\n");
      buffer.AppendFormatted("         ROMEString  fActive;\n");
      buffer.AppendFormatted("         Bool_t      fActiveModified;\n");
      for (j = 0; j < numOfBank[i]; j++) {
         buffer.AppendFormatted("         // %s bank\n", bankName[i][j].Data());
         buffer.AppendFormatted("         class %sBank {\n", bankName[i][j].Data());
         buffer.AppendFormatted("         public:\n");
         buffer.AppendFormatted("            ROMEString  fActive;\n");
         buffer.AppendFormatted("            Bool_t      fActiveModified;\n");
         buffer.AppendFormatted("            %sBank() {\n", bankName[i][j].Data());
         buffer.AppendFormatted("               fActiveModified = kFALSE;\n");
         buffer.AppendFormatted("            };\n");
         buffer.AppendFormatted("         };\n");
         buffer.AppendFormatted("         %sBank *f%sBank;\n", bankName[i][j].Data(), bankName[i][j].Data());
         buffer.AppendFormatted("         Bool_t  f%sBankModified;\n", bankName[i][j].Data());
      }
      buffer.AppendFormatted("         %sEvent() {\n", eventName[i].Data());
      buffer.AppendFormatted("            fActiveModified = kFALSE;\n");
      for (j = 0; j < numOfBank[i]; j++) {
         buffer.AppendFormatted("            f%sBankModified = kFALSE;\n", bankName[i][j].Data());
         buffer.AppendFormatted("            f%sBank = new %sBank();\n", bankName[i][j].Data(), bankName[i][j].Data());
      }
      buffer.AppendFormatted("         };\n");
      buffer.AppendFormatted("      };\n");
      buffer.AppendFormatted("      %sEvent *f%sEvent;\n", eventName[i].Data(), eventName[i].Data());
      buffer.AppendFormatted("      Bool_t   f%sEventModified;\n", eventName[i].Data());
   }
   buffer.AppendFormatted("      Bool_t   fMidasModified;\n");

   // Constructor
   buffer.AppendFormatted("      ConfigData() {\n");
   buffer.AppendFormatted("         fRunNumbersModified = kFALSE;\n");
   buffer.AppendFormatted("         fInputFileNamesModified = kFALSE;\n");
   buffer.AppendFormatted("         fModesModified = kFALSE;\n");
   buffer.AppendFormatted("         fModes = new Modes();\n");
   buffer.AppendFormatted("         fDataBasesModified        = kFALSE;\n");
   buffer.AppendFormatted("         fOnlineModified           = kFALSE;\n");
   buffer.AppendFormatted("         fPathsModified = kFALSE;\n");
   buffer.AppendFormatted("         fPaths = new Paths();\n");
   buffer.AppendFormatted("         fTabsModified             = kFALSE;\n");
   if (numOfNetFolder > 0)
      buffer.AppendFormatted("         fNetFoldersModified       = kFALSE;\n");
   for (i = 0; i < numOfTab; i++) {
      if (tabParentIndex[i] != -1)
         continue;
      buffer.AppendFormatted("         f%sTabModified = kFALSE;\n", tabName[i].Data());
      buffer.AppendFormatted("         f%sTab         = new %sTab();\n", tabName[i].Data(), tabName[i].Data());
   }
   for (i = 0; i < numOfFolder; i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("         f%sFolderModified = kFALSE;\n", folderName[i].Data());
         buffer.AppendFormatted("         f%sFolder = new %sFolder();\n", folderName[i].Data(), folderName[i].Data());
      }
   }
   buffer.AppendFormatted("         fTreesModified = kFALSE;\n");
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("         f%sTreeModified = kFALSE;\n", treeName[i].Data());
      buffer.AppendFormatted("         f%sTree = new %sTree();\n", treeName[i].Data(), treeName[i].Data());
   }
   if (numOfSteering[numOfTab] > 0) {
      buffer.AppendFormatted("         fGlobalSteeringModified  = kFALSE;\n");
      buffer.AppendFormatted("         fGlobalSteering          = new GlobalSteering();\n");
   }
   for (i = 0; i < numOfEvent; i++) {
      buffer.AppendFormatted("         f%sEventModified = kFALSE;\n", eventName[i].Data());
      buffer.AppendFormatted("         f%sEvent = new %sEvent();\n", eventName[i].Data(), eventName[i].Data());
   }
   buffer.AppendFormatted("         fMidasModified = kFALSE;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("\n");

   // Fields
   buffer.AppendFormatted("   ConfigData**        fConfigData;\n");
   buffer.AppendFormatted("   ROMEString          fXMLFile;\n");
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sConfig();\n", shortCut.Data());

   // methods
   buffer.AppendFormatted("   Bool_t WriteConfigurationFile(const Char_t *file);\n");
   buffer.AppendFormatted("   Bool_t ReadConfigurationFile(const Char_t *file);\n");
   buffer.AppendFormatted("   Bool_t CheckConfiguration(Int_t runNumber);\n");
   buffer.AppendFormatted("   Bool_t CheckConfiguration(const Char_t *file);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   Bool_t ReadConfiguration(ROMEXML *xml,ROMEString& path,Int_t index);\n");
   buffer.AppendFormatted("   Bool_t WriteConfiguration(ROMEXML *xml,Int_t index);\n");
   buffer.AppendFormatted("   Bool_t SetConfiguration(Int_t modIndex = 0,Int_t index = 0);\n");
   buffer.AppendFormatted("\n");

   // Footer
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("};\n\n");
   buffer.AppendFormatted("#endif   // %sConfig_H\n", shortCut.Data());

   // Write File
   WriteFile(hFile.Data(), buffer.Data(), 6);
   return kTRUE;
}


//______________________________________________________________________________
Bool_t ArgusBuilder::ReadXMLNetFolder()
{
   // read the net folder definitions out of the xml file
   Int_t type;
   Char_t *name;
   ROMEString currentNetFolderName = "";

   // count netFolders
   numOfNetFolder++;
   if (numOfNetFolder >= maxNumberOfNetFolders) {
      cout << "Maximal number of net folders reached : " << maxNumberOfNetFolders << " !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   // initialization
   netFolderName[numOfNetFolder] = "";
   netFolderTitle[numOfNetFolder] = "";
   netFolderHost[numOfNetFolder] = "localhost";
   netFolderPort[numOfNetFolder] = "9090";
   netFolderRoot[numOfNetFolder] = shortCut;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // end net object
      if (type == 15 && !strcmp(name, "NetFolder")) {
         // check input
         if (netFolderName[numOfNetFolder] == "") {
            cout << "The " << (numOfNetFolder + 1) << ". NetFolder has no name !" << endl;
            cout << "Terminating program." << endl;
            return kFALSE;
         }
         return kTRUE;
      }
      // net object name
      if (type == 1 && !strcmp(name, "NetFolderName")) {
         xml->GetValue(netFolderName[numOfNetFolder], netFolderName[numOfNetFolder]);
         // output
         if (makeOutput)
            netFolderName[numOfNetFolder].WriteLine();
      }
      // net object title
      if (type == 1 && !strcmp(name, "NetFolderTitle"))
         xml->GetValue(netFolderTitle[numOfNetFolder], netFolderTitle[numOfNetFolder]);

      // net object host
      if (type == 1 && !strcmp(name, "NetFolderHost"))
         xml->GetValue(netFolderHost[numOfNetFolder], netFolderHost[numOfNetFolder]);

      // net object port
      if (type == 1 && !strcmp(name, "NetFolderPort"))
         xml->GetValue(netFolderPort[numOfNetFolder], netFolderPort[numOfNetFolder]);

      // net object Root
      if (type == 1 && !strcmp(name, "NetFolderRoot"))
         xml->GetValue(netFolderRoot[numOfNetFolder], netFolderRoot[numOfNetFolder]);
   }
   return kTRUE;
}
