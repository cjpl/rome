#ifndef ROMEBuilder_H
#define ROMEBuilder_H

#include <ROMEXML.h>

const int maxNumberOfTasks = 200;
const int maxNumberOfFolders = 100;
const int maxNumberOfTrees = 20;
const int maxNumberOfBranches = 10;
const int maxNumberOfBanks = 20;
const int maxNumberOfSteering = 20;
const int maxNumberOfSteeringField = 100;
const int maxNumberOfValues = 50;
const int maxNumberOfGetters = 10;
const int maxNumberOfSetters = 10;
const int maxNumberOfInclude = 10;
const int maxNumberOfHistos = 10;
const int maxNumberOfStructFields = 50;
const int bufferLength = 100000;


class ROMEBuilder
{
public:
   char* romeVersion;

   char* outDir;
   bool  makeOutput;
   bool  noLink;
   bool  offline;
   bool  sql;

private:
   ROMEXML* xml;

   char* shortCut;

// folders
   int numOfFolder;
   int numOfValue[maxNumberOfFolders];
   int numOfFolderInclude[maxNumberOfTasks];

   char* folderName[maxNumberOfFolders];
   char* folderDescription[maxNumberOfFolders];
   char* folderParentName[maxNumberOfFolders];
   char* folderTitle[maxNumberOfFolders];
   char* folderArray[maxNumberOfFolders];
   char* folderAuthor[maxNumberOfFolders];
   char* folderVersion[maxNumberOfFolders];
   char* folderInclude[maxNumberOfTasks][maxNumberOfInclude];
   bool  folderLocalFlag[maxNumberOfTasks][maxNumberOfInclude];
   bool folderDataBase[maxNumberOfFolders];
   bool folderUserCode[maxNumberOfFolders];

   char* valueName[maxNumberOfFolders][maxNumberOfValues];
   char* valueType[maxNumberOfFolders][maxNumberOfValues];
   char* valueRef[maxNumberOfFolders][maxNumberOfValues];
   char* valueInit[maxNumberOfFolders][maxNumberOfValues];
   char* valueComment[maxNumberOfFolders][maxNumberOfValues];
   char* valueDataBasePath[maxNumberOfFolders][maxNumberOfValues];
   char* valueArray[maxNumberOfFolders][maxNumberOfValues];

// task

   int numOfTask;
   int numOfHistos[maxNumberOfTasks];
   int numOfTaskSteering[maxNumberOfTasks];
   int numOfTaskSteerFields[maxNumberOfTasks][maxNumberOfSteering];
   int numOfTaskInclude[maxNumberOfTasks];
   char* taskName[maxNumberOfTasks];
   char* taskEventID[maxNumberOfTasks];
   char* taskDescription[maxNumberOfTasks];
   bool  taskFortran[maxNumberOfTasks];
   char* taskParentName[maxNumberOfTasks];
   char* taskAuthor[maxNumberOfTasks];
   char* taskVersion[maxNumberOfTasks];
   char* taskInclude[maxNumberOfTasks][maxNumberOfInclude];
   bool  taskLocalFlag[maxNumberOfTasks][maxNumberOfInclude];
   char* histoName[maxNumberOfTasks][maxNumberOfHistos];
   char* histoType[maxNumberOfTasks][maxNumberOfHistos];
   char* histoArray[maxNumberOfTasks][maxNumberOfHistos];
   char* histoTitle[maxNumberOfTasks][maxNumberOfHistos];
   char* histoFolderName[maxNumberOfTasks][maxNumberOfHistos];
   char* histoFolderTitle[maxNumberOfTasks][maxNumberOfHistos];
   char* histoXBin[maxNumberOfTasks][maxNumberOfHistos];
   char* histoXMin[maxNumberOfTasks][maxNumberOfHistos];
   char* histoXMax[maxNumberOfTasks][maxNumberOfHistos];
   char* histoYBin[maxNumberOfTasks][maxNumberOfHistos];
   char* histoYMin[maxNumberOfTasks][maxNumberOfHistos];
   char* histoYMax[maxNumberOfTasks][maxNumberOfHistos];
   char* histoZBin[maxNumberOfTasks][maxNumberOfHistos];
   char* histoZMin[maxNumberOfTasks][maxNumberOfHistos];
   char* histoZMax[maxNumberOfTasks][maxNumberOfHistos];
   char* taskSteerName[maxNumberOfTasks][maxNumberOfSteering];
   char* taskSteerParent[maxNumberOfTasks][maxNumberOfSteering];
   int   taskSteerDepth[maxNumberOfTasks][maxNumberOfSteering];
   char* taskSteerFieldName[maxNumberOfTasks][maxNumberOfSteering][maxNumberOfSteeringField];
   char* taskSteerFieldType[maxNumberOfTasks][maxNumberOfSteering][maxNumberOfSteeringField];
   char* taskSteerFieldInit[maxNumberOfTasks][maxNumberOfSteering][maxNumberOfSteeringField];
   char* taskSteerFieldComment[maxNumberOfTasks][maxNumberOfSteering][maxNumberOfSteeringField];

// tree
   int numOfTree;
   int numOfBranch[maxNumberOfTrees];
   char* treeName[maxNumberOfTrees];
   char* treeTitle[maxNumberOfTrees];
   char* branchName[maxNumberOfTrees][maxNumberOfBranches];
   char* branchFolder[maxNumberOfTrees][maxNumberOfBranches];

// banks
   int numOfBank;
   int numOfStructFields[maxNumberOfBanks];
   char* bankName[maxNumberOfBanks];
   char* bankType[maxNumberOfBanks];
   char* bankStructName[maxNumberOfBanks];
   char* structFieldName[maxNumberOfBanks][maxNumberOfStructFields];
   char* structFieldType[maxNumberOfBanks][maxNumberOfStructFields];
   char* structFieldSize[maxNumberOfBanks][maxNumberOfStructFields];
   bool  bankHasHeader;
   char* bankHeaderFolder;
   char* bankHeaderEventID;
   char* bankHeaderTriggerMask;
   char* bankHeaderSerialNumber;
   char* bankHeaderTimeStamp;

// steering
   int numOfSteering;
   int numOfSteerFields[maxNumberOfSteering];
   char* steerName[maxNumberOfSteering];
   char* steerParent[maxNumberOfSteering];
   int   steerDepth[maxNumberOfSteering];
   char* steerFieldName[maxNumberOfSteering][maxNumberOfSteeringField];
   char* steerFieldType[maxNumberOfSteering][maxNumberOfSteeringField];
   char* steerFieldInit[maxNumberOfSteering][maxNumberOfSteeringField];
   char* steerFieldComment[maxNumberOfSteering][maxNumberOfSteeringField];

// main
   char* mainAuthor;
   char* mainInstitute;
   char* mainCollaboration;
   char* mainEmail;
   char* mainProgName;

public:
   ROMEBuilder() {};

   bool ReadXMLFolder();
   bool WriteFolderCpp();
   bool WriteFolderH();
   bool ReadXMLTask();
   bool WriteTaskSteeringClass(char *buffer,int numOfTaskSteer,int numTask);
   void WriteTaskSteerConfigWrite(char *buffer,int numSteer,int numTask);
   void WriteTaskSteerConfigRead(char *buffer,int numSteer,int numTask);
   bool WriteTaskCpp();
   bool WriteTaskF();
   bool WriteTaskH();
   bool ReadXMLTree();
   bool ReadXMLMidasBanks();
   bool ReadXMLSteering();
   bool WriteSteering();
   bool WriteSteeringClass(char *buffer,int numOfSteer);
   bool WriteAnalyzerCpp();
   bool WriteAnalyzerH();
   void WriteSteerConfigWrite(char *buffer,int numOfSteer);
   void WriteSteerConfigRead(char *buffer,int numSteer);
   bool WriteIOCpp();
   bool WriteIOH();
   bool WriteEventLoopCpp();
   bool WriteEventLoopH();
   bool WriteMain();
   void WriteMakefile();
   void WriteHTMLDoku();
   void WriteDictionaryBat(char* buffer);
   void startBuilder(char* xmlFile);
   void GetFormat(char *buf,char *type);
   void setValue(char *buf,char *destination,char *source,char *type,int version);
   bool isFloatingType(char *type);
};

#endif   // ROMEBuilder_H
