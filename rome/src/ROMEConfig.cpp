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
#include "ROMEGraph.h"
#include "ROMEConfigHisto.h"
#include "ROMEConfigGraph.h"
#include "ROMEConfig.h"

ClassImp(ROMEConfig)

Bool_t ROMEConfig::ReadHistoConfiguration(ROMEXML* xml,const char* path,ROMEConfigHisto* configHisto)
{
   ROMEString fullPath;
   // fHistActive
   fullPath.SetFormatted("%sHistActive",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistActive,"");
   if (configHisto->fHistActive=="")
      configHisto->fHistActiveModified = false;
   else {
      configHisto->fHistActiveModified = true;
   }
   // fHistTitle
   fullPath.SetFormatted("%sHistTitle",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistTitle,"");
   if (configHisto->fHistTitle=="")
      configHisto->fHistTitleModified = false;
   else {
      configHisto->fHistTitleModified = true;
   }
   // fHistFolderTitle
   fullPath.SetFormatted("%sHistFolderTitle",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistFolderTitle,"");
   if (configHisto->fHistFolderTitle=="")
      configHisto->fHistFolderTitleModified = false;
   else {
      configHisto->fHistFolderTitleModified = true;
   }
   // fHistArraySize
   fullPath.SetFormatted("%sHistArraySize",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistArraySize,"");
   if (configHisto->fHistArraySize=="")
      configHisto->fHistArraySizeModified = false;
   else {
      configHisto->fHistArraySizeModified = true;
   }
   // fHistArrayStartIndex
   fullPath.SetFormatted("%sHistArrayStartIndex",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistArrayStartIndex,"");
   if (configHisto->fHistArrayStartIndex=="")
      configHisto->fHistArrayStartIndexModified = false;
   else {
      configHisto->fHistArrayStartIndexModified = true;
   }
   // fHistXLabel
   fullPath.SetFormatted("%sHistXLabel",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistXLabel,"");
   if (configHisto->fHistXLabel=="")
      configHisto->fHistXLabelModified = false;
   else {
      configHisto->fHistXLabelModified = true;
   }
   // fHistYLabel
   fullPath.SetFormatted("%sHistYLabel",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistYLabel,"");
   if (configHisto->fHistYLabel=="")
      configHisto->fHistYLabelModified = false;
   else {
      configHisto->fHistYLabelModified = true;
   }
   // fHistZLabel
   fullPath.SetFormatted("%sHistZLabel",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistZLabel,"");
   if (configHisto->fHistZLabel=="")
      configHisto->fHistZLabelModified = false;
   else {
      configHisto->fHistZLabelModified = true;
   }
   // fHistXNbins
   fullPath.SetFormatted("%sHistXNbins",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistXNbins,"");
   if (configHisto->fHistXNbins=="")
      configHisto->fHistXNbinsModified = false;
   else {
      configHisto->fHistXNbinsModified = true;
   }
   // fHistXmin
   fullPath.SetFormatted("%sHistXmin",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistXmin,"");
   if (configHisto->fHistXmin=="")
      configHisto->fHistXminModified = false;
   else {
      configHisto->fHistXminModified = true;
   }
   // fHistXmax
   fullPath.SetFormatted("%sHistXmax",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistXmax,"");
   if (configHisto->fHistXmax=="")
      configHisto->fHistXmaxModified = false;
   else {
      configHisto->fHistXmaxModified = true;
   }
   // fHistYNbins
   fullPath.SetFormatted("%sHistYNbins",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistYNbins,"");
   if (configHisto->fHistYNbins=="")
      configHisto->fHistYNbinsModified = false;
   else {
      configHisto->fHistYNbinsModified = true;
   }
   // fHistYmin
   fullPath.SetFormatted("%sHistYmin",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistYmin,"");
   if (configHisto->fHistYmin=="")
      configHisto->fHistYminModified = false;
   else {
      configHisto->fHistYminModified = true;
   }
   // fHistYmax
   fullPath.SetFormatted("%sHistYmax",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistYmax,"");
   if (configHisto->fHistYmax=="")
      configHisto->fHistYmaxModified = false;
   else {
      configHisto->fHistYmaxModified = true;
   }
   // fHistZNbins
   fullPath.SetFormatted("%sHistZNbins",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistZNbins,"");
   if (configHisto->fHistZNbins=="")
      configHisto->fHistZNbinsModified = false;
   else {
      configHisto->fHistZNbinsModified = true;
   }
   // fHistZmin
   fullPath.SetFormatted("%sHistZmin",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistZmin,"");
   if (configHisto->fHistZmin=="")
      configHisto->fHistZminModified = false;
   else {
      configHisto->fHistZminModified = true;
   }
   // fHistZmax
   fullPath.SetFormatted("%sHistZmax",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistZmax,"");
   if (configHisto->fHistZmax=="")
      configHisto->fHistZmaxModified = false;
   else {
      configHisto->fHistZmaxModified = true;
   }
   // fHistAccumulate
   fullPath.SetFormatted("%sHistAccumulate",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistAccumulate,"");
   if (configHisto->fHistAccumulate=="")
      configHisto->fHistAccumulateModified = false;
   else {
      configHisto->fHistAccumulateModified = true;
   }
   return true;
}
Bool_t ROMEConfig::CheckHistoConfigurationModified(ROMEConfigHisto* configHisto)
{
    if (configHisto->fHistActiveModified ||
        configHisto->fHistTitleModified ||
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
        configHisto->fHistZmaxModified ||
        configHisto->fHistAccumulateModified)
      return true;
    else
      return false;
}
Bool_t ROMEConfig::SetHistoConfiguration(ROMEHisto* histo,ROMEConfigHisto* configHisto)
{
   // fHistActive
   if (configHisto->fHistActiveModified) {
      if (configHisto->fHistActive=="true")
         histo->SetActive(true);
      else
         histo->SetActive(false);
   }
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
   // fHistAccumulate
   if (configHisto->fHistAccumulateModified) {
      if (configHisto->fHistAccumulate=="true")
         histo->SetAccumulation(true);
      else
         histo->SetAccumulation(false);
   }
   return true;
}
Bool_t ROMEConfig::WriteHistoConfiguration(ROMEXML* xml,ROMEConfigHisto* configHisto,Int_t commentLevel,ROMEStrArray& comment)
{
   // fHistActive
   if (commentLevel >= 3 && configHisto->fHistActiveModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistActiveModified) {
      xml->WriteElement("HistActive",configHisto->fHistActive.Data());
   }
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
   // fHistAccumulate
   if (commentLevel >= 3 && configHisto->fHistAccumulateModified)
      xml->WriteComment(comment.At(0).Data());
   if (configHisto->fHistAccumulateModified) {
      xml->WriteElement("HistAccumulate",configHisto->fHistAccumulate.Data());
   }
   return true;
}
Bool_t ROMEConfig::ReadGraphConfiguration(ROMEXML* xml,const char* path,ROMEConfigGraph* configGraph)
{
   ROMEString fullPath;
   // fGraphActive
   fullPath.SetFormatted("%sGraphActive",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphActive,"");
   if (configGraph->fGraphActive=="")
      configGraph->fGraphActiveModified = false;
   else {
      configGraph->fGraphActiveModified = true;
   }
   // fGraphTitle
   fullPath.SetFormatted("%sGraphTitle",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphTitle,"");
   if (configGraph->fGraphTitle=="")
      configGraph->fGraphTitleModified = false;
   else {
      configGraph->fGraphTitleModified = true;
   }
   // fGraphFolderTitle
   fullPath.SetFormatted("%sGraphFolderTitle",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphFolderTitle,"");
   if (configGraph->fGraphFolderTitle=="")
      configGraph->fGraphFolderTitleModified = false;
   else {
      configGraph->fGraphFolderTitleModified = true;
   }
   // fGraphArraySize
   fullPath.SetFormatted("%sGraphArraySize",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphArraySize,"");
   if (configGraph->fGraphArraySize=="")
      configGraph->fGraphArraySizeModified = false;
   else {
      configGraph->fGraphArraySizeModified = true;
   }
   // fGraphArrayStartIndex
   fullPath.SetFormatted("%sGraphArrayStartIndex",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphArrayStartIndex,"");
   if (configGraph->fGraphArrayStartIndex=="")
      configGraph->fGraphArrayStartIndexModified = false;
   else {
      configGraph->fGraphArrayStartIndexModified = true;
   }
   // fGraphXLabel
   fullPath.SetFormatted("%sGraphXLabel",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphXLabel,"");
   if (configGraph->fGraphXLabel=="")
      configGraph->fGraphXLabelModified = false;
   else {
      configGraph->fGraphXLabelModified = true;
   }
   // fGraphYLabel
   fullPath.SetFormatted("%sGraphYLabel",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphYLabel,"");
   if (configGraph->fGraphYLabel=="")
      configGraph->fGraphYLabelModified = false;
   else {
      configGraph->fGraphYLabelModified = true;
   }
   // fGraphZLabel
   fullPath.SetFormatted("%sGraphZLabel",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphZLabel,"");
   if (configGraph->fGraphZLabel=="")
      configGraph->fGraphZLabelModified = false;
   else {
      configGraph->fGraphZLabelModified = true;
   }
   return true;
}
Bool_t ROMEConfig::CheckGraphConfigurationModified(ROMEConfigGraph* configGraph)
{
    if (configGraph->fGraphActiveModified ||
        configGraph->fGraphTitleModified ||
        configGraph->fGraphFolderTitleModified ||
        configGraph->fGraphArraySizeModified ||
        configGraph->fGraphArrayStartIndexModified ||
        configGraph->fGraphXLabelModified ||
        configGraph->fGraphYLabelModified ||
        configGraph->fGraphZLabelModified)
      return true;
    else
      return false;
}
Bool_t ROMEConfig::SetGraphConfiguration(ROMEGraph* graph,ROMEConfigGraph* configGraph)
{
   // fGraphActive
   if (configGraph->fGraphActiveModified) {
      if (configGraph->fGraphActive=="true")
         graph->SetActive(true);
      else
         graph->SetActive(false);
   }
   // fGraphTitle
   if (configGraph->fGraphTitleModified) {
      graph->SetTitle(configGraph->fGraphTitle.Data());
   }
   // fGraphFolderTitle
   if (configGraph->fGraphFolderTitleModified) {
      graph->SetFolderTitle(configGraph->fGraphFolderTitle.Data());
   }
   // fGraphArraySize
   if (configGraph->fGraphArraySizeModified) {
      graph->SetArraySize(configGraph->fGraphArraySize.Data());
   }
   // fGraphArrayStartIndex
   if (configGraph->fGraphArrayStartIndexModified) {
      graph->SetArrayStartIndex(configGraph->fGraphArrayStartIndex.Data());
   }
   // fGraphXLabel
   if (configGraph->fGraphXLabelModified) {
      graph->SetXLabel(configGraph->fGraphXLabel.Data());
   }
   // fGraphYLabel
   if (configGraph->fGraphYLabelModified) {
      graph->SetYLabel(configGraph->fGraphYLabel.Data());
   }
   // fGraphZLabel
   if (configGraph->fGraphZLabelModified) {
      graph->SetZLabel(configGraph->fGraphZLabel.Data());
   }
   return true;
}
Bool_t ROMEConfig::WriteGraphConfiguration(ROMEXML* xml,ROMEConfigGraph* configGraph,Int_t commentLevel,ROMEStrArray& comment)
{
   // fGraphActive
   if (commentLevel >= 3 && configGraph->fGraphActiveModified)
      xml->WriteComment(comment.At(0).Data());
   if (configGraph->fGraphActiveModified) {
      xml->WriteElement("GraphActive",configGraph->fGraphActive.Data());
   }
   // fGraphTitle
   if (commentLevel >= 3 && configGraph->fGraphTitleModified)
      xml->WriteComment(comment.At(0).Data());
   if (configGraph->fGraphTitleModified) {
      xml->WriteElement("GraphTitle",configGraph->fGraphTitle.Data());
   }
   // fGraphFolderTitle
   if (commentLevel >= 3 && configGraph->fGraphFolderTitleModified)
      xml->WriteComment(comment.At(0).Data());
   if (configGraph->fGraphFolderTitleModified) {
      xml->WriteElement("GraphFolderTitle",configGraph->fGraphFolderTitle.Data());
   }
   // fGraphArraySize
   if (commentLevel >= 3 && configGraph->fGraphArraySizeModified)
      xml->WriteComment(comment.At(0).Data());
   if (configGraph->fGraphArraySizeModified) {
      xml->WriteElement("GraphArraySize",configGraph->fGraphArraySize.Data());
   }
   // fGraphArrayStartIndex
   if (commentLevel >= 3 && configGraph->fGraphArrayStartIndexModified)
      xml->WriteComment(comment.At(0).Data());
   if (configGraph->fGraphArrayStartIndexModified) {
      xml->WriteElement("GraphArrayStartIndex",configGraph->fGraphArrayStartIndex.Data());
   }
   // fGraphXLabel
   if (commentLevel >= 3 && configGraph->fGraphXLabelModified)
      xml->WriteComment(comment.At(0).Data());
   if (configGraph->fGraphXLabelModified) {
      xml->WriteElement("GraphXLabel",configGraph->fGraphXLabel.Data());
   }
   // fGraphYLabel
   if (commentLevel >= 3 && configGraph->fGraphYLabelModified)
      xml->WriteComment(comment.At(0).Data());
   if (configGraph->fGraphYLabelModified) {
      xml->WriteElement("GraphYLabel",configGraph->fGraphYLabel.Data());
   }
   // fGraphZLabel
   if (commentLevel >= 3 && configGraph->fGraphZLabelModified)
      xml->WriteComment(comment.At(0).Data());
   if (configGraph->fGraphZLabelModified) {
      xml->WriteElement("GraphZLabel",configGraph->fGraphZLabel.Data());
   }
   return true;
}

