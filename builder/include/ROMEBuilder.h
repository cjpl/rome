/********************************************************************
  ROMEBuilder.h, M. Schneebeli PSI

  $Log$
  Revision 1.18  2004/11/16 16:14:00  schneebeli_m
  implemented task hierarchy

  Revision 1.17  2004/10/21 13:03:51  schneebeli_m
  *** empty log message ***

  Revision 1.16  2004/10/14 09:53:41  schneebeli_m
  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag

  Revision 1.15  2004/10/08 12:03:17  schneebeli_m
  Changed XML format, included a rome.xsd schema file

  Revision 1.14  2004/10/05 07:52:44  schneebeli_m
  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed

  Revision 1.13  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/

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
   ROMEString experimentName;

   bool readExperiment;
   bool readAuthor;
   bool readFolders;
   bool readTasks;
   bool readTrees;
   bool readGlobalSteeringParameters;
   bool readMidasBanks;

   ROMEString parent[maxNumberOfTasks];
   int recursiveDepth;
   int recursiveSteerDepth;

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

   ROMEString valueName[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueType[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueInit[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueComment[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueDataBasePath[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueArray[maxNumberOfFolders][maxNumberOfValues];

// task

   int numOfTask;
   int numOfHistos[maxNumberOfTasks];
   int numOfTaskInclude[maxNumberOfTasks];
   ROMEString taskName[maxNumberOfTasks];
   ROMEString taskEventID[maxNumberOfTasks];
   ROMEString taskDescription[maxNumberOfTasks];
   bool  taskFortran[maxNumberOfTasks];
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
   int        steerParent[maxNumberOfTasks+1][maxNumberOfSteering];
   int        steerChildren[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteering];
   ROMEString steerFieldName[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldType[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldInit[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldComment[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];

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
   ROMEString structFieldName[maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString structFieldType[maxNumberOfBanks][maxNumberOfStructFields];
   ROMEString structFieldSize[maxNumberOfBanks][maxNumberOfStructFields];
   bool  bankHasHeader;
   ROMEString bankHeaderFolder;
   ROMEString bankHeaderEventID;
   ROMEString bankHeaderTriggerMask;
   ROMEString bankHeaderSerialNumber;
   ROMEString bankHeaderTimeStamp;

// main
   ROMEString mainAuthor;
   ROMEString mainInstitute;
   ROMEString mainCollaboration;
   ROMEString mainEmail;
   ROMEString mainProgName;
   ROMEString mainDescription;

public:
   ROMEBuilder() {};

   bool ReadXMLFolder();
   bool WriteFolderCpp();
   bool WriteFolderH();
   bool ReadXMLTask();
   bool WriteSteeringClass(ROMEString& buffer,int numOfTaskSteer,int numTask,int tab);
   bool WriteSteeringConfigClass(ROMEString& buffer,int numOfTaskSteer,int numTask,int tab);
   bool WriteSteeringConfigRead(ROMEString &buffer,int numSteer,int numTask,ROMEXML *xml,ROMEString& path,ROMEString& pointer,ROMEString& classPath);
   bool WriteSteeringConfigSet(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer);
   bool WriteSteeringConfigWrite(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer,int tab);
   bool WriteTaskConfigWrite(ROMEString &buffer,int parentIndex,ROMEString& pointer,int tab);
   bool WriteTaskConfigClass(ROMEString &buffer,int parentIndex,int tab);
   bool WriteTaskCpp();
   bool WriteTaskF();
   bool WriteTaskH();
   bool ReadXMLTree();
   bool ReadXMLMidasBanks();
   bool ReadXMLSteering(int iTask);
   bool WriteSteering(int iTask);
   bool WriteAnalyzerCpp();
   bool WriteAnalyzerH();
   bool WriteAnalyzerF();
   bool WriteConfigCpp();
   bool WriteConfigH();
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
   bool isNumber(ROMEString& string);
};

#endif   // ROMEBuilder_H
