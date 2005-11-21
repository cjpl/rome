/********************************************************************
  ArgusBuilder.h, R. Sawada

  $Id$

********************************************************************/
#ifndef ArgusBuilder_H
#define ArgusBuilder_H

#include <ROMEBuilder.h>
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

const Int_t maxNumberOfTabs                    =    200;
const Int_t maxNumberOfMenus                   =     20;
const Int_t maxNumberOfMenuItems               =     50;
const Int_t maxNumberOfThreadFunctions         =     10;
const Int_t maxNumberOfThreadFunctionArguments =     10;
const Int_t maxNumberOfNetFolders              =     10;
const Char_t* const LINE_TITLE = "NoDayWithoutItsLine";

class ArgusBuilder : public ROMEBuilder
{
public:
   ROMEString   argusVersion;
   
protected:
   Bool_t readTabs;

   Int_t recursiveTabDepth;
//   Int_t recursiveSteerDepth;
   Int_t recursiveMenuDepth;
   
// net folder
   Int_t      numOfNetFolder;
   ROMEString netFolderName[maxNumberOfNetFolders];
   ROMEString netFolderTitle[maxNumberOfNetFolders];
   ROMEString netFolderHost[maxNumberOfNetFolders];
   ROMEString netFolderPort[maxNumberOfNetFolders];
   ROMEString netFolderRoot[maxNumberOfNetFolders];

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

public:
   ArgusBuilder() {};
   virtual ~ArgusBuilder() {};

   Bool_t ReadXMLNetFolder();

   Bool_t WriteWindowCpp();
   Bool_t WriteWindowH();
   Bool_t ReadXMLTab();
   Bool_t ReadXMLMenu(Int_t currentNumberOfTabs);
   Bool_t WriteSteering(Int_t iTab);
   Bool_t WriteTabConfigWrite(ROMEString &buffer,Int_t parentIndex,ROMEString& pointer,Int_t tab);
   Bool_t WriteTabConfigClass(ROMEString &buffer,Int_t parentIndex,Int_t tab);
   Bool_t WriteTabCpp();
   Bool_t WriteTabH();
   Bool_t AddTab(ROMEString& buffer,Int_t& i);
   Bool_t AddMenuItems(ROMEString& buffer,Int_t i,Int_t j);
   Bool_t WriteMonitorCpp();
   Bool_t WriteMonitorH();
   Bool_t WriteConfigCpp();
   Bool_t WriteConfigH();
   Bool_t WriteMain();
   void   WriteMakefile();
   void   WriteHTMLDoku();
   void   WriteARGUSDictionary(ROMEString& buffer);
   void   WriteFrameworkDictionary(ROMEString& buffer);
   void   WriteTabDictionary(ROMEString& buffer);
   void   StartBuilder();
   void   GetMidasTID(ROMEString *buf,Char_t *type);
   Bool_t ReplaceHeader(const Char_t* filename,const Char_t* header,const Char_t* content,Int_t nspace = 0);
   Bool_t WriteFile(const Char_t* filename,const Char_t* content,Int_t nspace = 0);
   void   ROME2Argus(ROMEString &buffer);
};
#endif   // ArgusBuilder_H
