/********************************************************************
  ArgusBuilder.h, R. Sawada

  $Log$
  Revision 1.4  2005/02/04 22:58:46  sawada
  ROMEFolder

  Revision 1.3  2005/02/03 11:44:54  sawada
  IO to MIDAS ODB

  Revision 1.2  2005/02/01 21:53:24  sawada
  implemented thread functions of tabs.

  Revision 1.1.1.1  2005/01/29 22:45:07  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#ifndef ArgusBuilder_H
#define ArgusBuilder_H

#if defined( _MSC_VER )
#include <io.h>
#include <direct.h>
#endif
#if defined ( __linux__ ) ||  defined ( __APPLE__ )
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ROMEXML.h"
#include "ROMEString.h"

const int maxNumberOfTabs = 200;
const int maxNumberOfFolders = 100;
const int maxNumberOfSteering = 20;
const int maxNumberOfSteeringField = 100;
const int maxNumberOfValues = 50;
const int maxNumberOfInclude = 10;
const int maxNumberOfThreadFunctions = 4;
const int bufferLength = 100000;

class ArgusBuilder
{
public:
   ROMEString argusVersion;
   ROMEString outDir;
   bool       makeOutput;
   bool       noLink;
   bool       midas;
   bool       sql;
   bool       thread;
   bool       romefolder;
   
private:
   ROMEXML* xml;
   
   ROMEString shortCut;
   ROMEString experimentName;
   
   bool readExperiment;
   bool readAuthor;
   bool readFolders;
   bool readTabs;
   bool readGlobalSteeringParameters;

   ROMEString parent[maxNumberOfTabs];
   int recursiveTabDepth;
   int recursiveFolderDepth;
   int recursiveSteerDepth;
   int actualSteerIndex;
   
// folders
   int        numOfFolder;
   int        numOfValue[maxNumberOfFolders];
   int        numOfFolderInclude[maxNumberOfFolders];
   ROMEString folderName[maxNumberOfFolders];
   ROMEString folderDescription[maxNumberOfFolders];
   ROMEString folderParentName[maxNumberOfFolders];
   ROMEString folderTitle[maxNumberOfFolders];
   ROMEString folderArray[maxNumberOfFolders];
   ROMEString folderAuthor[maxNumberOfFolders];
   ROMEString folderVersion[maxNumberOfFolders];
   ROMEString folderInclude[maxNumberOfFolders][maxNumberOfInclude];
   ROMEString folderConnectionType[maxNumberOfFolders];
   bool       folderLocalFlag[maxNumberOfFolders][maxNumberOfInclude];
   bool       folderDataBase[maxNumberOfFolders];
   bool       folderUserCode[maxNumberOfFolders];
   bool       folderDefinedInROME[maxNumberOfFolders];
   ROMEString valueName[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueType[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueInit[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueComment[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueDataBasePath[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueArray[maxNumberOfFolders][maxNumberOfValues];
   
// tab
   int        numOfTab;
   ROMEString tabName[maxNumberOfTabs];
   ROMEString tabTitle[maxNumberOfTabs];
   ROMEString tabDescription[maxNumberOfTabs];
   ROMEString tabAuthor[maxNumberOfTabs];
   ROMEString tabVersion[maxNumberOfTabs];
      
// tab hierarchy
   int        numOfTabHierarchy;
   ROMEString tabHierarchyName[2*maxNumberOfTabs];
   ROMEString tabHierarchyTitle[2*maxNumberOfTabs];
   int        tabHierarchyParentIndex[2*maxNumberOfTabs];
   int        tabHierarchyClassIndex[2*maxNumberOfTabs];

// thread functions
   int        numOfThreadFunctions[maxNumberOfTabs];
   ROMEString threadFunctionName[maxNumberOfTabs][maxNumberOfThreadFunctions];

// steering
   int        numOfSteering[maxNumberOfTabs+1];
   int        numOfSteerFields[maxNumberOfTabs+1][maxNumberOfSteering];
   int        numOfSteerChildren[maxNumberOfTabs+1][maxNumberOfSteering];
   ROMEString steerName[maxNumberOfTabs+1][maxNumberOfSteering];
   int        steerParent[maxNumberOfTabs+1][maxNumberOfSteering];
   int        steerChildren[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteering];
   ROMEString steerFieldName[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldType[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldInit[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldComment[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteeringField];

   bool       bankHasHeader;
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
   ArgusBuilder() {};

   bool ReadXMLFolder();
   bool ReadXMLROMEFolder();
   bool WriteFolderCpp();
   bool WriteFolderH();
   bool WriteWindowCpp();
   bool WriteWindowH();
   bool ReadXMLTab();
   bool WriteSteeringClass(ROMEString& buffer,int numOfTabSteer,int numTab,int tab);
   bool WriteSteeringConfigClass(ROMEString& buffer,int numOfTabSteer,int numTab,int tab);
   bool WriteSteeringConfigRead(ROMEString &buffer,int numSteer,int numTab,ROMEXML *xml,ROMEString& path,ROMEString& pointer,ROMEString& classPath);
   bool WriteSteeringConfigSet(ROMEString &buffer,int numSteer,int numTab,ROMEString& pointer,ROMEString& steerPointer);
   bool WriteSteeringConfigWrite(ROMEString &buffer,int numSteer,int numTab,ROMEString& pointer,ROMEString& steerPointer,int tab);
   bool WriteTabConfigWrite(ROMEString &buffer,int parentIndex,ROMEString& pointer,int tab);
   bool WriteTabConfigClass(ROMEString &buffer,int parentIndex,int tab);
   bool WriteTabCpp();
   bool WriteTabH();
   bool ReadXMLTree();
   bool ReadXMLMidasBanks();
   bool ReadXMLSteering(int iTab);
   bool WriteSteering(int iTab);
   bool WriteMonitorCpp();
   bool WriteMonitorH();
   bool WriteConfigCpp();
   bool WriteConfigH();
   bool WriteMain();
   void WriteMakefile();
   void WriteHTMLDoku();
   void WriteDictionaryBat(ROMEString& buffer);
   void startBuilder(char* xmlFile);
   void GetFormat(ROMEString *buf,char *type);
   void GetMidasTID(ROMEString *buf,char *type);
   void setValue(ROMEString *buf,char *destination,char *source,char *type,int version);
};
#endif   // ArgusBuilder_H
