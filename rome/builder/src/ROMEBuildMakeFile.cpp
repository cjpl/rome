/********************************************************************

  ROMEBuildMakeFile.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include <Riostream.h>
#include "ROMEBuilder.h"

//______________________________________________________________________________
void ROMEBuilder::AddIncludeDirectories()
{
   numOfIncludeDirectories = 6;
   if (numOfDAQ > 0) numOfIncludeDirectories++;
   if (numOfDB > 0) numOfIncludeDirectories++;
   if (numOfTab > 0) numOfIncludeDirectories++;
   if (numOfTask > 0) numOfIncludeDirectories++;
#if defined( R__VISUAL_CPLUSPLUS )
   if (midas) numOfIncludeDirectories++;
   if (mysql) numOfIncludeDirectories++;
   if (sqlite) numOfIncludeDirectories++;
   if (sqlite3) numOfIncludeDirectories++;
#endif
   includeDirectories = new ROMEStrArray(numOfIncludeDirectories);
#if defined( R__VISUAL_CPLUSPLUS )
   includeDirectories->AddFormatted("$(ROOTSYS)/include");
#else
   includeDirectories->AddFormatted("$(shell $(ROOTCONFIG) --incdir)");
#endif
   includeDirectories->AddFormatted("$(ROMESYS)/include/");
   includeDirectories->AddFormatted("$(ROMESYS)/argus/include/");
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,14,0))
   includeDirectories->AddFormatted("$(ROMESYS)/include/array64");
#endif
   includeDirectories->AddFormatted("include/");
   includeDirectories->AddFormatted("./");
   includeDirectories->AddFormatted("include/generated");
   if (numOfDAQ > 0) includeDirectories->AddFormatted("include/daqs");
   if (numOfDB > 0) includeDirectories->AddFormatted("include/databases");
   if (numOfTab > 0) includeDirectories->AddFormatted("include/tabs");
   if (numOfTask > 0) includeDirectories->AddFormatted("include/tasks");
#if defined( R__VISUAL_CPLUSPLUS )
   if (midas) includeDirectories->AddFormatted("$(MIDASSYS)/include/");
   if (mysql) includeDirectories->AddFormatted("$(ROMESYS)/include/mysql/");
   if (sqlite) includeDirectories->AddFormatted("$(ROMESYS)/include/sqlite/");
   if (sqlite3) includeDirectories->AddFormatted("$(ROMESYS)/include/sqlite3/");
#endif // R__VISUAL_CPLUSPLUS
}

//______________________________________________________________________________
void ROMEBuilder::AddPrecompiledHeaders()
{
   precompiledHeaders->RemoveAll();
   precompiledHeaders->AddFormatted("generated/%sPrecompile.h", shortCut.Data());
   precompiledIncludeHeaders->RemoveAll();
   precompiledIncludeHeaders->AddFormatted("ROMEAnalyzer.h");
   precompiledIncludeHeaders->AddFormatted("ArgusWindow.h");
   precompiledIncludeHeaders->AddFormatted("TClonesArray.h");
   precompiledIncludeHeaders->AddFormatted("TH1F.h");
   precompiledIncludeHeaders->AddFormatted("TH2F.h");
   precompiledIncludeHeaders->AddFormatted("TH3F.h");
   precompiledIncludeHeaders->AddFormatted("ROMETGraph.h");
   precompiledIncludeHeaders->AddFormatted("ROMETCutG.h");
}

//______________________________________________________________________________
void ROMEBuilder::AddRomeHeaders()
{
   romeHeaders = new ROMEStrArray(50);
   romeHeaders->Add("$(ROMESYS)/include/ROME.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEAnalyzer.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEConfig.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEConfigGraph.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEConfigHisto.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEConfigToForm.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEDAQSystem.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEDataBaseDAQ.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEEventLoop.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEGraph.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEHisto.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMECompositeFrame.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMELabel.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMENetFolderServer.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMENetFolder.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMENoDAQSystem.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMENoDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEODBOfflineDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEODBOnlineDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEPath.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEPrint.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMERint.h");
   if (this->sql) {
      romeHeaders->Add("$(ROMESYS)/include/ROMESQL.h");
      romeHeaders->Add("$(ROMESYS)/include/ROMESQLDataBase.h");
   }
   romeHeaders->Add("$(ROMESYS)/include/ROMEStopwatch.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEStrArray.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEStr2DArray.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEString.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMETask.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMETextDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMETree.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMETreeInfo.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEUtilities.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEXML.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMEXMLDataBase.h");
   romeHeaders->Add("$(ROMESYS)/include/mxml.h");
   romeHeaders->Add("$(ROMESYS)/include/strlcpy.h");
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,14,0))
   romeHeaders->Add("$(ROMESYS)/include/array64/TArrayL64.h");
#endif
   romeHeaders->Add("$(ROMESYS)/include/ROMETGraph.h");
   romeHeaders->Add("$(ROMESYS)/include/ROMETCutG.h");
   romeHeaders->Add("$(ROMESYS)/include/TNetFolderServer.h");
   romeHeaders->Add("$(ROMESYS)/include/TNetFolder.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToForm.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToFormFrame.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToFormElement.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToFormElementSignal.h");
   romeHeaders->Add("$(ROMESYS)/include/XMLToFormWindow.h");
   if (numOfEvent > 0 || midas)
      romeHeaders->Add("$(ROMESYS)/include/ROMEMidasDAQ.h");
   if (this->orca)
      romeHeaders->Add("$(ROMESYS)/include/ROMEOrcaDAQ.h");
   if (numOfTree > 0)
      romeHeaders->Add("$(ROMESYS)/include/ROMERomeDAQ.h");
   if (numOfRootTree > 0)
      romeHeaders->Add("$(ROMESYS)/include/ROMERootDAQ.h");
   if (this->mysql)
      romeHeaders->Add("$(ROMESYS)/include/ROMEMySQL.h");
   if (this->pgsql)
      romeHeaders->Add("$(ROMESYS)/include/ROMEPgSQL.h");
   if (this->sqlite)
      romeHeaders->Add("$(ROMESYS)/include/ROMESQLite.h");
   if (this->sqlite3)
      romeHeaders->Add("$(ROMESYS)/include/ROMESQLite3.h");
}

//______________________________________________________________________________
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
      romeDictHeaders->Add("$(ROMESYS)/include/ROMECompositeFrame.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMELabel.h");
      romeLinkDefSuffix->Add("");
   }
   romeDictHeaders->Add("$(ROMESYS)/include/ROMEAnalyzer.h");
   romeLinkDefSuffix->Add("");
   romeDictHeaders->Add("$(ROMESYS)/include/ROMEEventLoop.h");
   romeLinkDefSuffix->Add("");
   romeDictHeaders->Add("$(ROMESYS)/include/ROMEODBOnlineDataBase.h");
   romeLinkDefSuffix->Add("");
   romeDictHeaders->Add("$(ROMESYS)/include/ROMERint.h");
   romeLinkDefSuffix->Add("");
   if (numOfEvent > 0 || midas) {
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEMidasDAQ.h");
      romeLinkDefSuffix->Add("");
   }
   if (!librome) {
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEGraph.h");
      romeLinkDefSuffix->Add("+");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEHisto.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETask.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETreeInfo.h");
      romeLinkDefSuffix->Add("+");
      romeDictHeaders->Add("$(ROMESYS)/include/TNetFolder.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/TNetFolderServer.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMENetFolder.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMENetFolderServer.h");
      romeLinkDefSuffix->Add("");
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,14,0))
      romeDictHeaders->Add("$(ROMESYS)/include/array64/TArrayL64.h");
      romeLinkDefSuffix->Add("-");
#endif
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETGraph.h");
      romeLinkDefSuffix->Add("+");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETCutG.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEString.h");
      romeLinkDefSuffix->Add("+");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEStrArray.h");
      romeLinkDefSuffix->Add("+");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEStr2DArray.h");
      romeLinkDefSuffix->Add("+");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEConfig.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEConfigGraph.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEConfigHisto.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMENoDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEODBOfflineDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETextDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEXMLDataBase.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMEXML.h");
      romeLinkDefSuffix->Add("");
      romeDictHeaders->Add("$(ROMESYS)/include/ROMETree.h");
      romeLinkDefSuffix->Add("+");
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
      if (numOfTree > 0) {
         romeDictHeaders->Add("$(ROMESYS)/include/ROMERomeDAQ.h");
         romeLinkDefSuffix->Add("");
      }
      if (numOfRootTree > 0) {
         romeDictHeaders->Add("$(ROMESYS)/include/ROMERootDAQ.h");
         romeLinkDefSuffix->Add("");
      }
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

//______________________________________________________________________________
void ROMEBuilder::AddRomeSources()
{
   romeSources = new ROMEStrArray(50);
   romeSources->Add("$(ROMESYS)/src/ROMEAnalyzer.cpp");
   romeSources->Add("$(ROMESYS)/src/ROMEEventLoop.cpp");
   romeSources->Add("$(ROMESYS)/src/ROMEODBOnlineDataBase.cpp");
   romeSources->Add("$(ROMESYS)/src/ROMEPrint.cpp");
   romeSources->Add("$(ROMESYS)/src/ROMERint.cpp");
   if (numOfEvent > 0 || midas)
      romeSources->Add("$(ROMESYS)/src/ROMEMidasDAQ.cpp");
   if (!librome) {
      romeSources->Add("$(ROMESYS)/src/mxml.c");
      romeSources->Add("$(ROMESYS)/src/ROMEDAQSystem.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEDataBaseDAQ.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEGraph.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEHisto.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEODBOfflineDataBase.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEPath.cpp");
      if (numOfTree > 0)
         romeSources->Add("$(ROMESYS)/src/ROMERomeDAQ.cpp");
      if (numOfRootTree > 0)
         romeSources->Add("$(ROMESYS)/src/ROMERootDAQ.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMESplashScreen.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEStr2DArray.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEStrArray.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEString.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMETask.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMETextDataBase.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEUtilities.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEXML.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEXMLDataBase.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMETree.cpp");
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,14,0))
      romeSources->Add("$(ROMESYS)/src/TArrayL64.cpp");
#endif
      romeSources->Add("$(ROMESYS)/src/ROMETGraph.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMETCutG.cpp");
      romeSources->Add("$(ROMESYS)/src/strlcpy.c");
      romeSources->Add("$(ROMESYS)/src/TNetFolderServer.cpp");
      romeSources->Add("$(ROMESYS)/src/TNetFolder.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMENetFolderServer.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMENetFolder.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEConfig.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMEConfigToForm.cpp");
      romeSources->Add("$(ROMESYS)/src/XMLToForm.cpp");
      romeSources->Add("$(ROMESYS)/src/XMLToFormWindow.cpp");
      romeSources->Add("$(ROMESYS)/src/XMLToFormFrame.cpp");
      romeSources->Add("$(ROMESYS)/src/XMLToFormElement.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMECompositeFrame.cpp");
      romeSources->Add("$(ROMESYS)/src/ROMELabel.cpp");
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
#if defined( R__VISUAL_CPLUSPLUS )
   // temporary
   if (midas) {
      romeSources->Add("$(MIDASSYS)/src/midas.c");
      romeSources->Add("$(MIDASSYS)/src/system.c");
      romeSources->Add("$(MIDASSYS)/src/odb.c");
   }
#endif // R__VISUAL_CPLUSPLUS
}

//______________________________________________________________________________
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

//______________________________________________________________________________
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
}

//______________________________________________________________________________
void ROMEBuilder::AddGeneratedHeaders()
{
   int i,j;
   int nRootClass = 0;
   for (i = 0; i < numOfRootTree; i++) {
      for (j = 0; j < numOfRootBranch[i]; j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            nRootClass++;
         }
      }
   }
   generatedHeaders = new ROMEStrArray(9 + TMath::Max(numOfFolder,0) + TMath::Max(numOfTask,0) +
                                       TMath::Max(numOfTab,0) + nRootClass);
   generatedHeaders->AddFormatted("include/generated/%sAnalyzer.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sEventLoop.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sWindow.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sConfig.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sDBAccess.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sNetFolderServer.h",shortCut.Data());
   if (readGlobalSteeringParameters)
      generatedHeaders->AddFormatted("include/generated/%sGlobalSteering.h",shortCut.Data());
   if (numOfEvent > 0 || midas)
      generatedHeaders->AddFormatted("include/generated/%sMidasDAQ.h",shortCut.Data());
   if (numOfTree > 0)
      generatedHeaders->AddFormatted("include/generated/%sRomeDAQ.h",shortCut.Data());
   if (numOfRootTree > 0)
      generatedHeaders->AddFormatted("include/generated/%sRootDAQ.h",shortCut.Data());
   generatedHeaders->AddFormatted("include/generated/%sConfigToForm.h",shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i)) {
         if (folderUserCode[i]) {
            generatedHeaders->AddFormatted("include/generated/%s%s_Base.h",shortCut.Data(),folderName[i].Data());
         } else {
            generatedHeaders->AddFormatted("include/generated/%s%s.h",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      generatedHeaders->AddFormatted("include/generated/%sT%s_Base.h",shortCut.Data(),taskName[i].Data());
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      generatedHeaders->AddFormatted("include/generated/%sT%s_Base.h",shortCut.Data(),tabName[i].Data());
   }
   for (i = 0; i < numOfRootTree; i++) {
      for (j = 0; j < numOfRootBranch[i]; j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            generatedHeaders->AddFormatted("include/generated/%s.h",rootBranchClassName[i][j].Data());
         }
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddGeneratedDictHeaders()
{
   int i,j;
   generatedDictHeaders = new ROMEStrArray(1);
   generatedLinkDefSuffix = new ROMEStrArray(1);
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
   generatedDictHeaders->AddFormatted("include/generated/%sDBAccess.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
   generatedDictHeaders->AddFormatted("include/generated/%sEventLoop.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
   generatedDictHeaders->AddFormatted("include/generated/%sNetFolderServer.h",shortCut.Data());
   generatedLinkDefSuffix->Add("");
   if (numOfEvent > 0 || midas) {
      generatedDictHeaders->AddFormatted("include/generated/%sMidasDAQ.h",shortCut.Data());
      generatedLinkDefSuffix->Add("");
   }
   if (numOfTree > 0) {
      generatedDictHeaders->AddFormatted("include/generated/%sRomeDAQ.h",shortCut.Data());
      generatedLinkDefSuffix->Add("");
   }
   if (numOfRootTree > 0) {
      generatedDictHeaders->AddFormatted("include/generated/%sRootDAQ.h",shortCut.Data());
      generatedLinkDefSuffix->Add("");
   }
   for (i = 0; i < numOfRootTree; i++) {
      for (j = 0; j < numOfRootBranch[i]; j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            generatedDictHeaders->AddFormatted("include/generated/%s.h",rootBranchClassName[i][j].Data());
            generatedLinkDefSuffix->Add("+");
         }
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddGeneratedFolderDictHeaders()
{
   int i;
   generatedFolderDictHeaders = new ROMEStrArray(TMath::Max(numOfFolder,0));
   generatedFolderLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfFolder,0));
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && !folderSupport[i]) {
         if (folderUserCode[i]) {
            generatedFolderDictHeaders->AddFormatted("include/generated/%s%s_Base.h",shortCut.Data(),
                                                     folderName[i].Data());
            generatedFolderLinkDefSuffix->Add("+");
         } else {
            generatedFolderDictHeaders->AddFormatted("include/generated/%s%s.h",shortCut.Data(),folderName[i].Data());
            generatedFolderLinkDefSuffix->Add("+");
         }
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddGeneratedSupportFolderDictHeaders()
{
   int i;
   generatedSupportFolderDictHeaders = new ROMEStrArray(TMath::Max(numOfFolder,0));
   generatedSupportFolderLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfFolder,0));
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && folderSupport[i]) {
         if (folderUserCode[i]) {
            generatedSupportFolderDictHeaders->AddFormatted("include/generated/%s%s_Base.h",shortCut.Data(),
                                                            folderName[i].Data());
            generatedSupportFolderLinkDefSuffix->Add("+");
         } else {
            generatedSupportFolderDictHeaders->AddFormatted("include/generated/%s%s.h",shortCut.Data(),
                                                            folderName[i].Data());
            generatedSupportFolderLinkDefSuffix->Add("+");
         }
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddGeneratedTaskDictHeaders()
{
   int i;
   generatedTaskDictHeaders = new ROMEStrArray(TMath::Max(numOfTask,0));
   generatedTaskLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfTask,0));
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      generatedTaskDictHeaders->AddFormatted("include/generated/%sT%s_Base.h",shortCut.Data(),taskName[i].Data());
      generatedTaskLinkDefSuffix->Add("");
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddGeneratedTabDictHeaders()
{
   int i;
   generatedTabDictHeaders = new ROMEStrArray(TMath::Max(numOfTab,0));
   generatedTabLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfTab,0));
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      generatedTabDictHeaders->AddFormatted("include/generated/%sT%s_Base.h",shortCut.Data(),tabName[i].Data());
      generatedTabLinkDefSuffix->Add("");
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddGeneratedSources()
{
   int i;
   generatedSources = new ROMEStrArray(12+TMath::Max(numOfTab,0)+TMath::Max(numOfTask,0));
   generatedSources->AddFormatted("src/generated/main.cpp");
   generatedSources->AddFormatted("src/generated/%sAnalyzer.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sAnalyzer2.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sAnalyzer3.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sAnalyzer4.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sEventLoop.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sWindow.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sWindow2.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sConfig.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sConfig2.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sConfig3.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sConfig4.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sDBAccess.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sNetFolderServer.cpp",shortCut.Data());
   if (numOfEvent > 0 || midas)
      generatedSources->AddFormatted("src/generated/%sMidasDAQ.cpp",shortCut.Data());
   if (numOfTree > 0)
      generatedSources->AddFormatted("src/generated/%sRomeDAQ.cpp",shortCut.Data());
   if (numOfRootTree > 0)
      generatedSources->AddFormatted("src/generated/%sRootDAQ.cpp",shortCut.Data());
   generatedSources->AddFormatted("src/generated/%sConfigToForm.cpp",shortCut.Data());
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      generatedSources->AddFormatted("src/generated/%sT%s_Base.cpp",shortCut.Data(),taskName[i].Data());
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      generatedSources->AddFormatted("src/generated/%sT%s_Base.cpp",shortCut.Data(),tabName[i].Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddFolderHeaders()
{
   int i;
   folderHeaders = new ROMEStrArray(TMath::Max(numOfFolder,0));
   folderLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfFolder,0));
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i)) {
         if (folderUserCode[i]) {
            folderHeaders->AddFormatted("include/folders/%s%s.h",shortCut.Data(),folderName[i].Data());
            folderLinkDefSuffix->Add("+");
         }
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddFolderSources()
{
   int i;
   folderSources = new ROMEStrArray(numOfFolder+1);
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i)) {
         if (folderUserCode[i]) {
            folderSources->AddFormatted("src/generated/%s%s_Base.cpp",shortCut.Data(),folderName[i].Data());
            folderSources->AddFormatted("src/folders/%s%s.cpp",shortCut.Data(),folderName[i].Data());
         } else {
            folderSources->AddFormatted("src/generated/%s%s.cpp",shortCut.Data(),folderName[i].Data());
         }
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddTaskHeaders()
{
   int i;
   taskHeaders = new ROMEStrArray(TMath::Max(numOfTask,0));
   taskLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfTask,0));
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      taskHeaders->AddFormatted("include/tasks/%sT%s.h",shortCut.Data(),taskName[i].Data());
      taskLinkDefSuffix->Add("");
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddTaskSources()
{
   int i;
   taskSources = new ROMEStrArray(numOfTask+1);
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      taskSources->AddFormatted("src/tasks/%sT%s.cpp",shortCut.Data(),taskName[i].Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddTabHeaders()
{
   int i;
   tabHeaders = new ROMEStrArray(TMath::Max(numOfTab,0));
   tabLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfTab,0));
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      tabHeaders->AddFormatted("include/tabs/%sT%s.h",shortCut.Data(),tabName[i].Data());
      tabLinkDefSuffix->Add("");
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddTabSources()
{
   int i;
   tabSources = new ROMEStrArray(numOfTab+1);
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      tabSources->AddFormatted("src/tabs/%sT%s.cpp",shortCut.Data(),tabName[i].Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddDAQHeaders()
{
   int i;
   daqHeaders = new ROMEStrArray(TMath::Max(numOfDAQ,0));
   daqLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfDAQ,0));
   for (i = 0;i < numOfDAQ; i++) {
      if (!daqUsed[i])
         continue;
      daqHeaders->AddFormatted("include/daqs/%s%sDAQ.h",shortCut.Data(),daqName[i].Data());
      daqLinkDefSuffix->Add("");
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddDAQSources()
{
   int i;
   daqSources = new ROMEStrArray(3+TMath::Max(numOfDAQ,0));
   for (i = 0; i < numOfDAQ; i++) {
      if (!daqUsed[i])
         continue;
      daqSources->AddFormatted("src/daqs/%s%sDAQ.cpp",shortCut.Data(),daqName[i].Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddDatabaseHeaders()
{
   int i;
   databaseHeaders = new ROMEStrArray(TMath::Max(numOfDB,0));
   databaseLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfDB,0));
   for (i = 0; i < numOfDB; i++) {
      databaseHeaders->AddFormatted("include/databases/%s%sDataBase.h",shortCut.Data(),dbName[i].Data());
      databaseLinkDefSuffix->Add("");
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddDatabaseSources()
{
   int i;
   databaseSources = new ROMEStrArray(TMath::Max(numOfDB,0));
   for (i = 0; i < numOfDB; i++) {
      databaseSources->AddFormatted("src/databases/%s%sDataBase.cpp",shortCut.Data(),dbName[i].Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddAdditionalDictHeaders()
{
   int i;
   additionalDictHeaders = new ROMEStrArray(TMath::Max(numOfMFDictHeaders,0));
   additionalDictLinkDefSuffix = new ROMEStrArray(TMath::Max(numOfMFDictHeaders,0));
   for (i = 0;i < numOfMFDictHeaders; i++) {
      if (!mfDictHeaderUsed[i])
         continue;
      additionalDictHeaders->Add(mfDictHeaderName[i].Data());
      additionalDictLinkDefSuffix->Add(mfDictHeaderLinkDefSuffix[i].Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddDictionaryHeaders()
{
   int i;

   Int_t nROME         = romeDictHeaders->GetEntriesFast();
   Int_t nArgus        = argusHeaders->GetEntriesFast();
   Int_t nGen          = generatedDictHeaders->GetEntriesFast();
   Int_t nGenFolder    = generatedFolderDictHeaders->GetEntriesFast();
   Int_t nGenSupFolder = generatedSupportFolderDictHeaders->GetEntriesFast();
   Int_t nGenTask      = generatedTaskDictHeaders->GetEntriesFast();
   Int_t nGenTab       = generatedTabDictHeaders->GetEntriesFast();
   Int_t nFolder       = folderHeaders->GetEntriesFast();
   Int_t nTask         = taskHeaders->GetEntriesFast();
   Int_t nTab          = tabHeaders->GetEntriesFast();
   Int_t nDAQ          = daqHeaders->GetEntriesFast();
   Int_t nDB           = databaseHeaders->GetEntriesFast();
   Int_t nAdd          = additionalDictHeaders->GetEntriesFast();

   Int_t nAll = nROME + nArgus + nGen + nGenFolder + nGenSupFolder + nGenTask + nGenTab +
         nFolder + nTask + nTab + nDAQ + nDB + nAdd;

   dictionaryHeaders = new ROMEStrArray(nAll);
   dictionaryLinkDefSuffix = new ROMEStrArray(nAll);

   for (i = 0; i < nTab; i++) {
      dictionaryHeaders->Add(tabHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(tabLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nGenTab; i++) {
      dictionaryHeaders->Add(generatedTabDictHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(generatedTabLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nROME; i++) {
      dictionaryHeaders->Add(romeDictHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(romeLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nArgus; i++) {
      dictionaryHeaders->Add(argusHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(argusLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nGen; i++) {
      dictionaryHeaders->Add(generatedDictHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(generatedLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nGenFolder; i++) {
      dictionaryHeaders->Add(generatedFolderDictHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(generatedFolderLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nGenSupFolder; i++) {
      dictionaryHeaders->Add(generatedSupportFolderDictHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(generatedSupportFolderLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nGenTask; i++) {
      dictionaryHeaders->Add(generatedTaskDictHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(generatedTaskLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nFolder; i++) {
      dictionaryHeaders->Add(folderHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(folderLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nTask; i++) {
      dictionaryHeaders->Add(taskHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(taskLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nDAQ; i++) {
      dictionaryHeaders->Add(daqHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(daqLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nDB; i++) {
      dictionaryHeaders->Add(databaseHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(databaseLinkDefSuffix->At(i).Data());
   }
   for (i = 0; i < nAdd; i++) {
      dictionaryHeaders->Add(additionalDictHeaders->At(i).Data());
      dictionaryLinkDefSuffix->Add(additionalDictLinkDefSuffix->At(i).Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::AddDictionarySources()
{
   int i;
   Int_t n = dictionaryHeaders->GetEntriesFast();
   dictionarySources = new ROMEStrArray(TMath::Max(n, 0));
   if (n > 0) {
      for (i = 0; i < (n - 1) / maxNumberOfClassesInDictionary + 1; i++) {
         dictionarySources->AddFormatted("dict/%sDict%d.cpp", shortCut.Data(),i);
      }
   }
}

//______________________________________________________________________________
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
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0))
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libRIO.lib");
   rootLibraries->AddFormatted("$(ROOTSYS)\\lib\\libNet.lib");
#endif
}

//______________________________________________________________________________
void ROMEBuilder::AddMysqlLibraries()
{
#if defined( R__VISUAL_CPLUSPLUS )
   if (this->mysql) {
      mysqlLibraries = new ROMEStrArray(1);
      mysqlLibraries->AddFormatted("$(ROMESYS)\\lib_win\\libmysql.lib");
   }
#endif
}

//______________________________________________________________________________
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

//______________________________________________________________________________
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

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileHeader(ROMEString& buffer)
{
   ROMEString tmp;
   buffer.AppendFormatted("##############################################################\n");
   buffer.AppendFormatted("## *** This file will be overwritten by the ROMEBuilder *** ##\n");
   buffer.AppendFormatted("## ***      Don't make manual changes to this file      *** ##\n");
   buffer.AppendFormatted("##############################################################\n");
#if defined( R__UNIX )
   buffer.AppendFormatted("#\n");
   buffer.AppendFormatted("# make              : compile executable\n");
   buffer.AppendFormatted("# make build        : execute romebuilder\n");
   buffer.AppendFormatted("# make so           : build shared library\n");
   buffer.AppendFormatted("# make -k clean     : remove intermediate files\n");
   buffer.AppendFormatted("# make -k distclean : remove following directries and files\n");
   buffer.AppendFormatted("#                     src/generated include/generated obj dict Makefile\n");
   buffer.AppendFormatted("# make -k depclean  : remove depend files (obj/*.d)\n");
   buffer.AppendFormatted("# make -k %sclean   : remove %s specific intermediate files\n", shortCut.ToLower(tmp),
                          shortCut.Data());
   buffer.AppendFormatted("#\n");
   buffer.AppendFormatted("# Some influential environment variables:\n");
   buffer.AppendFormatted("# CC                : C compiler command\n");
   buffer.AppendFormatted("# CXX               : C++ compiler command\n");
   buffer.AppendFormatted("# FC                : Fortran compiler command\n");
   buffer.AppendFormatted("# CXXLD             : Linker command\n");
   buffer.AppendFormatted("# %sOPT             : optimization and debug flag\n", shortCut.ToUpper(tmp));
   buffer.AppendFormatted("# %sINC             : additional include flag\n", shortCut.ToUpper(tmp));
   buffer.AppendFormatted("# %sCFLAGS          : additional C compile flag\n", shortCut.ToUpper(tmp));
   buffer.AppendFormatted("# %sCXXFLAGS        : additional C++ compile flag\n", shortCut.ToUpper(tmp));
   buffer.AppendFormatted("# %sFFLAGS          : additional Fortran compile flag\n", shortCut.ToUpper(tmp));
   buffer.AppendFormatted("# %sLDFLAGS         : additional link flag\n", shortCut.ToUpper(tmp));
   buffer.AppendFormatted("# %sSOFLAGS         : additional shared library link flag\n", shortCut.ToUpper(tmp));
   buffer.AppendFormatted("# %sVERBOSEMAKE     : switch for verbose make. 1 or 0.", shortCut.ToUpper(tmp));
   if (quietMake) {
      buffer.AppendFormatted(" (Default is 0)\n");
   } else {
      buffer.AppendFormatted(" (Default is 1)\n");
   }
#endif // R__UNIX
   buffer.AppendFormatted("\n");
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileLibsAndFlags(ROMEString& buffer)
{
   int i,j;
   ROMEString tmp,tmp2;
#if defined( R__UNIX )
   ROMEString tmp1, tmp3;
#endif

#if defined( R__VISUAL_CPLUSPLUS )
   // libs
   buffer.AppendFormatted("rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib "
                          "$(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib "
                          "$(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHist.lib "
                          "$(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMatrix.lib "
                          "$(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib "
                          "$(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib "
                          "$(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libWin32gdk.lib $(ROOTSYS)/lib/libThread.lib\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("sqllibs =");
   if (this->mysql)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libmysql.lib");
   if (this->pgsql)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libpq.lib");
   if (this->sqlite)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/sqlite.lib");
   if (this->sqlite3)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/sqlite3.lib");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("daqlibs =");
   for (i = 0; i < daqLibraries->GetEntriesFast(); i++) {
      buffer.AppendFormatted(" %s",daqLibraries->At(i).Data());
   }
   buffer.AppendFormatted("\n\n");
   buffer.AppendFormatted("clibs = wsock32.lib gdi32.lib user32.lib kernel32.lib\n");
   buffer.AppendFormatted("Libraries = $(rootlibs) $(clibs) $(sqllibs) $(daqlibs)\n");
   bool addLib = true;
   bool flagMatched = false;
   int k;
   for (i = 0; i < numOfMFWinLibs; i++) {
      addLib = true;
      for (j = 0; j < numOfMFWinLibFlags[i]; j++) {
         addLib = false;
         flagMatched = false;
         for (k = 0; k < flags.GetEntriesFast(); k++) {
            if (mfWinLibFlag[i][j] == flags.At(k)) {
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
   for (i = 0; i < daqFlags->GetEntriesFast(); i++) {
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
   for (i = 0; i < flags.GetEntriesFast(); i++)
      buffer.AppendFormatted(" /D%s",flags.At(i).Data());
   for (i = 0; i < affiliations.GetEntriesFast(); i++)
      buffer.AppendFormatted(" /DHAVE_%s",((ROMEString)affiliations.At(i)).ToUpper(tmp));
   for (i = 0; i < numOfMFPreDefs; i++)
      buffer.AppendFormatted(" /D%s",mfPreDefName[i].Data());
   buffer.AppendFormatted(" /D%s_%s",shortCut.ToUpper(tmp),mainProgName.ToUpper(tmp2));
   buffer.AppendFormatted("\n");
#if 0
   buffer.AppendFormatted("!IFDEF MIDAS_MAX_EVENT_SIZE\n");
   buffer.AppendFormatted("Flags = $(Flags) /DMAX_EVENT_SIZE=$(MIDAS_MAX_EVENT_SIZE)\n");
   buffer.AppendFormatted("!ENDIF\n");
#endif
   // fortran flags
   buffer.AppendFormatted("FortranFlags = $(%suserflags)\n",shortCut.ToLower(tmp));
#endif // R__VISUAL_CPLUSPLUS

#if defined( R__UNIX )
   buffer.AppendFormatted("## Current directory\n");
   buffer.AppendFormatted("PWDST := $(shell pwd)\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("## Compilers\n");
   buffer.AppendFormatted("CXX   ?= g++\n");
   buffer.AppendFormatted("CC    ?= gcc\n");
   buffer.AppendFormatted("FC    ?= g77\n");
   buffer.AppendFormatted("CXXLD ?= $(CXX)\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("## Verbose make option\n");
   buffer.AppendFormatted("NTARGETS_FILE := obj/ntargets\n");
   if (quietMake) {
      buffer.AppendFormatted("%sVERBOSEMAKE ?= 0\n", shortCut.Data());
   } else {
      buffer.AppendFormatted("%sVERBOSEMAKE ?= 1\n", shortCut.Data());
   }
   buffer.AppendFormatted("ifeq ($(%sVERBOSEMAKE), 0)\n", shortCut.Data());
   buffer.AppendFormatted("   ifndef NTARGETS_STOP\n");
   buffer.AppendFormatted("      DELETE_TARGETS_FILE := $(shell $(RM) $(NTARGETS_FILE))\n");
   buffer.AppendFormatted("      NTARGETS_TOTAL := $(shell $(MAKE) NTARGETS_STOP=yes -n $(MAKECMDGOALS) | \\\n");
   buffer.AppendFormatted("                          grep NTARGETS_MAGIC | wc -l)\n");
   buffer.AppendFormatted("      NTARGETS_TOTLEN := $(shell echo $(NTARGETS_TOTAL) | wc -c)\n");
   buffer.AppendFormatted("      NTARGETS_TOTLEN := $(shell expr $(NTARGETS_TOTLEN) - 1 )\n");
   buffer.AppendFormatted("      CREATE_TARGETS_FILE := $(shell $(MAKE) NTARGETS_STOP=yes -n $(MAKECMDGOALS) | \\\n");
   buffer.AppendFormatted("                               grep NTARGETS_MAGIC | \\\n");
   buffer.AppendFormatted("                               awk '{printf(\"\\\"%%$(NTARGETS_TOTLEN)d\\\":%%s\\n\", \\\n");
   buffer.AppendFormatted("                               $(NTARGETS_TOTAL)-NR+1,$$0);}' > \\\n");
   buffer.AppendFormatted("                               $(NTARGETS_FILE))\n");
   buffer.AppendFormatted("      NTARGETS = $(shell grep -m 1 $(subst .,\\.,$1) $(NTARGETS_FILE) | cut -d ':' -f 1)\n");
   buffer.AppendFormatted("   endif\n");
   buffer.AppendFormatted("   Q = @\n");
   buffer.AppendFormatted("   define %sechoing\n",shortCut.ToLower(tmp));
   buffer.AppendFormatted("      @if [ -e $(NTARGETS_FILE) ]; then \\\n");
   buffer.AppendFormatted("         echo [$(NTARGETS)/$(NTARGETS_TOTAL)] $1; \\\n");
   buffer.AppendFormatted("         set NTARGETS_MAGIC=yes; \\\n");
   buffer.AppendFormatted("      else \\\n");
   buffer.AppendFormatted("         echo $1; \\\n");
   buffer.AppendFormatted("      fi;\n");
   buffer.AppendFormatted("   endef\n");
   buffer.AppendFormatted("else\n");
   buffer.AppendFormatted("   Q =\n");
   buffer.AppendFormatted("   %sechoing =\n",shortCut.ToLower(tmp));
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("## Additional flags\n");
   // equal signs below should be '=' to allow change in Makefile.usr
   buffer.AppendFormatted("%sOPT      ?= -g -O\n",shortCut.ToUpper(tmp));
   buffer.AppendFormatted("%sCFLAGS   ?= -pipe -Wall -W\n",shortCut.ToUpper(tmp));
   buffer.AppendFormatted("%sCXXFLAGS ?= -pipe -Wall -W -Woverloaded-virtual\n",shortCut.ToUpper(tmp));
   buffer.AppendFormatted("%sFFLAGS   ?= -pipe -Wall -W\n",shortCut.ToUpper(tmp));
   buffer.AppendFormatted("%sCFLAGS   += $(%suserflags) $(%sOPT)\n",shortCut.ToUpper(tmp1),shortCut.ToLower(tmp2),
                          shortCut.ToUpper(tmp3));
   buffer.AppendFormatted("%sCXXFLAGS += $(%suserflags) $(%sOPT)\n",shortCut.ToUpper(tmp1),shortCut.ToLower(tmp2),
                          shortCut.ToUpper(tmp3));
   buffer.AppendFormatted("%sFFLAGS   += $(%suserflags) $(%sOPT)\n",shortCut.ToUpper(tmp1),shortCut.ToLower(tmp2),
                          shortCut.ToUpper(tmp3));
   buffer.AppendFormatted("%sLDFLAGS  += $(%suserflags) $(%sOPT)\n",shortCut.ToUpper(tmp1),shortCut.ToLower(tmp2),
                          shortCut.ToUpper(tmp3));
   buffer.AppendFormatted("%sSOFLAGS  += $(%suserflags) $(%sOPT)\n",shortCut.ToUpper(tmp1),shortCut.ToLower(tmp2),
                          shortCut.ToUpper(tmp3));
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("## Compile and link flags\n");
   buffer.AppendFormatted("rootlibs  := $(shell $(ROOTCONFIG) --libs) -lHtml -lThread\n");
   buffer.AppendFormatted("rootglibs := $(shell $(ROOTCONFIG) --glibs) -lHtml -lThread\n");
   buffer.AppendFormatted("rootcflags:= $(shell $(ROOTCONFIG) --cflags)\n");
   buffer.AppendFormatted("sqllibs   :=");
   if (this->mysql) {
      buffer.AppendFormatted(" $(shell mysql_config --libs)");
   }
   if (this->pgsql) {
      buffer.AppendFormatted(" -L$(shell pg_config --libdir) -lpq");
   }
   if (this->sqlite) {
      buffer.AppendFormatted(" -lsqlite");
   }
   if (this->sqlite3) {
      buffer.AppendFormatted(" -lsqlite3");
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("sqlcflags :=");
   if (this->sql) {
      buffer.AppendFormatted(" -DHAVE_SQL");
   }
   if (this->mysql) {
      buffer.AppendFormatted(" $(shell mysql_config --include) -DHAVE_MYSQL");
   }
   if (this->pgsql) {
      buffer.AppendFormatted(" -I$(shell pg_config --includedir) -DHAVE_PGSQL");
   }
   if (this->sqlite) {
      buffer.AppendFormatted(" -DHAVE_SQLITE");
   }
   if (this->sqlite3) {
      buffer.AppendFormatted(" -DHAVE_SQLITE3");
   }
   for (i = 0; i < flags.GetEntriesFast(); i++) {
      buffer.AppendFormatted(" -D%s",flags.At(i).Data());
   }
   for (i = 0; i < affiliations.GetEntriesFast(); i++) {
      buffer.AppendFormatted(" -DHAVE_%s", static_cast<ROMEString>(affiliations.At(i)).ToUpper(tmp));
   }
   for (i = 0; i < numOfMFPreDefs; i++) {
      buffer.AppendFormatted(" -D%s",mfPreDefName[i].Data());
   }
   buffer.AppendFormatted("\n");
   // OS Flaqs
#if defined( R__ALPHA )
   buffer.AppendFormatted("oscflags  :=\n");
   buffer.AppendFormatted("oslibs    := -lc -lbsd\n");
   buffer.AppendFormatted("osldflags :=\n");
   buffer.AppendFormatted("ossoflags := -Wl,-expect_unresolved,* -shared\n");
#elif defined( R__SGI )
   buffer.AppendFormatted("oscflags  :=\n");
   buffer.AppendFormatted("oslibs    :=\n");
   buffer.AppendFormatted("osldflags :=\n");
   buffer.AppendFormatted("ossoflags := -shared\n");
#elif defined( R__FBSD )
   buffer.AppendFormatted("oscflags  :=\n");
   buffer.AppendFormatted("oslibs    := -lbsd -lcompat\n");
   buffer.AppendFormatted("osldflags :=\n");
   buffer.AppendFormatted("ossoflags := -shared -Wl,-x\n");
#elif defined( R__MACOSX )
#   if defined(USE_PIC_UPPER)
   buffer.AppendFormatted("oscflags  := -fPIC");
#   else
#      if defined(USE_PIC_LOWER)
#         error Mac OSX does not support -fpic
   buffer.AppendFormatted("oscflags  := -fpic");
#      else
   buffer.AppendFormatted("oscflags  :=");
#      endif
#   endif
   buffer.AppendFormatted(" -Wno-unused-function  $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)\n");
   buffer.AppendFormatted("oslibs    := $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib)\n");
   buffer.AppendFormatted("osldflags := -bind_at_load -multiply_defined suppress\n");
   buffer.AppendFormatted("ifeq ($(MACOSX_DEPLOYMENT_TARGET),10.1)\n");
   buffer.AppendFormatted("ossoflags := -dynamiclib -single_module -undefined suppress\n");
   buffer.AppendFormatted("else\n");
   buffer.AppendFormatted("ifeq ($(MACOSX_DEPLOYMENT_TARGET),10.2)\n");
   buffer.AppendFormatted("ossoflags := -dynamiclib -single_module -undefined suppress\n");
   buffer.AppendFormatted("else\n");
   buffer.AppendFormatted("ossoflags := -dynamiclib -single_module -undefined dynamic_lookup\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("endif\n");
#elif defined( R__LINUX )
#   if defined(USE_PIC_UPPER)
   buffer.AppendFormatted("oscflags  := -fPIC");
#   else
#      if defined(USE_PIC_LOWER)
   buffer.AppendFormatted("oscflags  := -fpic");
#      else
   buffer.AppendFormatted("oscflags  :=");
#      endif
#   endif
   buffer.AppendFormatted(" -Wno-unused-function\n");
   buffer.AppendFormatted("oslibs    := -lutil\n");
   buffer.AppendFormatted("ossoflags := -shared\n");
#elif defined( R__SOLARIS )
   buffer.AppendFormatted("oscflags  :=\n");
   buffer.AppendFormatted("oslibs    := -lsocket -lnsl\n");
   buffer.AppendFormatted("ossoflags := -G\n");
#else
   buffer.AppendFormatted("oscflags  :=\n");
   buffer.AppendFormatted("oslibs    :=\n");
   buffer.AppendFormatted("ossoflags := -shared\n");
#endif
   // DAQ Flaqs
   buffer.AppendFormatted("daqlibs   := \n");
   buffer.AppendFormatted("daqcflags := \n");
   for (i = 0; i < daqFlags->GetEntriesFast(); i++) {
      buffer.AppendFormatted("daqcflags += %s\n",daqFlags->At(i).Data());
   }
   buffer.AppendFormatted("ifdef MIDAS_MAX_EVENT_SIZE\n");
   buffer.AppendFormatted("  daqcflags += -DMAX_EVENT_SIZE=$(MIDAS_MAX_EVENT_SIZE)\n");
   buffer.AppendFormatted("endif\n");
   // DAQ Libraries
   for (i = 0; i < daqLibraries->GetEntriesFast(); i++) {
      buffer.AppendFormatted("daqlibs   += %s\n",daqLibraries->At(i).Data());
   }
   buffer.AppendFormatted("clibs     := -lz $(SYSLIBS)\n");
   buffer.AppendFormatted("\n");

   // Flags
   buffer.AppendFormatted("Flags     := $(oscflags) $(rootcflags) $(sqlcflags) $(daqcflags)");
   for (i = 0; i < numOfMFPreDefs; i++)
      buffer.AppendFormatted(" -D%s",mfPreDefName[i].Data());
   buffer.AppendFormatted(" -D%s_%s",shortCut.ToUpper(tmp),mainProgName.ToUpper(tmp2));
   buffer.AppendFormatted("\n");

   // libs
   buffer.AppendFormatted("Libraries :=\n");
   buffer.AppendFormatted("Libraries += $(oslibs) $(sqllibs) $(daqlibs) $(rootglibs) $(clibs)\n");
   for (i = 0; i < numOfMFUnixLibs; i++) {
      for (j = 0; j < numOfMFUnixLibFlags[i]; j++)
         buffer.AppendFormatted("ifdef %s\n",mfUnixLibFlag[i][j].Data());
      buffer.AppendFormatted("Libraries += -l%s\n",mfUnixLibName[i].Data());
      for (j = 0; j < numOfMFUnixLibFlags[i]; j++)
         buffer.AppendFormatted("endif # %s\n",mfUnixLibFlag[i][j].Data());
   }
   buffer.AppendFormatted("LDFLAGS   := $(osldflags)\n");
   buffer.AppendFormatted("SOFLAGS   := $(ossoflags)\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("## Object specific comiple options\n");
   buffer.AppendFormatted("NOOPT                     %s -O0\n",kEqualSign);
   Int_t n;
   // equal signs below should be '=' to allow change in Makefile.usr
   n = dictionaryHeaders->GetEntriesFast();
   if (n > 0) {
      for (i = 0; i < (n - 1) / maxNumberOfClassesInDictionary + 1; i++) {
         buffer.AppendFormatted("Dict%dOpt                        = $(NOOPT)\n", i);
      }
   }
   buffer.AppendFormatted("%sUserDictOpt                    = $(NOOPT)\n",shortCut.Data());

   buffer.AppendFormatted("%sAnalyzer3Opt                   = $(NOOPT)\n",shortCut.Data());
   buffer.AppendFormatted("%sAnalyzer4Opt                   = $(NOOPT)\n",shortCut.Data());
   buffer.AppendFormatted("%sConfigOpt                      = $(NOOPT)\n",shortCut.Data());
   buffer.AppendFormatted("%sConfig2Opt                     = $(NOOPT)\n",shortCut.Data());
   buffer.AppendFormatted("%sConfig3Opt                     = $(NOOPT)\n",shortCut.Data());
   buffer.AppendFormatted("%sConfig4Opt                     = $(NOOPT)\n",shortCut.Data());
   buffer.AppendFormatted("%sConfigToFormOpt                = $(NOOPT)\n",shortCut.Data());
   if (midas) {
     buffer.AppendFormatted("%sMidasDAQOpt                    = -fno-strict-aliasing\n",shortCut.Data());
   }
     buffer.AppendFormatted("ROMEMidasDAQOpt                  = -fno-strict-aliasing\n");
   buffer.AppendFormatted("\n");
#endif // R__UNIX
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileIncludes(ROMEString& buffer)
{
   ROMEString separator = " ";
   ROMEString tmp;
#if defined( R__UNIX )
   separator = " \\\n            ";
#endif // R__UNIX
   buffer.AppendFormatted("## Include directories\n");
   buffer.AppendFormatted("Includes %s $(%sINC) ",kEqualSign, shortCut.ToUpper(tmp));
   GetIncludeDirString(tmp,separator.Data(),kFlagSign);
   buffer.Append(tmp);
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefilePrecompiledHeaders(ROMEString& buffer)
{
   ROMEString separator = " ";
   ROMEString tmp;
#if defined( R__UNIX )
   separator = " \\\n            ";
#endif // R__UNIX
   buffer.AppendFormatted("## Precompiled headers\n");
   buffer.AppendFormatted("PCHHEADERS %s",kEqualSign);
   int i;
   for (i = 0; i < precompiledHeaders->GetEntriesFast(); i++)
      buffer.AppendFormatted("%sinclude/%s.gch",separator.Data(),precompiledHeaders->At(i).Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
}

//______________________________________________________________________________
void ROMEBuilder::GetIncludeDirString(ROMEString& buffer,const char* separator,const char* flagSign)
{
   int i;
   buffer = "";
   for (i = 0; i < includeDirectories->GetEntriesFast(); i++) {
      if (i > 0)
         buffer.AppendFormatted(separator);
      buffer.AppendFormatted("%sI%s",flagSign,includeDirectories->At(i).Data());
   }
   for (i = 0; i < numOfMFIncDirs; i++) {
      if (i > 0 || includeDirectories->GetEntriesFast() > 0)
         buffer.AppendFormatted(separator);
      buffer.AppendFormatted("%sI%s",flagSign,mfIncDir[i].Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileObjects(ROMEString& buffer,ROMEStrArray* sources)
{
   int i;
   ROMEString path;
   ROMEString fileName;
   ROMEString fileExtension;
   static ROMEString separator = " ";
   if (sources->GetEntriesFast() > 0) {
#if defined( R__UNIX )
      static bool calledFirstTime = true;
      if (calledFirstTime) {
         if (dynamicLink) {
            buffer.AppendFormatted("dlobjects +=");
         } else {
            buffer.AppendFormatted("objects +=");
         }
         calledFirstTime = false;
      }
#else
      buffer.AppendFormatted("objects %s $(objects)",kEqualSign);
#endif // R__UNIX
      for (i = 0; i < sources->GetEntriesFast(); i++) {
         AnalyzeFileName(sources->At(i).Data(),path,fileName,fileExtension);
         if (!dynamicLink || (dynamicLink && fileName != "main"))
            buffer.AppendFormatted("%sobj/%s%s",separator.Data(),fileName.Data(),kObjectSuffix);
#if defined( R__UNIX )
         separator = " \\\n           ";
#endif // R__UNIX
      }
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("\n");
#endif // R__VISUAL_CPLUSPLUS
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileUserDictObject(ROMEString& buffer)
{
#if defined( R__UNIX )
   buffer.AppendFormatted("ifdef DictionaryHeaders\n");
   if (dynamicLink) {
      buffer.AppendFormatted("dlobjects := obj/%sUserDict%s $(dlobjects)\n",shortCut.Data(), kObjectSuffix);
   } else {
      buffer.AppendFormatted("objects := obj/%sUserDict%s $(objects)\n",shortCut.Data(), kObjectSuffix);
   }
   buffer.AppendFormatted("endif\n");
#else
   buffer.AppendFormatted("objects = $(objects) obj/%sUserDict%s\n",shortCut.Data(), kObjectSuffix);
#endif // R__UNIX
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileUserDictDependFiles(ROMEString& buffer)
{
#if defined( R__UNIX )
   buffer.AppendFormatted("ifdef DictionaryHeaders\n");
   buffer.AppendFormatted("dependfiles += obj/%sUserDict.d\n",shortCut.Data());
   buffer.AppendFormatted("endif\n");
#endif // R__UNIX
   buffer.AppendFormatted("\n");
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileDictionaryList(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers)
{
   ROMEString separator = " ";
   ROMEString tmp;
#if defined( R__UNIX )
   separator = " \\\n     ";
#endif // R__UNIX
   Int_t i;
   Int_t n = headers->GetEntriesFast();
   if (n > 0) {
      for (i = 0; i < (n - 1) / maxNumberOfClassesInDictionary + 1; i++) {
         buffer.AppendFormatted("%sionaryHeaders%d %s ", dictionaryName, i, kEqualSign);
         GetDictHeaderString(tmp,headers,separator.Data(), kFALSE, i);
         // Use Append instead of AppendFormatted because includes can be long and AppendFormatted may not work. (limit is 2048 chars)
         buffer.Append(tmp);
         buffer.Append("\n\n");
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::GetDictHeaderString(ROMEString& buffer,ROMEStrArray* headers,const char* separator,
                                      Bool_t withoutPath, Int_t iFile)
{
   int i;

   ROMEString str;
   buffer = "";
   for (i = iFile * maxNumberOfClassesInDictionary;
        i < headers->GetEntriesFast() && i < (iFile + 1) * maxNumberOfClassesInDictionary; i++) {
      if (i > iFile * maxNumberOfClassesInDictionary) {
         buffer.AppendFormatted(separator);
      }
      str = headers->At(i);
      if (withoutPath) {
         str = str(str.Last('/') + 1, str.Length());
      }
      buffer.AppendFormatted(str.Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileDictionary(ROMEString& buffer,const char* dictionaryName,ROMEStrArray* headers,
                                          const char* /* linkDefName */)
{
   if (!headers->GetEntriesFast())
      return;

   ROMEString str;
   ROMEString tmp;
   ROMEString bufferT;
   ROMEString dictionaryNameMod;
   ROMEString arguments;
   ROMEString includedirs;
   ROMEString includes;
   int i;

   Int_t nClass = headers->GetEntriesFast();
   Int_t iFile;

   for (iFile = 0; iFile < (nClass - 1) / maxNumberOfClassesInDictionary + 1; iFile++) {
      dictionaryNameMod.SetFormatted("%s%d", dictionaryName, iFile);
      // depend file
#if defined( R__UNIX )
      buffer.AppendFormatted("obj/%sionary%d.d:dict/%s%d.h\n", dictionaryName, iFile, dictionaryName, iFile);
      buffer.AppendFormatted("\t$(call %sechoing, \"creating  obj/%sionary%d.d\")\n",shortCut.ToLower(tmp),dictionaryName, iFile);
      buffer.AppendFormatted("\t$(Q)$(CXX) $(Flags) $(Includes) -MM -MT dict/%s%d.cpp src/generated/%s%dDummy.cpp | \\\n\tsed \"s/.\\/dict\\/%s%d.h//g\" | sed \"s/dict\\/%s%d.h//g\" > $@ ",
                             dictionaryName, iFile, dictionaryName, iFile, dictionaryName, iFile, dictionaryName, iFile);
      buffer.AppendFormatted(" || ($(RM) $@; exit 1;)\n");
      buffer.AppendFormatted("\n");
#endif
      //dummy source file
      WriteMakefileDictDummyCpp(dictionaryNameMod.Data());
      dictionaryNames->AddFormatted(dictionaryNameMod.Data());

      // Output files
      bufferT.SetFormatted("dict/%s%d.h dict/%s%d.cpp:",dictionaryName, iFile, dictionaryName, iFile);
      buffer.AppendFormatted(bufferT.Data());
      bufferT.ReplaceAll(" ",";");
      bufferT.ReplaceAll(":","");
      dictionaryOutputs->AddFormatted(bufferT.Data());

      // Dependencies
      buffer.AppendFormatted(" $(%sionaryHeaders%d)",dictionaryName, iFile);
      buffer.AppendFormatted(" $(%sionary%dDep)", dictionaryName, iFile);
      buffer.AppendFormatted(" include/generated/%s%dLinkDef.h\n", dictionaryName, iFile);

      // Echo
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("\t@echo \"creating  dict/%s%d.cpp dict/%s%d.h\"\n",
                             dictionaryName, iFile, dictionaryName, iFile);
#else
      buffer.AppendFormatted("\t$(call %sechoing, \"creating  dict/%s%d.cpp dict/%s%d.h\")\n",shortCut.ToLower(tmp),
                             dictionaryName, iFile, dictionaryName, iFile);
      buffer.AppendFormatted("\t-@$(RM) dict/%s%d.cpp dict/%s%d.h\n",dictionaryName, iFile, dictionaryName, iFile);
#endif // R__UNIX

      // Command
      WriteRootCintCall(buffer);
      arguments.SetFormatted(" -f dict/%s%d.cpp -c -p",dictionaryName, iFile);
      for (i = 0; i < affiliations.GetEntriesFast(); i++) {
         arguments.AppendFormatted(" -DHAVE_%s", static_cast<ROMEString>(affiliations.At(i)).ToUpper(tmp));
      }
      buffer.AppendFormatted(arguments.Data());
#if defined( R__UNIX )
      buffer.AppendFormatted(" $(Includes)");
#endif // R__UNIX
      buffer.AppendFormatted(" $(DictionaryIncludes)");
      buffer.AppendFormatted(" $(%sionaryHeaders%d)",dictionaryName, iFile);
      if (dictionaryType > 0) {
         buffer.AppendFormatted(" include/generated/%s%dLinkDef.h",dictionaryName, iFile);
      }
      buffer.AppendFormatted("\n\n\n");

      GetDictHeaderString(bufferT,headers,";",false,iFile);
      dictionaryDependencies->AddFormatted(bufferT.Data());
      GetIncludeDirString(includedirs," ","-");
      GetDictHeaderString(includes,headers," ",true,iFile);

      // Use Append instead of AppendFormatted because includes can be long and AppendFormatted may not work. (limit is 2048 chars)
      ROMEString command = "rootcint";
      command.Append(arguments);
      command.Append(" ");
      command.Append(includedirs);
      for (i = 0; i < numOfMFDictIncDirs; i++)
         command.AppendFormatted(" -I%s",mfDictIncDir[i].Data());
      command.Append(" ");
      command.Append(includes);
      if (dictionaryType > 0) {
         command.Append(" include/generated/");
         command.Append(dictionaryName);
         command += iFile;
         command.Append("LinkDef.h");
      }
      dictionaryCommands->Add(command);
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileDictDummyCpp(const char* dictionaryName)
{

   ROMEString cppFile;
   cppFile.SetFormatted("src/generated/%sDummy.cpp", dictionaryName);
   ROMEString buffer;
   buffer.SetFormatted("#include \"dict/%s.h\"\n", dictionaryName);
   WriteFile(cppFile.Data(),buffer.Data(),6);
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileUserDictionaryList(ROMEString& buffer)
{
   ROMEString str;
   ROMEString tmp;
   ROMEString separator = " ";
#if defined( R__UNIX )
   separator = " \\\n      ";
#endif // R__UNIX

   if (numOfMFDictIncDirs > 0) {
#if defined( R__UNIX )
      buffer.AppendFormatted("DictionaryIncludes +=");
#else
      buffer.AppendFormatted("DictionaryIncludes %s $(DictionaryIncludes)", kEqualSign);
#endif // R__UNIX
      GetUserDictIncludeDirString(tmp,separator.Data());
      buffer.Append(tmp);
      buffer.Append("\n");
   }
}

//______________________________________________________________________________
void ROMEBuilder::GetUserDictIncludeDirString(ROMEString& buffer,const char* separator)
{
   int i;
   buffer = "";
   ROMEString str;
   for (i = 0; i < numOfMFDictIncDirs; i++) {
      str = mfDictIncDir[i].Data();
#if defined( R__VISUAL_CPLUSPLUS )
      str.ReplaceAll("$(","%");
      str.ReplaceAll(")","%");
#endif
      if (i > 0)
         buffer.AppendFormatted(separator);
      buffer.AppendFormatted("-I%s",str.Data());
   }
}

#if 0
//______________________________________________________________________________
void ROMEBuilder::GetUserDictHeaderString(ROMEString& buffer,const char* separator, Int_t iFile)
{
   int i;
   bool first = true;
   buffer = "";
   for (i = iFile * maxNumberOfClassesInDictionary;
        i < numOfMFDictHeaders && i < (iFile + 1) * maxNumberOfClassesInDictionary; i++) {
      if (!mfDictHeaderUsed[i]) {
         continue;
      }
      if (!first) {
         buffer.AppendFormatted(separator);
      }
      buffer.AppendFormatted(mfDictHeaderName[i].Data());
      first = false;
   }
}
#endif

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileUserDictionary(ROMEString& buffer)
{
   int i;
   ROMEString bufferT;
   ROMEString str;
   ROMEString tmp;
   ROMEString dictionaryName;
   dictionaryName.SetFormatted("%sUserDict",shortCut.Data());
   ROMEString dictionaryNameMod;
   ROMEString arguments;
   ROMEString includedirs;
   ROMEString includes;

#if 0
   // dictionary depend file
   buffer.AppendFormatted("obj/%sUserDictionary.d:dict/%sUserDict.h\n",shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("\t$(call %sechoing, \"creating  obj/%sUserDictionary.d\")\n",shortCut.ToLower(tmp),
                          shortCut.Data());
   buffer.AppendFormatted("\t$(Q)$(CXX) $(Flags) $(Includes) -MM -MT dict/%sUserDict.cpp src/generated/%sUserDictDummy.cpp | sed \"s/.\\/dict\\/%sUserDict.h//g\" | sed \"s/dict\\/%sUserDict.h//g\" > $@ ",
                             shortCut.Data(), shortCut.Data(), shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted(" \\\n\t   || ($(RM) obj/%sUserDictionary.d; exit 1;)\n",shortCut.Data());
   buffer.AppendFormatted("\n");
#endif
   //dummy source file
   WriteMakefileDictDummyCpp(dictionaryName);

   // Output files
   bufferT.SetFormatted("dict/%sUserDict.h dict/%sUserDict.cpp:",shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted(bufferT.Data());
   bufferT.ReplaceAll(" ",";");
   bufferT.ReplaceAll(":","");
   dictionaryOutputs->AddFormatted(bufferT.Data());

   // Dependencies
   buffer.AppendFormatted(" $(DictionaryHeaders)\n");

   // Command
#if defined( R__UNIX )
   buffer.AppendFormatted("ifdef DictionaryHeaders\n");
   buffer.AppendFormatted("\t@if [ -e dict/%sUserDict.cpp ]; then $(RM) dict/%sUserDict.cpp; fi;\n",shortCut.Data(),
                             shortCut.Data());
   buffer.AppendFormatted("\t@if [ -e dict/%sUserDict.h ]; then $(RM) dict/%sUserDict.h; fi;\n",shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("\t$(call %sechoing, \"creating  dict/%sUserDict.h dict/%sUserDict.cpp\")\n",
                          shortCut.ToLower(tmp),shortCut.Data(), shortCut.Data());
#endif
   WriteRootCintCall(buffer);
   arguments.SetFormatted(" -f dict/%sUserDict.cpp -c -p",shortCut.Data());
   for (i = 0; i < affiliations.GetEntriesFast(); i++) {
      arguments.AppendFormatted(" -DHAVE_%s", static_cast<ROMEString>(affiliations.At(i)).ToUpper(tmp));
   }
   buffer.AppendFormatted(arguments.Data());
#if defined( R__UNIX )
   buffer.AppendFormatted(" $(Includes)");
#endif // R__UNIX
   buffer.AppendFormatted(" $(DictionaryIncludes)");
   buffer.AppendFormatted(" $(DictionaryHeaders)");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("\n");

#if 0
   GetUserDictHeaderString(bufferT,";",iFile);
   dictionaryDependencies->AddFormatted(bufferT.Data());
   GetIncludeDirString(includedirs," ","-");
   GetUserDictIncludeDirString(bufferT," ");
   includedirs.AppendFormatted(" %s",bufferT.Data());
   GetUserDictHeaderString(includes," ",iFile);
#if defined( R__UNIX )
   dictionaryCommands->AddFormatted("$(Q)$(ROOTCINT)%s %s %s",arguments.Data(),includedirs.Data(),includes.Data());
#else
   ROMEString command = "rootcint";
   command.Append(arguments);
   command.Append(" ");
   command.Append(includedirs);
   command.Append(" ");
   command.Append(includes);
   dictionaryCommands->Add(command);
#endif
#endif
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileCompileStatements(ROMEString& buffer,ROMEStrArray* sources,const char* flag,
                                                 ROMEString *objdir)
{
   int i;
   ROMEString path;
   ROMEString name;
   ROMEString ext;
   ROMEString additionalFlag = flag;
#if defined( R__UNIX )
   ROMEString commandPrefix;
   ROMEString tmp;
   ROMEString compiler;
   ROMEString compileOption;
#endif
   ROMEString objdirectory;
   if (objdir) {
      objdirectory = *objdir;
   } else {
      objdirectory = "obj";
   }

   for (i = 0; i < sources->GetEntriesFast(); i++) {
      AnalyzeFileName(sources->At(i).Data(),path,name,ext);
#if defined( R__UNIX )
      if (ext == "c") {
         compiler.SetFormatted("$(Q)$(CC)");
         compileOption.SetFormatted("$(%sCFLAGS)", shortCut.ToUpper(tmp));
      } else if (ext == "F" || ext == "f") {
         compiler.SetFormatted("$(Q)$(FC)");
         compileOption.SetFormatted("$(%sFFLAGS)", shortCut.ToUpper(tmp));
      } else if (ext == "h") { // c++ precompiled header
         compiler.SetFormatted("$(Q)$(CXX)");
         compileOption.SetFormatted("$(%sCXXFLAGS) -x c++-header", shortCut.ToUpper(tmp));
      } else {
         compiler.SetFormatted("$(Q)$(CXX)");
         compileOption.SetFormatted("$(%sCXXFLAGS) %s", shortCut.ToUpper(tmp),additionalFlag.Data());
      }

      path.ReplaceAll("\\","/");

      if (ext == "h") {
         buffer.AppendFormatted("include/%s.gch : ./include/%s $(%sDep)\n"
                                ,sources->At(i).Data(),sources->At(i).Data(),name.Data());
         buffer.AppendFormatted("\t$(call %sechoing, \"compiling include/%s.gch\")\n",shortCut.ToLower(tmp),
                                sources->At(i).Data());
         buffer.AppendFormatted("\t%s -c %s $(Flags) $(%sOpt) $(Includes) -MMD -MP -MF %s/%s.d $< -o $@\n"
                                ,compiler.Data(),compileOption.Data(),name.Data(),objdirectory.Data(),name.Data());
      } else {//if (path.Index("/dict/") != -1 || path.Index("dict/") == 0) {
         buffer.AppendFormatted("%s/%s%s : %s $(%sDep)\n",objdirectory.Data()
                                ,name.Data(),kObjectSuffix,sources->At(i).Data(),name.Data());
         buffer.AppendFormatted("\t$(call %sechoing, \"compiling %s/%s%s\")\n",shortCut.ToLower(tmp),
                                objdirectory.Data()
                                ,name.Data(),kObjectSuffix);
         buffer.AppendFormatted("\t%s -c %s $(Flags) $(%sOpt) $(Includes) -MMD -MP -MF %s/%s.d -MT $@ $< -o $@\n"
                                ,compiler.Data(),compileOption.Data(),name.Data(),objdirectory.Data(),name.Data());
      }
      buffer.AppendFormatted("\n");
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
      buffer.AppendFormatted("!INCLUDE obj/%s.d\n",name.Data());
      if (path.Index("/dict/") != -1 || path.Index("dict/") == 0) {
         buffer.AppendFormatted("obj/%s.obj: dict/%s.cpp\n",name.Data(),name.Data(),name.Data());
      } else {
         buffer.AppendFormatted("obj/%s.obj: %s\n",name.Data(),sources->At(i).Data(),name.Data());
      }
      buffer.AppendFormatted("\t@cd obj/\n",name.Data());
      buffer.AppendFormatted("\t@start /MIN %s\n",name.Data());
      buffer.AppendFormatted("\t@cd ../\n",name.Data());
      buffer.AppendFormatted("\t@cl /nologo /c $(Flags) $(Includes) %s /Foobj/%s.obj\n",sources->At(i).Data(),
                             name.Data());

      batBuffer.AppendFormatted("cd ../\n");
      batBuffer.AppendFormatted("cd %s\n",path.Data());
      batBuffer.AppendFormatted("rmkdepend");
      for (j = 0; j < includeDirectories->GetEntriesFast(); j++) {
         str = includeDirectories->At(j).Data();
         str.ReplaceAll("$(","%");
         str.ReplaceAll(")","%");
         batBuffer.AppendFormatted(" -I%s",str.Data());
      }
      for (j = 0; j < numOfMFIncDirs; j++) {
         str = mfIncDir[j].Data();
         str.ReplaceAll("$(","%");
         str.ReplaceAll(")","%");
         batBuffer.AppendFormatted(" -I%s",str.Data());
      }
      batBuffer.AppendFormatted(" -f %sobj/%s.d -o .obj -p obj/ %s.%s\n",outDir.Data(),name.Data(),name.Data(),
                                ext.Data());
      batBuffer.AppendFormatted("ReplaceInFile -f %sobj/%s.d -s ./ -r %s\n",outDir.Data(),name.Data(),path.Data());
      batBuffer.AppendFormatted("ReplaceInFile -f %sobj/%s.d -s \" *.h\" -r \" %s/*.h\" -v \" /\\\"\n",outDir.Data(),
                                name.Data(),path.Data());
      batBuffer.AppendFormatted("cd %sobj\n",outDir.Data());
      batBuffer.AppendFormatted("exit\n",outDir.Data());
      batFileName.SetFormatted("%sobj/%s.bat",outDir.Data(),name.Data());
      WriteFile(batFileName.Data(),batBuffer.Data(),6);
      dFileName.SetFormatted("%sobj/%s.d",outDir.Data(),name.Data());
      if (gSystem->AccessPathName(dFileName.Data(),kFileExists))
         WriteFile(dFileName.Data(),dBuffer.Data(),6);
#endif // R__VISUAL_CPLUSPLUS
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileDependFiles(ROMEString& buffer,ROMEStrArray* sources)
{
#if defined( R__UNIX )
   if (!sources->GetEntriesFast())
      return;

   int i;
   ROMEString path;
   ROMEString name;
   ROMEString ext;

   buffer.AppendFormatted("dependfiles +=");
   for (i = 0; i < sources->GetEntriesFast(); i++) {
      AnalyzeFileName(sources->At(i).Data(),path,name,ext);
      buffer.AppendFormatted(" obj/%s.d",name.Data());
      if (i != sources->GetEntriesFast() - 1)
         buffer.AppendFormatted(" \\\n              ");
   }
   buffer.AppendFormatted("\n");
#endif // R__UNIX
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileAdditionalSourceFilesObjects(ROMEString& buffer)
{
   // Write Additional Source Files Objects
   int i,j,k;
   ROMEString path;
   ROMEString name;
   ROMEString ext;

   // without flags
   for (i = 0; i < numOfMFSources; i++) {
      if (!mfSourceFileUsed[i])
         continue;
      if (numOfMFSourceFlags[i])
         continue;
      AnalyzeFileName(mfSourceFileName[i].Data(),path,name,ext);
#if defined( R__UNIX )
      buffer.AppendFormatted(" \\\n           %s/%s%s",mfSourceFileObjPath[i].Data(),name.Data(),kObjectSuffix);
#else
      buffer.AppendFormatted("objects %s $(objects) %s/%s%s\n",kEqualSign,mfSourceFileObjPath[i].Data(),name.Data(),
                             kObjectSuffix);
#endif // R__UNIX
   }
   buffer.AppendFormatted("\n");

   // without cflags
   for (i = 0; i < numOfMFSources; i++) {
      if (!mfSourceFileUsed[i])
         continue;
      if (numOfMFSourceFlags[i] < 1)
         continue;
      buffer.AppendFormatted("\n");
      bool *commandLineFlag = new bool[numOfMFSourceFlags[i]];
      for (j = 0; j < numOfMFSourceFlags[i]; j++) {
         commandLineFlag[j] = false;
         for (k = 0; k < flags.GetEntriesFast(); k++) {
            if (mfSourceFileFlag[i][j] == flags[k])
               commandLineFlag[j] = true;
         }
      }
      for (j = 0; j < numOfMFSourceFlags[i]; j++) {
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
      if (dynamicLink) {
         buffer.AppendFormatted("dlobjects += %s/%s%s\n",mfSourceFileObjPath[i].Data(),name.Data(),kObjectSuffix);
      } else {
         buffer.AppendFormatted("objects += %s/%s%s\n",mfSourceFileObjPath[i].Data(),name.Data(),kObjectSuffix);
      }
#else
      buffer.AppendFormatted("objects %s $(objects) %s/%s%s\n",kEqualSign,mfSourceFileObjPath[i].Data(),name.Data(),
                             kObjectSuffix);
#endif // R__UNIX
      for (j = numOfMFSourceFlags[i] - 1; j >= 0; j--) {
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

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileAdditionalSourceDependFiles(ROMEString& buffer)
{
   // Write Additional Source Files Objects
   int i,j,k;
   ROMEString path;
   ROMEString name;
   ROMEString ext;

   for (i = 0; i < numOfMFSources; i++) {
      if (!mfSourceFileUsed[i])
         continue;
      bool *commandLineFlag = new bool[numOfMFSourceFlags[i]];
      for (j = 0; j < numOfMFSourceFlags[i]; j++) {
         commandLineFlag[j] = false;
         for (k = 0; k < flags.GetEntriesFast(); k++) {
            if (mfSourceFileFlag[i][j] == flags[k])
               commandLineFlag[j] = true;
         }
      }
      for (j = 0; j < numOfMFSourceFlags[i]; j++) {
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
      buffer.AppendFormatted("dependfiles += %s/%s.d\n",mfSourceFileObjPath[i].Data(),name.Data());
#endif // R__UNIX
      for (j = numOfMFSourceFlags[i] - 1; j >= 0; j--) {
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

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileAdditionalSourceFilesCompileStatments(ROMEString& buffer,const char* flag)
{
   // Write Additional Source Files Compile Commands
   int i,j,k;
   for (i = 0; i < numOfMFSources; i++) {
      if (!mfSourceFileUsed[i])
         continue;
      bool *commandLineFlag = new bool[numOfMFSourceFlags[i]];
      for (j = 0; j < numOfMFSourceFlags[i]; j++) {
         commandLineFlag[j] = false;
         for (k = 0; k < flags.GetEntriesFast(); k++) {
            if (mfSourceFileFlag[i][j] == flags[k])
               commandLineFlag[j] = true;
         }
      }
      for (j = 0; j < numOfMFSourceFlags[i]; j++) {
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
      ROMEString objdir;
      tempArray->AddFormatted("%s%s",mfSourceFilePath[i].Data(),mfSourceFileName[i].Data());
      objdir.SetFormatted("%s",mfSourceFileObjPath[i].Data());
      WriteMakefileCompileStatements(buffer,tempArray,flag,&objdir);
      delete tempArray;
      for (j = numOfMFSourceFlags[i] - 1; j >= 0; j--) {
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

//______________________________________________________________________________
void ROMEBuilder::WriteMakefileBuildRule(ROMEString& buffer,const char *builder)
{
   ROMEString xmlbasename = gSystem->BaseName(xmlFile);

   buffer.AppendFormatted("\t%s -i %s -o .", builder, xmlbasename.Data());
   if (makeOutput)
      buffer.AppendFormatted(" -v");
   if (noLink)
      buffer.AppendFormatted(" -nl");
   if (dynamicLink)
      buffer.AppendFormatted(" -dl");
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
   if (pch)
      buffer.AppendFormatted(" -pch");
   if (minRebuild)
      buffer.AppendFormatted(" -minrb");
   if (quietMake)
      buffer.AppendFormatted(" -qm");
   if (flags.GetEntriesFast())
      buffer.AppendFormatted(" -f");
   int i;
   for (i = 0; i < flags.GetEntriesFast(); i++)
      buffer.AppendFormatted(" %s",flags.At(i).Data());
   if (affiliations.GetEntriesFast())
      buffer.AppendFormatted(" -a");
   for (i = 0; i < affiliations.GetEntriesFast(); i++)
      buffer.AppendFormatted(" %s",affiliations.At(i).Data());
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted(" -nvp");
#endif
   buffer.AppendFormatted("\n");
}

//______________________________________________________________________________
void ROMEBuilder::WriteMakefile() {
   // write a Makefile
   ROMEString buffer;
   ROMEString tmp,tmp2,tmp3,tmp4,tmp5;
   ROMEString path, name, ext;
   int i;

   dictionaryNames = new ROMEStrArray(50);
   dictionaryOutputs = new ROMEStrArray(50);
   dictionaryDependencies = new ROMEStrArray(50);
   dictionaryCommands = new ROMEStrArray(50);

   AddIncludeDirectories();
   AddRomeHeaders();
   AddRomeDictHeaders();
   AddArgusHeaders();
   AddGeneratedHeaders();
   AddGeneratedDictHeaders();
   AddGeneratedFolderDictHeaders();
   AddGeneratedSupportFolderDictHeaders();
   AddGeneratedTaskDictHeaders();
   AddGeneratedTabDictHeaders();
   AddFolderHeaders();
   AddTaskHeaders();
   AddTabHeaders();
   AddDAQHeaders();
   AddDatabaseHeaders();
   AddAdditionalDictHeaders();
   AddDictionaryHeaders();
   AddRomeSources();
   AddArgusSources();
   AddGeneratedSources();
   AddFolderSources();
   AddTaskSources();
   AddTabSources();
   AddDAQSources();
   AddDatabaseSources();
   AddDictionarySources();
   AddRootLibraries();
   AddMysqlLibraries();
   AddDAQLibraries();
   AddDAQFlags();
   AddPrecompiledHeaders();
   if (pch) {
      WritePrecompiledHeaders();
   }

   WriteMakefileHeader(buffer);

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("LD_LIBRARY_PATH=$(ROOTSYS)/lib\n");
#else
   buffer.AppendFormatted("ifdef ROOTSYS\n");
   buffer.AppendFormatted("ROOTCONFIG := $(ROOTSYS)/bin/root-config\n");
   buffer.AppendFormatted("ROOTCINT   := $(ROOTSYS)/bin/rootcint\n");
   buffer.AppendFormatted("else\n");
   buffer.AppendFormatted("ROOTCONFIG := root-config\n");
   buffer.AppendFormatted("ROOTCINT   := rootcint\n");
   buffer.AppendFormatted("endif\n");
#   if defined( R__MACOSX )
   buffer.AppendFormatted("FINK_DIR        := $(shell which fink 2>&1 | sed -ne \"s/\\/bin\\/fink//p\")\n");
   buffer.AppendFormatted("MACOSX_MAJOR    := $(shell sw_vers | sed -n 's/ProductVersion:[^0-9]*//p' | cut -d . -f 1)\n");
   buffer.AppendFormatted("MACOSX_MINOR    := $(shell sw_vers | sed -n 's/ProductVersion:[^0-9]*//p' | cut -d . -f 2)\n");
   buffer.AppendFormatted("export MACOSX_DEPLOYMENT_TARGET := $(MACOSX_MAJOR).$(MACOSX_MINOR)\n");
   buffer.AppendFormatted("export DYLD_LIBRARY_PATH := $(DYLD_LIBRARY_PATH):$(shell $(ROOTCONFIG) --libdir)\n");
#   else
   buffer.AppendFormatted("export LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(shell $(ROOTCONFIG) --libdir)\n");
#   endif
#endif
   buffer.AppendFormatted("\n");

   WriteMakefileLibsAndFlags(buffer);
   WriteMakefileIncludes(buffer);
   WriteMakefilePrecompiledHeaders(buffer);

   // Dictionary list
   buffer.AppendFormatted("## Dictionary headers\n");
   WriteMakefileDictionaryList(buffer,shortCut+"Dict",dictionaryHeaders);
   buffer.AppendFormatted("DictionaryIncludes %s $(%sINC)",kEqualSign, shortCut.ToUpper(tmp));
#if defined( R__VISUAL_CPLUSPLUS )
   GetIncludeDirString(tmp," ","");
   buffer.Append(tmp);
   if (this->midas)
      buffer.AppendFormatted(" -I$(MIDASSYS)/include/");
#else
   if (this->midas)
      buffer.AppendFormatted(" \\\n                      -I$(MIDASSYS)/include -DHAVE_MIDAS");
   if (this->sql)
      buffer.AppendFormatted(" \\\n                      $(sqlcflags)");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("\n");
   WriteMakefileUserDictionaryList(buffer);
   buffer.AppendFormatted("\n");

// Objects
// -------
   buffer.AppendFormatted("## Objects\n");
   if (dynamicLink) {
      buffer.AppendFormatted("objects   += obj/main.o\n\n");
   }
   WriteMakefileObjects(buffer,generatedSources);
   WriteMakefileObjects(buffer,tabSources);
   WriteMakefileObjects(buffer,taskSources);
   WriteMakefileObjects(buffer,romeSources);
   WriteMakefileObjects(buffer,argusSources);
   WriteMakefileObjects(buffer,folderSources);
   WriteMakefileObjects(buffer,daqSources);
   WriteMakefileObjects(buffer,databaseSources);
   WriteMakefileObjects(buffer,dictionarySources);
   WriteMakefileAdditionalSourceFilesObjects(buffer);
   if (librome) {
      if (dynamicLink) {
         buffer.AppendFormatted("dlobjects += $(ROMESYS)/librome.a\n");
      } else {
         buffer.AppendFormatted("objects += $(ROMESYS)/librome.a\n");
      }
   }
   buffer.AppendFormatted("\n\n");

// Depend files
   buffer.AppendFormatted("## dependence files\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("dependfiles %s\n",kEqualSign);
#else
/*
   WriteMakefileDependFiles(buffer,romeSources);
   WriteMakefileDependFiles(buffer,argusSources);
   WriteMakefileDependFiles(buffer,folderSources);
   WriteMakefileDependFiles(buffer,taskSources);
   WriteMakefileDependFiles(buffer,tabSources);
   WriteMakefileDependFiles(buffer,generatedSources);
   WriteMakefileDependFiles(buffer,daqSources);
   WriteMakefileDependFiles(buffer,databaseSources);
   WriteMakefileDependFiles(buffer,additionalDictSources);
   WriteMakefileAdditionalSourceDependFiles(buffer);
   buffer.AppendFormatted("dependfiles %s $(objects:%s=.d)\n", kEqualSign,kObjectSuffix);
   if (pch) {
      for (i = 0; i < precompiledHeaders->GetEntriesFast(); i++) {
         AnalyzeFileName(precompiledHeaders->At(i),path,name,ext);
         buffer.AppendFormatted("dependfiles += obj/%s.d\n",name.Data());
      }
   }
*/
   Int_t n;
   n = dictionaryHeaders->GetEntriesFast();
   if (n > 0) {
      for (i = 0; i < (n - 1) / maxNumberOfClassesInDictionary + 1; i++) {
         buffer.AppendFormatted("dependfiles += obj/%sDictionary%d.d\n", shortCut.Data(), i);
      }
   }
   // buffer.AppendFormatted("dependfiles += obj/%sUserDictionary.d\n",shortCut.Data());
   buffer.AppendFormatted("\n");
#endif // R__UNIX

   buffer.AppendFormatted("##\n");
   buffer.AppendFormatted("## Compile and link rules\n");
   buffer.AppendFormatted("##\n");
// all
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("all:startecho obj");
#else
   buffer.AppendFormatted("all:obj");
#endif
   for (i = 0; i < objDirList.GetEntriesFast(); i++) {
      buffer.AppendFormatted(" %s",objDirList.At(i).Data());
   }
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted(" %s%s.exe endecho",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2));
#else
   buffer.AppendFormatted(" pch %s%s.exe\n",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2));
   buffer.AppendFormatted("\t@$(RM) $(NTARGETS_FILE)\n");
#endif
   buffer.AppendFormatted("\n\n");

   // user makefile
   buffer.AppendFormatted("# Include user Makefile\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("!INCLUDE Makefile.winusr\n");
#else
   buffer.AppendFormatted("-include Makefile.usr\n");
#endif // R__VISUAL_CPLUSPLUS
   // this must be after including Makefile.usr
   WriteMakefileUserDictObject(buffer);
//   WriteMakefileUserDictDependFiles(buffer);

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
   buffer.AppendFormatted("\n");

// make user obj directories
   for (i = 0; i < objDirList.GetEntriesFast(); i++) {
      buffer.AppendFormatted("%s:\n",objDirList.At(i).Data());
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("\t@mkdir %s\n\n",objDirList.At(i).Data());
#else
      buffer.AppendFormatted("\t@if [ ! -d  %s ] ; then \\\n",objDirList.At(i).Data());
      buffer.AppendFormatted("\t\techo \"Making directory %s\" ; \\\n",objDirList.At(i).Data());
      buffer.AppendFormatted("\t\tmkdir -p %s; \\\n",objDirList.At(i).Data());
      buffer.AppendFormatted("\tfi;\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("\n");
   }

// Link Statement
// --------------
   buffer.AppendFormatted("## Link statements\n");

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("%s%s.exe: $(dependfiles) $(objects) $(%s%sDep)\n",shortCut.ToLower(tmp),
                          mainProgName.ToLower(tmp2),shortCut.ToLower(tmp3),mainProgName.ToLower(tmp4));
   buffer.AppendFormatted("\t@echo linking %s%s...\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\t@cl /nologo /Fe%s%s.exe $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
#else
   ROMEString linker;
   ROMEString linkCommand;
   linker = "$(Q)$(CXXLD)";
   linkCommand = "$(Q)ln -sf";
   if (dynamicLink) {
      buffer.AppendFormatted("%s%s.exe: $(dependfiles) $(objects) obj/lib%s%s%s\n",shortCut.ToLower(tmp),
                             mainProgName.ToLower(tmp2),shortCut.Data(),mainProgName.Data(),kSharedObjectSuffix);
      buffer.AppendFormatted("\t$(call %sechoing, \"linking   %s%s.exe\")\n",shortCut.ToLower(tmp),
                             shortCut.ToLower(tmp2),mainProgName.ToLower(tmp3));
      buffer.AppendFormatted("\t%s $(%sLDFLAGS) $(LDFLAGS) -o $@ $(PWDST)/obj/lib%s%s%s $(objects) $(Libraries)\n",
                             linker.Data(),shortCut.ToUpper(tmp),shortCut.Data(),mainProgName.Data(),
                             kSharedObjectSuffix);
   } else {
      buffer.AppendFormatted("%s%s.exe: $(dependfiles) $(objects) $(%s%sDep)\n",shortCut.ToLower(tmp),
                             mainProgName.ToLower(tmp2),shortCut.ToLower(tmp3),mainProgName.ToLower(tmp4));
      buffer.AppendFormatted("\t$(call %sechoing, \"linking   %s%s.exe\")\n",shortCut.ToLower(tmp),
                             shortCut.ToLower(tmp2),mainProgName.ToLower(tmp3));
      buffer.AppendFormatted("\t%s $(%sLDFLAGS) $(LDFLAGS) -o $@ $(objects) $(Libraries)\n", linker.Data(),
                             shortCut.ToUpper(tmp));
   }
   buffer.AppendFormatted("ifneq (,$(findstring lib%s%s%s, $(shell ls)))\n",shortCut.ToLower(tmp),
                          mainProgName.ToLower(tmp2),kSharedObjectSuffix);
   buffer.AppendFormatted("ifeq ($(NTARGETS_STOP), yes)\n");
   buffer.AppendFormatted("\t@$(MAKE) %sVERBOSEMAKE=yes so\n", shortCut.Data());
   buffer.AppendFormatted("else\n");
   buffer.AppendFormatted("\t@$(MAKE) so\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("\t@$(RM) $(NTARGETS_FILE)\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("so: lib%s%s%s\n",shortCut.ToLower(tmp),mainProgName.ToLower(tmp2),kSharedObjectSuffix);
   // this library is for loading from ROOT session
   buffer.AppendFormatted("lib%s%s%s: $(dlobjects) $(objects) $(dependfiles) $(lib%s%sDep)\n",shortCut.ToLower(tmp),
                          mainProgName.ToLower(tmp2),kSharedObjectSuffix,shortCut.ToLower(tmp3),
                          mainProgName.ToLower(tmp4));
   buffer.AppendFormatted("\t$(call %sechoing, \"linking   lib%s%s%s\")\n",shortCut.ToLower(tmp),
                          shortCut.ToLower(tmp2),mainProgName.ToLower(tmp3),kSharedObjectSuffix);
   buffer.AppendFormatted("\t%s $(%sSOFLAGS) $(SOFLAGS) -o lib%s%s%s $(dlobjects) $(objects) $(Libraries)\n",
                          linker.Data(),shortCut.ToUpper(tmp),shortCut.ToLower(tmp2),mainProgName.ToLower(tmp3),
                          kSharedObjectSuffix);
#if defined( R__MACOSX )
   buffer.AppendFormatted("\t%s lib%s%s.dylib lib%s%s.so\n",linkCommand.Data(),shortCut.ToLower(tmp),
                          mainProgName.ToLower(tmp2),shortCut.ToLower(tmp3),mainProgName.ToLower(tmp4));
#endif // R__MACOSX
   buffer.AppendFormatted("\t@$(RM) $(NTARGETS_FILE)\n");
   buffer.AppendFormatted("\n");
   // this library is for linking executable binary
   if (dynamicLink) {
      buffer.AppendFormatted("obj/lib%s%s%s: $(dlobjects) $(dependfiles) $(%s%sDep)\n",shortCut.Data(),
                             mainProgName.Data(),kSharedObjectSuffix,shortCut.ToLower(tmp3),
                             mainProgName.ToLower(tmp4));
      buffer.AppendFormatted("\t$(call %sechoing, \"linking   obj/lib%s%s%s\")\n",shortCut.ToLower(tmp),
                             shortCut.Data(),mainProgName.Data(),kSharedObjectSuffix);
      buffer.AppendFormatted("\t%s $(%sSOFLAGS) $(SOFLAGS) -o $(PWDST)/obj/lib%s%s%s $(dlobjects)\n",linker.Data(),
                             shortCut.ToUpper(tmp),shortCut.Data(),mainProgName.Data(),kSharedObjectSuffix);
   }
   buffer.AppendFormatted("\n");
#endif // R__UNIX

// PCH
// ------------------
   buffer.AppendFormatted("## Precompiled header file generation\n");
   if (pch) {
      buffer.AppendFormatted("pch: $(PCHHEADERS)\n");
   } else {
      buffer.AppendFormatted("pch:\n");
      buffer.AppendFormatted("\t-@$(RM) $(PCHHEADERS)\n");
   }
   buffer.AppendFormatted("\n");

// Compile Statements
// ------------------
   buffer.AppendFormatted("## Compile statements\n");
   ROMEString additionalFlag;

#if 0
   // recompile librome.a.
   // commented out because of two reasons.
   //  1. This is, in priciple, responsibility of users. One should recompile librome.a when ROME is updated.
   //  2. It is not known if the user has write permission of the directory. ROMESYS can be in system, or shared directory read by several users.
   if (librome) {
      buffer.AppendFormatted("$(ROMESYS)/librome.a: $(ROMESYS)/include/*h $(ROMESYS)/src/*cpp\n");
      buffer.AppendFormatted("\t@$(MAKE) -C $(ROMESYS) librome.a\n");
      buffer.AppendFormatted("\n");
   }
#endif

   if (pch) {
      additionalFlag.SetFormatted("-include generated/%sPrecompile.h", shortCut.Data());
   }
   WriteMakefileCompileStatements(buffer,romeSources,additionalFlag.Data());
   WriteMakefileCompileStatements(buffer,argusSources,additionalFlag.Data());
   WriteMakefileCompileStatements(buffer,generatedSources,additionalFlag.Data());
   WriteMakefileCompileStatements(buffer,taskSources,additionalFlag.Data());
   WriteMakefileCompileStatements(buffer,tabSources,additionalFlag.Data());
   WriteMakefileCompileStatements(buffer,folderSources,additionalFlag.Data());
   WriteMakefileCompileStatements(buffer,daqSources,additionalFlag.Data());
   WriteMakefileCompileStatements(buffer,databaseSources,additionalFlag.Data());
   WriteMakefileCompileStatements(buffer,dictionarySources,additionalFlag.Data());
   WriteMakefileCompileStatements(buffer,precompiledHeaders,additionalFlag.Data());
   WriteMakefileAdditionalSourceFilesCompileStatments(buffer,additionalFlag.Data());
   ROMEStrArray *tempArray = new ROMEStrArray(1);
   tempArray->AddFormatted("dict/%sUserDict.cpp",shortCut.Data());
   WriteMakefileCompileStatements(buffer,tempArray,additionalFlag.Data());
   delete tempArray;
   buffer.AppendFormatted("\n");

// Dictionary
   WriteMakefileDictionary(buffer,shortCut+"Dict",dictionaryHeaders);
   WriteMakefileUserDictionary(buffer);
   buffer.AppendFormatted("\n");

// LinkDef.h
   WriteLinkDefH(dictionaryHeaders,dictionaryLinkDefSuffix,shortCut+"Dict");
   WriteUserLinkDefH();


   buffer.AppendFormatted("## Rebuild rules\n");
#if defined( R__VISUAL_CPLUSPLUS )
   ROMEString str;
   char* romesys = getenv("ROMESYS");
   buffer.AppendFormatted("build: \n");
   str.SetFormatted("%s/bin/romebuilder.exe",romesys);
   str.ReplaceAll("/","\\");
   str.ReplaceAll("\\\\","\\");
   WriteMakefileBuildRule(buffer,str.Data());
#else
   buffer.AppendFormatted("build: \n");
   WriteMakefileBuildRule(buffer,"$(ROMESYS)/bin/romebuilder.exe");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("src/generated/%% include/generated%%:\n");
   WriteMakefileBuildRule(buffer,"$(ROMESYS)/bin/romebuilder.exe -nl");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("## Cleaning rules\n");
   buffer.AppendFormatted("depclean:\n");
   buffer.AppendFormatted("\t-$(RM) obj/*.d");
   for (i = 0; i < objDirList.GetEntriesFast(); i++) {
      buffer.AppendFormatted(" %s/*.d",objDirList.At(i).Data());
   }
   buffer.AppendFormatted(" $(PCHHEADERS)\n");

   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("clean: depclean userclean\n");
   buffer.AppendFormatted("\t-$(RM) obj/*%s",kObjectSuffix);
   for (i = 0; i < objDirList.GetEntriesFast(); i++) {
      buffer.AppendFormatted(" %s/*%s",objDirList.At(i).Data(),kObjectSuffix);
   }
   buffer.AppendFormatted(" G__auto*LinkDef.h dict/*.h dict/*.cpp");
   buffer.AppendFormatted(" $(NTARGETS_FILE)");
   if (pch)
      buffer.AppendFormatted(" include/generated/*.gch");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("distclean: userdistclean clean\n");
   buffer.AppendFormatted("\t-$(RM) -R src/generated include/generated obj Makefile\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sclean: userclean\n",shortCut.ToLower(tmp));
   buffer.AppendFormatted("\t-$(RM) obj/%s*%s obj/%s*.d",shortCut.Data(),kObjectSuffix,shortCut.Data());
   for (i = 0; i < objDirList.GetEntriesFast(); i++) {
      buffer.AppendFormatted(" $(RM) %s/%s*%s %s/%s*.d",objDirList.At(i).Data(),shortCut.Data(),kObjectSuffix
                             ,objDirList.At(i).Data(),shortCut.Data());
   }
   buffer.AppendFormatted(" G__auto*LinkDef.h\n");

// .d files need to be place at the last
#if defined( R__VISUAL_CPLUSPLUS )
//   buffer.AppendFormatted("!INCLUDE obj/*.d\n");
#else
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("## Include dependence files\n");
   buffer.AppendFormatted("SkipDepInclude = no\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ifeq ($(MAKECMDGOALS), dep)\n");
   buffer.AppendFormatted("SkipDepInclude = yes\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ifeq ($(MAKECMDGOALS), depclean)\n");
   buffer.AppendFormatted("SkipDepInclude = yes\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ifeq ($(MAKECMDGOALS), clean)\n");
   buffer.AppendFormatted("SkipDepInclude = yes\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ifeq ($(MAKECMDGOALS), distclean)\n");
   buffer.AppendFormatted("SkipDepInclude = yes\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ifeq ($(MAKECMDGOALS), %sclean)\n",shortCut.ToLower(tmp));
   buffer.AppendFormatted("SkipDepInclude = yes\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ifeq ($(MAKECMDGOALS), build)\n");
   buffer.AppendFormatted("SkipDepInclude = yes\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ifeq ($(SkipDepInclude), no)\n");
   buffer.AppendFormatted("-include obj/*.d");
   for (i = 0; i < objDirList.GetEntriesFast(); i++) {
      buffer.AppendFormatted(" %s/*.d",objDirList.At(i).Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("endif\n");
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

//______________________________________________________________________________
void ROMEBuilder::WriteRootCintCall(ROMEString& buffer)
{
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("\t@-%%ROOTSYS%%\\bin\\rootcint");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("\t$(Q)$(ROOTCINT)");
#endif
}

//______________________________________________________________________________
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
      usrBuffer.AppendFormatted("# 2) Add mySource%s to the list of objects, e.g. objects += mySource%s\n",
                                kObjectSuffix,kObjectSuffix);
      usrBuffer.AppendFormatted("# 3) Add compile statment, e.g.\n");
      usrBuffer.AppendFormatted("#       obj/mySource%s: mySource.cpp\n",kObjectSuffix);
      usrBuffer.AppendFormatted("#\t$(CXX) -c $(Flags) $(Includes) mySource.cpp -o obj/mySource%s\n",kObjectSuffix);
      usrBuffer.AppendFormatted("# 4) Add include paths for the rootcint, e.g. DictionaryIncludes += -ImyPath\n");
      usrBuffer.AppendFormatted("# 5) Add header files for the rootcint, e.g. DictionaryHeaders += myHeader.h/\n");
      usrBuffer.AppendFormatted("# 6) Add clean target, e.g.\n");
      usrBuffer.AppendFormatted("#       userclean:\n");
      usrBuffer.AppendFormatted("#\t$(RM) your_file.h\n");
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("userclean:\n");
      usrBuffer.AppendFormatted("\t@echo ''\n");
      usrBuffer.AppendFormatted("userdistclean:\n");
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
      usrBuffer.AppendFormatted("# 2) Add mySource%s to the list of objects, e.g. objects = $(objects) mySource%s\n",
                                kObjectSuffix,kObjectSuffix);
      usrBuffer.AppendFormatted("# 3) Add compile statment, e.g.\n");
      usrBuffer.AppendFormatted("#       obj/mySource%s: mySource.cpp\n",kObjectSuffix);
      usrBuffer.AppendFormatted("#\tcl /c $(Flags) $(Includes) mySource.cpp /Foobj/mySource%s\n",kObjectSuffix);
      usrBuffer.AppendFormatted("# 4) Add include paths for the rootcint, e.g. DictionaryIncludes = $(DictionaryIncludes) -ImyPath\n");
      usrBuffer.AppendFormatted("# 5) Add header files for the rootcint, e.g. DictionaryHeaders = $(DictionaryHeaders) myHeader.h/\n");
      usrBuffer.AppendFormatted("# 6) Add clean target, e.g.\n");
      usrBuffer.AppendFormatted("#       userclean:\n");
      usrBuffer.AppendFormatted("#\t$(RM) your_file.h\n");
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("userclean:\n",shortCut.Data());
      usrBuffer.AppendFormatted("\t@echo ''\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
      WriteFile(makeFile.Data(),usrBuffer.Data(),0);
   }
#endif
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteLinkDefH(ROMEStrArray *headers, ROMEStrArray *ldsuffix,const char* dictionaryName)
{
   ROMEString buffer;
   ROMEString classname;
   int i;
   Int_t iFile, nClass;
   ROMEString filename;
   if (!dictionaryName) {
      return false;
   }

   nClass = headers->GetEntriesFast();

   for (iFile = 0; iFile < (nClass - 1) / maxNumberOfClassesInDictionary + 1; iFile++) {
#if defined( R__VISUAL_CPLUSPLUS )
      filename.SetFormatted("%sinclude/generated/%s%dLinkDef.h", outDir.Data(),dictionaryName, iFile);
#else
      filename.SetFormatted("include/generated/%s%dLinkDef.h", dictionaryName, iFile);
#endif

      WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, true);

      buffer.SetFormatted("#ifdef __CINT__\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("#pragma link off all globals;\n");
      buffer.AppendFormatted("#pragma link off all classes;\n");
      buffer.AppendFormatted("#pragma link off all functions;\n");
      buffer.AppendFormatted("\n");
      if (headers->GetEntriesFast() > 0) {
         for (i = iFile * maxNumberOfClassesInDictionary;
              i < nClass && i < (iFile + 1) * maxNumberOfClassesInDictionary; i++) {
            classname = gSystem->BaseName(headers->At(i).Data());
            classname.Resize(classname.Length()-2); // remove ".h"
            if (ldsuffix->At(i).Length() > 0) {
               buffer.AppendFormatted("#pragma link C++ class %s%s;\n", classname.Data(), ldsuffix->At(i).Data());
            } else {
               buffer.AppendFormatted("#pragma link C++ class %s;\n", classname.Data());
            }
         }
      }
      buffer.AppendFormatted("#endif\n");
      WriteFile(filename.Data(), buffer.Data(), 6);
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteUserLinkDefH()
{
   ROMEString buffer;
   ROMEString classname;
   int i;
   ROMEString filename;

   Int_t iFile, nClass;
   nClass = numOfMFDictHeaders;
   for (iFile = 0; iFile < (nClass - 1) / maxNumberOfClassesInDictionary + 1; iFile++) {
      filename.SetFormatted("include/generated/%sUserDict%dLinkDef.h", shortCut.Data(), iFile);

      WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, true);

      buffer.SetFormatted("#ifdef __CINT__\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("#pragma link off all globals;\n");
      buffer.AppendFormatted("#pragma link off all classes;\n");
      buffer.AppendFormatted("#pragma link off all functions;\n");
      buffer.AppendFormatted("\n");
      for (i = iFile * maxNumberOfClassesInDictionary;
           i < (iFile + 1) * maxNumberOfClassesInDictionary && i < nClass; i++) {
         if (mfDictHeaderUsed[i]) {
            classname = gSystem->BaseName(mfDictHeaderName[i]);
            classname.Resize(classname.Length()-2); // remove ".h"
            if (mfDictHeaderLinkDefSuffix[i].Length() > 0) {
               buffer.AppendFormatted("#pragma link C++ class %s%s;\n", classname.Data(),
                                      mfDictHeaderLinkDefSuffix[i].Data());
            } else {
               buffer.AppendFormatted("#pragma link C++ class %s;\n", classname.Data());
            }
         }
      }
      buffer.AppendFormatted("#endif\n");
      WriteFile(filename.Data(), buffer.Data(), 6);
   }
   return true;
}
