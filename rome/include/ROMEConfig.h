/********************************************************************
  ROMEConfig.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEConfig_H
#define ROMEConfig_H

#include <TObject.h>

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

public:
   ROMEConfig() { fCommentLevel = kCommentLevelSP; }
   virtual ~ROMEConfig() {}

   void           SetCommentLevel(Int_t level) { fCommentLevel = level; }
   Int_t          GetCommentLevel() { return fCommentLevel; }
   virtual Bool_t WriteConfigurationFile(const char *file) = 0;
   virtual Bool_t ReadConfigurationFile(const char *file) = 0;
   virtual Bool_t CheckConfiguration(Long64_t runNumber) = 0;
   virtual Bool_t CheckConfiguration(const char *file) = 0;
   virtual Bool_t CheckConfigurationModified(Int_t index) = 0;

   ClassDef(ROMEConfig, 0) // Base configuration class
};

#endif   // ROMEConfig_H
