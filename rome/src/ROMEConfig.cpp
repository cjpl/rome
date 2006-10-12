//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ROMEConfig                                                                 //
//                                                                            //
// The ROME configuration class
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////
// $Id$

#include "ROMEXML.h"
#include "ROMEStrArray.h"
#include "ROMEHisto.h"
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
Bool_t ROMEConfig::CheckHistoConfigurationModified(ROMEConfigHisto* configHisto)
{
    if (configHisto->fHistTitleModified ||
        configHisto->fHistFolderTitleModified ||
        configHisto->fHistArraySizeModified ||
        configHisto->fHistArrayStartIndexModified ||
        configHisto->fHistXLabelModified ||
        configHisto->fHistYLabelModified ||
        configHisto->fHistZLabelModified ||
        configHisto->fHistXNbinsModified ||
        configHisto->fHistXminModified ||
        configHisto->fHistXmaxModified ||
        configHisto->fHistYNbinsModified ||
        configHisto->fHistYminModified ||
        configHisto->fHistYmaxModified ||
        configHisto->fHistZNbinsModified ||
        configHisto->fHistZminModified ||
        configHisto->fHistZmaxModified)
      return true;
    else
      return false;
}
Bool_t ROMEConfig::SetHistoConfiguration(ROMEHisto* histo,ROMEConfigHisto* configHisto)
{
   // fHistTitle
   if (configHisto->fHistTitleModified) {
      histo->SetTitle(configHisto->fHistTitle.Data());
   }
   // fHistFolderTitle
   if (configHisto->fHistFolderTitleModified) {
      histo->SetFolderTitle(configHisto->fHistFolderTitle.Data());
   }
   // fHistArraySize
   if (configHisto->fHistArraySizeModified) {
      histo->SetArraySize(configHisto->fHistArraySize.Data());
   }
   // fHistArrayStartIndex
   if (configHisto->fHistArrayStartIndexModified) {
      histo->SetArrayStartIndex(configHisto->fHistArrayStartIndex.Data());
   }
   // fHistXLabel
   if (configHisto->fHistXLabelModified) {
      histo->SetXLabel(configHisto->fHistXLabel.Data());
   }
   // fHistYLabel
   if (configHisto->fHistYLabelModified) {
      histo->SetYLabel(configHisto->fHistYLabel.Data());
   }
   // fHistZLabel
   if (configHisto->fHistZLabelModified) {
      histo->SetZLabel(configHisto->fHistZLabel.Data());
   }
   // fHistXNbins
   if (configHisto->fHistXNbinsModified) {
      histo->SetXNbins(configHisto->fHistXNbins.Data());
   }
   // fHistXmin
   if (configHisto->fHistXminModified) {
      histo->SetXmin(configHisto->fHistXmin.Data());
   }
   // fHistXmax
   if (configHisto->fHistXmaxModified) {
      histo->SetXmax(configHisto->fHistXmax.Data());
   }
   // fHistYNbins
   if (configHisto->fHistYNbinsModified) {
      histo->SetYNbins(configHisto->fHistYNbins.Data());
   }
   // fHistYmin
   if (configHisto->fHistYminModified) {
      histo->SetYmin(configHisto->fHistYmin.Data());
   }
   // fHistYmax
   if (configHisto->fHistYmaxModified) {
      histo->SetYmax(configHisto->fHistYmax.Data());
   }
   // fHistZNbins
   if (configHisto->fHistZNbinsModified) {
      histo->SetZNbins(configHisto->fHistZNbins.Data());
   }
   // fHistZmin
   if (configHisto->fHistZminModified) {
      histo->SetZmin(configHisto->fHistZmin.Data());
   }
   // fHistZmax
   if (configHisto->fHistZmaxModified) {
      histo->SetZmax(configHisto->fHistZmax.Data());
   }
   return true;
}
Bool_t ROMEConfig::WriteHistoConfiguration(ROMEXML* xml,ROMEConfigHisto* configHisto,Int_t commentLevel,ROMEStrArray& comment)
{
   // fHistTitle
   if (commentLevel >= 3 && configHisto->fHistTitleModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistTitleModified) {
      xml->WriteElement("HistTitle",configHisto->fHistTitle.Data());
   }
   // fHistFolderTitle
   if (commentLevel >= 3 && configHisto->fHistFolderTitleModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistFolderTitleModified) {
      xml->WriteElement("HistFolderTitle",configHisto->fHistFolderTitle.Data());
   }
   // fHistArraySize
   if (commentLevel >= 3 && configHisto->fHistArraySizeModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistArraySizeModified) {
      xml->WriteElement("HistArraySize",configHisto->fHistArraySize.Data());
   }
   // fHistArrayStartIndex
   if (commentLevel >= 3 && configHisto->fHistArrayStartIndexModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistArrayStartIndexModified) {
      xml->WriteElement("HistArrayStartIndex",configHisto->fHistArrayStartIndex.Data());
   }
   // fHistXLabel
   if (commentLevel >= 3 && configHisto->fHistXLabelModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistXLabelModified) {
      xml->WriteElement("HistXLabel",configHisto->fHistXLabel.Data());
   }
   // fHistYLabel
   if (commentLevel >= 3 && configHisto->fHistYLabelModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistYLabelModified) {
      xml->WriteElement("HistYLabel",configHisto->fHistYLabel.Data());
   }
   // fHistZLabel
   if (commentLevel >= 3 && configHisto->fHistZLabelModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistZLabelModified) {
      xml->WriteElement("HistZLabel",configHisto->fHistZLabel.Data());
   }
   // fHistXNbins
   if (commentLevel >= 3 && configHisto->fHistXNbinsModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistXNbinsModified) {
      xml->WriteElement("HistXNbins",configHisto->fHistXNbins.Data());
   }
   // fHistXmin
   if (commentLevel >= 3 && configHisto->fHistXminModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistXminModified) {
      xml->WriteElement("HistXmin",configHisto->fHistXmin.Data());
   }
   // fHistXmax
   if (commentLevel >= 3 && configHisto->fHistXmaxModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistXmaxModified) {
      xml->WriteElement("HistXmax",configHisto->fHistXmax.Data());
   }
   // fHistYNbins
   if (commentLevel >= 3 && configHisto->fHistYNbinsModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistYNbinsModified) {
      xml->WriteElement("HistYNbins",configHisto->fHistYNbins.Data());
   }
   // fHistYmin
   if (commentLevel >= 3 && configHisto->fHistYminModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistYminModified) {
      xml->WriteElement("HistYmin",configHisto->fHistYmin.Data());
   }
   // fHistYmax
   if (commentLevel >= 3 && configHisto->fHistYmaxModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistYmaxModified) {
      xml->WriteElement("HistYmax",configHisto->fHistYmax.Data());
   }
   // fHistZNbins
   if (commentLevel >= 3 && configHisto->fHistZNbinsModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistZNbinsModified) {
      xml->WriteElement("HistZNbins",configHisto->fHistZNbins.Data());
   }
   // fHistZmin
   if (commentLevel >= 3 && configHisto->fHistZminModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistZminModified) {
      xml->WriteElement("HistZmin",configHisto->fHistZmin.Data());
   }
   // fHistZmax
   if (commentLevel >= 3 && configHisto->fHistZmaxModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistZmaxModified) {
      xml->WriteElement("HistZmax",configHisto->fHistZmax.Data());
   }
   return true;
}
