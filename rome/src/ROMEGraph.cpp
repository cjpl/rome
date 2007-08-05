// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEGraph                                                            //
//                                                                      //
//  The Base Class for all Graphs in ROME.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "ROMEGraph.h"
#include "ROMEiostream.h"

ClassImp(ROMEGraph)

static char* fgCstop = 0;

//______________________________________________________________________________
ROMEGraph::ROMEGraph()
:TNamed("", "")
,fActive(kTRUE)
,fTitleOrig("")
,fFolderTitle("")
,fFolderTitleOrig("")
,fArraySize("")
,fArraySizeOrig(0)
,fArrayStartIndex("")
,fArrayStartIndexOrig(0)
,fXLabel("")
,fXLabelOrig("")
,fYLabel("")
,fYLabelOrig("")
,fZLabel("")
,fZLabelOrig("")
,fXmin("")
,fXminOrig(0)
,fXmax("")
,fXmaxOrig(0)
,fYmin("")
,fYminOrig(0)
,fYmax("")
,fYmaxOrig(0)
,fZmin("")
,fZminOrig(0)
,fZmax("")
,fZmaxOrig(0)
{
}

//______________________________________________________________________________
void ROMEGraph::SetOriginal(const char* title,const char* folderTitle,Int_t arraySize,Int_t arrayStartIndex,
                            const char* xLabel,const char* yLabel,const char* zLabel,Double_t xmin,Double_t xmax,
                            Double_t ymin,Double_t ymax,Double_t zmin,Double_t zmax)
{
   fTitleOrig = title;
   fFolderTitleOrig = folderTitle;
   fArraySizeOrig = arraySize;
   fArrayStartIndexOrig = arrayStartIndex;
   fXLabelOrig = xLabel;
   fYLabelOrig = yLabel;
   fZLabelOrig = zLabel;
   fXminOrig = xmin;
   fXmaxOrig = xmax;
   fYminOrig = ymin;
   fYmaxOrig = ymax;
   fZminOrig = zmin;
   fZmaxOrig = zmax;
}

//______________________________________________________________________________
const char* ROMEGraph::GetTitle() const{
   if (fTitle.Length()==0)
      return fTitleOrig;
   else
      return fTitle;
}

//______________________________________________________________________________
const char* ROMEGraph::GetFolderTitle() const
{
   if (fFolderTitle.Length()==0)
      return fFolderTitleOrig;
   else
      return fFolderTitle;
}

//______________________________________________________________________________
const char* ROMEGraph::GetXLabel() const
{
   if (fXLabel.Length()==0)
      return fXLabelOrig;
   else
      return fXLabel;
}

//______________________________________________________________________________
const char* ROMEGraph::GetYLabel() const
{
   if (fYLabel.Length()==0)
      return fYLabelOrig;
   else
      return fYLabel;
}

//______________________________________________________________________________
const char* ROMEGraph::GetZLabel() const
{
   if (fZLabel.Length()==0)
      return fZLabelOrig;
   else
      return fZLabel;
}

//______________________________________________________________________________
int ROMEGraph::GetArraySize() const
{
   if (fArraySize.Length()==0)
      return fArraySizeOrig;
   else
      return strtol(fArraySize.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEGraph::GetArraySizeString(ROMEString &string) const
{
   if (fArraySize.Length()==0)
      string.SetFormatted("%d",fArraySizeOrig);
   else
      string = fArraySize;
   return string.Data();
}

//______________________________________________________________________________
int ROMEGraph::GetArrayStartIndex() const
{
   if (fArrayStartIndex.Length()==0)
      return fArrayStartIndexOrig;
   else
      return strtol(fArrayStartIndex.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEGraph::GetArrayStartIndexString(ROMEString &string) const
{
   if (fArrayStartIndex.Length()==0)
      string.SetFormatted("%d",fArrayStartIndexOrig);
   else
      string = fArrayStartIndex;
   return string.Data();
}

//______________________________________________________________________________
double ROMEGraph::GetXmin() const
{
   if (fXmin.Length()==0)
      return fXminOrig;
   else
      return strtol(fXmin.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEGraph::GetXminString(ROMEString &string) const
{
   if (fXmin.Length()==0)
      string.SetFormatted("%g",fXminOrig);
   else
      string = fXmin;
   return string.Data();
}

//______________________________________________________________________________
double ROMEGraph::GetXmax() const
{
   if (fXmax.Length()==0)
      return fXmaxOrig;
   else
      return strtol(fXmax.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEGraph::GetXmaxString(ROMEString &string) const
{
   if (fXmax.Length()==0)
      string.SetFormatted("%g",fXmaxOrig);
   else
      string = fXmax;
   return string.Data();
}

//______________________________________________________________________________
double ROMEGraph::GetYmin() const
{
   if (fYmin.Length()==0)
      return fYminOrig;
   else
      return strtol(fYmin.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEGraph::GetYminString(ROMEString &string) const
{
   if (fYmin.Length()==0)
      string.SetFormatted("%g",fYminOrig);
   else
      string = fYmin;
   return string.Data();
}

//______________________________________________________________________________
double ROMEGraph::GetYmax() const
{
   if (fYmax.Length()==0)
      return fYmaxOrig;
   else
      return strtol(fYmax.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEGraph::GetYmaxString(ROMEString &string) const
{
   if (fYmax.Length()==0)
      string.SetFormatted("%g",fYmaxOrig);
   else
      string = fYmax;
   return string.Data();
}

//______________________________________________________________________________
double ROMEGraph::GetZmin() const
{
   if (fZmin.Length()==0)
      return fZminOrig;
   else
      return strtol(fZmin.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEGraph::GetZminString(ROMEString &string) const
{
   if (fZmin.Length()==0)
      string.SetFormatted("%g",fZminOrig);
   else
      string = fZmin;
   return string.Data();
}

//______________________________________________________________________________
double ROMEGraph::GetZmax() const
{
   if (fZmax.Length()==0)
      return fZmaxOrig;
   else
      return strtol(fZmax.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEGraph::GetZmaxString(ROMEString &string) const
{
   if (fZmax.Length()==0)
      string.SetFormatted("%g",fZmaxOrig);
   else
      string = fZmax;
   return string.Data();
}
