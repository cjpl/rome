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

//______________________________________________________________________________
Bool_t ROMEConfig::ReadHistoConfiguration(ROMEXML* xml,const char* path,ROMEConfigHisto* configHisto) const
{
   ROMEString fullPath;
   // fHistActive
   fullPath.SetFormatted("%sHistActive",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistActive,"");
   configHisto->fHistActiveModified = (configHisto->fHistActive!="");
   // fHistWrite
   fullPath.SetFormatted("%sHistWrite",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistWrite,"");
   configHisto->fHistWriteModified = (configHisto->fHistWrite!="");
   // fHistTitle
   fullPath.SetFormatted("%sHistTitle",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistTitle,"");
   configHisto->fHistTitleModified = (configHisto->fHistTitle!="");
   // fHistFolderTitle
   fullPath.SetFormatted("%sHistFolderTitle",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistFolderTitle,"");
   configHisto->fHistFolderTitleModified = (configHisto->fHistFolderTitle!="");
   // fHistArraySize
   fullPath.SetFormatted("%sHistArraySize",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistArraySize,"");
   configHisto->fHistArraySizeModified = (configHisto->fHistArraySize!="");
   // fHistArrayStartIndex
   fullPath.SetFormatted("%sHistArrayStartIndex",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistArrayStartIndex,"");
   configHisto->fHistArrayStartIndexModified = (configHisto->fHistArrayStartIndex!="");
   // fHistXLabel
   fullPath.SetFormatted("%sHistXLabel",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistXLabel,"");
   configHisto->fHistXLabelModified = (configHisto->fHistXLabel!="");
   // fHistYLabel
   fullPath.SetFormatted("%sHistYLabel",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistYLabel,"");
   configHisto->fHistYLabelModified = (configHisto->fHistYLabel!="");
   // fHistZLabel
   fullPath.SetFormatted("%sHistZLabel",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistZLabel,"");
   configHisto->fHistZLabelModified = (configHisto->fHistZLabel!="");
   // fHistXNbins
   fullPath.SetFormatted("%sHistXNbins",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistXNbins,"");
   configHisto->fHistXNbinsModified = (configHisto->fHistXNbins!="");
   // fHistXmin
   fullPath.SetFormatted("%sHistXmin",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistXmin,"");
   configHisto->fHistXminModified = (configHisto->fHistXmin!="");
   // fHistXmax
   fullPath.SetFormatted("%sHistXmax",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistXmax,"");
   configHisto->fHistXmaxModified = (configHisto->fHistXmax!="");
   // fHistYNbins
   fullPath.SetFormatted("%sHistYNbins",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistYNbins,"");
   configHisto->fHistYNbinsModified = (configHisto->fHistYNbins!="");
   // fHistYmin
   fullPath.SetFormatted("%sHistYmin",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistYmin,"");
   configHisto->fHistYminModified = (configHisto->fHistYmin!="");
   // fHistYmax
   fullPath.SetFormatted("%sHistYmax",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistYmax,"");
   configHisto->fHistYmaxModified = (configHisto->fHistYmax!="");
   // fHistZNbins
   fullPath.SetFormatted("%sHistZNbins",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistZNbins,"");
   configHisto->fHistZNbinsModified = (configHisto->fHistZNbins!="");
   // fHistZmin
   fullPath.SetFormatted("%sHistZmin",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistZmin,"");
   configHisto->fHistZminModified = (configHisto->fHistZmin!="");
   // fHistZmax
   fullPath.SetFormatted("%sHistZmax",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistZmax,"");
   configHisto->fHistZmaxModified = (configHisto->fHistZmax!="");
   // fHistOption
   fullPath.SetFormatted("%sHistOption",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistOption,"");
   configHisto->fHistOptionModified = (configHisto->fHistOption!="");
   // fHistAccumulate
   fullPath.SetFormatted("%sHistAccumulate",path);
   xml->GetPathValue(fullPath.Data(),configHisto->fHistAccumulate,"");
   configHisto->fHistAccumulateModified = (configHisto->fHistAccumulate!="");
   return true;
}

//______________________________________________________________________________
Bool_t ROMEConfig::CheckHistoConfigurationModified(ROMEConfigHisto* configHisto) const
{
   return (configHisto->fHistActiveModified ||
           configHisto->fHistWriteModified ||
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
           configHisto->fHistOptionModified ||
           configHisto->fHistAccumulateModified);
}

//______________________________________________________________________________
Bool_t ROMEConfig::SetHistoConfiguration(ROMEHisto* histo,ROMEConfigHisto* configHisto) const
{
   // fHistActive
   if (configHisto->fHistActiveModified) {
      histo->SetActive((configHisto->fHistActive=="true"));
   }
   // fHistWrite
   if (configHisto->fHistWriteModified) {
      histo->SetWrite((configHisto->fHistWrite=="true"));
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
   // fHistOption
   if (configHisto->fHistOptionModified) {
      histo->SetOption(configHisto->fHistOption.Data());
   }
   // fHistAccumulate
   if (configHisto->fHistAccumulateModified) {
      histo->SetAccumulate((configHisto->fHistAccumulate=="true"));
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEConfig::WriteHistoConfiguration(ROMEXML* xml,ROMEConfigHisto* configHisto,
                                           Int_t commentLevel,ROMEStrArray& comment) const
{
   Int_t iComment = 0;
   // fHistActive
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistActiveModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistActiveModified) {
      xml->WriteElement("HistActive",configHisto->fHistActive.Data());
   }
   iComment++;
   // fHistWrite
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistWriteModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistWriteModified) {
      xml->WriteElement("HistWrite",configHisto->fHistWrite.Data());
   }
   iComment++;
   // fHistTitle
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistTitleModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistTitleModified) {
      xml->WriteElement("HistTitle",configHisto->fHistTitle.Data());
   }
   iComment++;
   // fHistFolderTitle
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistFolderTitleModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistFolderTitleModified) {
      xml->WriteElement("HistFolderTitle",configHisto->fHistFolderTitle.Data());
   }
   iComment++;
   // fHistArraySize
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistArraySizeModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistArraySizeModified) {
      xml->WriteElement("HistArraySize",configHisto->fHistArraySize.Data());
   }
   iComment++;
   // fHistArrayStartIndex
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistArrayStartIndexModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistArrayStartIndexModified) {
      xml->WriteElement("HistArrayStartIndex",configHisto->fHistArrayStartIndex.Data());
   }
   iComment++;
   // fHistXLabel
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistXLabelModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistXLabelModified) {
      xml->WriteElement("HistXLabel",configHisto->fHistXLabel.Data());
   }
   iComment++;
   // fHistYLabel
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistYLabelModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistYLabelModified) {
      xml->WriteElement("HistYLabel",configHisto->fHistYLabel.Data());
   }
   iComment++;
   // fHistZLabel
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistZLabelModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistZLabelModified) {
      xml->WriteElement("HistZLabel",configHisto->fHistZLabel.Data());
   }
   iComment++;
   // fHistXNbins
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistXNbinsModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistXNbinsModified) {
      xml->WriteElement("HistXNbins",configHisto->fHistXNbins.Data());
   }
   iComment++;
   // fHistXmin
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistXminModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistXminModified) {
      xml->WriteElement("HistXmin",configHisto->fHistXmin.Data());
   }
   iComment++;
   // fHistXmax
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistXmaxModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistXmaxModified) {
      xml->WriteElement("HistXmax",configHisto->fHistXmax.Data());
   }
   iComment++;
   // fHistYNbins
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistYNbinsModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistYNbinsModified) {
      xml->WriteElement("HistYNbins",configHisto->fHistYNbins.Data());
   }
   iComment++;
   // fHistYmin
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistYminModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistYminModified) {
      xml->WriteElement("HistYmin",configHisto->fHistYmin.Data());
   }
   iComment++;
   // fHistYmax
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistYmaxModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistYmaxModified) {
      xml->WriteElement("HistYmax",configHisto->fHistYmax.Data());
   }
   iComment++;
   // fHistZNbins
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistZNbinsModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistZNbinsModified) {
      xml->WriteElement("HistZNbins",configHisto->fHistZNbins.Data());
   }
   iComment++;
   // fHistZmin
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistZminModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistZminModified) {
      xml->WriteElement("HistZmin",configHisto->fHistZmin.Data());
   }
   iComment++;
   // fHistZmax
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistZmaxModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistZmaxModified) {
      xml->WriteElement("HistZmax",configHisto->fHistZmax.Data());
   }
   iComment++;
   // fHistOption
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistOptionModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistOptionModified) {
      xml->WriteElement("HistOption",configHisto->fHistOption.Data());
   }
   iComment++;
   // fHistAccumulate
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configHisto->fHistAccumulateModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configHisto->fHistAccumulateModified) {
      xml->WriteElement("HistAccumulate",configHisto->fHistAccumulate.Data());
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEConfig::ReadGraphConfiguration(ROMEXML* xml,const char* path,ROMEConfigGraph* configGraph) const
{
   ROMEString fullPath;
   // fGraphActive
   fullPath.SetFormatted("%sGraphActive",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphActive,"");
   configGraph->fGraphActiveModified = (configGraph->fGraphActive!="");
   // fGraphWrite
   fullPath.SetFormatted("%sGraphWrite",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphWrite,"");
   configGraph->fGraphWriteModified = (configGraph->fGraphWrite!="");
   // fGraphTitle
   fullPath.SetFormatted("%sGraphTitle",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphTitle,"");
   configGraph->fGraphTitleModified = (configGraph->fGraphTitle!="");
   // fGraphFolderTitle
   fullPath.SetFormatted("%sGraphFolderTitle",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphFolderTitle,"");
   configGraph->fGraphFolderTitleModified = (configGraph->fGraphFolderTitle!="");
   // fGraphArraySize
   fullPath.SetFormatted("%sGraphArraySize",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphArraySize,"");
   configGraph->fGraphArraySizeModified = (configGraph->fGraphArraySize!="");
   // fGraphArrayStartIndex
   fullPath.SetFormatted("%sGraphArrayStartIndex",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphArrayStartIndex,"");
   configGraph->fGraphArrayStartIndexModified = (configGraph->fGraphArrayStartIndex!="");
   // fGraphXLabel
   fullPath.SetFormatted("%sGraphXLabel",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphXLabel,"");
   configGraph->fGraphXLabelModified = (configGraph->fGraphXLabel!="");
   // fGraphYLabel
   fullPath.SetFormatted("%sGraphYLabel",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphYLabel,"");
   configGraph->fGraphYLabelModified = (configGraph->fGraphYLabel!="");
   // fGraphZLabel
   fullPath.SetFormatted("%sGraphZLabel",path);
   xml->GetPathValue(fullPath.Data(),configGraph->fGraphZLabel,"");
   configGraph->fGraphZLabelModified = (configGraph->fGraphZLabel!="");
   return true;
}

//______________________________________________________________________________
Bool_t ROMEConfig::CheckGraphConfigurationModified(ROMEConfigGraph* configGraph) const
{
   return (configGraph->fGraphActiveModified ||
           configGraph->fGraphWriteModified ||
           configGraph->fGraphTitleModified ||
           configGraph->fGraphFolderTitleModified ||
           configGraph->fGraphArraySizeModified ||
           configGraph->fGraphArrayStartIndexModified ||
           configGraph->fGraphXLabelModified ||
           configGraph->fGraphYLabelModified ||
           configGraph->fGraphZLabelModified);
}

//______________________________________________________________________________
Bool_t ROMEConfig::SetGraphConfiguration(ROMEGraph* graph,ROMEConfigGraph* configGraph) const
{
   // fGraphActive
   if (configGraph->fGraphActiveModified) {
      graph->SetActive((configGraph->fGraphActive=="true"));
   }
   // fGraphWrite
   if (configGraph->fGraphWriteModified) {
      graph->SetWrite((configGraph->fGraphWrite=="true"));
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

//______________________________________________________________________________
Bool_t ROMEConfig::WriteGraphConfiguration(ROMEXML* xml,ROMEConfigGraph* configGraph,
                                           Int_t commentLevel,ROMEStrArray& comment) const
{
   Int_t iComment = 0;
   // fGraphActive
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configGraph->fGraphActiveModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configGraph->fGraphActiveModified) {
      xml->WriteElement("GraphActive",configGraph->fGraphActive.Data());
   }
   iComment++;
   // fGraphWrite
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configGraph->fGraphWriteModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configGraph->fGraphWriteModified) {
      xml->WriteElement("GraphWrite",configGraph->fGraphWrite.Data());
   }
   iComment++;
   // fGraphTitle
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configGraph->fGraphTitleModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configGraph->fGraphTitleModified) {
      xml->WriteElement("GraphTitle",configGraph->fGraphTitle.Data());
   }
   iComment++;
   // fGraphFolderTitle
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configGraph->fGraphFolderTitleModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configGraph->fGraphFolderTitleModified) {
      xml->WriteElement("GraphFolderTitle",configGraph->fGraphFolderTitle.Data());
   }
   iComment++;
   // fGraphArraySize
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configGraph->fGraphArraySizeModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configGraph->fGraphArraySizeModified) {
      xml->WriteElement("GraphArraySize",configGraph->fGraphArraySize.Data());
   }
   iComment++;
   // fGraphArrayStartIndex
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configGraph->fGraphArrayStartIndexModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configGraph->fGraphArrayStartIndexModified) {
      xml->WriteElement("GraphArrayStartIndex",configGraph->fGraphArrayStartIndex.Data());
   }
   iComment++;
   // fGraphXLabel
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configGraph->fGraphXLabelModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configGraph->fGraphXLabelModified) {
      xml->WriteElement("GraphXLabel",configGraph->fGraphXLabel.Data());
   }
   iComment++;
   // fGraphYLabel
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configGraph->fGraphYLabelModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configGraph->fGraphYLabelModified) {
      xml->WriteElement("GraphYLabel",configGraph->fGraphYLabel.Data());
   }
   iComment++;
   // fGraphZLabel
   if (commentLevel >= ROMEConfig::kCommentLevelParam && configGraph->fGraphZLabelModified) {
      xml->WriteComment(comment.At(iComment).Data());
   }
   if (configGraph->fGraphZLabelModified) {
      xml->WriteElement("GraphZLabel",configGraph->fGraphZLabel.Data());
   }
   return true;
}

