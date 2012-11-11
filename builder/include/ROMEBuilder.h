/********************************************************************
  ROMEBuilder.h, M. Schneebeli PSI

  $Id$

********************************************************************/

#ifndef ROMEBuilder_H
#define ROMEBuilder_H

#include <RVersion.h>
#include "ROMEXML.h"
#include "ROMEString.h"
#include "ROMEStrArray.h"
#include "ROMEStr2DArray.h"

class TArrayI;
class ROMEConfigParameter;
class ROMEConfigParameterGroup;

const Int_t maxNumberOfPathObjectInterpreterCodes = 10;
const Int_t maxNumberOfInclude = 50;
const Int_t maxNumberOfValueDimension = 3;
const Int_t maxNumberOfHistoSingleObjectTabs = 20;
const Int_t maxNumberOfGraphSingleObjectTabs = 20;
const Int_t maxNumberOfTabSingleObjects = 40;
const Int_t maxNumberOfTabObjectDisplays = 20;
const Int_t maxNumberOfTabObjectDisplayObjectTypes = 20;
const Int_t maxNumberOfTabObjectDisplayObjects = 20;
const Int_t maxNumberOfMenus = 20;
const Int_t maxNumberOfMenuItems = 100;
const Int_t maxNumberOfThreadFunctions = 10;
const Int_t maxNumberOfThreadFunctionArguments = 10;
const Int_t maxNumberOfRunHeaders = 50;
const Int_t maxNumberOfBanks = 50;
const Int_t maxNumberOfRootBranches = 20;
const Int_t maxNumberOfRootBranchFields = 20;
const Int_t maxNumberOfStructFields = 50;
const Int_t maxNumberOfMFWinLibFlags = 10;
const Int_t maxNumberOfMFUnixLibFlags = 10;
const Int_t maxNumberOfMFSourceFlags = 10;
const Int_t maxNumberOfArguments = 40;
const Int_t maxNumberOfClassesInDictionary = 15;

const Ssiz_t kTStringResizeIncrement = 1<<15;
const char* const LINE_TITLE = "NoDayWithoutItsLine";

const char  valueCounter[] = {'i','j','k'};
const char* const ROMECommandLineOptions = ":i:r:e:m:p:o:b:D:q:v:ng:c:nc:ns:docu:h:I:R:rh:pi:po:ph";
const char* const cloSeparator = ":";
#if defined( R__VISUAL_CPLUSPLUS )
const char* const kEqualSign = "=";
const char* const kFlagSign = "/";
const char* const kObjectSuffix = ".obj";
const char* const kSharedObjectSuffix = ".dll";
#else
const char* const kEqualSign =":=";
const char* const kFlagSign = "-";
const char* const kObjectSuffix = ".o";
#if defined( R__MACOSX )
const char* const kSharedObjectSuffix = ".dylib";
#else
const char* const kSharedObjectSuffix = ".so";
#endif
#endif
const char* const kHeaderEndMark = "/////////////////////////////////////----///////////////////////////////////////";
const char* const kMethodLine = "//______________________________________________________________________________\n";

extern ROMEXML* configXSD;

class ROMEBuilder
{
public:
   ROMEString    romeVersion;
   int           romeRevisionCode;
   bool          romeStable;
   ROMEString    makeFlag;

protected:
   ROMEStrArray *histoParameters;
   ROMEStrArray *histoParameterTypes;
   ROMEStrArray *histoParameterWidgetTypes;
   ROMEStrArray *graphParameters;
   ROMEStrArray *graphParameterTypes;
   ROMEStrArray *graphParameterWidgetTypes;

   ROMEString    outDir;
   ROMEString    xmlFile;
   Bool_t        makeOutput;
   Bool_t        noLink;
   Bool_t        midas;
   Bool_t        xz;
   Bool_t        bzip2;
   Bool_t        orca;
   Bool_t        sql;
   Bool_t        mysql;
   Bool_t        pgsql;
   Bool_t        sqlite;
   Bool_t        sqlite3;
   Bool_t        noVP;
   Bool_t        librome;
   Bool_t        dynamicLink;
   Bool_t        pch;
   Bool_t        minRebuild;
   Bool_t        quietMake;
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
   Int_t maxNumberOfHistos;
   Int_t maxNumberOfGraphs;
   Int_t maxNumberOfFolders;
   Int_t maxNumberOfValues;
   Int_t maxNumberOfTrees;
   Int_t maxNumberOfBranches;
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
   Int_t maxNumberOfSteering;
   Int_t maxNumberOfSteeringField;
   Int_t maxNumberOfAffiliations;
   Int_t maxNumberOfAuthors;

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
   ROMEString   *folderInheritName;
   ROMEString   *folderTitle;
   ROMEString   *folderArray;
   Int_t        *numOfFolderAuthors;
   ROMEString  **folderAuthor;
   ROMEString  **folderAuthorInstitute;
   ROMEString  **folderAuthorCollaboration;
   ROMEString  **folderAuthorEmail;
   ROMEString   *folderVersion;
   ROMEString  **folderInclude;
   Bool_t      **folderLocalFlag;
   Bool_t       *folderDataBase;
   Bool_t       *folderIgnoreTObjectStreamer;
   Bool_t       *folderUserCode;
   Bool_t       *folderSupport;
   Bool_t       *folderNoReset;
   Bool_t       *folderNoResetModified;
   Bool_t       *folderHasClassField;
   Bool_t        hasFolderUserCode;
   Bool_t        hasFolderGenerated;
   Bool_t        hasSupportFolderGenerated;
   Bool_t        hasTaskGenerated;

   ROMEString  **valueName;
   ROMEString  **valueType;
   ROMEString  **valueInit;
   ROMEString  **valueComment;
   Int_t       **valueDimension;
   ROMEString ***valueArray;
   ROMEString  **valueArraySpecifier;
   ROMEString  **valueDBName;
   ROMEString  **valueDBPath;
   ROMEString  **valueDBIf;
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
   ROMEString   *taskName;
   Int_t        *numOfTaskAffiliations;
   ROMEString  **taskAffiliation;
   Bool_t       *taskUsed;
   ROMEString   *taskEventID;
   ROMEString   *taskDescription;
   ROMEString   *taskShortDescription;
   ROMEString   *taskUsage;
   ROMEString   *taskStatus;
   ROMEString   *taskToDo;
   ROMEString   *taskKnownProblems;
   Int_t        *numOfTaskAuthors;
   ROMEString  **taskAuthor;
   ROMEString  **taskAuthorInstitute;
   ROMEString  **taskAuthorCollaboration;
   ROMEString  **taskAuthorEmail;
   ROMEString   *taskVersion;
   ROMEString   *taskDependence;
   Int_t        *numOfTaskInclude;
   ROMEString  **taskInclude;
   Bool_t      **taskLocalFlag;
   Int_t        *numOfTaskAccessedFolder;
   Int_t       **taskAccessedFolder;
   Int_t        *numOfTaskConnectedFrom;
   ROMEString  **taskConnectedFrom;
   Int_t        *numOfHistos;
   ROMEString  **histoComment;
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
   ROMEString  **histoOption;
   Int_t       **numOfHistoSingleObjectTabs;
   ROMEString ***histoSingleObjectTabName;
   ROMEString ***histoSingleObjectTabIndex;
   ROMEString ***histoSingleObjectTabArrayIndex;
   Int_t        *numOfGraphs;
   ROMEString  **graphName;
   ROMEString  **graphTitle;
   ROMEString  **graphFolderName;
   ROMEString  **graphFolderTitle;
   ROMEString  **graphType;
   ROMEString  **graphArraySize;
   ROMEString  **graphArrayStartIndex;
   ROMEString  **graphXLabel;
   ROMEString  **graphYLabel;
   ROMEString  **graphZLabel;
   ROMEString  **graphXmin;
   ROMEString  **graphXmax;
   ROMEString  **graphYmin;
   ROMEString  **graphYmax;
   ROMEString  **graphZmin;
   ROMEString  **graphZmax;
   Int_t       **numOfGraphSingleObjectTabs;
   ROMEString ***graphSingleObjectTabName;
   ROMEString ***graphSingleObjectTabIndex;
   ROMEString ***graphSingleObjectTabArrayIndex;

// task hierarchy
   Int_t         numOfTaskHierarchy;
   ROMEString   *taskHierarchyName;
   Int_t        *taskHierarchyParentIndex;
   Int_t        *taskHierarchyClassIndex;
   Int_t        *taskHierarchyMultiplicity;
   Int_t        *taskHierarchyLevel;
   Int_t        *taskHierarchyObjectIndex;
   ROMEString   *taskHierarchySuffix;
   Int_t        *numOfTaskHierarchyConnectedFrom;
   ROMEString  **taskHierarchyConnectedFrom;

// task hierarchyWOAffiliation
   Int_t         numOfTaskHierarchyWOAffiliation;
   ROMEString   *taskHierarchyNameWOAffiliation;
   ROMEString   *taskHierarchySuffixWOAffiliation;

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
   ROMEString ***steerFieldFormat;
   ROMEString ***steerFieldArraySize;
   ROMEString ***steerFieldInit;
   ROMEString ***steerFieldComment;
   ROMEString ***steerFieldShortDescription;
   ROMEString ***steerFieldCLOption;
   ROMEString ***steerFieldCLDescription;
   Int_t      ***numOfSteerFieldAffiliations;
   ROMEString ****steerFieldAffiliation;
   Bool_t     ***steerFieldUsed;
   Bool_t     ***steerFieldHotLink;
   Bool_t        haveSteerFieldHotLinks;

   Int_t         numOfGSPInclude;
   ROMEString   *gspInclude;
   Bool_t       *gspLocalFlag;
   ROMEString    usedCLO;

// tab
   Int_t         numOfTab;
   ROMEString   *tabName;
   ROMEString   *tabSuffix;
   ROMEString   *tabTitle;
   Bool_t       *tabObjectDisplay;
   Int_t        *numOfTabAffiliations;
   ROMEString  **tabAffiliation;
   Bool_t       *tabUsed;
   Int_t        *tabUsedIndex;
   ROMEString   *tabDescription;
   ROMEString   *tabShortDescription;
   ROMEString   *tabUsage;
   ROMEString   *tabStatus;
   ROMEString   *tabToDo;
   ROMEString   *tabKnownProblems;
   Int_t        *numOfTabAuthors;
   ROMEString  **tabAuthor;
   ROMEString  **tabAuthorInstitute;
   ROMEString  **tabAuthorCollaboration;
   ROMEString  **tabAuthorEmail;
   ROMEString   *tabVersion;
   ROMEString   *tabDependence;
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
   Int_t        *numOfTabSingleObjects;
   ROMEString  **tabSingleObjectName;
   Int_t       **tabSingleObjectIndex;
   Int_t       **tabSingleObjectArrayIndexStart;
   Int_t       **tabSingleObjectArrayIndexEnd;
   Int_t       **tabSingleObjectTaskHierarchyIndex;
   Int_t       **tabSingleObjectTaskIndex;
   Int_t       **tabSingleObjectObjectIndex;
   ROMEString  **tabSingleObjectType;
   Int_t        *tabSingleObjectIndexMax;
   Int_t        *numOfTabObjectDisplays;
   ROMEString  **tabObjectDisplayName;
   ROMEString  **tabObjectDisplayTitle;
   Int_t       **numOfTabObjectDisplayObjects;
   ROMEString  ***tabObjectDisplayObject;
   ROMEString  ***tabObjectDisplayType;
   Int_t       **tabObjectDisplayTaskHierarchyIndex;
   Int_t       **tabObjectDisplayTaskIndex;
   Int_t       ***tabObjectDisplayObjectIndex;
   Int_t       **tabObjectDisplayTaskHierarchyNumber;
   ROMEStrArray  tabObjectDisplaySupportedObjects;
   Int_t       ***tabObjectDisplayObjectTypeIndex;
   Int_t        *numOfTabObjectDisplayObjectTypes;
   ROMEString  **tabObjectDisplayObjectType;

   Bool_t        hasDependenceCheck;

// tree
   Int_t         numOfTree;
   Int_t        *numOfBranch;
   Int_t        *numOfRunHeader;
   ROMEString   *treeName;
   ROMEString   *treeTitle;
   ROMEString   *treeFileName;
   ROMEString   *treeDescription;
   ROMEString  **branchName;
   ROMEString  **branchFolder;
   ROMEString  **branchBufferSize;
   ROMEString  **branchSplitLevel;
   Bool_t      **branchSplitLevelSpecified;
   ROMEString  **runHeaderName;
   ROMEString  **runHeaderFolder;
   Int_t       **runHeaderFolderIndex;

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
   ROMEString    maxEventSize;
   Int_t         numOfEvent;
   Int_t        *numOfBank;
   Int_t       **numOfStructFields;
   ROMEString   *eventName;
   ROMEString   *eventID;
   ROMEString   *eventTriggerMask;
   ROMEString   *eventSamplingRate;
   Int_t         maxEventID;
   ROMEString  **bankName;
   ROMEString  **bankAlias;
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
   ROMEString   *mfDictHeaderLinkDefSuffix;
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
   ROMEString   *mfSourceFileObjPath;
   ROMEString   *mfHeaderFileName;
   ROMEString   *mfHeaderFilePath;
   Int_t        *numOfMFSourceFlags;
   ROMEString  **mfSourceFileFlag;
   Int_t        *numOfMFSourceAffiliations;
   ROMEString  **mfSourceFileAffiliation;
   Bool_t       *mfSourceFileUsed;
   ROMEStrArray  objDirList;

// main
   Int_t        numOfMainAuthors;
   ROMEString   *mainAuthor;
   ROMEString   *mainInstitute;
   ROMEString   *mainCollaboration;
   ROMEString   *mainEmail;
   ROMEString    mainProgName;
   ROMEString    mainProgNameExtension;
   ROMEString    mainDefinitionVersion;
   ROMEString    mainDescription;
   ROMEString    styleSheet;
   ROMEString    baseURL;
   Int_t         dictionaryType;

// Makefile
   ROMEStrArray* dictionaryNames;
   ROMEStrArray* dictionaryOutputs;
   ROMEStrArray* dictionaryDependencies;
   ROMEStrArray* dictionaryCommands;
   ROMEStrArray* dictionaryHeaders;
   ROMEStrArray* dictionarySources;
   ROMEStrArray* dictionaryLinkDefSuffix;
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
   ROMEStrArray* generatedSupportFolderDictHeaders;
   ROMEStrArray* generatedSupportFolderLinkDefSuffix;
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
   ROMEStrArray* daqLinkDefSuffix;
   ROMEStrArray* databaseHeaders;
   ROMEStrArray* databaseSources;
   ROMEStrArray* databaseLinkDefSuffix;
   ROMEStrArray* additionalDictHeaders;
   ROMEStrArray* additionalDictLinkDefSuffix;
   ROMEStrArray* rootLibraries;
   ROMEStrArray* mysqlLibraries;
   ROMEStrArray* daqLibraries;
   ROMEStrArray* daqFlags;

   ROMEStrArray* precompiledHeaders;
   ROMEStrArray* precompiledIncludeHeaders;

// Configuration
   ROMEConfigParameterGroup* mainParGroup;
   int maxConfigParameterHierarchyLevel;

private:
   ROMEBuilder(const ROMEBuilder &builder); // not implemented
   ROMEBuilder &operator=(const ROMEBuilder &rhs); // not implemented

public:
   ROMEBuilder();
   ~ROMEBuilder();

   Bool_t  StartBuilder();
   Bool_t  ReadCommandLineParameters(Int_t argc, const char *argv[]);
   Bool_t  CheckFileAndPath();
   Bool_t  GetNoLink() { return noLink; }
   Bool_t  GetMakeOutput() { return makeOutput; }
   ROMEString* GetMakeFlag() { return &makeFlag; }

private:
   // XML Read Methods
   Bool_t  CountXMLOccurrenceFolder(const ROMEXML *xmlfile, const char* root);
   Bool_t  CountXMLOccurrenceTaskHierarchy(const ROMEXML *xmlfile, const char* root, Int_t &nTaskHierarchy);
   Bool_t  CountXMLOccurrenceTask(const ROMEXML *xmlfile, const char* root, Int_t &nTask);
   Bool_t  CountXMLOccurrenceTab(const ROMEXML *xmlfile, const char* root);
   Bool_t  CountXMLOccurrenceTree(const ROMEXML *xmlfile, const char* root);
   Bool_t  CountXMLOccurrenceGSP(const ROMEXML *xmlfile);
   Bool_t  CountXMLOccurrenceSteering(const ROMEXML *xmlfile, const char* root, Int_t &nSteering);
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
   Bool_t  ReadXMLSteering(Int_t iTask,Bool_t gsp);
   Bool_t  ReadXMLUserMakefile();
   void    ParseSVNKeyword(ROMEString& str);
   void    FormatText(ROMEString& str, Bool_t stripSpace, const char* invalidCharacters = 0);

   // Code Write Methods
   Bool_t  WriteFolderCpp();
   Bool_t  WriteFolderH();
   Bool_t  WriteAllFoldersH();
   Bool_t  WriteSteeringClass(ROMEString& buffer,Int_t numOfTaskSteer,Int_t numTask,Int_t tab);
   Bool_t  WriteSteeringReadParameters(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,
                                       ROMEString& steerPointer) ;
   Bool_t  WriteSteeringParameterUsage(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,
                                       ROMEString& steerPointer) ;
   Int_t   WriteSteeringInterpreterCode(ROMEString &buffer,Int_t codeNumber,Int_t numSteer,Int_t numTask,
                                        ROMEString& path1,ROMEString& path2,Int_t tab);
   Int_t   WriteSteeringInterpreterValue(ROMEString &buffer,const char* type,Int_t codeNumber,Int_t numSteer,
                                         Int_t numTask,ROMEString& steerPointer,Int_t tab);
   void    WriteObjectInterpreterValue(ROMEString &buffer,const char* type,const char* fctName);
   void    WriteReadDataBaseFolder(ROMEString &buffer,Int_t numFolder,Int_t type);
   void    WriteReadDataBaseFields(ROMEString &buffer,Int_t numFolder,Int_t type, ROMEString &tmpFolderName);
   void    WriteFolderGetterInclude(ROMEString &buffer,Int_t numFolder);
   void    WriteFolderSetterInclude(ROMEString &buffer,Int_t numFolder);
   void    WriteFolderGetterSource(ROMEString &buffer,Int_t numFolder);
   void    WriteFolderSetterSource(ROMEString &buffer,Int_t numFolder);
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
   Bool_t  WriteAnalyzer4Cpp();
   Bool_t  WriteAnalyzerH();
   Bool_t  WriteFillObjectStorageObject(ROMEString &buffer,const char *objectPointer,const char *objectStoragePointer,
                                        const char *objectActivePointer,bool bypass);
   Bool_t  WriteWindowCpp();
   Bool_t  WriteWindow2Cpp();
   Bool_t  WriteWindowH();
   Bool_t  WriteDBAccessCpp();
   Bool_t  WriteDBAccessH();
   Bool_t  AddMenuItems(ROMEString& buffer,Int_t i,Int_t j,Int_t iHeredity,Int_t jHeredity,Int_t jOffset);
   Bool_t  WriteConfigToFormCpp();
   Bool_t  WriteConfigToFormH();
   Bool_t  WriteConfigToFormSubMethods(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,ROMEString tabPointer,
                                       ROMEString configPointer,int level,int tab);
   Bool_t  WriteConfigToFormSave(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,ROMEString pointer,
                                 ROMEString tabPointer,ROMEString configPointer,int level,int tab,ROMEString indexes);
   Bool_t  WriteConfigCpp();
   Bool_t  WriteConfig2Cpp();
   Bool_t  WriteConfig3Cpp();
   Bool_t  WriteConfig4Cpp();
   Bool_t  WriteConfigH();
   Bool_t  AddConfigParameters();
   Bool_t  AddConfigParametersFolder();
   Bool_t  AddTaskConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex);
   Bool_t  AddTabConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex);
   Bool_t  AddSteeringConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t numSteer,Int_t numTask,
                                       ROMEString steerPointer,ROMEString taskPointer);
   Bool_t  CheckConfigParameters(ROMEConfigParameterGroup *parGroup);
   Bool_t  WriteConfigClass(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab);
   Bool_t  WriteConfigRead(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,
                           ROMEString className,ROMEString pointer,ROMEString indexes,Int_t* iSub);
   Bool_t  WriteConfigCheckModified(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,
                                    ROMEString className,ROMEString pointer,ROMEString indexes,Int_t* iSub);
   Bool_t  WriteConfigSet(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,
                          ROMEString pointer,Int_t* iSub);
   Bool_t  WriteConfigWrite(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,
                            ROMEString pointer,Int_t* iSub);
   Bool_t  WriteMidasDAQCpp();
   Bool_t  WriteMidasDAQH();
   Bool_t  WriteRomeDAQCpp();
   Bool_t  WriteRomeDAQH();
   Bool_t  WriteRootDAQCpp();
   Bool_t  WriteRootDAQH();
   Bool_t  WriteRootDAQClassesH();
   Bool_t  WriteRootDAQClassH(Int_t iTree,Int_t iBranch);
   Bool_t  WriteNetFolderServerCpp();
   Bool_t  WriteNetFolderServerH();
   Bool_t  WriteUpdateObjectsObject(ROMEString &buffer,const char *objectPointer,const char *objectStoragePointer,
                                    bool bypass);
   Bool_t  WriteDAQCpp();
   Bool_t  WriteDAQH();
   Bool_t  WriteDBCpp();
   Bool_t  WriteDBH();
   Bool_t  WriteEventLoopCpp();
   Bool_t  WriteEventLoopH();
   Bool_t  WriteMain();
   Bool_t  WriteReadTreesC();
   Bool_t  WriteDistillTreesC();
   Bool_t  WriteDOT();
   Bool_t  WriteVersionH();
   Bool_t  WriteLinkDefH(ROMEStrArray* headers, ROMEStrArray* ldsuffix, const char* dictionaryName);
   Bool_t  WriteUserLinkDefH();
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
   void    AddGeneratedSupportFolderDictHeaders();
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
   void    AddAdditionalDictHeaders();
   void    AddDictionaryHeaders();
   void    AddDictionarySources();
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
   void    WriteMakefileDictionary(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers,
                                   const char* linkDefName = 0);
   void    WriteMakefileDictionaryList(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers);
   void    GetDictHeaderString(ROMEString& buffer,ROMEStrArray* headers,const char* separator,Bool_t withoutPath=false,
                               Int_t iFile = 0);
   void    WriteMakefileDictDummyCpp(const char* dictionaryName);
   void    WriteMakefileUserDictionary(ROMEString& buffer);
   void    WriteMakefileUserDictionaryList(ROMEString& buffer);
   void    GetUserDictIncludeDirString(ROMEString& buffer,const char* separator);
   void    WriteMakefileCompileStatements(ROMEString& buffer,ROMEStrArray* sources, ROMEString *objdir = 0,
                                          const char* flag = 0, const char *dep = 0);
   void    WriteMakefileAdditionalSourceFilesObjects(ROMEString& buffer);
   void    WriteMakefileAdditionalSourceDependFiles(ROMEString& buffer);
   void    WriteMakefileAdditionalSourceFilesCompileStatments(ROMEString& buffer, const char* flag = 0,
                                                              const char *dep = 0);
   void    WriteMakefileBuildRule(ROMEString& buffer,const char* builder);
   void    WriteRootCintCall(ROMEString& buffer);
   void    WriteUserMakeFile();
   void    WriteMakefilePrecompiledHeaders(ROMEString& buffer);
   void    WriteHTMLDoku();
   void    WriteHTMLStyle(ROMEString &buffer);
   void    WriteHTMLSteering(ROMEString &buffer,Int_t numSteer,Int_t numTask,const char* group);
   Bool_t  ReplaceHeader(const char* filename,const char* header,const char* content,Int_t nspace = 0,
                         const char* str1 = 0, const char* str2 = 0,const char* condition = 0,
                         bool replaceWhenFound = true);
   Bool_t  ReplaceHeader(const char* filename,const char* header,const char* content,Int_t nspace, ROMEStrArray& arr1,
                         ROMEStrArray& arr2,ROMEStrArray& condition,TArrayI &replaceWhenFound);
   void    WriteHeader(ROMEString& buffer, Int_t nAuthor, const ROMEString* author, const ROMEString* email, Bool_t overwrite);
   void    WriteDescription(ROMEString& buffer, const char* className, const char* description, Bool_t endmark,
                            const char* header = 0);
   Bool_t  BackUpFile(const char* filename);
   Bool_t  accessFolder(const char* filename, Int_t numFolder, Bool_t includeUnused = kFALSE);
   Bool_t  accessFolderBuffer(ROMEString &fileBuffer, Int_t numFolder, const char* cachename, Bool_t includeUnused = kFALSE);
   Bool_t  accessHisto(const char* filename, Int_t numTask, Int_t numHisto);
   Bool_t  accessHistoBuffer(ROMEString &fileBuffer, Int_t numTask, Int_t numHisto, const char* cachename);
   Bool_t  accessGraph(const char* filename, Int_t numTask, Int_t numGraph);
   Bool_t  accessGraphBuffer(ROMEString &fileBuffer, Int_t numTask, Int_t numGraph, const char* cachename);
   void    UpdateAccessCache(ROMEString &fileBuffer, const char* filename);
   Bool_t  RemoveFile(const char* filename, const char* str = 0);
   void    RemoveDepFiles(const char* str = 0);
   void    RelativeWindowsPath(ROMEString &path,const char *referencePath);
   ROMEString& GetSteerPath(ROMEString& steerPath,int iTask,int iSteer,int iField,const char* seperator);
   Bool_t  FolderToBeGenerated(Int_t iFold) { return numOfValue[iFold] > 0 || folderInheritName[iFold].Length(); }

   // Visual Project Methods
   void    WriteVisualProjects(Int_t version);
   void    WriteVisualProjectSln(Int_t version,ROMEString& projectGUID);
   void    WriteVisualProjectProjSettings(ROMEXML *xml,Int_t version,ROMEString& projectGUID);
   void    WriteVisualProjectProjFiles(ROMEXML *xml,ROMEStrArray* files,const char* folderName,const char* filter);
   void    WriteVisualProjectProjFileDictCreators(ROMEXML *xml);
   void    WriteVisualProjectProjFileROMEBuilder(ROMEXML *xml);
   void    WriteVisualProjectProjUserSources(ROMEXML *xml);
   void    WriteVisualProjectProjFileConfiguration(ROMEXML *xml,const char *warningLevel,
                                                   const char* preCompiledHeaderFile);
   void    WriteVisualProjectProjUserHeaders(ROMEXML *xml);

   // Type Utility Methods
   void    setValue(ROMEString *buf,const char *destination,const char *source,const char *type,Int_t version);
   Bool_t  isFloatingType(const char *type);
   Bool_t  isNumber(const char * str);
   Bool_t  isBoolType(const char *type);
   Bool_t  isIntType(const char *type);
   Bool_t  isFolder(const char *type);
   Bool_t  isTArrayType(const char *type);
   Bool_t  isTArrayType(TString &type) { return isTArrayType(type.Data()); }
   const char* TArray2StandardType(const char *type,ROMEString &standardType);
   const char* TArray2StandardType(TString &type,ROMEString &standardType) {
      return TArray2StandardType(type.Data(),standardType); }
   Bool_t  isPointerType(const char *type) { TString tmp=type; return isPointerType(tmp); }
   Bool_t  isPointerType(TString &type) { ROMEString tmp = type; tmp.StripSpaces(); return tmp(tmp.Length()-1)=='*'; }
   ROMEString& convertType(const char *value,const char *oldType,const char *newType,ROMEString& stringBuffer);
   bool    toMidasODBType(ROMEString& type,ROMEString& midasODBType);

   // General Utility Methods
   void    Usage();
   void    AnalyzeFileName(const char* file,ROMEString& pathOfFile,ROMEString& nameOfFile,ROMEString& extensionOfFile);
   Bool_t  WriteFile(const char* filename,const char* content,Int_t nspace = 0, Bool_t backup = false);
   Bool_t  CopyFile(const char* oldFileName,const char* newFileName);

   template <class T> void* AllocateArray(T* p0, Int_t x1 = -1, Int_t x2 = -1, Int_t x3 = -1, Int_t x4 = -1,
                                          Int_t x5 = -1, Int_t x6 = -1);
   void* AllocateInt(Int_t x1, Int_t x2= -1, Int_t x3= -1, Int_t x4= -1, Int_t x5= -1, Int_t x6= -1);
   void* AllocateBool(Int_t x1, Int_t x2= -1, Int_t x3= -1, Int_t x4= -1, Int_t x5= -1, Int_t x6= -1);
   void* AllocateROMEString(Int_t x1, Int_t x2= -1, Int_t x3= -1, Int_t x4= -1, Int_t x5= -1, Int_t x6= -1);
   template <class T> void FreeArray(T* p0);
   template <class T> void FreeArray(T** p0);
   template <class T> void FreeArray(T*** p0);
   template <class T> void FreeArray(T**** p0);
   template <class T> void FreeArray(T***** p0);
   template <class T> void FreeArray(T****** p0);

   ROMEString& ProcessCommentCPP(ROMEString& org, ROMEString& result);
   ROMEString& ProcessCommentHTML(ROMEString& org, ROMEString& result);
   ROMEString& ProcessCommentString(ROMEString& org, ROMEString& result);
   ROMEString& ProcessAddressHTML(ROMEString& org, ROMEString& result);

   ROMEString& ParseDependences(ROMEString& org, ROMEString& result);
   Long64_t    SetCapacity(const char* filename, TString *str, Long64_t defaultSize = (1<<23) /* 8MB */);
};

#endif   // ROMEBuilder_H
