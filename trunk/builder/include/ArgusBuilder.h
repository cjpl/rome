/********************************************************************
  ArgusBuilder.h, R. Sawada

  $Log$
  Revision 1.25  2005/05/14 21:42:22  sawada
  Separated file writing function in builder.

  Revision 1.24  2005/05/06 18:37:05  sawada
  added ArgusVersion.h

  Revision 1.23  2005/05/06 12:08:23  schneebeli_m
  changed max number of menus

  Revision 1.22  2005/05/05 20:13:30  sawada
  code clean up.

  Revision 1.21  2005/05/05 20:08:04  sawada
  code clean up.

  Revision 1.20  2005/05/02 10:36:12  schneebeli_m
  added threadfunction arguments

  Revision 1.19  2005/04/28 16:26:26  sawada
  PostgreSQL support.
  Error message from GetMenuHandle.

  Revision 1.18  2005/04/27 20:17:42  sawada
  SQLite support.

  Revision 1.17  2005/04/22 15:29:08  schneebeli_m
  added menu id enumeration

  Revision 1.16  2005/04/01 12:31:19  sawada
  sub menu

  Revision 1.15  2005/03/28 10:54:37  sawada
  removed tab hierarchy.
  made ReadXMLMenu.

  Revision 1.14  2005/03/20 18:19:55  sawada
  unified makefile for windows and unix.

  Revision 1.13  2005/03/13 08:40:43  sawada
  modified handling of recursiveDepth.
  removed unused variables.

  Revision 1.12  2005/03/12 01:21:00  sawada
  Nested tab.

  Revision 1.11  2005/03/11 19:49:34  sawada
  *** empty log message ***

  Revision 1.10  2005/02/25 16:14:00  sawada
  bug fix and example for ROMEFolder

  Revision 1.9  2005/02/25 13:51:02  sawada
  added folderShortCut and ROMEProjectPath

  Revision 1.8  2005/02/24 23:25:01  sawada
  Removed thread flag in builder.
  Added ProcessMessageThread.
  Changed arguments of constructor of tabs.

  Revision 1.7  2005/02/22 10:43:31  sawada
  User defined menus.

  Revision 1.6  2005/02/21 19:06:55  sawada
  changed platform specifying macros

  Revision 1.5  2005/02/13 13:07:26  sawada
  added unistd.h

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

#include <RConfig.h>
#include <Rtypes.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   include <io.h>
#   include <direct.h>
#endif
#if defined( R__UNIX )
#   include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ArgusVersion.h"
#include "ROMEXML.h"
#include "ROMEString.h"

const Int_t maxNumberOfTabs                    =    200;
const Int_t maxNumberOfMenus                   =     10;
const Int_t maxNumberOfMenuItems               =     20;
const Int_t maxNumberOfFolders                 =    100;
const Int_t maxNumberOfSteering                =     20;
const Int_t maxNumberOfSteeringField           =    100;
const Int_t maxNumberOfValues                  =     50;
const Int_t maxNumberOfInclude                 =     10;
const Int_t maxNumberOfThreadFunctions         =     10;
const Int_t maxNumberOfThreadFunctionArguments =     10;
const Char_t LINE_TITLE[] = "NoDayWithoutItsLine";
const Ssiz_t kTstringResizeIncrement           =   4096;

class ArgusBuilder
{
public:
   ROMEString   argusVersion;
   ROMEString   outDir;
   Bool_t       makeOutput;
   Bool_t       noLink;
   Bool_t       midas;
   Bool_t       sql;
   Bool_t       mysql;
   Bool_t       pgsql;
   Bool_t       sqlite;
   Bool_t       sqlite3;
   Bool_t       romefolder;
   
private:
   ROMEXML* xml;
   
   ROMEString   shortCut;
   ROMEString   experimentName;
   
   Bool_t readExperiment;
   Bool_t readAuthor;
   Bool_t readFolders;
   Bool_t readTabs;
   Bool_t readGlobalSteeringParameters;

   Int_t recursiveTabDepth;
   Int_t recursiveFolderDepth;
   Int_t recursiveSteerDepth;
   Int_t recursiveMenuDepth;
   Int_t actualSteerIndex;
   
// folders
   ROMEString parent[maxNumberOfFolders];
   Int_t      numOfFolder;
   Int_t      numOfValue[maxNumberOfFolders];
   Int_t      numOfFolderInclude[maxNumberOfFolders];
   ROMEString folderName[maxNumberOfFolders];
   ROMEString folderRomeProjPath[maxNumberOfFolders];
   ROMEString folderDescription[maxNumberOfFolders];
   ROMEString folderParentName[maxNumberOfFolders];
   ROMEString folderTitle[maxNumberOfFolders];
   ROMEString folderArray[maxNumberOfFolders];
   ROMEString folderAuthor[maxNumberOfFolders];
   ROMEString folderVersion[maxNumberOfFolders];
   ROMEString folderInclude[maxNumberOfFolders][maxNumberOfInclude];
   ROMEString folderConnectionType[maxNumberOfFolders];
   Bool_t     folderLocalFlag[maxNumberOfFolders][maxNumberOfInclude];
   Bool_t     folderDataBase[maxNumberOfFolders];
   Bool_t     folderUserCode[maxNumberOfFolders];
   Bool_t     folderDefinedInROME[maxNumberOfFolders];
   ROMEString valueName[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueType[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueInit[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueComment[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueDataBasePath[maxNumberOfFolders][maxNumberOfValues];
   ROMEString valueArray[maxNumberOfFolders][maxNumberOfValues];
   
// tab
   Int_t      numOfTab;
   ROMEString tabName[maxNumberOfTabs];
   ROMEString tabTitle[maxNumberOfTabs];
   ROMEString tabDescription[maxNumberOfTabs];
   ROMEString tabAuthor[maxNumberOfTabs];
   ROMEString tabVersion[maxNumberOfTabs];
   Int_t      tabParentIndex[maxNumberOfTabs];
   Int_t      tabNumOfChildren[maxNumberOfTabs];
   Int_t      numOfMenu[maxNumberOfTabs];
   Int_t      numOfMenuItem[maxNumberOfTabs][maxNumberOfMenus];
   ROMEString menuTitle[maxNumberOfTabs][maxNumberOfMenus];
   Int_t      menuDepth[maxNumberOfTabs][maxNumberOfMenus];
   Int_t      menuItemChildMenuIndex[maxNumberOfTabs][maxNumberOfMenus][maxNumberOfMenuItems];
   ROMEString menuItemEnumName[maxNumberOfTabs][maxNumberOfMenus][maxNumberOfMenuItems];
   ROMEString menuItemTitle[maxNumberOfTabs][maxNumberOfMenus][maxNumberOfMenuItems];

// thread functions
   Int_t      numOfThreadFunctions[maxNumberOfTabs];
   Int_t      numOfThreadFunctionArguments[maxNumberOfTabs][maxNumberOfThreadFunctions];
   ROMEString threadFunctionName[maxNumberOfTabs][maxNumberOfThreadFunctions];
   ROMEString threadFunctionArgument[maxNumberOfTabs][maxNumberOfThreadFunctions][maxNumberOfThreadFunctionArguments];

// steering
   Int_t      numOfSteering[maxNumberOfTabs+1];
   Int_t      numOfSteerFields[maxNumberOfTabs+1][maxNumberOfSteering];
   Int_t      numOfSteerChildren[maxNumberOfTabs+1][maxNumberOfSteering];
   ROMEString steerName[maxNumberOfTabs+1][maxNumberOfSteering];
   Int_t      steerParent[maxNumberOfTabs+1][maxNumberOfSteering];
   Int_t      steerChildren[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteering];
   ROMEString steerFieldName[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldType[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldInit[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteeringField];
   ROMEString steerFieldComment[maxNumberOfTabs+1][maxNumberOfSteering][maxNumberOfSteeringField];

   Bool_t     bankHasHeader;
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

   Bool_t ReadXMLFolder();
   Bool_t ReadXMLROMEFolder();
   Bool_t WriteFolderCpp();
   Bool_t WriteFolderH();
   Bool_t WriteWindowCpp();
   Bool_t WriteWindowH();
   Bool_t ReadXMLTab();
   Bool_t ReadXMLMenu(Int_t currentNumberOfTabs);
   Bool_t WriteSteeringClass(ROMEString& buffer,Int_t numOfTabSteer,Int_t numTab,Int_t tab);
   Bool_t WriteSteeringConfigClass(ROMEString& buffer,Int_t numOfTabSteer,Int_t numTab,Int_t tab);
   Bool_t WriteSteeringConfigRead(ROMEString &buffer,Int_t numSteer,Int_t numTab,ROMEXML *xml,ROMEString& path,ROMEString& pointer,ROMEString& classPath);
   Bool_t WriteSteeringConfigSet(ROMEString &buffer,Int_t numSteer,Int_t numTab,ROMEString& pointer,ROMEString& steerPointer);
   Bool_t WriteSteeringConfigWrite(ROMEString &buffer,Int_t numSteer,Int_t numTab,ROMEString& pointer,ROMEString& steerPointer,Int_t tab);
   Bool_t WriteTabConfigWrite(ROMEString &buffer,Int_t parentIndex,ROMEString& pointer,Int_t tab);
   Bool_t WriteTabConfigClass(ROMEString &buffer,Int_t parentIndex,Int_t tab);
   Bool_t WriteTabCpp();
   Bool_t WriteTabH();
   Bool_t AddTab(ROMEString& buffer,Int_t& i);
   Bool_t AddMenuItems(ROMEString& buffer,Int_t i,Int_t j);
   Bool_t ReadXMLTree();
   Bool_t ReadXMLMidasBanks();
   Bool_t ReadXMLSteering(Int_t iTab);
   Bool_t WriteSteering(Int_t iTab);
   Bool_t WriteMonitorCpp();
   Bool_t WriteMonitorH();
   Bool_t WriteConfigCpp();
   Bool_t WriteConfigH();
   Bool_t WriteMain();
   Char_t* EqualSign();
   void   WriteMakefile(Char_t* xmlFile);
   void   WriteHTMLDoku();
   void   WriteDictionaryBat(ROMEString& buffer);
   bool   ReplaceHeader(const char* filename,const char* header,const char* content,int nspace = 0);
   bool   WriteFile(const char* filename,const char* content,int nspace = 0);
   void   startBuilder(Char_t* xmlFile);
   void   GetFormat(ROMEString *buf,Char_t *type);
   void   GetMidasTID(ROMEString *buf,Char_t *type);
   void   setValue(ROMEString *buf,Char_t *destination,Char_t *source,Char_t *type,Int_t version);
   Bool_t IsNumber(const Char_t *type);
};
#endif   // ArgusBuilder_H
