/********************************************************************
  ROMEConfigHisto.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEConfigHisto_H
#define ROMEConfigHisto_H

#include <ROMEString.h>
#include <TObject.h>

class ROMEConfigHisto : public TObject
{
public:
   ROMEString   fHistActive;
   Bool_t       fHistActiveModified;
   ROMEString   fHistWrite;
   Bool_t       fHistWriteModified;
   ROMEString   fHistTitle;
   Bool_t       fHistTitleModified;
   ROMEString   fHistFolderTitle;
   Bool_t       fHistFolderTitleModified;
   ROMEString   fHistArraySize;
   Bool_t       fHistArraySizeModified;
   ROMEString   fHistArrayStartIndex;
   Bool_t       fHistArrayStartIndexModified;
   ROMEString   fHistXLabel;
   Bool_t       fHistXLabelModified;
   ROMEString   fHistYLabel;
   Bool_t       fHistYLabelModified;
   ROMEString   fHistZLabel;
   Bool_t       fHistZLabelModified;
   ROMEString   fHistXNbins;
   Bool_t       fHistXNbinsModified;
   ROMEString   fHistXmin;
   Bool_t       fHistXminModified;
   ROMEString   fHistXmax;
   Bool_t       fHistXmaxModified;
   ROMEString   fHistYNbins;
   Bool_t       fHistYNbinsModified;
   ROMEString   fHistYmin;
   Bool_t       fHistYminModified;
   ROMEString   fHistYmax;
   Bool_t       fHistYmaxModified;
   ROMEString   fHistZNbins;
   Bool_t       fHistZNbinsModified;
   ROMEString   fHistZmin;
   Bool_t       fHistZminModified;
   ROMEString   fHistZmax;
   Bool_t       fHistZmaxModified;
   ROMEString   fHistOption;
   Bool_t       fHistOptionModified;
   ROMEString   fHistAccumulate;
   Bool_t       fHistAccumulateModified;

   ROMEConfigHisto()
   :TObject()
   ,fHistActive("false")
   ,fHistActiveModified(kFALSE)
   ,fHistWrite("false")
   ,fHistWriteModified(kFALSE)
   ,fHistTitle("")
   ,fHistTitleModified(kFALSE)
   ,fHistFolderTitle("")
   ,fHistFolderTitleModified(kFALSE)
   ,fHistArraySize("1")
   ,fHistArraySizeModified(kFALSE)
   ,fHistArrayStartIndex("0")
   ,fHistArrayStartIndexModified(kFALSE)
   ,fHistXLabel("X")
   ,fHistXLabelModified(kFALSE)
   ,fHistYLabel("Y")
   ,fHistYLabelModified(kFALSE)
   ,fHistZLabel("Z")
   ,fHistZLabelModified(kFALSE)
   ,fHistXNbins("1")
   ,fHistXNbinsModified(kFALSE)
   ,fHistXmin("0")
   ,fHistXminModified(kFALSE)
   ,fHistXmax("1")
   ,fHistXmaxModified(kFALSE)
   ,fHistYNbins("1")
   ,fHistYNbinsModified(kFALSE)
   ,fHistYmin("0")
   ,fHistYminModified(kFALSE)
   ,fHistYmax("1")
   ,fHistYmaxModified(kFALSE)
   ,fHistZNbins("1")
   ,fHistZNbinsModified(kFALSE)
   ,fHistZmin("0")
   ,fHistZminModified(kFALSE)
   ,fHistZmax("1")
   ,fHistZmaxModified(kFALSE)
   ,fHistOption("")
   ,fHistOptionModified(kFALSE)
   ,fHistAccumulate("false")
   ,fHistAccumulateModified(kFALSE)
   {
   }
   virtual ~ROMEConfigHisto() {}

   ClassDef(ROMEConfigHisto, 0) // Histo configuration class
};

#endif   // ROMEConfigHisto_H
