#ifndef ROMEBuilder_H
#define ROMEBuilder_H

#include <ROMEXML.h>
#include <ROMEString.h>

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
   ROMEString romeVersion;

   ROMEString outDir;
   bool  makeOutput;
   bool  noLink;
   bool  midas;
   bool  sql;

private:
   ROMEXML* xml;

   ROMEString shortCut;

// folders
   int numOfFolder;
   int numOfValue[maxNumberOfFolders];
   int numOfFolderInclude[maxNumberOfTasks];

   ROMEString folderName[maxNumberOfFolders];
   ROMEString folderDescription[maxNumberOfFolders];
   ROMEString folderParentName[maxNumberOfFolders];
   ROMEString folderTitle[maxNumberOfFolders];
   ROMEString folderArray[maxNumberOfFolders];
   ROMEString folderAuthor[maxNumberOfFolders];
   ROMEString folderVersion[maxNumberOfFolders];
   ROMEString folderInclude[maxNumberOfTasks][maxNumberOfInclude];
   bool  folderLocalFlag[maxNumberOfTasks][maxNumberOfInclude];
   bool folderDataBase[maxNumberOfFolders];
   bool folderUserCode[maxNumberOfFolders];

   ROMEString valueName[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueType[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueRef[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueInit[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueComment[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueDataBasePath[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueArray[maxNumberOfFolders][maxNumberOfValues];

// task

   int numOfTask;
   int numOfHistos[maxNumberOfTasks];
   int numOfTaskSteering[maxNumberOfTasks];
   int numOfTaskSteerFields[maxNumberOfTasks][maxNumberOfSteering];
   int numOfTaskInclude[maxNumberOfTasks];
   ROMEString taskName[maxNumberOfTasks];
   ROMEString taskEventID[maxNumberOfTasks];
   ROMEString taskDescription[maxNumberOfTasks];
   bool  taskFortran[maxNumberOfTasks];
   ROMEString taskParentName[maxNumberOfTasks];
   ROMEString taskAuthor[maxNumberOfTasks];
   ROMEString taskVersion[maxNumberOfTasks];
   ROMEString taskInclude[maxNumberOfTasks][maxNumberOfInclude];
   bool  taskLocalFlag[maxNumberOfTasks][maxNumberOfInclude];
   ROMEString histoName[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoType[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoArray[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoTitle[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoFolderName[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoFolderTitle[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoXBin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoXMin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoXMax[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoYBin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoYMin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoYMax[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoZBin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoZMin[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString histoZMax[maxNumberOfTasks][maxNumberOfHistos];
   ROMEString taskSteerName[maxNumberOfTasks][maxNumberOfSteering];
   ROMEString taskSteerParent[maxNumberOfTasks][maxNumberOfSteering];
   int   taskSteerDepth[maxNumberOfTasks][maxNumberOfSteering];
   ROMEString taskSteerFieldName[maxNumberOfTasks][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString taskSteerFieldType[maxNumberOfTasks][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString taskSteerFieldInit[maxNumberOfTasks][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString taskSteerFieldComment[maxNumberOfTasks][maxNumberOfSteering][maxNumberOfSteeringField];

// tree
   int numOfTree;
   int numOfBranch[maxNumberOfTrees];
   ROMEString treeName[maxNumberOfTrees];
   ROMEString treeTitle[maxNumberOfTrees];
   ROMEString branchName[maxNumberOfTrees][maxNumberOfBranches];
   ROMEString branchFolder[maxNumberOfTrees][maxNumberOfBranches];

// banks
   int numOfBank;
   int numOfStructFields[maxNumberOfBanks];
   ROMEString bankName[maxNumberOfBanks];
   ROMEString bankType[maxNumberOfBanks];
   ROMEString bankStructName[maxNumberOfBanks];
   ROMEString structFieldName[maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString structFieldType[maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString structFieldSize[maxNumberOfBanks][maxNumberOfStructFields];
   bool  bankHasHeader;
   ROMEString bankHeaderFolder;
   ROMEString bankHeaderEventID;
   ROMEString bankHeaderTriggerMask;
   ROMEString bankHeaderSerialNumber;
   ROMEString bankHeaderTimeStamp;

// steering
   int numOfSteering;
   int numOfSteerFields[maxNumberOfSteering];
   ROMEString steerName[maxNumberOfSteering];
   ROMEString steerParent[maxNumberOfSteering];
   int   steerDepth[maxNumberOfSteering];
   ROMEString steerFieldName[maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldType[maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldInit[maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldComment[maxNumberOfSteering][maxNumberOfSteeringField];

// main
   ROMEString mainAuthor;
   ROMEString mainInstitute;
   ROMEString mainCollaboration;
   ROMEString mainEmail;
   ROMEString mainProgName;

public:
   ROMEBuilder() {};

   bool ReadXMLFolder();
   bool WriteFolderCpp();
   bool WriteFolderH();
   bool ReadXMLTask();
   bool WriteTaskSteeringClass(ROMEString& buffer,int numOfTaskSteer,int numTask);
   void WriteTaskSteerConfigWrite(ROMEString& buffer,int numSteer,int numTask);
   void WriteTaskSteerConfigRead(ROMEString& buffer,int numSteer,int numTask);
   bool WriteTaskCpp();
   bool WriteTaskF();
   bool WriteTaskH();
   bool ReadXMLTree();
   bool ReadXMLMidasBanks();
   bool ReadXMLSteering();
   bool WriteSteering();
   bool WriteSteeringClass(ROMEString& buffer,int numOfSteer);
   bool WriteAnalyzerCpp();
   bool WriteAnalyzerH();
   void WriteSteerConfigWrite(ROMEString& buffer,int numOfSteer);
   void WriteSteerConfigRead(ROMEString& buffer,int numSteer);
   bool WriteIOCpp();
   bool WriteIOH();
   bool WriteEventLoopCpp();
   bool WriteEventLoopH();
   bool WriteMain();
   void WriteMakefile();
   void WriteHTMLDoku();
   void WriteDictionaryBat(ROMEString& buffer);
   void startBuilder(char* xmlFile);
   void GetFormat(ROMEString *buf,char *type);
   void setValue(ROMEString *buf,char *destination,char *source,char *type,int version);
   bool isFloatingType(char *type);
};

#endif   // ROMEBuilder_H
