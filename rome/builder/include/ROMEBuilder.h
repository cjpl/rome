/********************************************************************
  ROMEBuilder.h, M. Schneebeli PSI

  $Id$

********************************************************************/

#ifndef ROMEBuilder_H
#define ROMEBuilder_H

#include <ROMEXML.h>
#include <ROMEString.h>
#include <ROMEConfigParameter.h>

const Int_t maxNumberOfTasks = 300;
const Int_t maxNumberOfFolders = 200;
const Int_t maxNumberOfTrees = 50;
const Int_t maxNumberOfDAQ = 10;
const Int_t maxNumberOfDB = 10;
const Int_t maxNumberOfBranches = 20;
const Int_t maxNumberOfBanks = 50;
const Int_t maxNumberOfEvents = 5;
const Int_t maxNumberOfSteering = 50;
const Int_t maxNumberOfSteeringField = 100;
const Int_t maxNumberOfValues = 50;
const Int_t maxNumberOfValueDimension = 3;
const Int_t maxNumberOfInclude = 10;
const Int_t maxNumberOfHistos = 50;
const Int_t maxNumberOfHistoTabs = 5;
const Int_t maxNumberOfStructFields = 50;
const Int_t maxNumberOfMFDictHeaders = 100;
const Int_t maxNumberOfMFDictIncDirs = 20;
const Int_t maxNumberOfMFWinLibs = 20;
const Int_t maxNumberOfMFWinLibFlags = 5;
const Int_t maxNumberOfMFUnixLibs = 20;
const Int_t maxNumberOfMFUnixLibFlags = 5;
const Int_t maxNumberOfMFIncDirs = 20;
const Int_t maxNumberOfMFPreDefs = 20;
const Int_t maxNumberOfMFSources = 100;
const Int_t maxNumberOfMFSourceFlags = 5;
const Int_t maxNumberOfEventRequests = 5;
const Int_t maxNumberOfPathObjectInterpreterCodes = 10;
const Int_t maxNumberOfTabs = 200;
const Int_t maxNumberOfTabHistos = 20;
const Int_t maxNumberOfMenus = 20;
const Int_t maxNumberOfMenuItems = 50;
const Int_t maxNumberOfThreadFunctions = 10;
const Int_t maxNumberOfThreadFunctionArguments = 10;
const Int_t maxNumberOfNetFolders = 10;
const Ssiz_t kTStringResizeIncrement = 4096;
const char* const LINE_TITLE = "NoDayWithoutItsLine";

const char  valueCounter[maxNumberOfValueDimension] = {'i','j','k'};
const char* const ROMECommandLineOptions = ":i:b:q:ns:m:r:e:o:docu:";
const char* const cloSeparator = ":";
#if defined( R__VISUAL_CPLUSPLUS )
const char* const kEqualSign = "=";
const char* const kFlagSign = "/";
#else
const char* const kEqualSign =":=";
const char* const kFlagSign = "-";
#endif
const char* const kHeaderEndMark = "/////////////////////////////////////----///////////////////////////////////////";

class ROMEBuilder
{
public:
   ROMEString    romeVersion;

protected:
   Bool_t        haveFortranTask;

   ROMEString    outDir;
   ROMEString    xmlFile;
   Bool_t        makeOutput;
   Bool_t        noLink;
   Bool_t        midas;
   Bool_t        orca;
   Bool_t        sql;
   Bool_t        mysql;
   Bool_t        pgsql;
   Bool_t        sqlite;
   Bool_t        sqlite3;
   Bool_t        noVP;
   ROMEStrArray  flags;

   ROMEXML*      xml;

   ROMEString    shortCut;
   ROMEString    experimentName;

   Bool_t        readExperiment;
   Bool_t        readAuthor;
   Bool_t        readFolders;
   Bool_t        readTasks;
   Bool_t        readTrees;
   Bool_t        readGlobalSteeringParameters;
   Bool_t        readMidasBanks;

   Bool_t        hasStructuredBank;

   ROMEString    parent[maxNumberOfTasks];
   Int_t         recursiveDepth;
   Int_t         recursiveSteerDepth;
   Int_t         actualSteerIndex;

   Int_t         fNumberOfInterpreterCodes;

   Int_t         recursiveTabDepth;
   Int_t         recursiveMenuDepth;
   
// folders
   Int_t         numOfFolder;
   Int_t         numOfValue[maxNumberOfFolders];
   Int_t         numOfFolderInclude[maxNumberOfFolders];

   ROMEString    folderName[maxNumberOfFolders];
   ROMEString    folderDescription[maxNumberOfFolders];
   ROMEString    folderParentName[maxNumberOfFolders];
   ROMEString    folderTitle[maxNumberOfFolders];
   ROMEString    folderArray[maxNumberOfFolders];
   ROMEString    folderAuthor[maxNumberOfFolders];
   ROMEString    folderVersion[maxNumberOfFolders];
   ROMEString    folderInclude[maxNumberOfFolders][maxNumberOfInclude];
   Bool_t        folderNet[maxNumberOfFolders];
   Bool_t        folderLocalFlag[maxNumberOfFolders][maxNumberOfInclude];
   Bool_t        folderDataBase[maxNumberOfFolders];
   Bool_t        folderUserCode[maxNumberOfFolders];
   Bool_t        folderSupport[maxNumberOfFolders];
   Bool_t        folderNoReset[maxNumberOfFolders];
   Bool_t        hasFolderUserCode;
   Bool_t        hasFolderGenerated;

   ROMEString    valueName[maxNumberOfFolders][maxNumberOfValues];
   ROMEString    valueType[maxNumberOfFolders][maxNumberOfValues];
   ROMEString    valueInit[maxNumberOfFolders][maxNumberOfValues];
   ROMEString    valueComment[maxNumberOfFolders][maxNumberOfValues];
   Int_t         valueDimension[maxNumberOfFolders][maxNumberOfValues];
   ROMEString    valueArray[maxNumberOfFolders][maxNumberOfValues][maxNumberOfValueDimension];
   ROMEString    valueDBName[maxNumberOfFolders][maxNumberOfValues];
   ROMEString    valueDBPath[maxNumberOfFolders][maxNumberOfValues];
   Bool_t        valueNoBoundChech[maxNumberOfFolders][maxNumberOfValues];

// net folder
   Int_t         numOfNetFolder;
   ROMEString    netFolderName[maxNumberOfNetFolders];
   ROMEString    netFolderTitle[maxNumberOfNetFolders];
   ROMEString    netFolderHost[maxNumberOfNetFolders];
   ROMEString    netFolderPort[maxNumberOfNetFolders];
   ROMEString    netFolderRoot[maxNumberOfNetFolders];

// task
   Int_t         numOfTask;
   Int_t         numOfHistos[maxNumberOfTasks];
   Int_t         numOfTaskInclude[maxNumberOfTasks];
   ROMEString    taskName[maxNumberOfTasks];
   ROMEString    taskEventID[maxNumberOfTasks];
   ROMEString    taskDescription[maxNumberOfTasks];
   Bool_t        taskFortran[maxNumberOfTasks];
   Bool_t        taskUserCode[maxNumberOfTasks];
   ROMEString    taskAuthor[maxNumberOfTasks];
   ROMEString    taskAuthorInstitute[maxNumberOfTasks];
   ROMEString    taskAuthorCollaboration[maxNumberOfTasks];
   ROMEString    taskAuthorEmail[maxNumberOfTasks];
   ROMEString    taskVersion[maxNumberOfTasks];
   ROMEString    taskInclude[maxNumberOfTasks][maxNumberOfInclude];
   Bool_t        taskLocalFlag[maxNumberOfTasks][maxNumberOfInclude];
   ROMEString    histoName[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoTitle[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoFolderName[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoFolderTitle[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoType[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoArraySize[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoArrayStartIndex[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoXLabel[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoYLabel[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoZLabel[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoXNbins[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoXmin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoXmax[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoYNbins[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoYmin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoYmax[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoZNbins[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoZmin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoZmax[maxNumberOfTasks][maxNumberOfHistos];
   Int_t         numOfHistoTabs[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString    histoTabName[maxNumberOfTasks][maxNumberOfHistos][maxNumberOfHistoTabs];
   ROMEString    histoTabIndex[maxNumberOfTasks][maxNumberOfHistos][maxNumberOfHistoTabs];
   ROMEString    histoTabArrayIndex[maxNumberOfTasks][maxNumberOfHistos][maxNumberOfHistoTabs];
   Bool_t        hasTaskUserCode;
   Bool_t        hasTaskGenerated;

// task hierarchy
   Int_t         numOfTaskHierarchy;
   ROMEString    taskHierarchyName[2*maxNumberOfTasks];
   Int_t         taskHierarchyParentIndex[2*maxNumberOfTasks];
   Int_t         taskHierarchyClassIndex[2*maxNumberOfTasks];
   Int_t         taskHierarchyMultiplicity[2*maxNumberOfTasks];

// steering
   Int_t         numOfSteering[maxNumberOfTasks+1];
   Int_t         numOfSteerFields[maxNumberOfTasks+1][maxNumberOfSteering];
   Int_t         numOfSteerChildren[maxNumberOfTasks+1][maxNumberOfSteering];
   ROMEString    steerName[maxNumberOfTasks+1][maxNumberOfSteering];
   ROMEString    steerArraySize[maxNumberOfTasks+1][maxNumberOfSteering];
   Int_t         steerParent[maxNumberOfTasks+1][maxNumberOfSteering];
   Int_t         steerChildren[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteering];
   ROMEString    steerFieldName[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString    steerFieldType[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString    steerFieldArraySize[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString    steerFieldInit[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString    steerFieldComment[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString    steerFieldCLOption[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString    steerFieldCLDescription[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];

   ROMEString    usedCLO;

// tab
   Int_t         numOfTab;
   ROMEString    tabName[maxNumberOfTabs];
   ROMEString    tabTitle[maxNumberOfTabs];
   ROMEString    tabDescription[maxNumberOfTabs];
   ROMEString    tabAuthor[maxNumberOfTabs];
   ROMEString    tabAuthorInstitute[maxNumberOfTabs];
   ROMEString    tabAuthorCollaboration[maxNumberOfTabs];
   ROMEString    tabAuthorEmail[maxNumberOfTabs];
   ROMEString    tabVersion[maxNumberOfTabs];
   ROMEString    tabHeredity[maxNumberOfTabs];
   Int_t         tabHeredityIndex[maxNumberOfTabs];
   Int_t         tabParentIndex[maxNumberOfTabs];
   Int_t         tabNumOfChildren[maxNumberOfTabs];
   Int_t         numOfMenu[maxNumberOfTabs];
   Int_t         numOfMenuItem[maxNumberOfTabs][maxNumberOfMenus];
   ROMEString    menuTitle[maxNumberOfTabs][maxNumberOfMenus];
   Int_t         menuDepth[maxNumberOfTabs][maxNumberOfMenus];
   Int_t         menuItemChildMenuIndex[maxNumberOfTabs][maxNumberOfMenus][maxNumberOfMenuItems];
   ROMEString    menuItemEnumName[maxNumberOfTabs][maxNumberOfMenus][maxNumberOfMenuItems];
   ROMEString    menuItemTitle[maxNumberOfTabs][maxNumberOfMenus][maxNumberOfMenuItems];
   Int_t         numOfTabHistos[maxNumberOfTabs];
   ROMEString    tabHistoName[maxNumberOfTabs][maxNumberOfTabHistos];
   Int_t         tabHistoIndex[maxNumberOfTabs][maxNumberOfTabHistos];
   Int_t         tabHistoArrayIndexStart[maxNumberOfTabs][maxNumberOfTabHistos];
   Int_t         tabHistoArrayIndexEnd[maxNumberOfTabs][maxNumberOfTabHistos];
   Int_t         tabHistoTaskIndex[maxNumberOfTabs][maxNumberOfTabHistos];
   Int_t         tabHistoHistoIndex[maxNumberOfTabs][maxNumberOfTabHistos];
   Int_t         tabHistoIndexMax[maxNumberOfTabs];

// tree
   Int_t         numOfTree;
   Int_t         numOfBranch[maxNumberOfTrees];
   ROMEString    treeName[maxNumberOfTrees];
   ROMEString    treeTitle[maxNumberOfTrees];
   ROMEString    treeFileName[maxNumberOfTrees];
   ROMEString    treeDescription[maxNumberOfTrees];
   ROMEString    branchName[maxNumberOfTrees][maxNumberOfBranches];
   ROMEString    branchFolder[maxNumberOfTrees][maxNumberOfBranches];
   ROMEString    branchBufferSize[maxNumberOfTrees][maxNumberOfBranches];
   ROMEString    branchSplitLevel[maxNumberOfTrees][maxNumberOfBranches];

// thread functions
   Int_t         numOfThreadFunctions[maxNumberOfTabs];
   Int_t         numOfThreadFunctionArguments[maxNumberOfTabs][maxNumberOfThreadFunctions];
   ROMEString    threadFunctionName[maxNumberOfTabs][maxNumberOfThreadFunctions];
   ROMEString    threadFunctionArgument[maxNumberOfTabs][maxNumberOfThreadFunctions][maxNumberOfThreadFunctionArguments];

// daq
   Int_t         numOfDAQ;
   ROMEString    daqName[maxNumberOfDAQ];

// database
   Int_t         numOfDB;
   ROMEString    dbName[maxNumberOfDB];
   ROMEString    dbDescription[maxNumberOfDB];

// midas
   Int_t         numOfEvent;
   Int_t         numOfBank[maxNumberOfEvents];
   Int_t         numOfStructFields[maxNumberOfEvents][maxNumberOfBanks];
   ROMEString    eventName[maxNumberOfEvents];
   ROMEString    eventID[maxNumberOfEvents];
   ROMEString    eventTriggerMask[maxNumberOfEvents];
   ROMEString    eventSamplingRate[maxNumberOfEvents];
   ROMEString    bankName[maxNumberOfEvents][maxNumberOfBanks];
   ROMEString    bankType[maxNumberOfEvents][maxNumberOfBanks];
   Int_t         bankArraySize[maxNumberOfEvents][maxNumberOfBanks];
   Int_t         bankArrayStart[maxNumberOfEvents][maxNumberOfBanks];
   Int_t         bankArrayDigit[maxNumberOfEvents][maxNumberOfBanks];
   ROMEString    structFieldName[maxNumberOfEvents][maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString    structFieldType[maxNumberOfEvents][maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString    structFieldSize[maxNumberOfEvents][maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString    bankFieldArraySize[maxNumberOfEvents][maxNumberOfBanks][maxNumberOfStructFields];
   Bool_t        bankHasHeader;
   ROMEString    bankHeaderFolder;
   ROMEString    bankHeaderEventID;
   ROMEString    bankHeaderTriggerMask;
   ROMEString    bankHeaderSerialNumber;
   ROMEString    bankHeaderTimeStamp;

// user makefile
   Int_t         numOfIncludeDirectories;
   Int_t         numOfMFDictHeaders;
   ROMEString    mfDictHeaderName[maxNumberOfMFDictHeaders];
   Int_t         numOfMFDictIncDirs;
   ROMEString    mfDictIncDir[maxNumberOfMFDictIncDirs];

   Int_t         numOfMFWinLibs;
   ROMEString    mfWinLibName[maxNumberOfMFWinLibs];
   Int_t         numOfMFWinLibFlags[maxNumberOfMFWinLibs];
   ROMEString    mfWinLibFlag[maxNumberOfMFWinLibs][maxNumberOfMFWinLibFlags];

   Int_t         numOfMFUnixLibs;
   ROMEString    mfUnixLibName[maxNumberOfMFUnixLibs];
   Int_t         numOfMFUnixLibFlags[maxNumberOfMFUnixLibs];
   ROMEString    mfUnixLibFlag[maxNumberOfMFUnixLibs][maxNumberOfMFUnixLibFlags];

   Int_t         numOfMFIncDirs;
   ROMEString    mfIncDir[maxNumberOfMFIncDirs];
   Int_t         numOfMFPreDefs;
   ROMEString    mfPreDefName[maxNumberOfMFPreDefs];
   Int_t         numOfMFSources;
   ROMEString    mfSourceFileName[maxNumberOfMFSources];
   ROMEString    mfSourceFilePath[maxNumberOfMFSources];
   ROMEString    mfHeaderFileName[maxNumberOfMFSources];
   ROMEString    mfHeaderFilePath[maxNumberOfMFSources];
   Int_t         numOfMFSourceFlags[maxNumberOfMFSources];
   ROMEString    mfSourceFileFlag[maxNumberOfMFSources][maxNumberOfMFSourceFlags];

// main
   ROMEString    mainAuthor;
   ROMEString    mainInstitute;
   ROMEString    mainCollaboration;
   ROMEString    mainEmail;
   ROMEString    mainProgName;
   ROMEString    mainDescription;
   ROMEString    styleSheet;

// Makefile
   ROMEStrArray* includeDirectories;
   ROMEStrArray* romeHeaders;
   ROMEStrArray* romeDictHeaders;
   ROMEStrArray* romeSources;
   ROMEStrArray* romeLinkDefSuffix;
   ROMEStrArray* argusHeaders;
   ROMEStrArray* argusSources;
   ROMEStrArray* argusLinkDefSuffix;
   ROMEStrArray* generatedHeaders;
   ROMEStrArray* generatedDictHeaders;
   ROMEStrArray* generatedLinkDefSuffix;
   ROMEStrArray* generatedFolderDictHeaders;
   ROMEStrArray* generatedFolderLinkDefSuffix;
   ROMEStrArray* generatedTaskDictHeaders;
   ROMEStrArray* generatedTaskLinkDefSuffix;
   ROMEStrArray* generatedTabDictHeaders;
   ROMEStrArray* generatedTabLinkDefSuffix;
   ROMEStrArray* generatedSources;
   ROMEStrArray* folderHeaders;
   ROMEStrArray* folderSources;
   ROMEStrArray* folderLinkDefSuffix;
   ROMEStrArray* taskHeaders;
   ROMEStrArray* taskSources;
   ROMEStrArray* taskLinkDefSuffix;
   ROMEStrArray* tabHeaders;
   ROMEStrArray* tabSources;
   ROMEStrArray* tabLinkDefSuffix;
   ROMEStrArray* daqHeaders;
   ROMEStrArray* daqSources;
   ROMEStrArray* databaseHeaders;
   ROMEStrArray* databaseSources;
   ROMEStrArray* rootLibraries;
   ROMEStrArray* mysqlLibraries;
   ROMEStrArray* midasLibraries;

// Configuration
   ROMEConfigParameterGroup* mainParGroup;

public:
   ROMEBuilder() { haveFortranTask = false; };
   ~ROMEBuilder() {};

   void    StartBuilder();
   Bool_t  ReadCommandLineParameters(Int_t argc, char *argv[]);
   Bool_t  CheckFileAndPath();

private:
   // XML Read Methods
   Bool_t  ReadXMLDefinitionFile() ;
   Bool_t  ReadXMLFolder();
   Bool_t  ReadXMLNetFolder();
   Bool_t  ReadXMLTask();
   Bool_t  ReadXMLTab();
   Bool_t  ReadXMLMenu(Int_t currentNumberOfTabs);
   Bool_t  ReadXMLTree();
   Bool_t  ReadXMLDAQ();
   Bool_t  ReadXMLDB();
   Bool_t  ReadXMLMidasBanks();
   Bool_t  ReadXMLSteering(Int_t iTask);
   Bool_t  ReadXMLUserMakefile();
   void    ParseSVNKeyword(ROMEString& str);

   // Code Write Methods
   Bool_t  WriteFolderCpp();
   Bool_t  WriteFolderH();
   Bool_t  WriteSteeringClass(ROMEString& buffer,Int_t numOfTaskSteer,Int_t numTask,Int_t tab);
   Bool_t  WriteSteeringReadParameters(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,ROMEString& steerPointer) ;
   Bool_t  WriteSteeringParameterUsage(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,ROMEString& steerPointer) ;
   Int_t   WriteSteeringInterpreterCode(ROMEString &buffer,Int_t codeNumber,Int_t numSteer,Int_t numTask,ROMEString& path,Int_t tab);
   Int_t   WriteSteeringInterpreterValue(ROMEString &buffer,const char* type,Int_t codeNumber,Int_t numSteer,Int_t numTask,ROMEString& steerPointer,Int_t tab);
   void    WriteObjectInterpreterValue(ROMEString &buffer,const char* type,const char* fctName);
   void    WriteReadDataBaseFolder(ROMEString &buffer,Int_t numFolder,Int_t type);
   void    WriteFolderGetter(ROMEString &buffer,Int_t numFolder,Int_t scl,Int_t nameLen,Int_t typeLen);
   Bool_t  WriteTaskCpp();
   Bool_t  WriteTaskF();
   Bool_t  WriteTaskH();
   Bool_t  WriteTabCpp();
   Bool_t  WriteTabH();
   Bool_t  WriteSteering(Int_t iTask);
   Bool_t  WriteAnalyzerCpp();
   Bool_t  WriteAnalyzerH();
   Bool_t  WriteAnalyzerF();
   Bool_t  WriteWindowCpp();
   Bool_t  WriteWindowH();
   Bool_t  AddTab(ROMEString& buffer,Int_t& i);
   Bool_t  AddMenuItems(ROMEString& buffer,Int_t i,Int_t j,Int_t iHeredity,Int_t jHeredity);
   Bool_t  WriteConfigCpp();
   Bool_t  WriteConfigH();
   Bool_t  AddConfigParameters();
   Bool_t  AddTaskConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex);
   Bool_t  AddTabConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex);
   Bool_t  AddSteeringConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t numSteer,Int_t numTask,ROMEString steerPointer);
   Bool_t  WriteConfigClass(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab);
   Bool_t  WriteConfigRead(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString className,ROMEString pointer,ROMEString indexes);
   Bool_t  WriteConfigSet(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString pointer);
   Bool_t  WriteConfigWrite(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString pointer);
   Bool_t  WriteMidasDAQCpp();
   Bool_t  WriteMidasDAQH();
   Bool_t  WriteRomeDAQCpp();
   Bool_t  WriteRomeDAQH();
   Bool_t  WriteDataBaseDAQCpp();
   Bool_t  WriteDataBaseDAQH();
   Bool_t  WriteDAQCpp();
   Bool_t  WriteDAQH();
   Bool_t  WriteDBCpp();
   Bool_t  WriteDBH();
   Bool_t  WriteEventLoopCpp();
   Bool_t  WriteEventLoopH();
   Bool_t  WriteMain();
   Bool_t  WriteReadTreesC();
   Bool_t  WriteLinkDefHs();
   Bool_t  WriteLinkDefH(ROMEStrArray* headers, ROMEStrArray* ldsuffix, const char* filename);

   // Makefile Methods
   void    AddIncludeDirectories();
   void    AddRomeHeaders();
   void    AddRomeDictHeaders();
   void    AddRomeSources();
   void    AddArgusHeaders();
   void    AddArgusSources();
   void    AddGeneratedHeaders();
   void    AddGeneratedDictHeaders();
   void    AddGeneratedFolderDictHeaders();
   void    AddGeneratedTaskDictHeaders();
   void    AddGeneratedTabDictHeaders();
   void    AddGeneratedSources();
   void    AddFolderHeaders();
   void    AddFolderSources();
   void    AddTaskHeaders();
   void    AddTaskSources();
   void    AddTabHeaders();
   void    AddTabSources();
   void    AddDAQHeaders();
   void    AddDAQSources();
   void    AddDatabaseHeaders();
   void    AddDatabaseSources();
   void    AddRootLibraries();
   void    AddMysqlLibraries();
   void    AddMidasLibraries();
   void    WriteMakefile();
   void    WriteMakefileHeader(ROMEString& buffer);
   void    WriteMakefileLibsAndFlags(ROMEString& buffer);
   void    WriteMakefileIncludes(ROMEString& buffer);
   void    WriteMakefileDictIncludes(ROMEString& buffer);
   void    WriteMakefileObjects(ROMEString& buffer,ROMEStrArray* sources);
   void    WriteMakefileUserDictObject(ROMEString& buffer);
   void    WriteMakefileDictionary(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers,const char* linkDefName = 0);
   void    WriteMakefileUserDictionary(ROMEString& buffer);
   void    WriteMakefileCompileStatements(ROMEString& buffer,ROMEStrArray* sources);
   void    WriteMakefileAdditionalSourceFilesObjects(ROMEString& buffer);
   void    WriteMakefileAdditionalSourceFilesCompileStatments(ROMEString& buffer);
   void    WriteMakefileBuildRule(ROMEString& buffer,const char* builder);
   void    WriteRootCintCall(ROMEString& buffer);
   void    WriteUserMakeFile();
   void    WriteHTMLDoku();
   void    WriteHTMLStyle(ROMEString &buffer);
   void    WriteHTMLSteering(ROMEString &buffer,Int_t numSteer,Int_t numTask,const char* group);
   Bool_t  ReplaceHeader(const char* filename,const char* header,const char* content,Int_t nspace = 0, const char* str1 = 0, const char* str2 = 0);
   void    WriteHeader(ROMEString& buffer, const char* author, Bool_t overwrite);
   void    WriteDescription(ROMEString& buffer, const char* className, const char* description, Bool_t endmark);
   Bool_t  BackUpFile(const char* filename);
   Bool_t  accessFolder(ROMEString &fileBuffer, Int_t numFolder);
   Bool_t  RemoveFile(const char* filename, const char* str = 0);
   void    RemoveDepFiles(const char* str = 0);
   void    RelativeWindowsPath(ROMEString &path,const char *referencePath);

   // Visual Project Methods
   void    WriteVisualProjects(Int_t version);
   void    WriteVisualProjectSln(Int_t version,ROMEString& projectGUID);
   void    WriteVisualProjectProjSettings(ROMEXML *xml,Int_t version,ROMEString& projectGUID);
   void    WriteVisualProjectProjFiles(ROMEXML *xml,ROMEStrArray* files,const char* folderName,const char* filter);
   void    WriteVisualProjectProjUserSources(ROMEXML *xml);
   void    WriteVisualProjectProjWarningLevel(ROMEXML *xml,const char *level);
   void    WriteVisualProjectProjUserHeaders(ROMEXML *xml);

   // Type Utility Methods
   void    GetFormat(ROMEString *buf,const char *type);
   void    setValue(ROMEString *buf,const char *destination,const char *source,const char *type,Int_t version);
   Bool_t  isFloatingType(const char *type);
   Bool_t  isNumber(const char * str);
   Bool_t  isBoolType(const char *type);
   Bool_t  isIntType(const char *type);
   Bool_t  isFolder(const char *type);
   Bool_t  isTArrayType(const char *type);
   Bool_t  isTArrayType(TString &type) { return isTArrayType(type.Data()); };
   const char* TArray2StandardType(const char *type,ROMEString &standardType);
   const char* TArray2StandardType(TString &type,ROMEString &standardType) { return TArray2StandardType(type.Data(),standardType); };
   Bool_t  isRootClassType(const char *type) { TString tmp=type; return isRootClassType(tmp); };
   Bool_t  isRootClassType(TString &type) { ROMEString tmp=type; tmp.StripSpaces(); return tmp(0)=='T'; };
   Bool_t  isPointerType(const char *type) { TString tmp=type; return isPointerType(tmp); };
   Bool_t  isPointerType(TString &type) { ROMEString tmp = type; tmp.StripSpaces(); return tmp(tmp.Length()-1)=='*'; };
   ROMEString& convertType(const char *value,const char *oldType,const char *newType,ROMEString& stringBuffer);

   // General Utility Methods
   void    Usage();
   void    AnalyzeFileName(const char* file,ROMEString& pathOfFile,ROMEString& nameOfFile,ROMEString& extensionOfFile);
   Bool_t  WriteFile(const char* filename,const char* content,Int_t nspace = 0, Bool_t backup = false);
};

#endif   // ROMEBuilder_H
