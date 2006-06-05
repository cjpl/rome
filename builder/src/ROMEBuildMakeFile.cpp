/********************************************************************

  ROMEBuildMakeFile.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include "RConfig.h"
#include "Riostream.h"
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include "TSystem.h"
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "ROMEBuilder.h"

void ROMEBuilder::AddIncludeDirectories()
{
   numOfIncludeDirectories = 5;
   if (hasFolderUserCode) numOfIncludeDirectories++;
   if (numOfDAQ > 0) numOfIncludeDirectories++;
   if (numOfDB > 0) numOfIncludeDirectories++;
   if (numOfTab >0) numOfIncludeDirectories++;
   if (hasTaskUserCode) numOfIncludeDirectories++;
#if defined( R__VISUAL_CPLUSPLUS )
   if (mysql) numOfIncludeDirectories++;
   if (midas) numOfIncludeDirectories++;
#endif
   includeDirectories = new ROMEStrArray(numOfIncludeDirectories);
   includeDirectories->AddFormatted("$(ROOTSYS)/include/");
   includeDirectories->AddFormatted("$(ROMESYS)/include/");
   includeDirectories->AddFormatted("$(ROMESYS)/argus/include/");
   includeDirectories->AddFormatted(".");
   includeDirectories->AddFormatted("./include/");
   if (numOfTab >0)
      includeDirectories->AddFormatted("./include/tabs/");
   if (hasTaskUserCode)
      includeDirectories->AddFormatted("./include/tasks/");
   includeDirectories->AddFormatted("./include/generated/");
   if (hasFolderUserCode) includeDirectories->AddFormatted("./include/folders/");
   if (numOfDAQ > 0) includeDirectories->AddFormatted("./include/daqs/");
   if (numOfDB > 0) includeDirectories->AddFormatted("./include/databases/");
#if defined( R__VISUAL_CPLUSPLUS )
   if (mysql) includeDirectories->AddFormatted("$(ROMESYS)/include/mysql/");
   if (midas) includeDirectories->AddFormatted("$(MIDASSYS)/include/");
#endif
}

void ROMEBuilder::AddRomeHeaders()
{
   romeHeaders = new ROMEStrArray(50);
   romeHeaders->Add("$(ROMESYS)/include/ROMEAnalyzer.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEEventLoop.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEMidasDAQ.h");
   romeHeaders->Add("$(ROMESYS)/include/mxml.h");
   romeHeaders->Add("$(ROMESYS)/include/ROME.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEConfig.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEDAQSystem.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEDataBaseDAQ.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMENoDAQSystem.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMENoDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEODBOfflineDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEODBOnlineDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEPath.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMERint.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMERomeDAQ.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEStopwatch.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEStr2DArray.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEStrArray.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEString.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMETask.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMETextDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMETree.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMETreeInfo.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEUtilities.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEXML.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEXMLDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/TArrayL64.h");
   romeHeaders->Add("$(ROMESYS)/include/strlcpy.h");
   romeHeaders->Add("$(ROMESYS)/include/TNetFolderServer.h");
   romeHeaders->Add("$(ROMESYS)/include/TNetFolder.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMENetFolderServer.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMENetFolder.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEConfigToForm.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToForm.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToFormFrame.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToFormElement.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToFormElementSignal.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToFormWindow.h");
   if (this->orca)
      romeHeaders->Add("$(ROMESYS)/include/ROMEOrcaDAQ.h");
   if (this->mysql)
      romeHeaders->Add("$(ROMESYS)/include/ROMEMySQL.h");
   if (this->pgsql)
      romeHeaders->Add("$(ROMESYS)/include/ROMEPgSQL.h");
   if (this->sqlite)
      romeHeaders->Add("$(ROMESYS)/include/ROMESQLite.h");
   if (this->sqlite3)
      romeHeaders->Add("$(ROMESYS)/include/ROMESQLite3.h");
   if (this->sql) {
      romeHeaders->Add("$(ROMESYS)/include/ROMESQL.h");
      romeHeaders->Add("$(ROMESYS)/include/ROMESQLDataBase.h");
   }
}

void ROMEBuilder::AddRomeDictHeaders()
{
   romeDictHeaders = new ROMEStrArray(21);
   romeLinkDefSuffix = new ROMEStrArray(21);
   if (!librome) {
      romeDictHeaders->Add("$(ROMESYS)/include/XMLToForm.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/XMLToFormFrame.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/XMLToFormElement.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/XMLToFormElementSignal.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/XMLToFormWindow.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEConfigToForm.h");
      romeLinkDefSuffix->Add("");
   }
   romeDictHeaders->Add("$(ROMESYS)/include/ROMEAnalyzer.h");
   romeLinkDefSuffix->Add("");
   romeDictHeaders->Add("$(ROMESYS)/include/ROMEEventLoop.h");
   romeLinkDefSuffix->Add("");
   romeDictHeaders->Add("$(ROMESYS)/include/ROMEMidasDAQ.h");
   romeLinkDefSuffix->Add("");
   if (!librome) {
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETask.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMERint.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETreeInfo.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/TNetFolder.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/TNetFolderServer.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMENetFolder.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMENetFolderServer.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/TArrayL64.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEString.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEStrArray.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEStr2DArray.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEConfig.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMENoDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEODBOfflineDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEODBOnlineDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETextDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEXMLDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEXML.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETree.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEPath.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEStopwatch.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEDAQSystem.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEDataBaseDAQ.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMENoDAQSystem.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMERomeDAQ.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEUtilities.h");
      romeLinkDefSuffix->Add("");
   }
   if (this->sql) {
      romeDictHeaders->Add("$(ROMESYS)/include/ROMESQLDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMESQL.h");
      romeLinkDefSuffix->Add("");
   }
   if (this->mysql) {
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEMySQL.h");
      romeLinkDefSuffix->Add("");
   }
   if (this->pgsql) {
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEPgSQL.h");
      romeLinkDefSuffix->Add("");
   }
   if (this->sqlite) {
      romeDictHeaders->Add("$(ROMESYS)/include/ROMESQLite.h");
      romeLinkDefSuffix->Add("");
   }
   if (this->sqlite3) {
      romeDictHeaders->Add("$(ROMESYS)/include/ROMESQLite3.h");
      romeLinkDefSuffix->Add("");
   }
}

void ROMEBuilder::AddRomeSources()
{
   romeSources = new ROMEStrArray(50);
   romeSources->Add("$(ROMESYS)/src/ROMEAnalyzer.cpp");
   romeSources->Add("$(ROMESYS)/src/ROMEEventLoop.cpp");
   romeSources->Add("$(ROMESYS)/src/ROMEMidasDAQ.cpp");
   if (!librome) {
      romeSources->Add("$(ROMESYS)/src/mxml.c");
      romeSources->Add("$(ROMESYS)/src/ROMEDAQSystem.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEDataBaseDAQ.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEODBOfflineDataBase.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEODBOnlineDataBase.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEPath.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMERomeDAQ.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMESplashScreen.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEStr2DArray.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEStrArray.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEString.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMETask.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMERint.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMETextDataBase.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEUtilities.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEXML.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEXMLDataBase.cpp");
      romeSources->Add("$(ROMESYS)/src/TArrayL64.cpp");
      romeSources->Add("$(ROMESYS)/src/strlcpy.c");
      romeSources->Add("$(ROMESYS)/src/TNetFolderServer.cpp");
      romeSources->Add("$(ROMESYS)/src/TNetFolder.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMENetFolderServer.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMENetFolder.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEConfigToForm.cpp");
      romeSources->Add("$(ROMESYS)/src/XMLToForm.cpp");
      romeSources->Add("$(ROMESYS)/src/XMLToFormWindow.cpp");
   }
   if (this->orca)
      romeSources->Add("$(ROMESYS)/src/ROMEOrcaDAQ.cpp");
   if (this->mysql)
      romeSources->Add("$(ROMESYS)/src/ROMEMySQL.cpp");
   if (this->pgsql)
      romeSources->Add("$(ROMESYS)/src/ROMEPgSQL.cpp");
   if (this->sqlite)
      romeSources->Add("$(ROMESYS)/src/ROMESQLite.cpp");
   if (this->sqlite3)
      romeSources->Add("$(ROMESYS)/src/ROMESQLite3.cpp");
   if (this->sql) {
      romeSources->Add("$(ROMESYS)/src/ROMESQL.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMESQLDataBase.cpp");
   }
   if (romeDictHeaders->GetEntriesFast()>0) {
      if (!librome)
         romeSources->Add("dict/ROMEDict.cpp");
      else
         romeSources->Add("dict/ROMESDict.cpp");
   }
}

void ROMEBuilder::AddArgusHeaders()
{
   argusHeaders = new ROMEStrArray(5);
   argusLinkDefSuffix = new ROMEStrArray(5);
   if (!librome) {
      argusHeaders->Add("$(ROMESYS)/argus/include/ArgusWindow.h");
      argusLinkDefSuffix->Add("");
      argusHeaders->Add("$(ROMESYS)/argus/include/ArgusTextDialog.h");
      argusLinkDefSuffix->Add("");
      argusHeaders->Add("$(ROMESYS)/argus/include/ArgusAnalyzerController.h");
      argusLinkDefSuffix->Add("");
      argusHeaders->Add("$(ROMESYS)/argus/include/ArgusTab.h");
      argusLinkDefSuffix->Add("");
      argusHeaders->Add("$(ROMESYS)/argus/include/ArgusHistoDisplay.h");
      argusLinkDefSuffix->Add("");
   }
}

void ROMEBuilder::AddArgusSources()
{
   argusSources = new ROMEStrArray(6);
   if (!librome) {
      argusSources->Add("$(ROMESYS)/argus/src/ArgusWindow.cpp");
      argusSources->Add("$(ROMESYS)/argus/src/ArgusTextDialog.cpp");
      argusSources->Add("$(ROMESYS)/argus/src/ArgusAnalyzerController.cpp");
      argusSources->Add("$(ROMESYS)/argus/src/ArgusTab.cpp");
      argusSources->Add("$(ROMESYS)/argus/src/ArgusHistoDisplay.cpp");
   }
   if (argusHeaders->GetEntriesFast()>0)
      argusSources->Add("dict/ARGUSDict.cpp");
}

void ROMEBuilder::AddGeneratedHeaders()
{
   int i;
   generatedHeaders = new ROMEStrArray(9+TMath::Max(numOfFolder,0)+TMath::Max(numOfTask,0)+TMath::Max(numOfTab,0));
   generatedHeaders->AddFormatted("include/generated/%sAnalyzer.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sEventLoop.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sWindow.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sConfig.h",shortCut.Data());
   if (readGlobalSteeringParameters)
      generatedHeaders->AddFormatted("include/generated/%sGlobalSteering.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sMidasDAQ.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sRomeDAQ.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sConfigToForm.h",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0) {
         if (folderUserCode[i]) {
            generatedHeaders->AddFormatted("include/generated/%s%s_Base.h",shortCut.Data(),folderName[i].Data());
         }
         else {
            generatedHeaders->AddFormatted("include/generated/%s%s.h",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      if (taskUserCode[i]) {
         generatedHeaders->AddFormatted("include/generated/%sT%s_Base.h",shortCut.Data(),taskName[i].Data());
      }
      else {
         generatedHeaders->AddFormatted("include/generated/%sT%s.h",shortCut.Data(),taskName[i].Data());
      }
   }
   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      generatedHeaders->AddFormatted("include/generated/%sT%s_Base.h",shortCut.Data(),tabName[i].Data());
   }
}

void ROMEBuilder::AddGeneratedDictHeaders()
{
   generatedDictHeaders = new ROMEStrArray(4);
   generatedLinkDefSuffix = new ROMEStrArray(4);
   generatedDictHeaders->AddFormatted("include/generated/%sWindow.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
   generatedDictHeaders->AddFormatted("include/generated/%sAnalyzer.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
   if (readGlobalSteeringParameters) {
      generatedDictHeaders->AddFormatted("include/generated/%sGlobalSteering.h",shortCut.Data());
      generatedLinkDefSuffix->Add("");
   }
   generatedDictHeaders->AddFormatted("include/generated/%sConfigToForm.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
   generatedDictHeaders->AddFormatted("include/generated/%sConfig.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
   generatedDictHeaders->AddFormatted("include/generated/%sEventLoop.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
   generatedDictHeaders->AddFormatted("include/generated/%sMidasDAQ.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
   generatedDictHeaders->AddFormatted("include/generated/%sRomeDAQ.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
}

void ROMEBuilder::AddGeneratedFolderDictHeaders()
{
   int i;
   generatedFolderDictHeaders = new ROMEStrArray(TMath::Max(numOfFolder,0));
   generatedFolderLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfFolder,0));
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0) {
         if (folderUserCode[i]) {
            generatedFolderDictHeaders->AddFormatted("include/generated/%s%s_Base.h",shortCut.Data(),folderName[i].Data());
            generatedFolderLinkDefSuffix->Add("");
         }
        else {
            generatedFolderDictHeaders->AddFormatted("include/generated/%s%s.h",shortCut.Data(),folderName[i].Data());
            generatedFolderLinkDefSuffix->Add("");
         }
      }
   }
}

void ROMEBuilder::AddGeneratedTaskDictHeaders()
{
   int i;
   generatedTaskDictHeaders = new ROMEStrArray(TMath::Max(numOfTask,0));
   generatedTaskLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfTask,0));
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      if (taskUserCode[i]) {
         generatedTaskDictHeaders->AddFormatted("include/generated/%sT%s_Base.h",shortCut.Data(),taskName[i].Data());
         generatedTaskLinkDefSuffix->Add("");
      }
      else {
         generatedTaskDictHeaders->AddFormatted("include/generated/%sT%s.h",shortCut.Data(),taskName[i].Data());
         generatedTaskLinkDefSuffix->Add("");
      }
   }
}

void ROMEBuilder::AddGeneratedTabDictHeaders()
{
   int i;
   generatedTabDictHeaders = new ROMEStrArray(TMath::Max(numOfTab,0));
   generatedTabLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfTab,0));
   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      generatedTabDictHeaders->AddFormatted("include/generated/%sT%s_Base.h",shortCut.Data(),tabName[i].Data());
      generatedTabLinkDefSuffix->Add("");
   }
}

void ROMEBuilder::AddGeneratedSources()
{
   generatedSources = new ROMEStrArray(10);
   generatedSources->AddFormatted("src/generated/main.cpp");
   generatedSources->AddFormatted("src/generated/%sAnalyzer.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sEventLoop.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sWindow.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sConfig.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sMidasDAQ.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sRomeDAQ.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sConfigToForm.cpp",shortCut.Data());
   if (generatedDictHeaders->GetEntriesFast()>0)
      generatedSources->AddFormatted("dict/%sGeneratedDict.cpp",shortCut.Data());
   if (generatedFolderDictHeaders->GetEntriesFast()>0)
      generatedSources->AddFormatted("dict/%sGeneratedFolderDict.cpp",shortCut.Data());
   if (generatedTaskDictHeaders->GetEntriesFast()>0)
      generatedSources->AddFormatted("dict/%sGeneratedTaskDict.cpp",shortCut.Data());
   if (generatedTabDictHeaders->GetEntriesFast()>0)
      generatedSources->AddFormatted("dict/%sGeneratedTabDict.cpp",shortCut.Data());
}

void ROMEBuilder::AddFolderHeaders()
{
   int i;
   folderHeaders = new ROMEStrArray(TMath::Max(numOfFolder,0));
   folderLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfFolder,0));
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0) {
         if (folderUserCode[i]) {
            folderHeaders->AddFormatted("include/folders/%s%s.h",shortCut.Data(),folderName[i].Data());
            folderLinkDefSuffix->Add("");
         }
      }
   }
}

void ROMEBuilder::AddFolderSources()
{
   int i;
   folderSources = new ROMEStrArray(numOfFolder+1);
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0) {
         if (folderUserCode[i]) {
            folderSources->AddFormatted("src/generated/%s%s_Base.cpp",shortCut.Data(),folderName[i].Data());
            folderSources->AddFormatted("src/folders/%s%s.cpp",shortCut.Data(),folderName[i].Data());
         }
         else {
            folderSources->AddFormatted("src/generated/%s%s.cpp",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   if (folderHeaders->GetEntriesFast()>0)
      folderSources->AddFormatted("dict/%sFolderDict.cpp",shortCut.Data());
}

void ROMEBuilder::AddTaskHeaders()
{
   int i;
   taskHeaders = new ROMEStrArray(TMath::Max(numOfTask,0));
   taskLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfTask,0));
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      if (taskUserCode[i]) {
         taskHeaders->AddFormatted("include/tasks/%sT%s.h",shortCut.Data(),taskName[i].Data());
         taskLinkDefSuffix->Add("");
      }
   }
}

void ROMEBuilder::AddTaskSources()
{
   int i;
   taskSources = new ROMEStrArray(numOfTask+1);
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      taskSources->AddFormatted("src/tasks/%sT%s.cpp",shortCut.Data(),taskName[i].Data());
   }
   if (taskHeaders->GetEntriesFast()>0)
      taskSources->AddFormatted("dict/%sTaskDict.cpp",shortCut.Data());
}

void ROMEBuilder::AddTabHeaders()
{
   int i;
   tabHeaders = new ROMEStrArray(TMath::Max(numOfTab,0));
   tabLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfTab,0));
   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      tabHeaders->AddFormatted("include/tabs/%sT%s.h",shortCut.Data(),tabName[i].Data());
      tabLinkDefSuffix->Add("");
   }
}

void ROMEBuilder::AddTabSources()
{
   int i;
   tabSources = new ROMEStrArray(numOfTab+1);
   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      tabSources->AddFormatted("src/tabs/%sT%s.cpp",shortCut.Data(),tabName[i].Data());
   }
   if (tabHeaders->GetEntriesFast()>0)
      tabSources->AddFormatted("dict/%sTabDict.cpp",shortCut.Data());
}

void ROMEBuilder::AddDAQHeaders()
{
   int i;
   daqHeaders = new ROMEStrArray(TMath::Max(numOfDAQ,0));
   for (i=0;i<numOfDAQ;i++) {
      if (!daqUsed[i])
         continue;
      daqHeaders->AddFormatted("include/daqs/%s%s.h",shortCut.Data(),daqName[i].Data());
   }
}

void ROMEBuilder::AddDAQSources()
{
   int i;
   daqSources = new ROMEStrArray(3+TMath::Max(numOfDAQ,0));
   for (i=0;i<numOfDAQ;i++) {
      if (!daqUsed[i])
         continue;
      daqSources->AddFormatted("src/daqs/%s%s.cpp",shortCut.Data(),daqName[i].Data());
   }
   if (daqHeaders->GetEntriesFast() > 0)
      daqSources->AddFormatted("dict/%sDAQDict.cpp", shortCut.Data());
}

void ROMEBuilder::AddDatabaseHeaders()
{
   int i;
   databaseHeaders = new ROMEStrArray(TMath::Max(numOfDB,0));
   for (i=0;i<numOfDB;i++) {
      databaseHeaders->AddFormatted("include/databases/%s%sDataBase.h",shortCut.Data(),dbName[i].Data());
   }
}

void ROMEBuilder::AddDatabaseSources()
{
   int i;
   databaseSources = new ROMEStrArray(TMath::Max(numOfDB,0));
   for (i=0;i<numOfDB;i++) {
      databaseSources->AddFormatted("src/databases/%s%sDataBase.cpp",shortCut.Data(),dbName[i].Data());
   }
   if (databaseHeaders->GetEntriesFast() > 0)
      databaseSources->AddFormatted("dict/%sDBDict.cpp", shortCut.Data());
}

void ROMEBuilder::AddRootLibraries()
{
   rootLibraries = new ROMEStrArray(30);
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\gdk-1.3.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\glib-1.3.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libCore.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libCint.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libHist.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libGraf.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libGraf3d.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libGpad.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libTree.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libRint.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libPostscript.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libMatrix.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libPhysics.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libGui.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libHtml.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libWin32gdk.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libThread.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libMinuit.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libGeom.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libGeomPainter.lib");
}

void ROMEBuilder::AddMysqlLibraries()
{
#if defined( R__VISUAL_CPLUSPLUS )
   mysqlLibraries = new ROMEStrArray(3);
   mysqlLibraries->AddFormatted("$(ROMESYS)\\lib_win\\libmySQL.lib");
   mysqlLibraries->AddFormatted("$(ROMESYS)\\lib_win\\mysqlclient.lib");
   mysqlLibraries->AddFormatted("$(ROMESYS)\\lib_win\\mysys.lib");
#endif
}

void ROMEBuilder::AddDAQLibraries()
{
   daqLibraries = new ROMEStrArray(2);
#if defined( R__VISUAL_CPLUSPLUS )
   if (this->midas)
      daqLibraries->AddFormatted("$(ROMESYS)\\lib_win\\midas.lib");
#endif
#if defined( R__UNIX )
#if defined( R__ALPHA )
   if (this->midas)
      daqLibraries->AddFormatted("-L$(MIDASSYS)/osf1/lib -lmidas");
#elif defined( R__SGI )
   if (this->midas)
      daqLibraries->AddFormatted("-L$(MIDASSYS)/ultrix/lib -lmidas");
#elif defined( R__FBSD )
   if (this->midas)
      daqLibraries->AddFormatted("-L$(MIDASSYS)/freeBSD/lib -lmidas");
#elif defined( R__MACOSX )
   if (this->midas)
      daqLibraries->AddFormatted("-L$(MIDASSYS)/darwin/lib -lmidas");
#elif defined( R__LINUX )
   if (this->midas)
      daqLibraries->AddFormatted("-L$(MIDASSYS)/linux/lib -lmidas");
#elif defined( R__SOLARIS )
   if (this->midas)
      daqLibraries->AddFormatted("-L$(MIDASSYS)/solaris/lib -lmidas");
#else
   if (this->midas)
      daqLibraries->AddFormatted("-lmidas");
#endif
#endif
}

void ROMEBuilder::AddDAQFlags()
{
   daqFlags = new ROMEStrArray(2);
#if defined( R__VISUAL_CPLUSPLUS )
   if (this->midas)
      daqFlags->AddFormatted("/DHAVE_MIDAS");
   if (this->orca)
      daqFlags->AddFormatted("/DHAVE_ORCA");
#endif
#if defined( R__UNIX )
#if defined( R__ALPHA )
   if (this->midas)
      daqFlags->AddFormatted("-DOSF1 -I$(MIDASSYS)/include/ -DHAVE_MIDAS");
#elif defined( R__SGI )
   if (this->midas)
      daqFlags->AddFormatted("-DOS_ULTRIX -DNO_PTY -I$(MIDASSYS)/include/ -DHAVE_MIDAS");
#elif defined( R__FBSD )
   if (this->midas)
      daqFlags->AddFormatted("-DOS_FREEBSD -I$(MIDASSYS)/include/ -DHAVE_MIDAS");
#elif defined( R__MACOSX )
   if (this->midas)
      daqFlags->AddFormatted("-DOS_LINUX -DOS_DARWIN -DHAVE_STRLCPY -I$(MIDASSYS)/include/ -DHAVE_MIDAS");
#elif defined( R__LINUX )
   if (this->midas)
      daqFlags->AddFormatted("-DOS_LINUX -I$(MIDASSYS)/include/ -DHAVE_MIDAS");
#elif defined( R__SOLARIS )
   if (this->midas)
      daqFlags->AddFormatted("-DOS_SOLARIS -I$(MIDASSYS)/include/ -DHAVE_MIDAS");
#else
   if (this->midas)
      daqFlags->AddFormatted("-I$(MIDASSYS)/include/ -DHAVE_MIDAS");
#endif
#endif
}

void ROMEBuilder::WriteMakefileHeader(ROMEString& buffer)
{
   ROMEString tmp;
   buffer.AppendFormatted("##############################################################\n");
   buffer.AppendFormatted("## *** This file will be overwritten by the ROMEBuilder *** ##\n");
   buffer.AppendFormatted("## ***      Don't make manual changes to this file      *** ##\n");
   buffer.AppendFormatted("##############################################################\n");
   buffer.AppendFormatted("#\n");
   buffer.AppendFormatted("# make              : compile executable\n");
   buffer.AppendFormatted("# make build        : execute romebuilder\n");
   buffer.AppendFormatted("# make so           : build shared library\n");
   buffer.AppendFormatted("# make dep          : make depend files\n");
   buffer.AppendFormatted("# make -k clean     : remove intermediate files\n");
   buffer.AppendFormatted("# make -k distclean : remove following directries and files\n");
   buffer.AppendFormatted("#                     src/generated include/generated obj dict Makefile\n");
   buffer.AppendFormatted("# make -k depclean  : remove depend files (obj/*.d)\n");
   buffer.AppendFormatted("# make -k %sclean   : remove %s specific intermediate files\n", shortCut.ToLower(tmp), shortCut.Data());
   buffer.AppendFormatted("\n");
}

void ROMEBuilder::WriteMakefileLibsAndFlags(ROMEString& buffer)
{
   int i,j;
   ROMEString tmp;
#if defined( R__VISUAL_CPLUSPLUS )
   // libs
   buffer.AppendFormatted("rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libWin32gdk.lib");
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   buffer.AppendFormatted(" $(ROOTSYS)/lib/libThread.lib \n");
#endif // ROOT_VERSION
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("sqllibs =");
   if (this->mysql)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libmySQL.lib $(ROMESYS)/lib_win/mysys.lib $(ROMESYS)/lib_win/mysqlclient.lib");
   if (this->pgsql)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libpq.lib");
   if (this->sqlite)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/sqlite.lib");
   if (this->sqlite3)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/sqlite3.lib");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("daqlibs =");
   for (i=0;i<daqLibraries->GetEntriesFast();i++) {
      buffer.AppendFormatted(" %s",daqLibraries->At(i).Data());
   }
   buffer.AppendFormatted("\n\n");
   buffer.AppendFormatted("clibs = wsock32.lib gdi32.lib user32.lib kernel32.lib\n");
   buffer.AppendFormatted("Libraries = $(rootlibs) $(clibs) $(sqllibs) $(daqlibs)\n");
   bool addLib = true;
   bool flagMatched = false;
   int k;
   for (i=0;i<numOfMFWinLibs;i++) {
      addLib = true;
      for (j=0;j<numOfMFWinLibFlags[i];j++) {
         addLib = false;
         flagMatched = false;
         for (k=0;k<flags.GetEntriesFast();k++) {
            if (mfWinLibFlag[i][j]==flags.At(k)) {
               flagMatched = true;
               break;
            }
         }
         if (flagMatched) {
            addLib = true;
            break;
         }
      }
      if (addLib)
         buffer.AppendFormatted("Libraries = $(Libraries) %s\n",mfWinLibName[i].Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   // flags
   buffer.AppendFormatted("Flags = /GX /GR /MD $(%suserflags)",shortCut.ToLower(tmp));
   for (i=0;i<daqFlags->GetEntriesFast();i++) {
      buffer.AppendFormatted(" %s",daqFlags->At(i).Data());
   }
   if (this->sql)
      buffer.AppendFormatted(" /DHAVE_SQL");
   if (this->mysql)
      buffer.AppendFormatted(" /DHAVE_MYSQL");
   if (this->pgsql)
      buffer.AppendFormatted(" /DHAVE_PGSQL");
   if (this->sqlite)
      buffer.AppendFormatted(" /DHAVE_SQLITE");
   if (this->sqlite3)
      buffer.AppendFormatted(" /DHAVE_SQLITE3");
   for (i=0;i<flags.GetEntriesFast();i++)
      buffer.AppendFormatted(" /D%s",flags.At(i).Data());
   for (i=0;i<affiliations.GetEntriesFast();i++)
      buffer.AppendFormatted(" /DHAVE_%s",((ROMEString)affiliations.At(i)).ToUpper(tmp));
   for (i=0;i<numOfMFPreDefs;i++)
      buffer.AppendFormatted(" /D%s",mfPreDefName[i].Data());
   buffer.AppendFormatted("\n");
   // fortran flags
   buffer.AppendFormatted("FortranFlags = $(%suserflags)\n",shortCut.ToLower(tmp));
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
   buffer.AppendFormatted("CXX := g++\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("rootlibs := $(shell $(ROOTSYS)/bin/root-config --libs)\n");
   buffer.AppendFormatted("rootglibs := $(shell  $(ROOTSYS)/bin/root-config --glibs)\n");
   buffer.AppendFormatted("rootcflags := $(shell  $(ROOTSYS)/bin/root-config --cflags)\n");
   buffer.AppendFormatted("rootthreadlibs := -lThread\n");
   buffer.AppendFormatted("sqllibs :=");
   if (this->mysql)
      buffer.AppendFormatted(" $(shell mysql_config --libs)");
   if (this->pgsql)
      buffer.AppendFormatted(" -L$(shell pg_config --libdir) -lpq");
   if (this->sqlite)
      buffer.AppendFormatted(" -lsqlite");
   if (this->sqlite3)
      buffer.AppendFormatted(" -lsqlite3");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("sqlcflags :=");
   if (this->sql)
      buffer.AppendFormatted(" -DHAVE_SQL");
   if (this->mysql)
      buffer.AppendFormatted(" $(shell mysql_config --include) -DHAVE_MYSQL");
   if (this->pgsql)
      buffer.AppendFormatted(" -I$(shell pg_config --includedir) -DHAVE_PGSQL");
   if (this->sqlite)
      buffer.AppendFormatted(" -DHAVE_SQLITE");
   if (this->sqlite3)
      buffer.AppendFormatted(" -DHAVE_SQLITE3");
   buffer.AppendFormatted("\n");
#if defined( R__ALPHA )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lc -lbsd\n");
   buffer.AppendFormatted("soflags := -Wl,-expect_unresolved,* -shared\n");
#elif defined( R__SGI )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs :=\n");
   buffer.AppendFormatted("soflags := -shared\n");
#elif defined( R__FBSD )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lbsd -lcompat\n");
   buffer.AppendFormatted("soflags := -shared -Wl,-x\n");
#elif defined( R__MACOSX )
   buffer.AppendFormatted("FINK_DIR := $(shell which fink 2>&1 | sed -ne \"s/\\/bin\\/fink//p\")\n");
   buffer.AppendFormatted("MACOSX_MAJOR := $(shell sw_vers | sed -n 's/ProductVersion:[^0-9]*//p' | cut -d . -f 1)\n");
   buffer.AppendFormatted("MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion:[^0-9]*//p' | cut -d . -f 2)\n");
   buffer.AppendFormatted("MACOSX_DEPLOYMENT_TARGET := $(MACOSX_MAJOR).$(MACOSX_MINOR)\n");
   buffer.AppendFormatted("MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=$(MACOSX_MAJOR).$(MACOSX_MINOR)\n");
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function  $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)\n");
   buffer.AppendFormatted("oslibs := -bind_at_load -multiply_defined suppress $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib)\n");
   buffer.AppendFormatted("ifeq ($(MACOSX_DEPLOYMENT_TARGET),10.1)\n");
   buffer.AppendFormatted("soflags := -dynamiclib -single_module -undefined suppress\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("ifeq ($(MACOSX_DEPLOYMENT_TARGET),10.2)\n");
   buffer.AppendFormatted("soflags := -dynamiclib -single_module -undefined suppress\n");
   buffer.AppendFormatted("else\n");
   buffer.AppendFormatted("soflags := -dynamiclib -single_module -undefined dynamic_lookup\n");
   buffer.AppendFormatted("endif\n");
#elif defined( R__LINUX )
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function\n");
   buffer.AppendFormatted("oslibs := -lutil\n");
   buffer.AppendFormatted("soflags := -shared -Wl\n");
#elif defined( R__SOLARIS )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lsocket -lnsl\n");
   buffer.AppendFormatted("soflags := -G\n");
#else
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs :=\n");
   buffer.AppendFormatted("soflags := -shared\n");
#endif
   buffer.AppendFormatted("daqlibs := \n");
   buffer.AppendFormatted("daqcflags := \n");
   // DAQ Flaqs
   for (i=0;i<daqFlags->GetEntriesFast();i++) {
      buffer.AppendFormatted("daqcflags += %s\n",daqFlags->At(i).Data());
   }
   // DAQ Libraries
   for (i=0;i<daqLibraries->GetEntriesFast();i++) {
      buffer.AppendFormatted("daqlibs += %s\n",daqLibraries->At(i).Data());
   }
   buffer.AppendFormatted("clibs := -lHtml -lz $(SYSLIBS)");
   if (haveFortranTask)
      buffer.AppendFormatted(" -lg2c");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   // libs
   if (librome)
      buffer.AppendFormatted("Libraries := -L$(ROMESYS) -lrome\n");
   else
      buffer.AppendFormatted("Libraries :=\n");
   buffer.AppendFormatted("Libraries += $(oslibs) $(sqllibs) $(daqlibs) $(rootglibs) $(rootthreadlibs) $(clibs)\n");
   for (i=0;i<numOfMFUnixLibs;i++) {
      for (j=0;j<numOfMFUnixLibFlags[i];j++)
         buffer.AppendFormatted("ifdef %s\n",mfUnixLibFlag[i][j].Data());
      buffer.AppendFormatted("Libraries += -l%s\n",mfUnixLibName[i].Data());
      for (j=0;j<numOfMFUnixLibFlags[i];j++)
         buffer.AppendFormatted("endif # %s\n",mfUnixLibFlag[i][j].Data());
   }
   buffer.AppendFormatted("\n");
   // flags
   buffer.AppendFormatted("Flags := $(%suserflags) $(oscflags) $(rootcflags) $(sqlcflags) $(daqcflags)",shortCut.ToLower(tmp));
   for (i=0;i<numOfMFPreDefs;i++)
      buffer.AppendFormatted(" -D%s",mfPreDefName[i].Data());
   buffer.AppendFormatted("\n");
#endif // R__UNIX
}

void ROMEBuilder::WriteMakefileIncludes(ROMEString& buffer)
{
   int i;
   buffer.AppendFormatted("Includes %s",kEqualSign);
   for (i=0;i<includeDirectories->GetEntriesFast();i++)
      buffer.AppendFormatted(" %sI%s",kFlagSign,includeDirectories->At(i).Data());
   for (i=0;i<numOfMFIncDirs;i++)
      buffer.AppendFormatted(" %sI%s",kFlagSign,mfIncDir[i].Data());
#if defined( R__VISUAL_CPLUSPLUS )
   if (this->midas)
      buffer.AppendFormatted(" /I$(MIDASSYS)/include/");
   if (this->mysql)
      buffer.AppendFormatted(" /I$(ROMESYS)/include/mysql/");
   if (this->sqlite)
      buffer.AppendFormatted(" /I$(ROMESYS)/include/sqlite/");
   if (this->sqlite3)
      buffer.AppendFormatted(" /I$(ROMESYS)/include/sqlite3/");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
}

void ROMEBuilder::WriteMakefileObjects(ROMEString& buffer,ROMEStrArray* sources)
{
   int i;
   ROMEString path;
   ROMEString fileName;
   ROMEString fileExtension;
   if (sources->GetEntriesFast()>0) {
      buffer.AppendFormatted("objects %s $(objects)",kEqualSign);
      for (i=0;i<sources->GetEntriesFast();i++) {
         AnalyzeFileName(sources->At(i).Data(),path,fileName,fileExtension);
         buffer.AppendFormatted(" obj/%s.obj",fileName.Data());
      }
      buffer.AppendFormatted("\n");
   }
}

void ROMEBuilder::WriteMakefileUserDictObject(ROMEString& buffer)
{
#if defined( R__UNIX )
   int i;
   bool haveDict = false;
   for (i=0;i<numOfMFDictHeaders;i++) {
      if (mfDictHeaderUsed[i]) {
         haveDict = true;
         break;
      }
   }
   if (!haveDict) {
      buffer.AppendFormatted("ifdef DictionaryHeaders\n");
      buffer.AppendFormatted("objects += obj/%sUserDict.obj\n",shortCut.Data());
      buffer.AppendFormatted("endif\n");
   }
   else{
      buffer.AppendFormatted("objects += obj/%sUserDict.obj\n",shortCut.Data());
   }
#else
   buffer.AppendFormatted("objects = $(objects) obj/%sUserDict.obj\n",shortCut.Data());
#endif // R__UNIX
   buffer.AppendFormatted("\n");
}

void ROMEBuilder::WriteMakefileUserDictDependFiles(ROMEString& buffer)
{
#if defined( R__UNIX )
   int i;
   bool haveDict = false;
   for (i=0;i<numOfMFDictHeaders;i++) {
      if (mfDictHeaderUsed[i]) {
         haveDict = true;
         break;
      }
   }
   if (!haveDict) {
      buffer.AppendFormatted("ifdef DictionaryHeaders\n");
      buffer.AppendFormatted("dependfiles += obj/%sUserDict.d\n",shortCut.Data());
      buffer.AppendFormatted("endif\n");
   }
   else{
      buffer.AppendFormatted("dependfiles += obj/%sUserDict.d\n",shortCut.Data());
   }
#endif // R__UNIX
   buffer.AppendFormatted("\n");
}

void ROMEBuilder::WriteMakefileDictionary(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers,const char* linkDefName)
{
   if (!headers->GetEntriesFast())
      return;
   ROMEString str;
   ROMEString tmp;
   int i;
   // depend file
#if defined( R__UNIX )
   buffer.AppendFormatted("obj/%sionary.d:dict/%s.h\n",dictionaryName,dictionaryName);
   buffer.AppendFormatted("\t$(CXX) $(Flags) $(Includes) -M -MF $@ -MT dict/%s.cpp src/generated/%sDummy.cpp\n",dictionaryName,dictionaryName);
#endif
   //dummy source file
   WriteMakefileDictDummyCpp(dictionaryName);

   buffer.AppendFormatted("dict/%s.h dict/%s.cpp:",dictionaryName,dictionaryName);
   for (i=0;i<headers->GetEntriesFast();i++) {
      buffer.AppendFormatted(" %s",headers->At(i).Data());
   }
   if (linkDefName)
      buffer.AppendFormatted(" %s",linkDefName);
   buffer.AppendFormatted(" $(%sDep)\n", dictionaryName);
   buffer.AppendFormatted("\t@echo creating %s\n",dictionaryName);
   WriteRootCintCall(buffer);
   buffer.AppendFormatted(" -f dict/%s.cpp -c -p",dictionaryName);
   for (i=0;i<affiliations.GetEntriesFast();i++)
      buffer.AppendFormatted(" -DHAVE_%s",((ROMEString)affiliations.At(i)).ToUpper(tmp));
   for (i=0;i<includeDirectories->GetEntriesFast();i++) {
      str = includeDirectories->At(i).Data();
#if defined( R__VISUAL_CPLUSPLUS )
      str.ReplaceAll("$(","%");
      str.ReplaceAll(")","%");
#endif
      buffer.AppendFormatted(" -I%s",str.Data());
   }
   for (i=0;i<numOfMFDictIncDirs;i++)
      buffer.AppendFormatted(" -I%s",mfDictIncDir[i].Data());
   buffer.AppendFormatted(" $(DictionaryIncludes)");
   for (i=0;i<headers->GetEntriesFast();i++) {
      buffer.AppendFormatted(" %s",headers->At(i).Data());
   }
   if (linkDefName)
      buffer.AppendFormatted(" %s",linkDefName);
   buffer.Append("\n\n");
}

void ROMEBuilder::WriteMakefileDictDummyCpp(const char* dictionaryName)
{

   ROMEString cppFile;
   cppFile.SetFormatted("src/generated/%sDummy.cpp", dictionaryName);
   ROMEString buffer;
   buffer.SetFormatted("#include \"dict/%s.h\"\n", dictionaryName);
   WriteFile(cppFile.Data(),buffer.Data(),6);
}

void ROMEBuilder::WriteMakefileUserDictionary(ROMEString& buffer)
{
   int i;
   ROMEString str;
   ROMEString tmp;
   ROMEString dictionaryName;
   dictionaryName.SetFormatted("%sUserDict",shortCut.Data());

   // dictionary depend file
   buffer.AppendFormatted("obj/%sUserDictionary.d:dict/%sUserDict.h\n",shortCut.Data(),shortCut.Data());
#if defined( R__UNIX )
   buffer.AppendFormatted("\t$(CXX) $(Flags) $(Includes) -M -MF $@ -MT dict/%sUserDict.cpp src/generated/%sUserDictDummy.cpp\n",shortCut.Data(),shortCut.Data());
#endif
   WriteMakefileDictDummyCpp(dictionaryName.Data());

   buffer.AppendFormatted("dict/%sUserDict.h dict/%sUserDict.cpp:",shortCut.Data(),shortCut.Data());
   for (i=0;i<numOfMFDictHeaders;i++) {
      if (!mfDictHeaderUsed[i])
         continue;
      buffer.AppendFormatted(" %s",mfDictHeaderName[i].Data());
   }
   buffer.AppendFormatted(" $(DictionaryHeaders)\n");
   WriteRootCintCall(buffer);
   buffer.AppendFormatted(" -f dict/%sUserDict.cpp -c -p",shortCut.Data());
   for (i=0;i<affiliations.GetEntriesFast();i++)
      buffer.AppendFormatted(" -DHAVE_%s",((ROMEString)affiliations.At(i)).ToUpper(tmp));
   for (i=0;i<includeDirectories->GetEntriesFast();i++) {
      str = includeDirectories->At(i).Data();
#if defined( R__VISUAL_CPLUSPLUS )
      str.ReplaceAll("$(","%");
      str.ReplaceAll(")","%");
#endif
      buffer.AppendFormatted(" -I%s",str.Data());
   }
   buffer.AppendFormatted(" $(DictionaryIncludes)");
   for (i=0;i<numOfMFDictIncDirs;i++)
      buffer.AppendFormatted(" -I%s",mfDictIncDir[i].Data());
   buffer.AppendFormatted(" $(DictionaryHeaders)");
   for (i=0;i<numOfMFDictHeaders;i++) {
      if (!mfDictHeaderUsed[i])
         continue;
      buffer.AppendFormatted(" %s",mfDictHeaderName[i].Data());
   }
   buffer.Append("\n\n");
}

void ROMEBuilder::WriteMakefileCompileStatements(ROMEString& buffer,ROMEStrArray* sources)
{
   int i;
   ROMEString path;
   ROMEString name;
   ROMEString ext;

   for (i=0;i<sources->GetEntriesFast();i++) {
      AnalyzeFileName(sources->At(i).Data(),path,name,ext);
      path.ReplaceAll("\\","/");
#if defined( R__UNIX )
      if (path.Index("/dict/")!=-1 || path.Index("dict/")==0)
         buffer.AppendFormatted("obj/%s.d: ./dict/%s.cpp\n",name.Data(),name.Data());
      else
         buffer.AppendFormatted("obj/%s.d: %s\n",name.Data(),sources->At(i).Data());
      buffer.AppendFormatted("\t$(CXX) $(Flags) $(Includes) -M -MF $@ -MT obj/%s.obj $<\n",name.Data());
      buffer.AppendFormatted("obj/%s.obj: %s $(%sDep)\n",name.Data(),sources->At(i).Data(),name.Data(),name.Data());
      buffer.AppendFormatted("\t$(CXX) -c $(Flags) $(%sOpt) $(Includes) %s -o obj/%s.obj\n",name.Data(),sources->At(i).Data(),name.Data());
#endif // R__UNIX
#if defined( R__VISUAL_CPLUSPLUS )
      int j;
      ROMEString batFileName;
      ROMEString batBuffer;
      ROMEString dFileName;
      ROMEString dBuffer= "\n";
      ROMEString str;
      path.ReplaceAll("$(","%");
      path.ReplaceAll(")","%");
      if (path.Index("/dict/")!=-1 || path.Index("dict/")==0)
         buffer.AppendFormatted("obj/%s.obj: dict/%s.cpp\n",name.Data(),name.Data(),name.Data());
      else {
         buffer.AppendFormatted("!INCLUDE obj/%s.d\n",name.Data());
         buffer.AppendFormatted("obj/%s.obj: %s\n",name.Data(),sources->At(i).Data(),name.Data());
      }
      if (!(path.Index("/dict/")!=-1 || path.Index("dict/")==0)) {
         buffer.AppendFormatted("\t@cd obj/\n",name.Data());
         buffer.AppendFormatted("\t@start /MIN %s\n",name.Data());
         buffer.AppendFormatted("\t@cd ../\n",name.Data());
      }
      buffer.AppendFormatted("\t@cl /nologo /c $(Flags) $(Includes) %s /Foobj/%s.obj\n",sources->At(i).Data(),name.Data());
      if (!(path.Index("/dict/")!=-1 || path.Index("dict/")==0)) {
         batBuffer.AppendFormatted("cd ../\n");
         batBuffer.AppendFormatted("cd %s\n",path.Data());
         batBuffer.AppendFormatted("rmkdepend");
         for (j=0;j<includeDirectories->GetEntriesFast();j++) {
            str = includeDirectories->At(j).Data();
            str.ReplaceAll("$(","%");
            str.ReplaceAll(")","%");
            batBuffer.AppendFormatted(" -I%s",str.Data());
         }
         for (j=0;j<numOfMFIncDirs;j++) {
            str = mfIncDir[j].Data();
            str.ReplaceAll("$(","%");
            str.ReplaceAll(")","%");
            batBuffer.AppendFormatted(" -I%s",str.Data());
         }
         batBuffer.AppendFormatted(" -f %sobj/%s.d -o .obj -p obj/ %s.%s\n",outDir.Data(),name.Data(),name.Data(),ext.Data());
         batBuffer.AppendFormatted("ReplaceInFile -f %sobj/%s.d -s ./ -r %s\n",outDir.Data(),name.Data(),path.Data());
         batBuffer.AppendFormatted("ReplaceInFile -f %sobj/%s.d -s \" *.h\" -r \" %s/*.h\" -v \" /\\\"\n",outDir.Data(),name.Data(),path.Data());
         batBuffer.AppendFormatted("cd %sobj\n",outDir.Data());
         batBuffer.AppendFormatted("exit\n",outDir.Data());
         batFileName.SetFormatted("%sobj/%s.bat",outDir.Data(),name.Data());
         WriteFile(batFileName.Data(),batBuffer.Data(),6);
         dFileName.SetFormatted("%sobj/%s.d",outDir.Data(),name.Data());
         if (gSystem->AccessPathName(dFileName.Data(),kFileExists))
            WriteFile(dFileName.Data(),dBuffer.Data(),6);
      }
#endif // R__VISUAL_CPLUSPLUS
   }
}

void ROMEBuilder::WriteMakefileDependFiles(ROMEString& buffer,ROMEStrArray* sources)
{
#if defined( R__UNIX )
   int i;
   ROMEString path;
   ROMEString name;
   ROMEString ext;

   for (i=0;i<sources->GetEntriesFast();i++) {
      AnalyzeFileName(sources->At(i).Data(),path,name,ext);
      buffer.AppendFormatted("dependfiles += obj/%s.d\n",name.Data());
   }
#endif // R__UNIX
}

void ROMEBuilder::WriteMakefileAdditionalSourceFilesObjects(ROMEString& buffer)
{
   // Write Additional Source Files Objects
   int i,j,k;
   ROMEString path;
   ROMEString name;
   ROMEString ext;

   for (i=0;i<numOfMFSources;i++) {
      if (!mfSourceFileUsed[i])
         continue;
      bool *commandLineFlag = new bool[numOfMFSourceFlags[i]];
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         commandLineFlag[j] = false;
         for (k=0;k<flags.GetEntriesFast();k++) {
            if (mfSourceFileFlag[i][j]==flags[k])
               commandLineFlag[j] = true;
         }
      }
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!IFDEF %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("ifdef %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      AnalyzeFileName(mfSourceFileName[i].Data(),path,name,ext);
      buffer.AppendFormatted("objects %s $(objects) obj/%s.obj\n",kEqualSign,name.Data());
      for (j=numOfMFSourceFlags[i]-1;j>=0;j--) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!ENDIF # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("endif # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      delete [] commandLineFlag;
   }
}

void ROMEBuilder::WriteMakefileAdditionalSourceDependFiles(ROMEString& buffer)
{
   // Write Additional Source Files Objects
   int i,j,k;
   ROMEString path;
   ROMEString name;
   ROMEString ext;

   for (i=0;i<numOfMFSources;i++) {
      if (!mfSourceFileUsed[i])
         continue;
      bool *commandLineFlag = new bool[numOfMFSourceFlags[i]];
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         commandLineFlag[j] = false;
         for (k=0;k<flags.GetEntriesFast();k++) {
            if (mfSourceFileFlag[i][j]==flags[k])
               commandLineFlag[j] = true;
         }
      }
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!IFDEF %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("ifdef %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      AnalyzeFileName(mfSourceFileName[i].Data(),path,name,ext);
#if defined( R__UNIX )
      buffer.AppendFormatted("dependfiles += obj/%s.d\n",name.Data());
#endif // R__UNIX
      for (j=numOfMFSourceFlags[i]-1;j>=0;j--) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!ENDIF # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("endif # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      delete [] commandLineFlag;
   }
}

void ROMEBuilder::WriteMakefileAdditionalSourceFilesCompileStatments(ROMEString& buffer)
{
   // Write Additional Source Files Compile Commands
   int i,j,k;
   for (i=0;i<numOfMFSources;i++) {
      if (!mfSourceFileUsed[i])
         continue;
      bool *commandLineFlag = new bool[numOfMFSourceFlags[i]];
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         commandLineFlag[j] = false;
         for (k=0;k<flags.GetEntriesFast();k++) {
            if (mfSourceFileFlag[i][j]==flags[k])
               commandLineFlag[j] = true;
         }
      }
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!IFDEF %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("ifdef %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      ROMEStrArray *tempArray = new ROMEStrArray(1);
      tempArray->AddFormatted("%s%s",mfSourceFilePath[i].Data(),mfSourceFileName[i].Data());
      WriteMakefileCompileStatements(buffer,tempArray);
      delete tempArray;
      for (j=numOfMFSourceFlags[i]-1;j>=0;j--) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!ENDIF # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("endif # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      delete [] commandLineFlag;
   }
}

void ROMEBuilder::WriteMakefileBuildRule(ROMEString& buffer,const char *builder)
{
   ROMEString xmlbasename = gSystem->BaseName(xmlFile);

   buffer.AppendFormatted("build: \n");
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
   buffer += builder;
   buffer.AppendFormatted(" -i %s -o .", xmlbasename.Data());
   if (makeOutput)
      buffer.AppendFormatted(" -v");
   if (noLink)
      buffer.AppendFormatted(" -nl");
   if (orca)
      buffer.AppendFormatted(" -orca");
   if (midas)
      buffer.AppendFormatted(" -midas");
   if (mysql)
      buffer.AppendFormatted(" -mysql");
   if (pgsql)
      buffer.AppendFormatted(" -pgsql");
   if (sqlite)
      buffer.AppendFormatted(" -sqlite");
   if (sqlite3)
      buffer.AppendFormatted(" -sqlite3");
   if (flags.GetEntriesFast())
      buffer.AppendFormatted(" -f");
   int i;
   for (i=0;i<flags.GetEntriesFast();i++)
      buffer.AppendFormatted(" %s",flags.At(i).Data());
   if (affiliations.GetEntriesFast())
      buffer.AppendFormatted(" -a");
   for (i=0;i<affiliations.GetEntriesFast();i++)
      buffer.AppendFormatted(" %s",affiliations.At(i).Data());
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted(" -nvp");
#endif
   buffer.AppendFormatted("\n");
}

void ROMEBuilder::WriteMakefile() {
   // write a Makefile
   ROMEString buffer;
   ROMEString tmp,tmp2,tmp3,tmp4;

   AddIncludeDirectories();
   AddRomeHeaders();
   AddRomeDictHeaders();
   AddArgusHeaders();
   AddGeneratedHeaders();
   AddGeneratedDictHeaders();
   AddGeneratedFolderDictHeaders();
   AddGeneratedTaskDictHeaders();
   AddGeneratedTabDictHeaders();
   AddFolderHeaders();
   AddTaskHeaders();
   AddTabHeaders();
   AddDAQHeaders();
   AddDatabaseHeaders();
   AddRomeSources();
   AddArgusSources();
   AddGeneratedSources();
   AddFolderSources();
   AddTaskSources();
   AddTabSources();
   AddDAQSources();
   AddDatabaseSources();
   AddRootLibraries();
   AddMysqlLibraries();
   AddDAQLibraries();
   AddDAQFlags();

   WriteMakefileHeader(buffer);
   WriteMakefileLibsAndFlags(buffer);
   WriteMakefileIncludes(buffer);

   buffer.AppendFormatted("\n");
   // root cint headers
   buffer.AppendFormatted("DictionaryHeaders %s\n",kEqualSign);
   buffer.AppendFormatted("\n");

   // root cint includes
   buffer.AppendFormatted("DictionaryIncludes %s",kEqualSign);
#if defined( R__VISUAL_CPLUSPLUS )
   if (this->midas)
      buffer.AppendFormatted(" -I$(MIDASSYS)/include/");
#else
   if (this->midas)
      buffer.AppendFormatted(" -I$(MIDASSYS)/include -DHAVE_MIDAS");
   if (this->sql)
      buffer.AppendFormatted(" $(sqlcflags)");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("LD_LIBRARY_PATH=$(ROOTSYS)/lib\n");
#endif
   buffer.AppendFormatted("\n");

// all
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("all:startecho obj %s%s.exe endecho",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2));
#else
   buffer.AppendFormatted("all:startecho obj dep %s%s.exe endecho",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2));
#endif
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

// Objects
// -------
   WriteMakefileObjects(buffer,romeSources);
   WriteMakefileObjects(buffer,argusSources);
   WriteMakefileObjects(buffer,folderSources);
   WriteMakefileObjects(buffer,taskSources);
   WriteMakefileObjects(buffer,tabSources);
   WriteMakefileObjects(buffer,generatedSources);
   WriteMakefileObjects(buffer,daqSources);
   WriteMakefileObjects(buffer,databaseSources);
   WriteMakefileAdditionalSourceFilesObjects(buffer);

// Depend files
   buffer.AppendFormatted("dependfiles %s\n",kEqualSign);
   WriteMakefileDependFiles(buffer,romeSources);
   WriteMakefileDependFiles(buffer,argusSources);
   WriteMakefileDependFiles(buffer,folderSources);
   WriteMakefileDependFiles(buffer,taskSources);
   WriteMakefileDependFiles(buffer,tabSources);
   WriteMakefileDependFiles(buffer,generatedSources);
   WriteMakefileDependFiles(buffer,daqSources);
   WriteMakefileDependFiles(buffer,databaseSources);
   WriteMakefileAdditionalSourceDependFiles(buffer);

#if defined( R__UNIX )
   if (romeDictHeaders->GetEntries() > 0) {
      if (!librome)
         buffer.AppendFormatted("dependfiles += obj/ROMEDictionary.d\n");
      else
         buffer.AppendFormatted("dependfiles += obj/ROMESDictionary.d\n");
   }
   if (argusHeaders->GetEntries() > 0)
      buffer.AppendFormatted("dependfiles += obj/ARGUSDictionary.d\n");
   if (generatedDictHeaders->GetEntries() > 0)
      buffer.AppendFormatted("dependfiles += obj/%sGeneratedDictionary.d\n",shortCut.Data());
   if (hasFolderGenerated)
      buffer.AppendFormatted("dependfiles += obj/%sGeneratedFolderDictionary.d\n",shortCut.Data());
   if (hasFolderUserCode)
      buffer.AppendFormatted("dependfiles += obj/%sFolderDictionary.d\n",shortCut.Data());
   if (hasTaskGenerated)
      buffer.AppendFormatted("dependfiles += obj/%sGeneratedTaskDictionary.d\n",shortCut.Data());
   if (hasTaskUserCode)
      buffer.AppendFormatted("dependfiles += obj/%sTaskDictionary.d\n",shortCut.Data());
   if (numOfTab>0)
      buffer.AppendFormatted("dependfiles += obj/%sGeneratedTabDictionary.d\n",shortCut.Data());
   if (numOfTab>0)
      buffer.AppendFormatted("dependfiles += obj/%sTabDictionary.d\n",shortCut.Data());
   if (numOfMFDictHeaders>0)
      buffer.AppendFormatted("dependfiles += obj/%sUserDictionary.d\n",shortCut.Data());
#endif // R__UNIX

// user makefile
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("!INCLUDE Makefile.winusr\n");
#else
   buffer.AppendFormatted("-include Makefile.usr\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("\n");
   WriteMakefileUserDictObject(buffer);
   WriteMakefileUserDictDependFiles(buffer);

// echos
   buffer.AppendFormatted("startecho:\n");
   buffer.AppendFormatted("\t@echo building %s%s\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("endecho:\n");
   buffer.AppendFormatted("\t@echo done\n");
   buffer.AppendFormatted("\n");

// make obj
   buffer.AppendFormatted("obj:\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("\t@mkdir obj\n\n");
#else
   buffer.AppendFormatted("\t@if [ ! -d  obj ] ; then \\\n");
   buffer.AppendFormatted("\t\techo \"Making directory obj\" ; \\\n");
   buffer.AppendFormatted("\t\tmkdir obj; \\\n");
   buffer.AppendFormatted("\tfi;\n");
#endif // R__VISUAL_CPLUSPLUS

// Dictionary
   if (!librome)
      WriteMakefileDictionary(buffer,"ROMEDict",romeDictHeaders);
   else
      WriteMakefileDictionary(buffer,"ROMESDict",romeDictHeaders);
   WriteMakefileDictionary(buffer,"ARGUSDict",argusHeaders);
   WriteMakefileDictionary(buffer,shortCut+"GeneratedDict",generatedDictHeaders);
   WriteMakefileDictionary(buffer,shortCut+"GeneratedFolderDict",generatedFolderDictHeaders);
   WriteMakefileDictionary(buffer,shortCut+"GeneratedTaskDict",generatedTaskDictHeaders);
   WriteMakefileDictionary(buffer,shortCut+"GeneratedTabDict",generatedTabDictHeaders);
   WriteMakefileDictionary(buffer,shortCut+"FolderDict",folderHeaders);
   WriteMakefileDictionary(buffer,shortCut+"TaskDict",taskHeaders);
   WriteMakefileDictionary(buffer,shortCut+"TabDict",tabHeaders);
   WriteMakefileDictionary(buffer,shortCut+"DAQDict",daqHeaders);
   WriteMakefileDictionary(buffer,shortCut+"DBDict",databaseHeaders);
   WriteMakefileUserDictionary(buffer);


// Link Statement
// --------------
   buffer.AppendFormatted("%s%s.exe: $(objects)",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2));
   if (librome)
      buffer.AppendFormatted(" $(ROMESYS)/librome.a");
   buffer.AppendFormatted("\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("\t@echo linking %s%s...\n",shortCut.Data(),mainProgName.Data());
   if (haveFortranTask)
      buffer.AppendFormatted("\t@cl /nologo /Fe%s%s.exe $(objects) $(Libraries) /link /nodefaultlib:\"libcmtd.lib\" /FORCE:MULTIPLE\n\n",shortCut.Data(),mainProgName.Data());
   else
      buffer.AppendFormatted("\t@cl /nologo /Fe%s%s.exe $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
   buffer.AppendFormatted("\t$(CXX) $(Flags) -o $@ $(objects) $(Libraries)\n");
   buffer.AppendFormatted("so: lib%s%s.so\n",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2));
   buffer.AppendFormatted("lib%s%s.so: $(objects)\n",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2));
   buffer.AppendFormatted("\t");
#if defined( R__MACOSX )
   buffer.AppendFormatted("$(MACOSXTARGET) ");
#endif // R__MACOSX
   buffer.AppendFormatted("$(CXX) $(Flags) $(soflags) -o lib%s%s.so $(objects) $(Libraries)\n",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2));
#if defined( R__MACOSX )
   buffer.AppendFormatted("\tln -sf lib%s%s.so lib%s%s.dylib",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2),shortCut.ToLower(tmp3),mainProgName.ToLower(tmp4));
#endif // R__MACOSX
   buffer.AppendFormatted("\n");
#endif // R__UNIX

// Compile Statements
// ------------------
   WriteMakefileCompileStatements(buffer,romeSources);
   WriteMakefileCompileStatements(buffer,argusSources);
   WriteMakefileCompileStatements(buffer,generatedSources);
   WriteMakefileCompileStatements(buffer,taskSources);
   WriteMakefileCompileStatements(buffer,tabSources);
   WriteMakefileCompileStatements(buffer,folderSources);
   WriteMakefileCompileStatements(buffer,daqSources);
   WriteMakefileCompileStatements(buffer,databaseSources);
   WriteMakefileAdditionalSourceFilesCompileStatments(buffer);
   ROMEStrArray *tempArray = new ROMEStrArray(1);
   tempArray->AddFormatted("dict/%sUserDict.cpp",shortCut.Data());
   WriteMakefileCompileStatements(buffer,tempArray);
   delete tempArray;

   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("dep: $(dependfiles)\n");
   buffer.AppendFormatted("depclean:\n");
   buffer.AppendFormatted("\t-rm -f obj/*.d\n");
   buffer.AppendFormatted("clean: depclean userclean\n");
   buffer.AppendFormatted("\t-rm -f obj/*.obj G__auto*LinkDef.h\n");
   buffer.AppendFormatted("distclean: clean\n");
   buffer.AppendFormatted("\t-rm -rf src/generated include/generated obj Makefile\n");
   buffer.AppendFormatted("%sclean: userclean\n",shortCut.ToLower(tmp));
   buffer.AppendFormatted("\t-rm -f obj/%s*.obj obj/%s*.d G__auto*LinkDef.h\n",shortCut.Data(),shortCut.Data());

#if defined( R__VISUAL_CPLUSPLUS )
   WriteMakefileBuildRule(buffer,"$(ROMESYS)\\bin\\romebuilder.exe");
#else
   WriteMakefileBuildRule(buffer,"$(ROMESYS)/bin/romebuilder.exe");
#endif // R__VISUAL_CPLUSPLUS

// .d files need to be place at the last
#if defined( R__VISUAL_CPLUSPLUS )
//   buffer.AppendFormatted("!INCLUDE obj/*.d\n");
#else
   buffer.AppendFormatted("-include obj/*.d\n");
#endif // R__VISUAL_CPLUSPLUS

   ROMEString makeFile;
#if defined( R__UNIX )
   makeFile = "Makefile";
#endif // R__UNIX
#if defined( R__VISUAL_CPLUSPLUS )
   makeFile = "Makefile.win";
#endif // R__VISUAL_CPLUSPLUS
   // Write File
   WriteFile(makeFile.Data(),buffer.Data(),6);

   // Write Makefile.usr
   WriteUserMakeFile();

   // Write Visual C++ Projects
#if defined( R__VISUAL_CPLUSPLUS )
   if (!noVP) {
      WriteVisualProjects(2002);
      WriteVisualProjects(2003);
      WriteVisualProjects(2005);
   }
#endif // R__VISUAL_CPLUSPLUS
}

void ROMEBuilder::WriteRootCintCall(ROMEString& buffer)
{
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("@-%%ROOTSYS%%\\bin\\rootcint");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint");
#endif
}

void ROMEBuilder::WriteUserMakeFile()
{
   // Write Makefile.usr
   ROMEString makeFile;
#if defined( R__UNIX )
   makeFile = "Makefile.usr";
   ROMEString usrBuffer;
   if (gSystem->AccessPathName(makeFile.Data(),kFileExists)) {
      usrBuffer.SetFormatted("# User editable Makefile for the %s%s\n",shortCut.Data(),mainProgName.Data());
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("# Description:\n");
      usrBuffer.AppendFormatted("# 1) Add compile(link) options to Flags(Libraries), e.g. Flags += -g -O2 -Wall\n");
      usrBuffer.AppendFormatted("# 2) Add mySource.obj to the list of objects, e.g. objects += mySource.obj\n");
      usrBuffer.AppendFormatted("# 3) Add compile statment, e.g.\n");
      usrBuffer.AppendFormatted("#       obj/mySource.obj: mySource.cpp\n");
      usrBuffer.AppendFormatted("#\t$(CXX) -c $(Flags) $(Includes) mySource.cpp -o obj/mySource.obj\n");
      usrBuffer.AppendFormatted("# 4) Add include paths for the rootcint, e.g. DictionaryIncludes += -ImyPath\n");
      usrBuffer.AppendFormatted("# 5) Add header files for the rootcint, e.g. DictionaryHeaders += myHeader.h/\n");
      usrBuffer.AppendFormatted("# 6) Add clean target, e.g.\n");
      usrBuffer.AppendFormatted("#       userclean:\n");
      usrBuffer.AppendFormatted("#\trm your_file.h\n");
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("userclean:\n");
      usrBuffer.AppendFormatted("\t@echo ''\n");
      WriteFile(makeFile.Data(),usrBuffer.Data(),0);
   }
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   makeFile = "Makefile.winusr";
   ROMEString usrBuffer;
   if (gSystem->AccessPathName(makeFile.Data(),kFileExists)) {
      usrBuffer.SetFormatted("# User editable Makefile for the %s%s\n",shortCut.Data(),mainProgName.Data());
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("# Description:\n");
      usrBuffer.AppendFormatted("# 1) Add compile(link) options to Flags(Libraries), e.g. Flags = $(Flags) /GX /GR\n");
      usrBuffer.AppendFormatted("# 2) Add mySource.obj to the list of objects, e.g. objects = $(objects) mySource.obj\n");
      usrBuffer.AppendFormatted("# 3) Add compile statment, e.g.\n");
      usrBuffer.AppendFormatted("#       obj/mySource.obj: mySource.cpp\n");
      usrBuffer.AppendFormatted("#\tcl /c $(Flags) $(Includes) mySource.cpp /Foobj/mySource.obj\n");
      usrBuffer.AppendFormatted("# 4) Add include paths for the rootcint, e.g. DictionaryIncludes = $(DictionaryIncludes) -ImyPath\n");
      usrBuffer.AppendFormatted("# 5) Add header files for the rootcint, e.g. DictionaryHeaders = $(DictionaryHeaders) myHeader.h/\n");
      usrBuffer.AppendFormatted("# 6) Add clean target, e.g.\n");
      usrBuffer.AppendFormatted("#       userclean:\n");
      usrBuffer.AppendFormatted("#\trm your_file.h\n");
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("userclean:\n",shortCut.Data());
      usrBuffer.AppendFormatted("\t@echo ''\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
      WriteFile(makeFile.Data(),usrBuffer.Data(),0);
   }
#endif
}

