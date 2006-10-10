//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ROMEConfig                                                                 //
//                                                                            //
// The ROME configuration class
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////
// $Id: ROMEConfig.cpp 1427 2006-10-09 15:26:37Z egger_j $

#include "ROMEXML.h"
#include "ROMEString.h"
#include "ROMEConfigHisto.h"
#include "ROMEConfig.h"

ClassImp(ROMEConfig)

Bool_t ROMEConfig::ReadHistoConfiguration(ROMEXML* xml,const char* path,ROMEConfigHisto* histo)
{
   ROMEString fullPath;
   // fHistTitle
   fullPath.SetFormatted("%sHistTitle",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistTitle,"");
   if (histo->fHistTitle=="")
      histo->fHistTitleModified = false;
   else {
      histo->fHistTitleModified = true;
   }
   // fHistFolderTitle
   fullPath.SetFormatted("%sHistFolderTitle",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistFolderTitle,"");
   if (histo->fHistFolderTitle=="")
      histo->fHistFolderTitleModified = false;
   else {
      histo->fHistFolderTitleModified = true;
   }
   // fHistArraySize
   fullPath.SetFormatted("%sHistArraySize",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistArraySize,"");
   if (histo->fHistArraySize=="")
      histo->fHistArraySizeModified = false;
   else {
      histo->fHistArraySizeModified = true;
   }
   // fHistArrayStartIndex
   fullPath.SetFormatted("%sHistArrayStartIndex",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistArrayStartIndex,"");
   if (histo->fHistArrayStartIndex=="")
      histo->fHistArrayStartIndexModified = false;
   else {
      histo->fHistArrayStartIndexModified = true;
   }
   // fHistXLabel
   fullPath.SetFormatted("%sHistXLabel",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistXLabel,"");
   if (histo->fHistXLabel=="")
      histo->fHistXLabelModified = false;
   else {
      histo->fHistXLabelModified = true;
   }
   // fHistYLabel
   fullPath.SetFormatted("%sHistYLabel",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistYLabel,"");
   if (histo->fHistYLabel=="")
      histo->fHistYLabelModified = false;
   else {
      histo->fHistYLabelModified = true;
   }
   // fHistZLabel
   fullPath.SetFormatted("%sHistZLabel",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistZLabel,"");
   if (histo->fHistZLabel=="")
      histo->fHistZLabelModified = false;
   else {
      histo->fHistZLabelModified = true;
   }
   // fHistXNbins
   fullPath.SetFormatted("%sHistXNbins",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistXNbins,"");
   if (histo->fHistXNbins=="")
      histo->fHistXNbinsModified = false;
   else {
      histo->fHistXNbinsModified = true;
   }
   // fHistXmin
   fullPath.SetFormatted("%sHistXmin",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistXmin,"");
   if (histo->fHistXmin=="")
      histo->fHistXminModified = false;
   else {
      histo->fHistXminModified = true;
   }
   // fHistXmax
   fullPath.SetFormatted("%sHistXmax",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistXmax,"");
   if (histo->fHistXmax=="")
      histo->fHistXmaxModified = false;
   else {
      histo->fHistXmaxModified = true;
   }
   // fHistYNbins
   fullPath.SetFormatted("%sHistYNbins",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistYNbins,"");
   if (histo->fHistYNbins=="")
      histo->fHistYNbinsModified = false;
   else {
      histo->fHistYNbinsModified = true;
   }
   // fHistYmin
   fullPath.SetFormatted("%sHistYmin",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistYmin,"");
   if (histo->fHistYmin=="")
      histo->fHistYminModified = false;
   else {
      histo->fHistYminModified = true;
   }
   // fHistYmax
   fullPath.SetFormatted("%sHistYmax",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistYmax,"");
   if (histo->fHistYmax=="")
      histo->fHistYmaxModified = false;
   else {
      histo->fHistYmaxModified = true;
   }
   // fHistZNbins
   fullPath.SetFormatted("%sHistZNbins",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistZNbins,"");
   if (histo->fHistZNbins=="")
      histo->fHistZNbinsModified = false;
   else {
      histo->fHistZNbinsModified = true;
   }
   // fHistZmin
   fullPath.SetFormatted("%sHistZmin",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistZmin,"");
   if (histo->fHistZmin=="")
      histo->fHistZminModified = false;
   else {
      histo->fHistZminModified = true;
   }
   // fHistZmax
   fullPath.SetFormatted("%sHistZmax",path);
   xml->GetPathValue(fullPath.Data(),histo->fHistZmax,"");
   if (histo->fHistZmax=="")
      histo->fHistZmaxModified = false;
   else {
      histo->fHistZmaxModified = true;
   }
   return true;
}
