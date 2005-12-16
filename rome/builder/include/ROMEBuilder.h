/********************************************************************
  ROMEBuilder.h, M. Schneebeli PSI

  $Id$

********************************************************************/

#ifndef ROMEBuilder_H
#define ROMEBuilder_H

#include <ROMEXML.h>
#include <ROMEString.h>

const int bufferLength = 500000;
const int maxNumberOfTasks = 300;
const int maxNumberOfFolders = 200;
const int maxNumberOfTrees = 50;
const int maxNumberOfDAQ = 10;
const int maxNumberOfDB = 10;
const int maxNumberOfBranches = 20;
const int maxNumberOfBanks = 50;
const int maxNumberOfEvents = 5;
const int maxNumberOfSteering = 50;
const int maxNumberOfSteeringField = 100;
const int maxNumberOfValues = 50;
const int maxNumberOfValueDimension = 3;
const int maxNumberOfInclude = 10;
const int maxNumberOfHistos = 50;
const int maxNumberOfStructFields = 50;
const int maxNumberOfMFDictHeaders = 100;
const int maxNumberOfMFDictIncDirs = 20;
const int maxNumberOfMFWinLibs = 20;
const int maxNumberOfMFUnixLibs = 20;
const int maxNumberOfMFIncDirs = 20;
const int maxNumberOfMFPreDefs = 20;
const int maxNumberOfMFSources = 100;
const int maxNumberOfMFSourceFlags = 5;

const char valueCounter[maxNumberOfValueDimension] = {'i','j','k'};

const int maxNumberOfEventRequests = 5;
const int maxNumberOfPathObjectInterpreterCodes = 10;

const Ssiz_t kTStringResizeIncrement = 4096;

const char* const ROMECommandLineOptions = ":i:b:ns:m:r:e:docu:";
const char* const cloSeparator = ":";

class ROMEBuilder
{
protected:
   bool haveFortranTask;
public:
   ROMEString romeVersion;

   ROMEString outDir;
   ROMEString xmlFile;
   bool  makeOutput;
   bool  noLink;
   bool  midas;
   bool  orca;
   bool  sql;
   bool  mysql;
   bool  pgsql;
   bool  sqlite;
   bool  sqlite3;
   bool  noVP;
   ROMEStrArray flags;

protected:
   ROMEXML* xml;

   ROMEString shortCut;
   ROMEString experimentName;

   bool readExperiment;
   bool readAuthor;
   bool readFolders;
   bool readTasks;
   bool readTrees;
   bool readGlobalSteeringParameters;
   bool readMidasBanks;

   bool hasStructuredBank;

   ROMEString parent[maxNumberOfTasks];
   int recursiveDepth;
   int recursiveSteerDepth;
   int actualSteerIndex;

   int fNumberOfInterpreterCodes;

// folders
   int numOfFolder;
   int numOfValue[maxNumberOfFolders];
   int numOfFolderInclude[maxNumberOfFolders];

   ROMEString folderName[maxNumberOfFolders];
   ROMEString folderDescription[maxNumberOfFolders];
   ROMEString folderParentName[maxNumberOfFolders];
   ROMEString folderTitle[maxNumberOfFolders];
   ROMEString folderArray[maxNumberOfFolders];
   ROMEString folderAuthor[maxNumberOfFolders];
   ROMEString folderVersion[maxNumberOfFolders];
   ROMEString folderInclude[maxNumberOfFolders][maxNumberOfInclude];
   bool  folderLocalFlag[maxNumberOfFolders][maxNumberOfInclude];
   bool folderDataBase[maxNumberOfFolders];
   bool folderUserCode[maxNumberOfFolders];
   bool folderSupport[maxNumberOfFolders];
   bool folderNoReset[maxNumberOfFolders];

   ROMEString valueName[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueType[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueInit[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueComment[maxNumberOfFolders][maxNumberOfValues];
   int valueDimension[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueArray[maxNumberOfFolders][maxNumberOfValues][maxNumberOfValueDimension];
   ROMEString valueDBName[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueDBPath[maxNumberOfFolders][maxNumberOfValues];

// task

   int numOfTask;
   int numOfHistos[maxNumberOfTasks];
   int numOfTaskInclude[maxNumberOfTasks];
   ROMEString taskName[maxNumberOfTasks];
   ROMEString taskEventID[maxNumberOfTasks];
   ROMEString taskDescription[maxNumberOfTasks];
   bool taskFortran[maxNumberOfTasks];
   bool taskUserCode[maxNumberOfTasks];
   ROMEString taskAuthor[maxNumberOfTasks];
   ROMEString taskAuthorInstitute[maxNumberOfTasks];
   ROMEString taskAuthorCollaboration[maxNumberOfTasks];
   ROMEString taskAuthorEmail[maxNumberOfTasks];
   ROMEString taskVersion[maxNumberOfTasks];
   ROMEString taskInclude[maxNumberOfTasks][maxNumberOfInclude];
   bool  taskLocalFlag[maxNumberOfTasks][maxNumberOfInclude];
   ROMEString histoName[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoTitle[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoFolderName[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoFolderTitle[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoType[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoArraySize[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoArrayStartIndex[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoXLabel[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoYLabel[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoZLabel[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoXNbins[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoXmin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoXmax[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoYNbins[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoYmin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoYmax[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoZNbins[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoZmin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoZmax[maxNumberOfTasks][maxNumberOfHistos];

// task hierarchy

   int numOfTaskHierarchy;
   ROMEString taskHierarchyName[2*maxNumberOfTasks];
   int        taskHierarchyParentIndex[2*maxNumberOfTasks];
   int        taskHierarchyClassIndex[2*maxNumberOfTasks];

// steering

   int numOfSteering[maxNumberOfTasks+1];
   int numOfSteerFields[maxNumberOfTasks+1][maxNumberOfSteering];
   int numOfSteerChildren[maxNumberOfTasks+1][maxNumberOfSteering];
   ROMEString steerName[maxNumberOfTasks+1][maxNumberOfSteering];
   ROMEString steerArraySize[maxNumberOfTasks+1][maxNumberOfSteering];
   int        steerParent[maxNumberOfTasks+1][maxNumberOfSteering];
   int        steerChildren[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteering];
   ROMEString steerFieldName[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldType[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldArraySize[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldInit[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldComment[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldCLOption[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldCLDescription[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];

   ROMEString usedCLO;

// tree
   int numOfTree;
   int numOfBranch[maxNumberOfTrees];
   ROMEString treeName[maxNumberOfTrees];
   ROMEString treeTitle[maxNumberOfTrees];
   ROMEString treeFileName[maxNumberOfTrees];
   ROMEString branchName[maxNumberOfTrees][maxNumberOfBranches];
   ROMEString branchFolder[maxNumberOfTrees][maxNumberOfBranches];
   ROMEString branchBufferSize[maxNumberOfTrees][maxNumberOfBranches];
   ROMEString branchSplitLevel[maxNumberOfTrees][maxNumberOfBranches];

// daq
   int numOfDAQ;
   ROMEString daqName[maxNumberOfDAQ];

// database
   int numOfDB;
   ROMEString dbName[maxNumberOfDB];
   ROMEString dbDescription[maxNumberOfDB];

// midas
   int numOfEvent;
   int numOfBank[maxNumberOfEvents];
   int numOfStructFields[maxNumberOfEvents][maxNumberOfBanks];
   ROMEString eventName[maxNumberOfEvents];
   ROMEString eventID[maxNumberOfEvents];
   ROMEString eventTriggerMask[maxNumberOfEvents];
   ROMEString eventSamplingRate[maxNumberOfEvents];
   ROMEString bankName[maxNumberOfEvents][maxNumberOfBanks];
   ROMEString bankType[maxNumberOfEvents][maxNumberOfBanks];
   int bankArraySize[maxNumberOfEvents][maxNumberOfBanks];
   int bankArrayStart[maxNumberOfEvents][maxNumberOfBanks];
   int bankArrayDigit[maxNumberOfEvents][maxNumberOfBanks];
   ROMEString structFieldName[maxNumberOfEvents][maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString structFieldType[maxNumberOfEvents][maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString structFieldSize[maxNumberOfEvents][maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString bankFieldArraySize[maxNumberOfEvents][maxNumberOfBanks][maxNumberOfStructFields];
   bool  bankHasHeader;
   ROMEString bankHeaderFolder;
   ROMEString bankHeaderEventID;
   ROMEString bankHeaderTriggerMask;
   ROMEString bankHeaderSerialNumber;
   ROMEString bankHeaderTimeStamp;

// user makefile
   int numOfMFDictHeaders;
   ROMEString mfDictHeaderName[maxNumberOfMFDictHeaders];
   int numOfMFDictIncDirs;
   ROMEString mfDictIncDir[maxNumberOfMFDictIncDirs];
   int numOfMFWinLibs;
   ROMEString mfWinLibName[maxNumberOfMFWinLibs];
   int numOfMFUnixLibs;
   ROMEString mfUnixLibName[maxNumberOfMFUnixLibs];
   int numOfMFIncDirs;
   ROMEString mfIncDir[maxNumberOfMFIncDirs];
   int numOfMFPreDefs;
   ROMEString mfPreDefName[maxNumberOfMFPreDefs];
   int numOfMFSources;
   ROMEString mfSourceFileName[maxNumberOfMFSources];
   ROMEString mfSourceFilePath[maxNumberOfMFSources];
   ROMEString mfSourceFileDep[maxNumberOfMFSources];
   int numOfMFSourceFlags[maxNumberOfMFSources];
   ROMEString mfSourceFileFlag[maxNumberOfMFSources][maxNumberOfMFSourceFlags];

// main
   ROMEString mainAuthor;
   ROMEString mainInstitute;
   ROMEString mainCollaboration;
   ROMEString mainEmail;
   ROMEString mainProgName;
   ROMEString mainDescription;
   ROMEString styleSheet;

// Makefile
   ROMEStrArray* includeDirectories;
   ROMEStrArray* romeHeaders;
   ROMEStrArray* romeDictHeaders;
   ROMEStrArray* frameworkHeaders;
   ROMEStrArray* frameworkDictHeaders;
   ROMEStrArray* folderHeaders;
   ROMEStrArray* taskHeaders;
   ROMEStrArray* daqHeaders;
   ROMEStrArray* databaseHeaders;
   ROMEStrArray* romeSources;
   ROMEStrArray* frameworkSources;
   ROMEStrArray* folderSources;
   ROMEStrArray* taskSources;
   ROMEStrArray* daqSources;
   ROMEStrArray* databaseSources;

public:
   ROMEBuilder() { haveFortranTask = false; };
   virtual ~ROMEBuilder() {};

   bool ReadXMLFolder();
   bool WriteFolderCpp();
   bool WriteFolderH();
   bool ReadXMLTask();
   bool WriteSteeringClass(ROMEString& buffer,int numOfTaskSteer,int numTask,int tab);
   bool WriteSteeringConfigClass(ROMEString& buffer,int numOfTaskSteer,int numTask,int tab);
   bool WriteSteeringConfigRead(ROMEString &buffer,int numSteer,int numTask,ROMEXML *xml,ROMEString& path,ROMEString& pointer,ROMEString& classPath,ROMEString& index,ROMEString& blank,int *indexCounter);
   bool WriteSteeringConfigSet(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer,ROMEString& blank,int *indexCounter);
   bool WriteSteeringConfigWrite(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer,int tab,int *indexCounter);
   bool WriteSteeringReadParameters(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer) ;
   bool WriteSteeringParameterUsage(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer) ;
   int  WriteSteeringInterpreterCode(ROMEString &buffer,int codeNumber,int numSteer,int numTask,ROMEString& path,int tab);
   int  WriteSteeringInterpreterValue(ROMEString &buffer,const char* type,int codeNumber,int numSteer,int numTask,ROMEString& steerPointer,int tab);
   void WriteObjectInterpreterValue(ROMEString &buffer,const char* type,const char* fctName);
   void WriteReadDataBaseFolder(ROMEString &buffer,int numFolder,int type);
   void WriteFolderGetter(ROMEString &buffer,int numFolder,int scl,int nameLen,int typeLen);
   bool WriteTaskConfigWrite(ROMEString &buffer,int parentIndex,ROMEString& pointer,int tab);
   bool WriteTaskConfigClass(ROMEString &buffer,int parentIndex,int tab);
   bool WriteTaskCpp();
   bool WriteTaskF();
   bool WriteTaskH();
   bool ReadXMLTree();
   bool ReadXMLDAQ();
   bool ReadXMLDB();
   bool ReadXMLMidasBanks();
   bool ReadXMLSteering(int iTask);
   bool ReadXMLUserMakefile();
   bool WriteSteering(int iTask);
   bool WriteAnalyzerCpp();
   bool WriteAnalyzerH();
   bool WriteAnalyzerF();
   bool WriteConfigCpp();
   bool WriteConfigH();
   bool WriteMidasDAQCpp();
   bool WriteMidasDAQH();
   bool WriteRomeDAQCpp();
   bool WriteRomeDAQH();
   bool WriteDataBaseDAQCpp();
   bool WriteDataBaseDAQH();
   bool WriteDAQCpp();
   bool WriteDAQH();
   bool WriteDBCpp();
   bool WriteDBH();
   bool WriteEventLoopCpp();
   bool WriteEventLoopH();
   bool WriteMain();
   char* EqualSign();
   char* FlagSign();
   void AddInludeDirectories();
   void AddRomeHeaders();
   void AddRomeDictHeaders();
   void AddFrameworkHeaders();
   void AddFrameworkDictHeaders();
   void AddFolderHeaders();
   void AddTaskHeaders();
   void AddDAQHeaders();
   void AddDatabaseHeaders();
   void AddRomeSources();
   void AddFrameworkSources();
   void AddFolderSources();
   void AddTaskSources();
   void AddDAQSources();
   void AddDatabaseSources();
   void WriteMakefile();
   void WriteMakefileHeader(ROMEString& buffer);
   void WriteMakefileLibsAndFlags(ROMEString& buffer);
   void WriteMakefileIncludes(ROMEString& buffer);
   void WriteMakefileDictIncludes(ROMEString& buffer);
   void WriteMakefileObjects(ROMEString& buffer,ROMEStrArray* sources);
   void WriteMakefileUserDictObject(ROMEString& buffer);
   void WriteMakefileDictionary(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers);
   void WriteMakefileUserDictionary(ROMEString& buffer);
   void WriteMakefileCompileStatements(ROMEString& buffer,ROMEStrArray* sources);
   void WriteMakefileAdditionalSourceFilesObjects(ROMEString& buffer);
   void WriteMakefileAdditionalSourceFilesCompileStatments(ROMEString& buffer);
   void WriteMakefileBuildRule(ROMEString& buffer,const char* builder);
   void WriteRootCintCall(ROMEString& buffer);
   void WriteUserMakeFile();
   void WriteVisualProjects(int version,int subVersion);
   void WriteVisualProjectSln(int version,ROMEString& projectGUID);
   void WriteVisualProjectProjSettings(ROMEXML *xml,int version,int subVersion,ROMEString& projectGUID);
   void WriteVisualProjectProjSources(ROMEXML *xml,ROMEStrArray* sources,const char* folderName);
   void WriteVisualProjectProjUserSources(ROMEXML *xml);
   void WriteVisualProjectProjWarningLevel(ROMEXML *xml,const char *level);
   void GetRelativePath(const char* absolutePath,const char* referencePath,ROMEString &relativePath);
   void WriteHTMLDoku();
   void WriteHTMLStyle(ROMEString &buffer);
   void WriteHTMLSteering(ROMEString &buffer,int numSteer,int numTask,const char* group);
   virtual bool ReplaceHeader(const char* filename,const char* header,const char* content,int nspace = 0);
   virtual bool WriteFile(const char* filename,const char* content,int nspace = 0);
   void StartBuilder();
   void GetFormat(ROMEString *buf,const char *type);
   void setValue(ROMEString *buf,const char *destination,const char *source,const char *type,int version);
   bool isFloatingType(const char *type);
   bool isNumber(const char * string);
   bool isBoolType(const char *type);
   bool isIntType(const char *type);
   bool isFolder(const char *type);
   bool isTArrayType(const char *type);
   bool isTArrayType(TString &type) { return isTArrayType(type.Data()); };
   const char* TArray2StandardType(const char *type);
   const char* TArray2StandardType(TString &type) { return TArray2StandardType(type.Data()); };
   bool isRootClassType(const char *type) { TString tmp=type; return isRootClassType(tmp); };
   bool isRootClassType(TString &type) { ROMEString tmp=type; tmp.StripSpaces(); return tmp(0)=='T'; };
   bool isPointerType(const char *type) { TString tmp=type; return isPointerType(tmp); };
   bool isPointerType(TString &type) { ROMEString tmp = type; tmp.StripSpaces(); return tmp(tmp.Length()-1)=='*'; };
   bool accessFolder(ROMEString &fileBuffer, int numFolder);
   bool ReadCommandLineParameters(int argc, char *argv[]);
   void Usage();
   bool CheckFileAndPath();
   void MakeDir(ROMEString &path);
   void AnalyzeFileName(const char* file,ROMEString& pathOfFile,ROMEString& nameOfFile,ROMEString& extensionOfFile);

   ROMEString& convertType(const char *value,const char *oldType,const char *newType,ROMEString& stringBuffer);
};

#endif   // ROMEBuilder_H
