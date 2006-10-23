/********************************************************************
  ROMEConfig.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEConfig_H
#define ROMEConfig_H

#include <ROMEString.h>
#include <TObject.h>

class ROMEStrArray;
class ROMEXML;
class ROMEHisto;
class ROMEGraph;
class ROMEConfigHisto;
class ROMEConfigGraph;

class ROMEConfig : public TObject
{
public:
// Comment level
enum {
   kCommentLevelNone  = 0,  // no comment
   kCommentLevelSP    = 1,  // 0 + steering parameter
   kCommentLevelGroup = 2,  // 1 + ConfigParameterGroup
   kCommentLevelParam = 3,  // 2 + ConfigParameter
   kCommentLevelObj   = 4,  // 3 + ObjectDescription (task, tab, folder....)
   kCommentLevelAll   = 5,  // everything
   kCommentLevelNever = 1000// never show in config file. (but in GUI config)
};

protected:
   Int_t fCommentLevel;
   ROMEString fConfigFileName;
   ROMEString fConfigContent;

public:
   ROMEConfig() { fCommentLevel = kCommentLevelSP; }
   virtual ~ROMEConfig() {}

   void           SetCommentLevel(Int_t level) { fCommentLevel = level; }
   Int_t          GetCommentLevel() { return fCommentLevel; }
   const char*    GetConfigContent() { return fConfigContent.Data(); }
   const char*    GetConfigFileName() { return fConfigFileName.Data(); }
   virtual Bool_t WriteConfigurationFile(const char *file) = 0;
   virtual Bool_t ReadConfigurationFile(const char *file) = 0;
   virtual Bool_t CheckConfiguration(Long64_t runNumber) = 0;
   virtual Bool_t CheckConfiguration(const char *file) = 0;
   virtual Bool_t CheckConfigurationModified(Int_t index) = 0;
protected:
   Bool_t ReadHistoConfiguration(ROMEXML* xml,const char* path,ROMEConfigHisto* configHisto);
   Bool_t CheckHistoConfigurationModified(ROMEConfigHisto* configHisto);
   Bool_t SetHistoConfiguration(ROMEHisto* histo,ROMEConfigHisto* configHisto);
   Bool_t WriteHistoConfiguration(ROMEXML* xml,ROMEConfigHisto* configHisto,Int_t commentLevel,ROMEStrArray& comment);
   Bool_t ReadGraphConfiguration(ROMEXML* xml,const char* path,ROMEConfigGraph* configGraph);
   Bool_t CheckGraphConfigurationModified(ROMEConfigGraph* configGraph);
   Bool_t SetGraphConfiguration(ROMEGraph* graph,ROMEConfigGraph* configGraph);
   Bool_t WriteGraphConfiguration(ROMEXML* xml,ROMEConfigGraph* configGraph,Int_t commentLevel,ROMEStrArray& comment);

   ClassDef(ROMEConfig, 0) // Base configuration class
};

#endif   // ROMEConfig_H
