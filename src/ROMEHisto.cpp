// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEHisto                                                            //
//                                                                      //
//  The Base Class for all Histos in ROME.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "ROMEHisto.h"
#include "ROMEiostream.h"

ClassImp(ROMEHisto)

static char* fgCstop = 0;

//______________________________________________________________________________
ROMEHisto::ROMEHisto()
:TObject()
,fActive(kTRUE)
,fTitle("")
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
,fXNbins("")
,fXNbinsOrig(0)
,fXmin("")
,fXminOrig(0)
,fXmax("")
,fXmaxOrig(0)
,fYNbins("")
,fYNbinsOrig(0)
,fYmin("")
,fYminOrig(0)
,fYmax("")
,fYmaxOrig(0)
,fZNbins("")
,fZNbinsOrig(0)
,fZmin("")
,fZminOrig(0)
,fZmax("")
,fZmaxOrig(0)
,fAccumulation(kFALSE)
{
}

//______________________________________________________________________________
void ROMEHisto::SetOriginal(const char* title,const char* folderTitle,Int_t arraySize,Int_t arrayStartIndex,
                            const char* xLabel,const char* yLabel,const char* zLabel,Int_t xNbins,Double_t xmin,Double_t xmax,
                            Int_t yNbins,Double_t ymin,Double_t ymax,Int_t zNbins,Double_t zmin,Double_t zmax)
{
   fTitleOrig = title;
   fFolderTitleOrig = folderTitle;
   fArraySizeOrig = arraySize;
   fArrayStartIndexOrig = arrayStartIndex;
   fXLabelOrig = xLabel;
   fYLabelOrig = yLabel;
   fZLabelOrig = zLabel;
   fXNbinsOrig = xNbins;
   fXminOrig = xmin;
   fXmaxOrig = xmax;
   fYNbinsOrig = yNbins;
   fYminOrig = ymin;
   fYmaxOrig = ymax;
   fZNbinsOrig = zNbins;
   fZminOrig = zmin;
   fZmaxOrig = zmax;
}

//______________________________________________________________________________
const char* ROMEHisto::GetTitle() const{
   if (fTitle.Length()==0)
      return fTitleOrig;
   else
      return fTitle;
}

//______________________________________________________________________________
const char* ROMEHisto::GetFolderTitle() const
{
   if (fFolderTitle.Length()==0)
      return fFolderTitleOrig;
   else
      return fFolderTitle;
}

//______________________________________________________________________________
const char* ROMEHisto::GetXLabel() const
{
   if (fXLabel.Length()==0)
      return fXLabelOrig;
   else
      return fXLabel;
}

//______________________________________________________________________________
const char* ROMEHisto::GetYLabel() const
{
   if (fYLabel.Length()==0)
      return fYLabelOrig;
   else
      return fYLabel;
}

//______________________________________________________________________________
const char* ROMEHisto::GetZLabel() const
{
   if (fZLabel.Length()==0)
      return fZLabelOrig;
   else
      return fZLabel;
}

//______________________________________________________________________________
int ROMEHisto::GetArraySize() const
{
   if (fArraySize.Length()==0)
      return fArraySizeOrig;
   else
      return strtol(fArraySize.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetArraySizeString(ROMEString &string) const
{
   if (fArraySize.Length()==0)
      string.SetFormatted("%d",fArraySizeOrig);
   else
      string = fArraySize;
   return string.Data();
}

//______________________________________________________________________________
int ROMEHisto::GetArrayStartIndex() const
{
   if (fArrayStartIndex.Length()==0)
      return fArrayStartIndexOrig;
   else
      return strtol(fArrayStartIndex.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetArrayStartIndexString(ROMEString &string) const
{
   if (fArrayStartIndex.Length()==0)
      string.SetFormatted("%d",fArrayStartIndexOrig);
   else
      string = fArrayStartIndex;
   return string.Data();
}

//______________________________________________________________________________
int ROMEHisto::GetXNbins() const
{
   if (fXNbins.Length()==0)
      return fXNbinsOrig;
   else
      return strtol(fXNbins.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetXNbinsString(ROMEString &string) const
{
   if (fXNbins.Length()==0)
      string.SetFormatted("%d",fXNbinsOrig);
   else
      string = fXNbins;
   return string.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetXmin() const
{
   if (fXmin.Length()==0)
      return fXminOrig;
   else
      return strtod(fXmin.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetXminString(ROMEString &string) const
{
   if (fXmin.Length()==0)
      string.SetFormatted("%f",fXminOrig);
   else
      string = fXmin;
   return string.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetXmax() const
{
   if (fXmax.Length()==0)
      return fXmaxOrig;
   else
      return strtod(fXmax.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetXmaxString(ROMEString &string) const
{
   if (fXmax.Length()==0)
      string.SetFormatted("%f",fXmaxOrig);
   else
      string = fXmax;
   return string.Data();
}

//______________________________________________________________________________
int ROMEHisto::GetYNbins() const
{
   if (fYNbins.Length()==0)
      return fYNbinsOrig;
   else
      return strtol(fYNbins.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetYNbinsString(ROMEString &string) const
{
   if (fYNbins.Length()==0)
      string.SetFormatted("%d",fYNbinsOrig);
   else
      string = fYNbins;
   return string.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetYmin() const
{
   if (fYmin.Length()==0)
      return fYminOrig;
   else
      return strtod(fYmin.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetYminString(ROMEString &string) const
{
   if (fYmin.Length()==0)
      string.SetFormatted("%f",fYminOrig);
   else
      string = fYmin;
   return string.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetYmax() const
{
   if (fYmax.Length()==0)
      return fYmaxOrig;
   else
      return strtod(fYmax.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetYmaxString(ROMEString &string) const
{
   if (fYmax.Length()==0)
      string.SetFormatted("%f",fYmaxOrig);
   else
      string = fYmax;
   return string.Data();
}

//______________________________________________________________________________
int ROMEHisto::GetZNbins() const
{
   if (fZNbins.Length()==0)
      return fZNbinsOrig;
   else
      return strtol(fZNbins.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetZNbinsString(ROMEString &string) const
{
   if (fZNbins.Length()==0)
      string.SetFormatted("%d",fZNbinsOrig);
   else
      string = fZNbins;
   return string.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetZmin() const
{
   if (fZmin.Length()==0)
      return fZminOrig;
   else
      return strtod(fZmin.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetZminString(ROMEString &string) const
{
   if (fZmin.Length()==0)
      string.SetFormatted("%f",fZminOrig);
   else
      string = fZmin;
   return string.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetZmax() const
{
   if (fZmax.Length()==0)
      return fZmaxOrig;
   else
      return strtod(fZmax.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetZmaxString(ROMEString &string) const
{
   if (fZmax.Length()==0)
      string.SetFormatted("%f",fZmaxOrig);
   else
      string = fZmax;
   return string.Data();
}
