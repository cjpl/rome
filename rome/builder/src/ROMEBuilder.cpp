/********************************************************************

  ROMEBuilder.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include <Riostream.h>
#include "ROMEBuilder.h"

ROMEXML* configXSD;

ROMEBuilder::~ROMEBuilder()
{
   // Free memory

   // FIX ME !
   // multiple dimensional arrays are not freed yet.
   // I counld implement similar function as AllocateInt....
   // But specifing array size explictly might be dangerous
   // when there is an inconsistency between actual size and arguments.
   // So for the moment I only free the first dimension.
   // Ryu.
   delete [] parent;

   delete [] numOfValue;
   delete [] numOfFolderInclude;

   delete [] numOfFolderAffiliations;
   delete [] folderAffiliation;
   delete [] folderUsed;
   delete [] folderName;
   delete [] folderDescription;
   delete [] folderShortDescription;
   delete [] folderParentName;
   delete [] folderTitle;
   delete [] folderArray;
   delete [] folderAuthor;
   delete [] folderVersion;
   delete [] folderInclude;
   delete [] folderNet;
   delete [] folderLocalFlag;
   delete [] folderDataBase;
   delete [] folderUserCode;
   delete [] folderSupport;
   delete [] folderNoReset;

   delete [] valueName;
   delete [] valueType;
   delete [] valueInit;
   delete [] valueComment;
   delete [] valueDimension;
   delete [] valueArray;
   delete [] valueArraySpecifier;
   delete [] valueDBName;
   delete [] valueDBPath;
   delete [] valueNoBoundChech;
   delete [] valueIsTObject;

   // net folder
   delete [] netFolderName;
   delete [] netFolderTitle;
   delete [] netFolderHost;
   delete [] netFolderPort;
   delete [] netFolderRoot;

   // task
   delete [] numOfHistos;
   delete [] numOfTaskInclude;
   delete [] taskName;
   delete [] numOfTaskAffiliations;
   delete [] taskAffiliation;
   delete [] taskUsed;
   delete [] taskEventID;
   delete [] taskDescription;
   delete [] taskShortDescription;
   delete [] taskAuthor;
   delete [] taskAuthorInstitute;
   delete [] taskAuthorCollaboration;
   delete [] taskAuthorEmail;
   delete [] taskVersion;
   delete [] taskInclude;
   delete [] taskLocalFlag;
   delete [] histoName;
   delete [] histoTitle;
   delete [] histoFolderName;
   delete [] histoFolderTitle;
   delete [] histoType;
   delete [] histoArraySize;
   delete [] histoArrayStartIndex;
   delete [] histoXLabel;
   delete [] histoYLabel;
   delete [] histoZLabel;
   delete [] histoXNbins;
   delete [] histoXmin;
   delete [] histoXmax;
   delete [] histoYNbins;
   delete [] histoYmin;
   delete [] histoYmax;
   delete [] histoZNbins;
   delete [] histoZmin;
   delete [] histoZmax;
   delete [] numOfHistoTabs;
   delete [] histoTabName;
   delete [] histoTabIndex;
   delete [] histoTabArrayIndex;

   // task hierarchy
   delete [] taskHierarchyName;
   delete [] taskHierarchyParentIndex;
   delete [] taskHierarchyClassIndex;
   delete [] taskHierarchyMultiplicity;
   delete [] taskHierarchyLevel;
   delete [] taskHierarchyObjectIndex;
   delete [] taskHierarchySuffix;

   // steering
   delete [] numOfSteering;
   delete [] numOfSteerFields;
   delete [] numOfSteerChildren;
   delete [] steerName;
   delete [] steerArraySize;
   delete [] steerParent;
   delete [] steerChildren;
   delete [] numOfSteerAffiliations;
   delete [] steerAffiliation;
   delete [] steerUsed;
   delete [] steerFieldName;
   delete [] steerFieldType;
   delete [] steerFieldArraySize;
   delete [] steerFieldInit;
   delete [] steerFieldComment;
   delete [] steerFieldShortDescription;
   delete [] steerFieldCLOption;
   delete [] steerFieldCLDescription;
   delete [] numOfSteerFieldAffiliations;
   delete [] steerFieldAffiliation;
   delete [] steerFieldUsed;

   // tab
   delete [] tabName;
   delete [] tabSuffix;
   delete [] tabTitle;
   delete [] numOfTabAffiliations;
   delete [] tabAffiliation;
   delete [] tabUsed;
   delete [] tabDescription;
   delete [] tabShortDescription;
   delete [] tabAuthor;
   delete [] tabAuthorInstitute;
   delete [] tabAuthorCollaboration;
   delete [] tabAuthorEmail;
   delete [] tabVersion;
   delete [] tabHeredity;
   delete [] tabHeredityIndex;
   delete [] tabParentIndex;
   delete [] tabNumOfChildren;
   delete [] numOfMenu;
   delete [] numOfMenuItem;
   delete [] menuTitle;
   delete [] menuDepth;
   delete [] menuItemChildMenuIndex;
   delete [] menuItemEnumName;
   delete [] menuItemTitle;
   delete [] numOfTabHistos;
   delete [] tabHistoName;
   delete [] tabHistoIndex;
   delete [] tabHistoArrayIndexStart;
   delete [] tabHistoArrayIndexEnd;
   delete [] tabHistoTaskHierarchyIndex;
   delete [] tabHistoTaskIndex;
   delete [] tabHistoHistoIndex;
   delete [] tabHistoIndexMax;
   delete [] tabObjectName;
   delete [] tabObjectTitle;
   delete [] tabObject;
   delete [] tabObjectType;
   delete [] numOfTabObjectLines;
   delete [] tabObjectLineX1;
   delete [] tabObjectLineY1;
   delete [] tabObjectLineX2;
   delete [] tabObjectLineY2;
   delete [] tabObjectTaskHierarchyIndex;
   delete [] tabObjectTaskIndex;
   delete [] tabObjectHistoIndex;

   // tree
   delete [] numOfBranch;
   delete [] treeName;
   delete [] treeTitle;
   delete [] treeFileName;
   delete [] treeDescription;
   delete [] branchName;
   delete [] branchFolder;
   delete [] branchBufferSize;
   delete [] branchSplitLevel;

   // thread functions
   delete [] numOfThreadFunctions;
   delete [] numOfThreadFunctionArguments;
   delete [] threadFunctionName;
   delete [] threadFunctionArgument;

   // daq
   delete [] daqName;
   delete [] numOfDAQAffiliations;
   delete [] daqAffiliation;
   delete [] daqUsed;

   // database
   delete [] dbName;
   delete [] dbDescription;

   // midas
   delete [] numOfBank;
   delete [] numOfStructFields;
   delete [] eventName;
   delete [] eventID;
   delete [] eventTriggerMask;
   delete [] eventSamplingRate;
   delete [] bankName;
   delete [] bankType;
   delete [] bankArraySize;
   delete [] bankArrayStart;
   delete [] bankArrayDigit;
   delete [] structFieldName;
   delete [] structFieldType;
   delete [] structFieldSize;
   delete [] bankFieldArraySize;

   // root DAQ
   delete [] numOfRootBranch;
   delete [] numOfRootBranchField;
   delete [] rootTreeName;
   delete [] rootBranchName;
   delete [] rootBranchType;
   delete [] rootBranchArraySize;
   delete [] rootBranchClassName;
   delete [] rootBranchClassVersion;
   delete [] rootBranchFieldName;
   delete [] rootBranchFieldType;
   delete [] rootBranchFieldArraySize;

   // user makefile
   delete [] mfDictHeaderName;
   delete [] numOfMFDictHeaderAffiliations;
   delete [] mfDictHeaderAffiliation;
   delete [] mfDictHeaderUsed;
   delete [] mfDictIncDir;
   delete [] mfWinLibName;
   delete [] numOfMFWinLibFlags;
   delete [] mfWinLibFlag;
   delete [] mfUnixLibName;
   delete [] numOfMFUnixLibFlags;
   delete [] mfUnixLibFlag;
   delete [] mfIncDir;
   delete [] mfPreDefName;
   delete [] mfSourceFileName;
   delete [] mfSourceFilePath;
   delete [] mfHeaderFileName;
   delete [] mfHeaderFilePath;
   delete [] numOfMFSourceFlags;
   delete [] mfSourceFileFlag;
   delete [] numOfMFSourceAffiliations;
   delete [] mfSourceFileAffiliation;
   delete [] mfSourceFileUsed;
}

Bool_t ROMEBuilder::StartBuilder()
{

   int i,j,k,l;
   bool found;
   ROMEString str;

   TString::MaxWaste(kTStringResizeIncrement-1);
   TString::ResizeIncrement(kTStringResizeIncrement);

   if (!AllocateMemorySpace())
      return false;
   if (!ReadXMLDefinitionFile())
      return false;

   int tabNumber=numOfTab;
   int histoNumber=0;
   int is,ie,ind;
   // Check Histos & Tabs
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         for (k=0;k<numOfHistoTabs[taskHierarchyClassIndex[i]][j];k++) {
            found = false;
            for (l=0;l<numOfTab;l++) {
               if (!tabUsed[l])
                  continue;
               if (histoTabName[taskHierarchyClassIndex[i]][j][k]==tabName[l]) {
                  found = true;
                  tabNumber = l;
                  histoNumber = numOfTabHistos[l];
                  break;
               }
            }
            if (!found) {
               if (numOfTab<0)
                  numOfTab = 0;
               tabName[numOfTab] = histoTabName[taskHierarchyClassIndex[i]][j][k];
               tabTitle[numOfTab] = histoTabName[taskHierarchyClassIndex[i]][j][k];
               tabHistoDisplay[numOfTab] = false;
               tabAuthor[numOfTab] = mainAuthor;
               tabVersion[numOfTab] = "1";
               tabDescription[numOfTab] = "";
               tabShortDescription[numOfTab] = "";
               numOfSteering[numOfTab+numOfTask+1] = -1;
               numOfMenu[numOfTab] = -1;
               tabNumOfChildren[numOfTab] = 0;
               numOfThreadFunctions[numOfTab] = 0;
               tabParentIndex[numOfTab] = -1;

               tabNumber = numOfTab;
               histoNumber = 0;
               numOfTabHistos[numOfTab] = 0;
               tabUsed[numOfTab] = true;
               numOfTab++;
            }
            is = histoTabArrayIndex[taskHierarchyClassIndex[i]][j][k].ToInteger(); ie = is;
            if ((ind=histoTabArrayIndex[taskHierarchyClassIndex[i]][j][k].Index("-"))!=-1) {
               str = histoTabArrayIndex[taskHierarchyClassIndex[i]][j][k](0,ind);
               is = str.ToInteger();
               str = histoTabArrayIndex[taskHierarchyClassIndex[i]][j][k](ind+1,histoTabArrayIndex[taskHierarchyClassIndex[i]][j][k].Length()-ind-1);
               ie = str.ToInteger();
            }
            tabHistoName[tabNumber][histoNumber] = histoName[taskHierarchyClassIndex[i]][j];
            tabHistoIndex[tabNumber][histoNumber] = histoTabIndex[taskHierarchyClassIndex[i]][j][k].ToInteger();
            tabHistoArrayIndexStart[tabNumber][histoNumber] = is;
            tabHistoArrayIndexEnd[tabNumber][histoNumber] = ie;
            tabHistoTaskHierarchyIndex[tabNumber][histoNumber] = i;
            tabHistoTaskIndex[tabNumber][histoNumber] = taskHierarchyClassIndex[i];
            tabHistoHistoIndex[tabNumber][histoNumber] = j;
            numOfTabHistos[tabNumber]++;
         }
      }
   }

   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      tabHistoIndexMax[i] = 0;
      for (j=0;j<numOfTabHistos[i];j++) {
         tabHistoIndexMax[i] = TMath::Max(tabHistoIndex[i][j]+1+tabHistoArrayIndexEnd[i][j]-tabHistoArrayIndexStart[i][j],tabHistoIndexMax[i]);
      }
   }
   // fill task hierarchy index and suffix
   Int_t taskIndex = 0;
   Int_t suffixNumber = 0;
   Int_t multiplicity = 0;
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      taskHierarchyObjectIndex[i] = taskIndex++;
      suffixNumber = 0;
      multiplicity = 0;
      for (j=0;j<numOfTaskHierarchy;j++) {
         if (j!=i && taskHierarchyName[i]==taskHierarchyName[j]) {
            multiplicity++;
            if (j<i)
               suffixNumber++;
         }
      }
      if (multiplicity>0) {
         if (multiplicity<=9)
            taskHierarchySuffix[i].SetFormatted("_%01d",suffixNumber);
         if (multiplicity>9)
            taskHierarchySuffix[i].SetFormatted("_%02d",suffixNumber);
         if (multiplicity>99)
            taskHierarchySuffix[i].SetFormatted("_%03d",suffixNumber);
      }
      else
         taskHierarchySuffix[i] = "";
   }

   // fill tab suffix
   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      suffixNumber = 0;
      multiplicity = 0;
      for (j=0;j<numOfTab;j++) {
         if (j!=i && tabName[i]==tabName[j]) {
            multiplicity++;
            if (j<i)
               suffixNumber++;
         }
      }
      if (multiplicity>0) {
         if (multiplicity<=9)
            tabSuffix[i].SetFormatted("_%01d",suffixNumber);
         if (multiplicity>9)
            tabSuffix[i].SetFormatted("_%02d",suffixNumber);
         if (multiplicity>99)
            tabSuffix[i].SetFormatted("_%03d",suffixNumber);
      }
      else
         tabSuffix[i] = "";
   }

   if (!readGlobalSteeringParameters) {
      if (numOfTaskHierarchy==-1)
         numOfTaskHierarchy++;
      steerName[numOfTask][0] = "GlobalSteering";
      steerParent[numOfTask][0] = -1;
      numOfSteering[numOfTask] = 0;
      numOfSteerFields[numOfTask][0] = 0;
      numOfSteerChildren[numOfTask][0] = 0;
   }

   // Add DAQs to daqArray
   daqNameArray = new ROMEStrArray(1);
   daqTypeArray = new ROMEStrArray(1);
   daqDirArray = new ROMEStrArray(1);
   if (numOfEvent>0) {
      daqNameArray->AddLast("Midas");
      daqTypeArray->AddLast(shortCut.Data());
      daqDirArray->AddLast("generated/");
   }
   if (numOfTree>0) {
      daqNameArray->AddLast("Rome");
      daqTypeArray->AddLast(shortCut.Data());
      daqDirArray->AddLast("generated/");
   }
   if (numOfRootTree>0) {
      daqNameArray->AddLast("Root");
      daqTypeArray->AddLast(shortCut.Data());
      daqDirArray->AddLast("generated/");
   }
   daqNameArray->AddLast("DataBase");
   daqTypeArray->AddLast("ROME");
   daqDirArray->AddLast("");
   if (this->orca) {
      daqNameArray->AddLast("Orca");
      daqTypeArray->AddLast("ROME");
      daqDirArray->AddLast("");
   }
   for (i=0;i<numOfDAQ;i++) {
      if (!daqUsed[i])
         continue;
      str.SetFormatted("%s",daqName[i].Data());
      daqNameArray->AddLast(str.Data());
      daqTypeArray->AddLast(shortCut.Data());
      daqDirArray->AddLast("daqs/");
   }

   // make directories
   ROMEString path;
   path.SetFormatted("%sobj",outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%sdict",outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%ssrc", outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%sinclude", outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%ssrc/generated", outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%sinclude/generated", outDir.Data());
   gSystem->MakeDirectory(path.Data());
   if (numOfTask > 0) {
      path.SetFormatted("%ssrc/tasks", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      path.SetFormatted("%sinclude/tasks", outDir.Data());
      gSystem->MakeDirectory(path.Data());
   }
   if (numOfTab > 0) {
      path.SetFormatted("%ssrc/tabs", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      path.SetFormatted("%sinclude/tabs", outDir.Data());
      gSystem->MakeDirectory(path.Data());
   }
   if (numOfFolder > 0) {
      path.SetFormatted("%ssrc/folders", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      path.SetFormatted("%sinclude/folders", outDir.Data());
      gSystem->MakeDirectory(path.Data());
   }
   if (numOfDAQ > 0) {
      path.SetFormatted("%ssrc/daqs", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      path.SetFormatted("%sinclude/daqs", outDir.Data());
      gSystem->MakeDirectory(path.Data());
   }
   if (numOfDB > 0) {
      path.SetFormatted("%ssrc/databases", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      path.SetFormatted("%sinclude/databases", outDir.Data());
      gSystem->MakeDirectory(path.Data());
   }

   // write classes
   if (!AddConfigParameters()) return false;
   if (!CheckConfigParameters(mainParGroup)) return false;
   if (makeOutput)
      cout << "\n\nAnalyzer:" << endl;
   if (!WriteAnalyzerCpp()) return false;
   if (!WriteAnalyzer2Cpp()) return false;
   if (!WriteAnalyzer3Cpp()) return false;
   if (!WriteAnalyzerH()) return false;
   if (makeOutput)
      cout << "\n\nWindow:" << endl;
   if (!WriteWindowCpp()) return false;
   if (!WriteWindow2Cpp()) return false;
   if (!WriteWindowH()) return false;
   if (makeOutput)
      cout << "\n\nFolders:" << endl;
   if (!WriteFolderH()) return false;
   if (!WriteFolderCpp()) return false;
   if (!WriteAllFoldersH()) return false;
   if (makeOutput)
      cout << "\n\nTasks:" << endl;
   if (!WriteTaskCpp()) return false;
   if (!WriteBaseTaskCpp()) return false;
   if (!WriteTaskH()) return false;
   if (!WriteBaseTaskH()) return false;
   if (makeOutput)
      cout << "\n\nTabs:" << endl;
   if (!WriteTabCpp()) return false;
   if (!WriteBaseTabCpp()) return false;
   if (!WriteTabH()) return false;
   if (!WriteBaseTabH()) return false;
   if (makeOutput)
      cout << "\n\nUser DAQ Systems:" << endl;
   if (!WriteDAQCpp()) return false;
   if (!WriteDAQH()) return false;
   if (makeOutput)
      cout << "\n\nUser Database Interfaces:" << endl;
   if (!WriteDBCpp()) return false;
   if (!WriteDBH()) return false;
   if (makeOutput)
      cout << "\n\nFramework:" << endl;
   if (readGlobalSteeringParameters)
      if (!WriteSteering(numOfTask)) return false;
   if (!WriteConfigToFormCpp()) return false;
   if (!WriteConfigToFormH()) return false;
   if (!WriteConfigCpp()) return false;
   if (!WriteConfigH()) return false;
   if (numOfEvent>0) {
      if (!WriteMidasDAQCpp()) return false;
      if (!WriteMidasDAQH()) return false;
   }
   if (numOfTree>0) {
      if (!WriteRomeDAQCpp()) return false;
      if (!WriteRomeDAQH()) return false;
   }
   if (numOfRootTree>0) {
      if (!WriteRootDAQCpp()) return false;
      if (!WriteRootDAQH()) return false;
      if (!WriteRootDAQClassesH()) return false;
   }
   if (!WriteEventLoopCpp()) return false;
   if (!WriteEventLoopH()) return false;
   if (!WriteMain()) return false;
   if (!WriteReadTreesC()) return false;
   if (!WriteVersionH()) return false;

   ROMEString buffer;
   gSystem->ChangeDirectory(outDir.Data());

// Linking
   WriteMakefile();
   if (!WriteLinkDefHs()) return false;
   if (noLink) {
/*      ROMEString tempStr;
#if defined( R__UNIX )
      tempStr.SetFormatted("make -e");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      tempStr.SetFormatted("nmake -f Makefile.win");
#endif
      tempStr.AppendFormatted(" dict/%sGeneratedDict.cpp",shortCut.Data());
      if (librome) {
         tempStr.AppendFormatted(" dict/ROMESDict.cpp");
      }
      else {
         tempStr.AppendFormatted(" dict/ROMEDict.cpp");
         tempStr.AppendFormatted(" dict/ARGUSDict.cpp"); // currently no header when librome mode.
      }
      if (generatedFolderDictHeaders->GetEntriesFast()>0)
         tempStr.AppendFormatted(" dict/%sGeneratedFolderDict.cpp",shortCut.Data());
      if (folderHeaders->GetEntriesFast()>0)
         tempStr.AppendFormatted(" dict/%sFolderDict.cpp",shortCut.Data());
      if (generatedTaskDictHeaders->GetEntriesFast()>0)
         tempStr.AppendFormatted(" dict/%sGeneratedTaskDict.cpp",shortCut.Data());
      if (taskHeaders->GetEntriesFast()>0)
         tempStr.AppendFormatted(" dict/%sTaskDict.cpp",shortCut.Data());
      if (generatedTabDictHeaders->GetEntriesFast()>0)
         tempStr.AppendFormatted(" dict/%sGeneratedTabDict.cpp",shortCut.Data());
      if (tabHeaders->GetEntriesFast()>0)
         tempStr.AppendFormatted(" dict/%sTabDict.cpp",shortCut.Data());
      if (daqHeaders->GetEntriesFast()>0)
         tempStr.AppendFormatted(" dict/%sDAQDict.cpp",shortCut.Data());
      if (databaseHeaders->GetEntriesFast()>0)
         tempStr.AppendFormatted(" dict/%sDBDict.cpp",shortCut.Data());
      if (numOfMFDictHeaders>0)
         tempStr.AppendFormatted(" dict/%sUserDict.cpp",shortCut.Data());
      gSystem->Exec(tempStr.Data());*/
   }
// Documentation
   if (makeOutput) cout << "\nWrite HTML Documentation." << endl;
   WriteHTMLDoku();
#if (ROOT_VERSION_CODE < ROOT_VERSION(4,0,0))
   cout << endl << endl;
   cout << "******************* WARNING *******************" << endl;
   cout << "You are using a version of ROOT which does not" << endl;
   cout << "provide all the functionality that ROME needs!" << endl;
   cout << endl;
   cout << "However, ROME will work fine." << endl;
   cout << "But you can not use the following features:" << endl;
#if (ROOT_VERSION_CODE < ROOT_VERSION(4,1,0))
   cout << endl;
   cout << " - Circular trees (e.g. MaxNumberOfEntries>0)" << endl;
   cout << " - The socket interface" << endl;
   cout << endl;
   cout << " --> All of the above are available with version 4.01/00" << endl;
   cout << endl;
#endif // 4.01/00
   cout << endl;
#endif // ROOT_VERSION
   return true;
}

Bool_t ROMEBuilder::ReadCommandLineParameters(int argc, char *argv[])
{
   makeOutput = false;
   noLink = false;
   midas = false;
   orca = false;
   sql = false;
   mysql = false;
   pgsql = false;
   sqlite = false;
   sqlite3 = false;
   noVP = false;
#if defined (HAVE_LIBROME)
   librome = true;
#else
   librome = false;
#endif

   char workDir[kMAXPATHLEN];
   strcpy(workDir,gSystem->WorkingDirectory());

   outDir = workDir;
   outDir.Append("/");

   if (argc==1) {
      Usage();
      return false;
   }
   for (int i=1;i<argc;i++) {
      // -- only for testing (start) --
      if (!strcmp(argv[i],"-meg")) {
         makeOutput = false;
         midas = true;
         noLink = true;
         sql = true;
         mysql = true;
         outDir = "C:/meg/meganalyzer/";
         xmlFile = "C:/meg/meganalyzer/MEGAnalyzer.xml";
         affiliations.AddAtAndExpand("meg",0);
      }
      else if (!strcmp(argv[i],"-drs")) {
         makeOutput = false;
         midas = false;
         noLink = true;
         sql = false;
         mysql = false;
         outDir = "C:/meg/meganalyzer/";
         xmlFile = "C:/meg/meganalyzer/MEGAnalyzer.xml";
         flags.AddAtAndExpand("HAVE_DRS",0);
         flags.AddAtAndExpand("HAVE_MSCB",1);
         flags.AddAtAndExpand("HAVE_USB",2);
         flags.AddAtAndExpand("HAVE_AFG3251",3);
         flags.AddAtAndExpand("HAVE_VME",4);
         affiliations.AddAtAndExpand("drs",0);
      }
      else if (!strcmp(argv[i],"-dch")) {
         makeOutput = false;
         midas = true;
         noLink = true;
         sql = false;
         mysql = false;
         outDir = "C:/meg/meganalyzer/";
         xmlFile = "C:/meg/meganalyzer/MEGAnalyzer.xml";
         affiliations.AddAtAndExpand("dch",0);
      }
      else if (!strcmp(argv[i],"-drsdch")) {
         makeOutput = false;
         midas = true;
         noLink = true;
         sql = false;
         mysql = false;
         outDir = "C:/meg/meganalyzer/";
         xmlFile = "C:/meg/meganalyzer/MEGAnalyzer.xml";
         affiliations.AddAtAndExpand("dch",0);
         affiliations.AddAtAndExpand("drs",1);
         flags.AddAtAndExpand("HAVE_DRS",0);
         flags.AddAtAndExpand("HAVE_MSCB",1);
         flags.AddAtAndExpand("HAVE_USB",2);
         flags.AddAtAndExpand("HAVE_AFG3251",3);
         flags.AddAtAndExpand("HAVE_VME",4);
      }
      else if (!strcmp(argv[i],"-stepbystep")) {
         noLink = true;
         outDir = "C:/rome/examples/stepbystep/";
         xmlFile = "C:/rome/examples/stepbystep/stepbystep.xml";
      }
      else if (!strcmp(argv[i],"-readwrite")) {
         noLink = true;
         outDir = "C:/rome/examples/tree_write_read/";
         xmlFile = "C:/rome/examples/tree_write_read/tree_write_read.xml";
      }
      else if (!strcmp(argv[i],"-multi")) {
         noLink = true;
         outDir = "C:/rome/examples/multirun/";
         xmlFile = "C:/rome/examples/multirun/multirun.xml";
      }
      else if (!strcmp(argv[i],"-taskhierarchy")) {
         noLink = true;
         outDir = "C:/rome/examples/taskhierarchy/";
         xmlFile = "C:/rome/examples/taskhierarchy/taskhierarchy.xml";
      }
      else if (!strcmp(argv[i],"-netfolder")) {
         noLink = true;
         outDir = "C:/rome/examples/netfolder/";
         xmlFile = "C:/rome/examples/netfolder/netfolder.xml";
      }
      else if (!strcmp(argv[i],"-database")) {
         noLink = true;
         outDir = "C:/rome/examples/dbexample/";
         xmlFile = "C:/rome/examples/dbexample/RDBframework.xml";
      }
      else if (!strcmp(argv[i],"-histogui")) {
         noLink = true;
         outDir = "C:/rome/examples/histogui/";
         xmlFile = "C:/rome/examples/histogui/histogui.xml";
      }
      else if (!strcmp(argv[i],"-rootDAQ")) {
         noLink = true;
         outDir = "C:/rome/examples/rootDAQ/";
         xmlFile = "C:/rome/examples/rootDAQ/rootDAQ.xml";
      }
      else if (!strcmp(argv[i],"-lp")) {
         makeOutput = true;
         midas = true;
         noLink = true;
         outDir = "C:/lpframework/";
         xmlFile = "C:/lpframework/lpframework.xml";
      }
      else if (!strcmp(argv[i],"-laura")) {
         noLink = true;
         outDir = "C:/Data/Testprojects/laura/";
         xmlFile = "C:/Data/Testprojects/laura/rootDAQClean.xml";
      }
      // -- only for testing (end) --
      else if (!strcmp(argv[i],"-v")) {
         makeOutput = true;
      }
      else if (!strcmp(argv[i],"-nvp")) {
         noVP = true;
      }
      else if (!strcmp(argv[i],"-nl")) {
         noLink = true;
      }
      else if (!strcmp(argv[i],"-nosql")) {
         cout<<"-nosql is obsolete. SQL support is off by default."<<endl;
      }
      else if (!strcmp(argv[i],"-mysql")) {
         mysql = true;
      }
      else if (!strcmp(argv[i],"-pgsql")) {
         pgsql = true;
      }
      else if (!strcmp(argv[i],"-sqlite")) {
         sqlite = true;
      }
      else if (!strcmp(argv[i],"-sqlite3")) {
         sqlite3 = true;
      }
      else if (!strcmp(argv[i],"-midas")) {
         midas = true;
         ROMEString midasFile;
         midasFile = getenv("MIDASSYS");
         midasFile.Append("/include/midas.h");
         if (gSystem->AccessPathName(midasFile.Data(),kFileExists)) {
            cout << "Midas library not found. Have you set the MIDASSYS environment variable ?" << endl;
            return false;
         }
      }
      else if (!strcmp(argv[i],"-orca")) {
         orca = true;
      }
      else if (!strcmp(argv[i],"-f")&&i<argc-1) {
         i++;
         while (argv[i][0]!='-') {
            flags.AddLast((const char*)argv[i]);
            i++;
            if (i>argc-1)
               break;
         }
         i--;
      }
      else if (!strcmp(argv[i],"-i")&&i<argc-1) {
         xmlFile = argv[i+1];
         i++;
      }
      else if (!strcmp(argv[i],"-o")&&i<argc-1) {
         outDir = argv[i+1];
         if (outDir[outDir.Length()-1]!='/' && outDir[outDir.Length()-1]!='\\')
            outDir.Append("/");
         i++;
      }
      else if (!strcmp(argv[i],"-a")&&i<argc-1) {
         i++;
         while (argv[i][0]!='-') {
            affiliations.AddLast((const char*)argv[i]);
            i++;
            if (i>argc-1)
               break;
         }
         i--;
      }
      else if (argv[i][0]=='-') {
         Usage();
         return false;
      }
      else {
         xmlFile = argv[i];
      }
   }

   sql = (mysql || pgsql || sqlite || sqlite3 );

   return true;
}

Bool_t ROMEBuilder::CheckFileAndPath()
{
   if (gSystem->AccessPathName(xmlFile.Data(),kFileExists)) {
      if ( xmlFile == "")
         cout << "No inputfile specified." << endl;
      else
         cout << "Inputfile '" << xmlFile.Data() << "' not found." << endl;
      return false;
   }
   ROMEString path;
   path = outDir;
   path.Remove(path.Length()-1);
   if (gSystem->AccessPathName(path.Data(),kFileExists)) {
      cout << "Outputpath '" << outDir.Data() << "' not found." << endl;
      return false;
   }
   return true;
}

void ROMEBuilder::Usage()
{
   cout << "  -i        Inputfile" << endl;
   cout << "  -o        Outputfile path" << endl;
   cout << "  -v        Verbose Mode (no Argument)" << endl;
   cout << "  -nl       No Linking (no Argument)" << endl;
   cout << "  -midas    Generated program can be connected to a midas online system (no Argument)" << endl;
   cout << "  -orca     Generated program can be connected to a orca DAQ system (no Argument)" << endl;
   cout << "  -mysql    Generated program can be connected to a MySQL server (no Argument)" << endl;
   cout << "  -pgsql    Generated program can be connected to a PostgreSQL server (no Argument)" << endl;
   cout << "  -sqlite   Generated program can be connected to a SQLite database (no Argument)" << endl;
   cout << "  -sqlite3  Generated program can be connected to a SQLite3 database (no Argument)" << endl;
   cout << "  -f        Compile flags added to the Makefile" << endl;
}

void ROMEBuilder::AnalyzeFileName(const char* file,ROMEString& pathOfFile,ROMEString& nameOfFile,ROMEString& extensionOfFile)
{
   Ssiz_t ind;
   ROMEString str = file;
   ind = TMath::Max(str.Last('/'),str.Last('\\'));
   pathOfFile = str(0,ind+1);
   str = str(ind+1,str.Length()-ind-1);
   ind = str.Last('.');
   if (ind==-1) {
      nameOfFile = str;
      extensionOfFile = "";
   }
   else {
      nameOfFile = str(0,ind);
      extensionOfFile = str(ind+1,str.Length()-ind-1);
   }
}

Bool_t ROMEBuilder::WriteFile(const char* filename,const char* body,Int_t nspace, Bool_t backup)
{
   // return true when backup file is created
   fstream *fileStream;
   ROMEString fileBuffer;
   bool backupCreated = false;

   if ((fileStream = new fstream(filename,ios::in))) {
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
   }

   if (fileBuffer != body) {
      if (backup)
         backupCreated = BackUpFile(filename);
      if (!(fileStream = new fstream(filename,ios::out | ios::trunc))) {
         if (makeOutput) cout << "\n\nError : Failed to open '" << filename << "' !!!" << endl;
         return backupCreated;
      }
      if (makeOutput) cout << setw(nspace)<< "" << filename << endl;
      *fileStream<<body;
      delete fileStream;
   }
   return backupCreated;
}

void ROMEBuilder::GetFormat(ROMEString* buf,const char *type)
{
   buf->Resize(0);
   // get the format specifier (like '%s') of a declaration type
   if (!strcmp(type,"int") ||
       !strcmp(type,"unsigned int") ||
       !strcmp(type,"Int_t") ||
       !strcmp(type,"UInt_t") ||

       !strcmp(type,"long") ||
       !strcmp(type,"unsigned long") ||
       !strcmp(type,"Long_t") ||
       !strcmp(type,"ULong_t") ||

       !strcmp(type,"short") ||
       !strcmp(type,"unsigned short") ||
       !strcmp(type,"Short_t") ||
       !strcmp(type,"UShort_t") ||

       !strcmp(type,"long long") ||
       !strcmp(type,"unsigned long long") ||

       !strcmp(type,"Style_t") ||
       !strcmp(type,"Marker_t") ||
       !strcmp(type,"Color_t") ||
       !strcmp(type,"Font_t") ||
       !strcmp(type,"Version_t")) {
      buf->Append("%d");
   }
   else if (!strcmp(type,"char") ||
       !strcmp(type,"unsigned char") ||
       !strcmp(type,"Char_t") ||
       !strcmp(type,"UChar_t") ||

       !strcmp(type,"Option_t") ||
       !strcmp(type,"Text_t")) {
      buf->Append("%s");
   }
   else if (!strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      buf->Append("%g");
   }
   else if (!strcmp(type,"bool") ||
      !strcmp(type,"Bool_t")) {
      buf->Append("%d");
   }
   else {
      buf->Append("%s");
   }
}

void ROMEBuilder::setValue(ROMEString* buf,const char *destination,const char *source,const char *type,Int_t version)
{
   buf->Resize(0);
   // returns code which transformes a source variable of any type into a destination variable of type character
   if (
       !strcmp(type,"int") ||
       !strcmp(type,"unsigned int") ||
       !strcmp(type,"Int_t") ||
       !strcmp(type,"UInt_t") ||

       !strcmp(type,"long") ||
       !strcmp(type,"unsigned long") ||
       !strcmp(type,"Long_t") ||
       !strcmp(type,"ULong_t") ||

       !strcmp(type,"short") ||
       !strcmp(type,"unsigned short") ||
       !strcmp(type,"Short_t") ||
       !strcmp(type,"UShort_t") ||

       !strcmp(type,"long long") ||
       !strcmp(type,"unsigned long long") ||

       !strcmp(type,"Style_t") ||
       !strcmp(type,"Marker_t") ||
       !strcmp(type,"Color_t") ||
       !strcmp(type,"Font_t") ||
       !strcmp(type,"Version_t")) {
      if (version==0)
         buf->AppendFormatted("%s = strtol(%s,&cstop,10)",destination,source);
      else
         buf->AppendFormatted("strtol(%s,&cstop,10)",source);
   }
   else if (
       !strcmp(type,"bool") ||
       !strcmp(type,"Bool_t")) {
      if (version==0)
         buf->AppendFormatted("%s = gAnalyzer->strtobool(%s)",destination,source);
      else
         buf->AppendFormatted("gAnalyzer->strtobool(%s)",source);
   }
   else if (
       !strcmp(type,"char") ||
       !strcmp(type,"unsigned char") ||
       !strcmp(type,"Char_t") ||
       !strcmp(type,"UChar_t") ||

       !strcmp(type,"Option_t") ||
       !strcmp(type,"Text_t")) {
      if (version==0)
         buf->AppendFormatted("strcpy(%s,%s)",destination,source);
      else
         buf->AppendFormatted("%s",source);
   }
   else if (
       !strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||
       !strcmp(type,"Double32_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      if (version==0)
         buf->AppendFormatted("%s = strtod(%s,&cstop)",destination,source);
      else
         buf->AppendFormatted("strtod(%s,&cstop)",source);
   }
   else {
      if (version==0)
         buf->AppendFormatted("%s = %s",destination,source);
      else
         buf->AppendFormatted("%s",source);
   }
}

Bool_t ROMEBuilder::isNumber(const char* str)
{
   if (strcmp(str,"float")&&strcmp(str,"Float_t")&&
       strcmp(str,"double")&&strcmp(str,"Double_t")&&strcmp(str,"Double32_t")&&
       strcmp(str,"int")&&strcmp(str,"Int_t")&&
       strcmp(str,"unsigned int")&&strcmp(str,"UInt_t")&&
       strcmp(str,"long")&&strcmp(str,"Long_t")&&
       strcmp(str,"unsigned long")&&strcmp(str,"ULong_t")&&
       strcmp(str,"short")&&strcmp(str,"Short_t")&&
       strcmp(str,"unsigned short")&&strcmp(str,"UShort_t")&&
       strcmp(str,"Long64_t")&&strcmp(str,"ULong64_t")&&
       strcmp(str,"long long")&&strcmp(str,"unsigned long long"))
      return false;
   return true;
}

Bool_t ROMEBuilder::isFloatingType(const char *type)
{
   if (
       !strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||
       !strcmp(type,"Double32_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      return true;
   }
   return false;
}

Bool_t ROMEBuilder::isBoolType(const char *type)
{
   if (
       !strcmp(type,"bool") ||
       !strcmp(type,"Bool_t")) {
      return true;
   }
   return false;
}

Bool_t ROMEBuilder::isIntType(const char *type)
{
// check if the type is valid for array length specifier in TTree.
// unsigned, char, short, Long64_t are not valid.
   return (
      !strcmp(type,"int")
      || !strcmp(type,"Int_t")
      );
}

ROMEString& ROMEBuilder::convertType(const char *value,const char *oldType,const char *newType,ROMEString& stringBuffer)
{
   int type = 0;
   if (!strcmp(newType,"int") || !strcmp(newType,"Int_t"))
      type = 1;
   else if (!strcmp(newType,"double") || !strcmp(newType,"Double_t"))
      type = 2;
   else if (!strcmp(newType,"ROMEString&"))
      type = 3;
   else
      return stringBuffer;
   if (type==1 || type==2) {
      if (isNumber(oldType) || isBoolType(oldType)) {
         if (type==1)
            return stringBuffer.SetFormatted("(int)%s",value);
         if (type==2)
            return stringBuffer.SetFormatted("(double)%s",value);
      }
      else {
         if (type==1)
            stringBuffer.SetFormatted("strtol(%s,&cstop,10)",value);
         if (type==2)
            stringBuffer.SetFormatted("strtod(%s,&cstop)",value);
         if (!strcmp(oldType,"std::string"))
            stringBuffer.Insert(strlen(value)+7,".c_str()");
         else if (!strcmp(oldType,"ROMEString") || !strcmp(oldType,"TString"))
            stringBuffer.Insert(strlen(value)+7,".Data()");
         return stringBuffer;
      }
   }
   else if (type==3) {
      if (isBoolType(oldType))
         return stringBuffer;
      ROMEString tmp = stringBuffer;
      if (!strcmp(oldType,"std::string"))
         return stringBuffer.SetFormatted("%s=%s.c_str()",tmp.Data(),value);
      if (!strcmp(oldType,"ROMEString") || !strcmp(oldType,"TString"))
         return stringBuffer.SetFormatted("%s=%s",tmp.Data(),value);
      if (isFloatingType(oldType))
         return stringBuffer.SetFormatted("%s.SetFormatted(\"%%f\",%s)",tmp.Data(),value);
      else
         return stringBuffer.SetFormatted("%s.SetFormatted(\"%%d\",%s)",tmp.Data(),value);
   }
   return stringBuffer;
}

Bool_t ROMEBuilder::isFolder(const char *type)
{
   int j;
   ROMEString str;
   for (j=0;j<numOfFolder;j++) {
      if (!strcmp(type,folderName[j].Data()))
         return true;
      str.SetFormatted("%s*",folderName[j].Data());
      if (!strcmp(type,str.Data()))
         return true;
      str.SetFormatted("%s%s*",shortCut.Data(),folderName[j].Data());
      if (!strcmp(type,str.Data()))
         return true;
   }
   return false;
}

Bool_t ROMEBuilder::isTArrayType(const char *type)
{
   int j;
   ROMEString str;
   const char arrayTypes[][8]
      = {"TArrayC"
         ,"TArrayI"
         ,"TArrayD"
         ,"TArrayL"
         ,"TArrayF"
         ,"TArrayS"
      };

   for (j=0;j<6;j++) {
      if (!strcmp(type,arrayTypes[j]))
         return true;
      str.SetFormatted("%s*",arrayTypes[j]);
      if (!strcmp(type,str.Data()))
         return true;
   }
   return false;
}

const char* ROMEBuilder::TArray2StandardType(const char *type,ROMEString &standardType)
{
   int j;
   ROMEString str;
   const char arrayTypes[][2][9]
      = { {"TArrayC","Char_t  "}
         ,{"TArrayI","Int_t   "}
         ,{"TArrayD","Double_t"}
         ,{"TArrayL","Long_t  "}
         ,{"TArrayF","Float_t "}
         ,{"TArrayS","Short_t "}
      };

   for (j=0;j<6;j++) {
      if (!strcmp(type,arrayTypes[j][0])) {
         standardType = arrayTypes[j][1];
         return standardType.Data();
      }
      str.SetFormatted("%s*",(char*)arrayTypes[j]);
      if (!strcmp(type,str.Data())) {
         standardType = arrayTypes[j][1];
         return standardType.Data();
      }
   }
   return type;
}

void* ROMEBuilder::AllocateInt(Int_t x1, Int_t x2, Int_t x3, Int_t x4, Int_t x5, Int_t x6)
{
   Int_t n = 0;

   Int_t *p1;
   Int_t **p2;
   Int_t ***p3;
   Int_t ****p4;
   Int_t *****p5;
   Int_t ******p6;
   Int_t i;

   while(1) {
      if(x1) n++; else break;
      if(x2) n++; else break;
      if(x3) n++; else break;
      if(x4) n++; else break;
      if(x5) n++; else break;
      if(x6) n++; else break;
      break;
   }

   switch(n) {
      case 1:
         p1 = new Int_t[x1];
         return p1;

      case 2:
         p2 = new Int_t*[x1];
         for (i=0;i<x1;i++)
            p2[i] = static_cast<Int_t*>(AllocateInt(x2));
         return p2;

      case 3:
         p3 = new Int_t**[x1];
         for (i=0;i<x1;i++)
            p3[i] = static_cast<Int_t**>(AllocateInt(x2, x3));
         return p3;

      case 4:
         p4 = new Int_t***[x1];
         for (i=0;i<x1;i++)
            p4[i] = static_cast<Int_t***>(AllocateInt(x2, x3, x4));
         return p4;

      case 5:
         p5 = new Int_t****[x1];
         for (i=0;i<x1;i++)
            p5[i] = static_cast<Int_t****>(AllocateInt(x2, x3, x4, x5));
         return p5;

      case 6:
         p6 = new Int_t*****[x1];
         for (i=0;i<x1;i++)
            p6[i] = static_cast<Int_t*****>(AllocateInt(x2, x3, x4, x5, x6));
         return p6;
   };

   return 0;
}

void* ROMEBuilder::AllocateBool(Int_t x1, Int_t x2, Int_t x3, Int_t x4, Int_t x5, Int_t x6)
{
   Int_t n = 0;

   Bool_t *p1;
   Bool_t **p2;
   Bool_t ***p3;
   Bool_t ****p4;
   Bool_t *****p5;
   Bool_t ******p6;
   Int_t i;

   while(1) {
      if(x1) n++; else break;
      if(x2) n++; else break;
      if(x3) n++; else break;
      if(x4) n++; else break;
      if(x5) n++; else break;
      if(x6) n++; else break;
      break;
   }

   switch(n) {
      case 1:
         p1 = new Bool_t[x1];
         return p1;

      case 2:
         p2 = new Bool_t*[x1];
         for (i=0;i<x1;i++)
            p2[i] = static_cast<Bool_t*>(AllocateBool(x2));
         return p2;

      case 3:
         p3 = new Bool_t**[x1];
         for (i=0;i<x1;i++)
            p3[i] = static_cast<Bool_t**>(AllocateBool(x2, x3));
         return p3;

      case 4:
         p4 = new Bool_t***[x1];
         for (i=0;i<x1;i++)
            p4[i] = static_cast<Bool_t***>(AllocateBool(x2, x3, x4));
         return p4;

      case 5:
         p5 = new Bool_t****[x1];
         for (i=0;i<x1;i++)
            p5[i] = static_cast<Bool_t****>(AllocateBool(x2, x3, x4, x5));
         return p5;

      case 6:
         p6 = new Bool_t*****[x1];
         for (i=0;i<x1;i++)
            p6[i] = static_cast<Bool_t*****>(AllocateBool(x2, x3, x4, x5, x6));
         return p6;
   };

   return 0;
}

void* ROMEBuilder::AllocateROMEString(Int_t x1, Int_t x2, Int_t x3, Int_t x4, Int_t x5, Int_t x6)
{
   Int_t n = 0;

   ROMEString *p1;
   ROMEString **p2;
   ROMEString ***p3;
   ROMEString ****p4;
   ROMEString *****p5;
   ROMEString ******p6;
   Int_t i;

   while(1) {
      if(x1) n++; else break;
      if(x2) n++; else break;
      if(x3) n++; else break;
      if(x4) n++; else break;
      if(x5) n++; else break;
      if(x6) n++; else break;
      break;
   }

   switch(n) {
      case 1:
         p1 = new ROMEString[x1];
         return p1;

      case 2:
         p2 = new ROMEString*[x1];
         for (i=0;i<x1;i++)
            p2[i] = static_cast<ROMEString*>(AllocateROMEString(x2));
         return p2;

      case 3:
         p3 = new ROMEString**[x1];
         for (i=0;i<x1;i++)
            p3[i] = static_cast<ROMEString**>(AllocateROMEString(x2, x3));
         return p3;

      case 4:
         p4 = new ROMEString***[x1];
         for (i=0;i<x1;i++)
            p4[i] = static_cast<ROMEString***>(AllocateROMEString(x2, x3, x4));
         return p4;

      case 5:
         p5 = new ROMEString****[x1];
         for (i=0;i<x1;i++)
            p5[i] = static_cast<ROMEString****>(AllocateROMEString(x2, x3, x4, x5));
         return p5;

      case 6:
         p6 = new ROMEString*****[x1];
         for (i=0;i<x1;i++)
            p6[i] = static_cast<ROMEString*****>(AllocateROMEString(x2, x3, x4, x5, x6));
         return p6;
   };

   return 0;
}
