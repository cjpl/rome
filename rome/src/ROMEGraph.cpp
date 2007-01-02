// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEGraph                                                            //
//                                                                      //
//  The Base Class for all Graphs in ROME.
//                                                                      //
//  $Id: ROMEGraph.cpp 1433 2006-10-10 17:11:13Z sawada $
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "ROMEGraph.h"
#include "ROMEiostream.h"

ClassImp(ROMEGraph)

ROMEGraph::ROMEGraph():TObject()
{
}

const char* ROMEGraph::GetTitle() const{
   if (fTitle.Length()==0)
      return fTitleOrig;
   else
      return fTitle;
}
const char* ROMEGraph::GetFolderTitle() {
   if (fFolderTitle.Length()==0)
      return fFolderTitleOrig;
   else
      return fFolderTitle;
}
const char* ROMEGraph::GetXLabel() {
   if (fXLabel.Length()==0)
      return fXLabelOrig;
   else
      return fXLabel;
}
const char* ROMEGraph::GetYLabel() {
   if (fYLabel.Length()==0)
      return fYLabelOrig;
   else
      return fYLabel;
}
const char* ROMEGraph::GetZLabel() {
   if (fZLabel.Length()==0)
      return fZLabelOrig;
   else
      return fZLabel;
}
int ROMEGraph::GetArraySize() {
   if (fArraySize.Length()==0)
      return fArraySizeOrig;
   else
      return strtol(fArraySize.Data(),&fCstop,10);
}
const char* ROMEGraph::GetArraySizeString(ROMEString &string) {
   if (fArraySize.Length()==0)
      string.SetFormatted("%d",fArraySizeOrig);
   else
      string = fArraySize;
   return string.Data();
}
int ROMEGraph::GetArrayStartIndex() {
   if (fArrayStartIndex.Length()==0)
      return fArrayStartIndexOrig;
   else
      return strtol(fArrayStartIndex.Data(),&fCstop,10);
}
const char* ROMEGraph::GetArrayStartIndexString(ROMEString &string) {
   if (fArrayStartIndex.Length()==0)
      string.SetFormatted("%d",fArrayStartIndexOrig);
   else
      string = fArrayStartIndex;
   return string.Data();
}
double ROMEGraph::GetXmin() {
   if (fXmin.Length()==0)
      return fXminOrig;
   else
      return strtol(fXmin.Data(),&fCstop,10);
}
const char* ROMEGraph::GetXminString(ROMEString &string) {
   if (fXmin.Length()==0)
      string.SetFormatted("%g",fXminOrig);
   else
      string = fXmin;
   return string.Data();
}
double ROMEGraph::GetXmax() {
   if (fXmax.Length()==0)
      return fXmaxOrig;
   else
      return strtol(fXmax.Data(),&fCstop,10);
}
const char* ROMEGraph::GetXmaxString(ROMEString &string) {
   if (fXmax.Length()==0)
      string.SetFormatted("%g",fXmaxOrig);
   else
      string = fXmax;
   return string.Data();
}
double ROMEGraph::GetYmin() {
   if (fYmin.Length()==0)
      return fYminOrig;
   else
      return strtol(fYmin.Data(),&fCstop,10);
}
const char* ROMEGraph::GetYminString(ROMEString &string) {
   if (fYmin.Length()==0)
      string.SetFormatted("%g",fYminOrig);
   else
      string = fYmin;
   return string.Data();
}
double ROMEGraph::GetYmax() {
   if (fYmax.Length()==0)
      return fYmaxOrig;
   else
      return strtol(fYmax.Data(),&fCstop,10);
}
const char* ROMEGraph::GetYmaxString(ROMEString &string) {
   if (fYmax.Length()==0)
      string.SetFormatted("%g",fYmaxOrig);
   else
      string = fYmax;
   return string.Data();
}
double ROMEGraph::GetZmin() {
   if (fZmin.Length()==0)
      return fZminOrig;
   else
      return strtol(fZmin.Data(),&fCstop,10);
}
const char* ROMEGraph::GetZminString(ROMEString &string) {
   if (fZmin.Length()==0)
      string.SetFormatted("%g",fZminOrig);
   else
      string = fZmin;
   return string.Data();
}
double ROMEGraph::GetZmax() {
   if (fZmax.Length()==0)
      return fZmaxOrig;
   else
      return strtol(fZmax.Data(),&fCstop,10);
}
const char* ROMEGraph::GetZmaxString(ROMEString &string) {
   if (fZmax.Length()==0)
      string.SetFormatted("%g",fZmaxOrig);
   else
      string = fZmax;
   return string.Data();
}
