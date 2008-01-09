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
,fWrite(kTRUE)
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
,fOption("")
,fOptionOrig("")
,fAccumulate(kFALSE)
{
}

//______________________________________________________________________________
void ROMEHisto::SetOriginal(const char* title,const char* folderTitle,Int_t arraySize,Int_t arrayStartIndex,
                            const char* xLabel,const char* yLabel,const char* zLabel,Int_t xNbins,Double_t xmin,Double_t xmax,
                            Int_t yNbins,Double_t ymin,Double_t ymax,Int_t zNbins,Double_t zmin,Double_t zmax,const char* option)
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
   fOptionOrig = option;
}

//______________________________________________________________________________
const char* ROMEHisto::GetTitle() const{
   if (fTitle.Length() == 0)
      return fTitleOrig;
   else
      return fTitle;
}

//______________________________________________________________________________
const char* ROMEHisto::GetFolderTitle() const
{
   if (fFolderTitle.Length() == 0)
      return fFolderTitleOrig;
   else
      return fFolderTitle;
}

//______________________________________________________________________________
const char* ROMEHisto::GetXLabel() const
{
   if (fXLabel.Length() == 0)
      return fXLabelOrig;
   else
      return fXLabel;
}

//______________________________________________________________________________
const char* ROMEHisto::GetYLabel() const
{
   if (fYLabel.Length() == 0)
      return fYLabelOrig;
   else
      return fYLabel;
}

//______________________________________________________________________________
const char* ROMEHisto::GetZLabel() const
{
   if (fZLabel.Length() == 0)
      return fZLabelOrig;
   else
      return fZLabel;
}

//______________________________________________________________________________
int ROMEHisto::GetArraySize() const
{
   if (fArraySize.Length() == 0)
      return fArraySizeOrig;
   else
      return strtol(fArraySize.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetArraySizeString(ROMEString &str) const
{
   if (fArraySize.Length() == 0)
      str.SetFormatted("%d",fArraySizeOrig);
   else
      str = fArraySize;
   return str.Data();
}

//______________________________________________________________________________
int ROMEHisto::GetArrayStartIndex() const
{
   if (fArrayStartIndex.Length() == 0)
      return fArrayStartIndexOrig;
   else
      return strtol(fArrayStartIndex.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetArrayStartIndexString(ROMEString &str) const
{
   if (fArrayStartIndex.Length() == 0)
      str.SetFormatted("%d",fArrayStartIndexOrig);
   else
      str = fArrayStartIndex;
   return str.Data();
}

//______________________________________________________________________________
int ROMEHisto::GetXNbins() const
{
   if (fXNbins.Length() == 0)
      return fXNbinsOrig;
   else
      return strtol(fXNbins.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetXNbinsString(ROMEString &str) const
{
   if (fXNbins.Length() == 0)
      str.SetFormatted("%d",fXNbinsOrig);
   else
      str = fXNbins;
   return str.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetXmin() const
{
   if (fXmin.Length() == 0)
      return fXminOrig;
   else
      return strtod(fXmin.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetXminString(ROMEString &str) const
{
   if (fXmin.Length() == 0)
      str.SetFormatted("%g",fXminOrig);
   else
      str = fXmin;
   return str.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetXmax() const
{
   if (fXmax.Length() == 0)
      return fXmaxOrig;
   else
      return strtod(fXmax.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetXmaxString(ROMEString &str) const
{
   if (fXmax.Length() == 0)
      str.SetFormatted("%g",fXmaxOrig);
   else
      str = fXmax;
   return str.Data();
}

//______________________________________________________________________________
int ROMEHisto::GetYNbins() const
{
   if (fYNbins.Length() == 0)
      return fYNbinsOrig;
   else
      return strtol(fYNbins.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetYNbinsString(ROMEString &str) const
{
   if (fYNbins.Length() == 0)
      str.SetFormatted("%d",fYNbinsOrig);
   else
      str = fYNbins;
   return str.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetYmin() const
{
   if (fYmin.Length() == 0)
      return fYminOrig;
   else
      return strtod(fYmin.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetYminString(ROMEString &str) const
{
   if (fYmin.Length() == 0)
      str.SetFormatted("%g",fYminOrig);
   else
      str = fYmin;
   return str.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetYmax() const
{
   if (fYmax.Length() == 0)
      return fYmaxOrig;
   else
      return strtod(fYmax.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetYmaxString(ROMEString &str) const
{
   if (fYmax.Length() == 0)
      str.SetFormatted("%g",fYmaxOrig);
   else
      str = fYmax;
   return str.Data();
}

//______________________________________________________________________________
int ROMEHisto::GetZNbins() const
{
   if (fZNbins.Length() == 0)
      return fZNbinsOrig;
   else
      return strtol(fZNbins.Data(),&fgCstop,10);
}

//______________________________________________________________________________
const char* ROMEHisto::GetZNbinsString(ROMEString &str) const
{
   if (fZNbins.Length() == 0)
      str.SetFormatted("%d",fZNbinsOrig);
   else
      str = fZNbins;
   return str.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetZmin() const
{
   if (fZmin.Length() == 0)
      return fZminOrig;
   else
      return strtod(fZmin.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetZminString(ROMEString &str) const
{
   if (fZmin.Length() == 0)
      str.SetFormatted("%g",fZminOrig);
   else
      str = fZmin;
   return str.Data();
}

//______________________________________________________________________________
double ROMEHisto::GetZmax() const
{
   if (fZmax.Length() == 0)
      return fZmaxOrig;
   else
      return strtod(fZmax.Data(),&fgCstop);
}

//______________________________________________________________________________
const char* ROMEHisto::GetZmaxString(ROMEString &str) const
{
   if (fZmax.Length() == 0)
      str.SetFormatted("%g",fZmaxOrig);
   else
      str = fZmax;
   return str.Data();
}

//______________________________________________________________________________
const char *ROMEHisto::GetOption() const
{
   if (fOption.Length()  == 0) {
      return fOptionOrig;
   }
   return fOption;
}
