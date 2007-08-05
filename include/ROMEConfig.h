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
   ROMEConfig():fCommentLevel(kCommentLevelSP),fConfigFileName(""),fConfigContent("") {}
   virtual ~ROMEConfig() {}

   void           SetCommentLevel(Int_t level) { fCommentLevel = level; }
   Int_t          GetCommentLevel() const { return fCommentLevel; }
   const char*    GetConfigContent() const { return fConfigContent.Data(); }
   const char*    GetConfigFileName() const { return fConfigFileName.Data(); }
   virtual Bool_t WriteConfigurationFile(const char *file) const = 0;
   virtual Bool_t ReadConfigurationFile(const char *file) = 0;
   virtual Bool_t CheckConfiguration(Long64_t runNumber) = 0;
   virtual Bool_t CheckConfiguration(const char *file) = 0;
   virtual Bool_t CheckConfigurationModified(Int_t index) const = 0;

protected:
   Bool_t ReadHistoConfiguration(ROMEXML* xml,const char* path,ROMEConfigHisto* configHisto) const;
   Bool_t CheckHistoConfigurationModified(ROMEConfigHisto* configHisto) const;
   Bool_t SetHistoConfiguration(ROMEHisto* histo,ROMEConfigHisto* configHisto) const;
   Bool_t WriteHistoConfiguration(ROMEXML* xml,ROMEConfigHisto* configHisto,Int_t commentLevel,ROMEStrArray& comment) const;
   Bool_t ReadGraphConfiguration(ROMEXML* xml,const char* path,ROMEConfigGraph* configGraph) const;
   Bool_t CheckGraphConfigurationModified(ROMEConfigGraph* configGraph) const;
   Bool_t SetGraphConfiguration(ROMEGraph* graph,ROMEConfigGraph* configGraph) const;
   Bool_t WriteGraphConfiguration(ROMEXML* xml,ROMEConfigGraph* configGraph,Int_t commentLevel,ROMEStrArray& comment) const;

   ClassDef(ROMEConfig, 0) // Base configuration class
};

#endif   // ROMEConfig_H
