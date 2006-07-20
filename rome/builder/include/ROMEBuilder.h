/********************************************************************
  ROMEBuilder.h, M. Schneebeli PSI

  $Id$

********************************************************************/

#ifndef ROMEBuilder_H
#define ROMEBuilder_H

#include "ROMEString.h"
#include "ROMEStrArray.h"
#include "ROMEStr2DArray.h"

class ROMEXML;
class ROMEConfigParameter;
class ROMEConfigParameterGroup;

const Int_t maxNumberOfPathObjectInterpreterCodes = 10;
const Int_t maxNumberOfInclude = 10;
const Int_t maxNumberOfValues = 50;
const Int_t maxNumberOfValueDimension = 3;
const Int_t maxNumberOfHistos = 50;
const Int_t maxNumberOfHistoTabs = 5;
const Int_t maxNumberOfTabHistos = 20;
const Int_t maxNumberOfTabObjects = 20;
const Int_t maxNumberOfMenus = 20;
const Int_t maxNumberOfMenuItems = 100;
const Int_t maxNumberOfThreadFunctions = 10;
const Int_t maxNumberOfThreadFunctionArguments = 10;
const Int_t maxNumberOfBranches = 20;
const Int_t maxNumberOfBanks = 50;
const Int_t maxNumberOfRootBranches = 20;
const Int_t maxNumberOfRootBranchFields = 20;
const Int_t maxNumberOfStructFields = 50;
const Int_t maxNumberOfSteering = 50;
const Int_t maxNumberOfSteeringField = 100;
const Int_t maxNumberOfMFWinLibFlags = 5;
const Int_t maxNumberOfMFUnixLibFlags = 5;
const Int_t maxNumberOfMFSourceFlags = 5;
const Int_t maxNumberOfAffiliations = 20;
const Int_t maxNumberOfHistoLines = 5;

const Ssiz_t kTStringResizeIncrement = 4096;
const char* const LINE_TITLE = "NoDayWithoutItsLine";

const char  valueCounter[] = {'i','j','k'};
const char* const ROMECommandLineOptions = ":a:i:b:D:q:v:ns:ng:m:r:e:o:docu:";
const char* const cloSeparator = ":";
#if defined( R__VISUAL_CPLUSPLUS )
const char* const kEqualSign = "=";
const char* const kFlagSign = "/";
const char* const kObjectSuffix = ".obj";
#else
const char* const kEqualSign =":=";
const char* const kFlagSign = "-";
const char* const kObjectSuffix = ".o";
#endif
const char* const kHeaderEndMark = "/////////////////////////////////////----///////////////////////////////////////";

extern ROMEXML* configXSD;

class ROMEBuilder
{
public:
   ROMEString    romeVersion;

protected:
   ROMEString    outDir;
   ROMEString    xmlFile;
   ROMEString    xsdFile;
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
   Bool_t        librome;
   Bool_t        pch;
   ROMEStrArray  flags;
   ROMEStrArray  affiliations;
   ROMEStr2DArray affiliationList;

   ROMEXML*      xml;

   ROMEString    shortCut;
   ROMEString    experimentName;

   Bool_t        readExperiment;
   Bool_t        readGlobalSteeringParameters;

   Bool_t        hasStructuredBank;

   // max numbers
   Int_t maxNumberOfTasks;
   Int_t maxNumberOfFolders;
   Int_t maxNumberOfTrees;
   Int_t maxNumberOfDAQ;
   Int_t maxNumberOfDB;
   Int_t maxNumberOfEvents;
   Int_t maxNumberOfRootTrees;
   Int_t maxNumberOfMFDictHeaders;
   Int_t maxNumberOfMFDictIncDirs;
   Int_t maxNumberOfMFWinLibs;
   Int_t maxNumberOfMFUnixLibs;
   Int_t maxNumberOfMFIncDirs;
   Int_t maxNumberOfMFPreDefs;
   Int_t maxNumberOfMFSources;
   Int_t maxNumberOfTabs;
   Int_t maxNumberOfNetFolders;

   ROMEString   *parent;
   Int_t         recursiveDepth;
   Int_t         recursiveSteerDepth;
   Int_t         actualSteerIndex;

   Int_t         fNumberOfInterpreterCodes;

   Int_t         recursiveTabDepth;
   Int_t         recursiveMenuDepth;

   ROMEStrArray *daqNameArray;
   ROMEStrArray *daqTypeArray;
   ROMEStrArray *daqDirArray;

// folders
   Int_t         numOfFolder;
   Int_t        *numOfValue;
   Int_t        *numOfFolderInclude;

   Int_t        *numOfFolderAffiliations;
   ROMEString  **folderAffiliation;
   Bool_t       *folderUsed;
   ROMEString   *folderName;
   ROMEString   *folderDescription;
   ROMEString   *folderShortDescription;
   ROMEString   *folderParentName;
   ROMEString   *folderTitle;
   ROMEString   *folderArray;
   ROMEString   *folderAuthor;
   ROMEString   *folderVersion;
   ROMEString  **folderInclude;
   Bool_t       *folderNet;
   Bool_t      **folderLocalFlag;
   Bool_t       *folderDataBase;
   Bool_t       *folderUserCode;
   Bool_t       *folderSupport;
   Bool_t       *folderNoReset;
   Bool_t        hasFolderUserCode;
   Bool_t        hasFolderGenerated;

   ROMEString  **valueName;
   ROMEString  **valueType;
   ROMEString  **valueInit;
   ROMEString  **valueComment;
   Int_t       **valueDimension;
   ROMEString ***valueArray;
   ROMEString  **valueArraySpecifier;
   ROMEString  **valueDBName;
   ROMEString  **valueDBPath;
   Bool_t      **valueNoBoundChech;
   Bool_t      **valueIsTObject;

// net folder
   Int_t         numOfNetFolder;
   ROMEString   *netFolderName;
   ROMEString   *netFolderTitle;
   ROMEString   *netFolderHost;
   ROMEString   *netFolderPort;
   ROMEString   *netFolderRoot;

// task
   Int_t         numOfTask;
   Int_t        *numOfHistos;
   Int_t        *numOfTaskInclude;
   ROMEString   *taskName;
   Int_t        *numOfTaskAffiliations;
   ROMEString  **taskAffiliation;
   Bool_t       *taskUsed;
   ROMEString   *taskEventID;
   ROMEString   *taskDescription;
   ROMEString   *taskShortDescription;
   ROMEString   *taskAuthor;
   ROMEString   *taskAuthorInstitute;
   ROMEString   *taskAuthorCollaboration;
   ROMEString   *taskAuthorEmail;
   ROMEString   *taskVersion;
   ROMEString  **taskInclude;
   Bool_t      **taskLocalFlag;
   ROMEString  **histoName;
   ROMEString  **histoTitle;
   ROMEString  **histoFolderName;
   ROMEString  **histoFolderTitle;
   ROMEString  **histoType;
   ROMEString  **histoArraySize;
   ROMEString  **histoArrayStartIndex;
   ROMEString  **histoXLabel;
   ROMEString  **histoYLabel;
   ROMEString  **histoZLabel;
   ROMEString  **histoXNbins;
   ROMEString  **histoXmin;
   ROMEString  **histoXmax;
   ROMEString  **histoYNbins;
   ROMEString  **histoYmin;
   ROMEString  **histoYmax;
   ROMEString  **histoZNbins;
   ROMEString  **histoZmin;
   ROMEString  **histoZmax;
   Int_t       **numOfHistoTabs;
   ROMEString ***histoTabName;
   ROMEString ***histoTabIndex;
   ROMEString ***histoTabArrayIndex;

// task hierarchy
   Int_t         numOfTaskHierarchy;
   ROMEString   *taskHierarchyName;
   Int_t        *taskHierarchyParentIndex;
   Int_t        *taskHierarchyClassIndex;
   Int_t        *taskHierarchyMultiplicity;
   Int_t        *taskHierarchyLevel;
   Int_t        *taskHierarchyObjectIndex;
   ROMEString   *taskHierarchySuffix;

// steering
   Int_t        *numOfSteering;
   Int_t       **numOfSteerFields;
   Int_t       **numOfSteerChildren;
   ROMEString  **steerName;
   ROMEString  **steerArraySize;
   Int_t       **steerParent;
   Int_t      ***steerChildren;
   Int_t       **numOfSteerAffiliations;
   ROMEString ***steerAffiliation;
   Bool_t      **steerUsed;
   ROMEString ***steerFieldName;
   ROMEString ***steerFieldType;
   ROMEString ***steerFieldArraySize;
   ROMEString ***steerFieldInit;
   ROMEString ***steerFieldComment;
   ROMEString ***steerFieldShortDescription;
   ROMEString ***steerFieldCLOption;
   ROMEString ***steerFieldCLDescription;
   Int_t      ***numOfSteerFieldAffiliations;
   ROMEString ****steerFieldAffiliation;
   Bool_t     ***steerFieldUsed;

   Int_t         numOfGSPInclude;
   ROMEString   *gspInclude;
   Bool_t       *gspLocalFlag;
   ROMEString    usedCLO;

// tab
   Int_t         numOfTab;
   ROMEString   *tabName;
   ROMEString   *tabSuffix;
   ROMEString   *tabTitle;
   Bool_t       *tabHistoDisplay;
   Int_t        *numOfTabAffiliations;
   ROMEString  **tabAffiliation;
   Bool_t       *tabUsed;
   ROMEString   *tabDescription;
   ROMEString   *tabShortDescription;
   ROMEString   *tabAuthor;
   ROMEString   *tabAuthorInstitute;
   ROMEString   *tabAuthorCollaboration;
   ROMEString   *tabAuthorEmail;
   ROMEString   *tabVersion;
   ROMEString   *tabHeredity;
   Int_t        *tabHeredityIndex;
   Int_t        *tabParentIndex;
   Int_t        *tabNumOfChildren;
   Int_t        *numOfMenu;
   Int_t       **numOfMenuItem;
   ROMEString  **menuTitle;
   Int_t       **menuDepth;
   Int_t      ***menuItemChildMenuIndex;
   ROMEString ***menuItemEnumName;
   ROMEString ***menuItemTitle;
   Int_t        *numOfTabHistos;
   ROMEString  **tabHistoName;
   Int_t       **tabHistoIndex;
   Int_t       **tabHistoArrayIndexStart;
   Int_t       **tabHistoArrayIndexEnd;
   Int_t       **tabHistoTaskHierarchyIndex;
   Int_t       **tabHistoTaskIndex;
   Int_t       **tabHistoHistoIndex;
   Int_t        *tabHistoIndexMax;
   Int_t        *numOfTabObjects;
   ROMEString  **tabObjectName;
   ROMEString  **tabObjectTitle;
   ROMEString  **tabObject;
   ROMEString  **tabObjectType;
   Int_t       **tabObjectTaskHierarchyIndex;
   Int_t       **tabObjectTaskIndex;
   Int_t       **tabObjectHistoIndex;
   Int_t       **tabObjectTaskHierarchyNumber;
   ROMEStrArray  tabObjectSupportedHistos;

// tree
   Int_t         numOfTree;
   Int_t        *numOfBranch;
   ROMEString   *treeName;
   ROMEString   *treeTitle;
   ROMEString   *treeFileName;
   ROMEString   *treeDescription;
   ROMEString  **branchName;
   ROMEString  **branchFolder;
   ROMEString  **branchBufferSize;
   ROMEString  **branchSplitLevel;

// thread functions
   Int_t        *numOfThreadFunctions;
   Int_t       **numOfThreadFunctionArguments;
   ROMEString  **threadFunctionName;
   ROMEString ***threadFunctionArgument;

// daq
   Int_t         numOfDAQ;
   ROMEString   *daqName;
   Int_t        *numOfDAQAffiliations;
   ROMEString  **daqAffiliation;
   Bool_t       *daqUsed;

// database
   Int_t         numOfDB;
   ROMEString   *dbName;
   ROMEString   *dbDescription;

// midas
   Int_t         numOfEvent;
   Int_t        *numOfBank;
   Int_t       **numOfStructFields;
   ROMEString   *eventName;
   ROMEString   *eventID;
   ROMEString   *eventTriggerMask;
   ROMEString   *eventSamplingRate;
   ROMEString  **bankName;
   ROMEString  **bankType;
   Int_t       **bankArraySize;
   Int_t       **bankArrayStart;
   Int_t       **bankArrayDigit;
   ROMEString ***structFieldName;
   ROMEString ***structFieldType;
   ROMEString ***structFieldSize;
   ROMEString ***bankFieldArraySize;
   Bool_t        bankHasHeader;
   ROMEString    bankHeaderFolder;
   ROMEString    bankHeaderEventID;
   ROMEString    bankHeaderTriggerMask;
   ROMEString    bankHeaderSerialNumber;
   ROMEString    bankHeaderTimeStamp;

// root DAQ
   Int_t         numOfRootTree;
   Int_t        *numOfRootBranch;
   Int_t       **numOfRootBranchField;
   ROMEString   *rootTreeName;
   ROMEString  **rootBranchName;
   ROMEString  **rootBranchType;
   ROMEString  **rootBranchArraySize;
   ROMEString  **rootBranchClassName;
   ROMEString  **rootBranchClassVersion;
   ROMEString ***rootBranchFieldName;
   ROMEString ***rootBranchFieldType;
   ROMEString ***rootBranchFieldArraySize;

// user makefile
   Int_t         numOfMFDictHeaders;
   ROMEString   *mfDictHeaderName;
   Int_t        *numOfMFDictHeaderAffiliations;
   ROMEString  **mfDictHeaderAffiliation;
   Bool_t       *mfDictHeaderUsed;

   Int_t         numOfIncludeDirectories;
   Int_t         numOfMFDictIncDirs;
   ROMEString   *mfDictIncDir;

   Int_t         numOfMFWinLibs;
   ROMEString   *mfWinLibName;
   Int_t        *numOfMFWinLibFlags;
   ROMEString  **mfWinLibFlag;

   Int_t         numOfMFUnixLibs;
   ROMEString   *mfUnixLibName;
   Int_t        *numOfMFUnixLibFlags;
   ROMEString  **mfUnixLibFlag;

   Int_t         numOfMFIncDirs;
   ROMEString   *mfIncDir;

   Int_t         numOfMFPreDefs;
   ROMEString   *mfPreDefName;

   Int_t         numOfMFSources;
   ROMEString   *mfSourceFileName;
   ROMEString   *mfSourceFilePath;
   ROMEString   *mfHeaderFileName;
   ROMEString   *mfHeaderFilePath;
   Int_t        *numOfMFSourceFlags;
   ROMEString  **mfSourceFileFlag;
   Int_t        *numOfMFSourceAffiliations;
   ROMEString  **mfSourceFileAffiliation;
   Bool_t       *mfSourceFileUsed;

// main
   ROMEString    mainAuthor;
   ROMEString    mainInstitute;
   ROMEString    mainCollaboration;
   ROMEString    mainEmail;
   ROMEString    mainProgName;
   ROMEString    mainDescription;
   ROMEString    styleSheet;

// Makefile
   ROMEStrArray* dictionaryNames;
   ROMEStrArray* dictionaryOutputs;
   ROMEStrArray* dictionaryDependencies;
   ROMEStrArray* dictionaryCommands;
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
   ROMEStrArray* daqLibraries;
   ROMEStrArray* daqFlags;

   ROMEStrArray* precompiledHeaders;
   ROMEStrArray* precompiledIncludeHeaders;

// Configuration
   ROMEConfigParameterGroup* mainParGroup;
   int maxConfigParameterHierarchyLevel;

public:
   ROMEBuilder() { };
   ~ROMEBuilder();

   Bool_t  StartBuilder();
   Bool_t  ReadCommandLineParameters(Int_t argc, char *argv[]);
   Bool_t  CheckFileAndPath();
   Bool_t  GetNoLink() { return noLink; }
   Bool_t  GetMakeOutput() { return makeOutput; }

private:
   // XML Read Methods
   Bool_t  AllocateMemorySpace();
   Bool_t  ReadXMLDefinitionFile();
   Bool_t  ReadXMLFolder();
   Bool_t  ReadXMLNetFolder();
   Bool_t  ReadXMLTask();
   Bool_t  ReadXMLTab();
   Bool_t  ReadXMLMenu(Int_t currentNumberOfTabs);
   Bool_t  ReadXMLTree();
   Bool_t  ReadXMLDAQ();
   Bool_t  ReadXMLDB();
   Bool_t  ReadXMLMidasBanks();
   Bool_t  ReadXMLRootDAQ();
   Bool_t  ReadXMLSteering(Int_t iTask);
   Bool_t  ReadXMLUserMakefile();
   void    ParseSVNKeyword(ROMEString& str);

   // Code Write Methods
   Bool_t  WriteFolderCpp();
   Bool_t  WriteFolderH();
   Bool_t  WriteAllFoldersH();
   Bool_t  WriteSteeringClass(ROMEString& buffer,Int_t numOfTaskSteer,Int_t numTask,Int_t tab);
   Bool_t  WriteSteeringReadParameters(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,ROMEString& steerPointer) ;
   Bool_t  WriteSteeringParameterUsage(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,ROMEString& steerPointer) ;
   Int_t   WriteSteeringInterpreterCode(ROMEString &buffer,Int_t codeNumber,Int_t numSteer,Int_t numTask,ROMEString& path1,ROMEString& path2,Int_t tab);
   Int_t   WriteSteeringInterpreterValue(ROMEString &buffer,const char* type,Int_t codeNumber,Int_t numSteer,Int_t numTask,ROMEString& steerPointer,Int_t tab);
   void    WriteObjectInterpreterValue(ROMEString &buffer,const char* type,const char* fctName);
   void    WriteReadDataBaseFolder(ROMEString &buffer,Int_t numFolder,Int_t type);
   void    WriteFolderGetterInclude(ROMEString &buffer,Int_t numFolder);
   void    WriteFolderGetterSource(ROMEString &buffer,Int_t numFolder);
   Bool_t  WriteTaskCpp();
   Bool_t  WriteBaseTaskCpp();
   Bool_t  WriteTaskH();
   Bool_t  WriteBaseTaskH();
   Bool_t  WriteTabCpp();
   Bool_t  WriteBaseTabCpp();
   Bool_t  WriteTabH();
   Bool_t  WriteBaseTabH();
   Bool_t  WriteSteering(Int_t iTask);
   Bool_t  WriteAnalyzerCpp();
   Bool_t  WriteAnalyzer2Cpp();
   Bool_t  WriteAnalyzer3Cpp();
   Bool_t  WriteAnalyzerH();
   Bool_t  WriteWindowCpp();
   Bool_t  WriteWindow2Cpp();
   Bool_t  WriteWindowH();
   Bool_t  AddTab(ROMEString& buffer,Int_t& i);
   Bool_t  AddMenuItems(ROMEString& buffer,Int_t i,Int_t j,Int_t iHeredity,Int_t jHeredity,Int_t jOffset);
   Bool_t  WriteConfigToFormCpp();
   Bool_t  WriteConfigToFormH();
   Bool_t  WriteConfigToFormSubMethods(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,ROMEString tabPointer,ROMEString configPointer,int level);
   Bool_t  WriteConfigToFormSave(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,ROMEString pointer,ROMEString tabPointer,ROMEString configPointer);
   Bool_t  WriteConfigCpp();
   Bool_t  WriteConfigH();
   Bool_t  AddConfigParameters();
   Bool_t  AddTaskConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex);
   Bool_t  AddTabConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex);
   Bool_t  AddSteeringConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t numSteer,Int_t numTask,ROMEString steerPointer,ROMEString taskPointer);
   Bool_t  CheckConfigParameters(ROMEConfigParameterGroup *parGroup);
   Bool_t  WriteConfigClass(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab);
   Bool_t  WriteConfigRead(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString className,ROMEString pointer,ROMEString indexes);
   Bool_t  WriteConfigCheckModified(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString className,ROMEString pointer,ROMEString indexes);
   Bool_t  WriteConfigSet(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString pointer);
   Bool_t  WriteConfigWrite(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString pointer);
   Bool_t  WriteMidasDAQCpp();
   Bool_t  WriteMidasDAQH();
   Bool_t  WriteRomeDAQCpp();
   Bool_t  WriteRomeDAQH();
   Bool_t  WriteRootDAQCpp();
   Bool_t  WriteRootDAQH();
   Bool_t  WriteRootDAQClassesH();
   Bool_t  WriteRootDAQClassH(Int_t iTree,Int_t iBranch);
   Bool_t  WriteDAQCpp();
   Bool_t  WriteDAQH();
   Bool_t  WriteDBCpp();
   Bool_t  WriteDBH();
   Bool_t  WriteEventLoopCpp();
   Bool_t  WriteEventLoopH();
   Bool_t  WriteMain();
   Bool_t  WriteReadTreesC();
   Bool_t  WriteVersionH();
   Bool_t  WriteLinkDefHs();
   Bool_t  WriteLinkDefH(ROMEStrArray* headers, ROMEStrArray* ldsuffix, const char* filename);
   Bool_t  WritePrecompiledHeaders();

   // Makefile Methods
   void    AddIncludeDirectories();
   void    AddPrecompiledHeaders();
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
   void    AddDAQLibraries();
   void    AddDAQFlags();
   void    WriteMakefile();
   void    WriteMakefileHeader(ROMEString& buffer);
   void    WriteMakefileLibsAndFlags(ROMEString& buffer);
   void    WriteMakefileIncludes(ROMEString& buffer);
   void    GetIncludeDirString(ROMEString& buffer,const char* separator,const char* flagSign);
   void    WriteMakefileDictIncludes(ROMEString& buffer);
   void    WriteMakefileObjects(ROMEString& buffer,ROMEStrArray* sources);
   void    WriteMakefileDependFiles(ROMEString& buffer,ROMEStrArray* sources);
   void    WriteMakefileUserDictObject(ROMEString& buffer);
   void    WriteMakefileUserDictDependFiles(ROMEString& buffer);
   void    WriteMakefileDictionary(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers,const char* linkDefName = 0);
   void    WriteMakefileDictionaryList(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers);
   void    GetDictHeaderString(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers,const char* separator);
   void    WriteMakefileDictDummyCpp(const char* dictionaryName);
   void    WriteMakefileUserDictionary(ROMEString& buffer);
   void    WriteMakefileUserDictionaryList(ROMEString& buffer);
   void    GetUserDictIncludeDirString(ROMEString& buffer,const char* separator);
   void    GetUserDictHeaderString(ROMEString& buffer,const char* separator);
   void    WriteMakefileCompileStatements(ROMEString& buffer,ROMEStrArray* sources,const char* flag = 0);
   void    WriteMakefileAdditionalSourceFilesObjects(ROMEString& buffer);
   void    WriteMakefileAdditionalSourceDependFiles(ROMEString& buffer);
   void    WriteMakefileAdditionalSourceFilesCompileStatments(ROMEString& buffer,const char* flag = 0);
   void    WriteMakefileBuildRule(ROMEString& buffer,const char* builder);
   void    WriteRootCintCall(ROMEString& buffer);
   void    WriteUserMakeFile();
   void    WriteMakefilePrecompiledHeaders(ROMEString& buffer);
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
   void    WriteVisualProjectProjFileDictCreators(ROMEXML *xml);
   void    WriteVisualProjectProjFileROMEBuilder(ROMEXML *xml);
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
   Bool_t  isPointerType(const char *type) { TString tmp=type; return isPointerType(tmp); };
   Bool_t  isPointerType(TString &type) { ROMEString tmp = type; tmp.StripSpaces(); return tmp(tmp.Length()-1)=='*'; };
   ROMEString& convertType(const char *value,const char *oldType,const char *newType,ROMEString& stringBuffer);

   // General Utility Methods
   void    Usage();
   void    AnalyzeFileName(const char* file,ROMEString& pathOfFile,ROMEString& nameOfFile,ROMEString& extensionOfFile);
   Bool_t  WriteFile(const char* filename,const char* content,Int_t nspace = 0, Bool_t backup = false);
   Bool_t  CopyFile(const char* oldFileName,const char* newFileName);

   void* AllocateInt(Int_t x1, Int_t x2=0, Int_t x3=0, Int_t x4=0, Int_t x5=0, Int_t x6=0);
   void* AllocateBool(Int_t x1, Int_t x2=0, Int_t x3=0, Int_t x4=0, Int_t x5=0, Int_t x6=0);
   void* AllocateROMEString(Int_t x1, Int_t x2=0, Int_t x3=0, Int_t x4=0, Int_t x5=0, Int_t x6=0);

   ROMEString& ProcessCommentCPP(ROMEString& org, ROMEString& result);
   ROMEString& ProcessCommentHTML(ROMEString& org, ROMEString& result);
   ROMEString& ProcessCommentString(ROMEString& org, ROMEString& result);
};

#endif   // ROMEBuilder_H