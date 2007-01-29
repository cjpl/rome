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
   ROMEString   fHistAccumulate;
   Bool_t       fHistAccumulateModified;
   ROMEConfigHisto() {
      fHistActiveModified = false;
      fHistTitleModified = false;
      fHistFolderTitleModified = false;
      fHistArraySizeModified = false;
      fHistArrayStartIndexModified = false;
      fHistXLabelModified = false;
      fHistYLabelModified = false;
      fHistZLabelModified = false;
      fHistXNbinsModified = false;
      fHistXminModified = false;
      fHistXmaxModified = false;
      fHistYNbinsModified = false;
      fHistYminModified = false;
      fHistYmaxModified = false;
      fHistZNbinsModified = false;
      fHistZminModified = false;
      fHistZmaxModified = false;
      fHistAccumulateModified = false;
   }
   virtual ~ROMEConfigHisto() {}

   ClassDef(ROMEConfigHisto, 0) // Histo configuration class
};

#endif   // ROMEConfigHisto_H
