/********************************************************************
  ROMEBuilder.h, M. Schneebeli PSI

  $Log$
  Revision 1.37  2005/04/22 16:25:14  sawada
  user defined command line option.

  Revision 1.36  2005/04/20 15:12:38  schneebeli_m
  histo and path in definition xml

  Revision 1.35  2005/04/15 14:22:00  sawada
  Makefile: added dependence on inherited class objects

  Revision 1.34  2005/04/04 10:14:54  schneebeli_m
  UserDataBase implemented

  Revision 1.33  2005/04/04 07:06:15  schneebeli_m
  bankFieldArraySize

  Revision 1.32  2005/04/01 14:56:22  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.31  2005/03/23 15:51:26  schneebeli_m
  BankFieldArray

  Revision 1.30  2005/03/23 09:06:11  schneebeli_m
  libxml replaced by mxml, Bool SP error

  Revision 1.29  2005/03/21 10:39:40  schneebeli_m
  Midas event definition implemented

  Revision 1.28  2005/03/18 16:12:57  schneebeli_m
  Event request & Histo in romeConfig

  Revision 1.27  2005/03/17 15:44:02  schneebeli_m
  Makefile for windows

  Revision 1.26  2005/03/08 11:00:36  sawada
  Improvement Makefile for UNIX.

  Revision 1.25  2005/01/25 16:36:22  schneebeli_m
  histo label

  Revision 1.24  2005/01/24 15:20:24  schneebeli_m
  Seperated DAQ classes

  Revision 1.23  2005/01/14 17:13:52  schneebeli_m
  HistoArrayStartIndex

  Revision 1.22  2005/01/07 11:37:32  schneebeli_m
  Bank Arrays, Folder Array Getter

  Revision 1.21  2005/01/05 10:37:05  schneebeli_m
  Bank enumeration

  Revision 1.20  2004/12/06 09:04:34  schneebeli_m
  minor changes

  Revision 1.19  2004/11/23 08:54:25  schneebeli_m
  steering parameter hierarchy error removed

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
const int maxNumberOfDAQ = 20;
const int maxNumberOfDB = 20;
const int maxNumberOfBranches = 10;
const int maxNumberOfBanks = 20;
const int maxNumberOfEvents = 5;
const int maxNumberOfSteering = 20;
const int maxNumberOfSteeringField = 100;
const int maxNumberOfValues = 50;
const int maxNumberOfGetters = 10;
const int maxNumberOfSetters = 10;
const int maxNumberOfInclude = 10;
const int maxNumberOfHistos = 10;
const int maxNumberOfStructFields = 50;
const int bufferLength = 500000;

const int maxNumberOfEventRequests = 5;
const int maxNumberOfPathObjectInterpreterCodes = 10;

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

   ROMEString valueName[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueType[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueInit[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueComment[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueArray[maxNumberOfFolders][maxNumberOfValues];
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
   int        steerParent[maxNumberOfTasks+1][maxNumberOfSteering];
   int        steerChildren[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteering];
   ROMEString steerFieldName[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldType[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldInit[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldComment[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldCLOption[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldCLDescription[maxNumberOfTasks+1][maxNumberOfSteering][maxNumberOfSteeringField];

// tree
   int numOfTree;
   int numOfBranch[maxNumberOfTrees];
   ROMEString treeName[maxNumberOfTrees];
   ROMEString treeTitle[maxNumberOfTrees];
   ROMEString branchName[maxNumberOfTrees][maxNumberOfBranches];
   ROMEString branchFolder[maxNumberOfTrees][maxNumberOfBranches];

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

// main
   ROMEString mainAuthor;
   ROMEString mainInstitute;
   ROMEString mainCollaboration;
   ROMEString mainEmail;
   ROMEString mainProgName;
   ROMEString mainDescription;

// Compile Dependencies
   ROMEString analyzerDep;
   ROMEString eventLoopDep;
   ROMEString configDep;
   ROMEString midasDep;
   ROMEString rootDep;
   ROMEString daqDep;

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
   bool WriteSteeringReadParameters(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer) ;
   bool WriteSteeringParameterUsage(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer) ;
   int  WriteSteeringInterpreterCode(ROMEString &buffer,int codeNumber,int numSteer,int numTask,ROMEString& path,int tab);
   int  WriteSteeringInterpreterValue(ROMEString &buffer,const char* type,int codeNumber,int numSteer,int numTask,ROMEString& steerPointer,int tab);
   void WriteObjectInterpreterValue(ROMEString &buffer,const char* type,const char* fctName);
   void WriteReadDataBaseFolders(ROMEString &buffer,int type);
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
   bool WriteSteering(int iTask);
   bool WriteAnalyzerCpp();
   bool WriteAnalyzerH();
   bool WriteAnalyzerF();
   bool WriteConfigCpp();
   bool WriteConfigH();
   bool WriteMidasCpp();
   bool WriteMidasH();
   bool WriteRootCpp();
   bool WriteRootH();
   bool WriteDAQCpp();
   bool WriteDAQH();
   bool WriteDBCpp();
   bool WriteDBH();
   bool WriteEventLoopCpp();
   bool WriteEventLoopH();
   bool WriteMain();
   char* EqualSign();
   void WriteMakefile();
   void WriteHTMLDoku();
   void WriteDictionaryBat(ROMEString& buffer);
   void startBuilder(const char* xmlFile);
   void GetFormat(ROMEString *buf,const char *type);
   void setValue(ROMEString *buf,const char *destination,const char *source,const char *type,int version);
   bool isFloatingType(const char *type);
   bool isNumber(const char * string);
   bool isBoolType(const char *type);
   ROMEString& convertType(const char *value,const char *oldType,const char *newType,ROMEString& stringBuffer);
};

#endif   // ROMEBuilder_H
