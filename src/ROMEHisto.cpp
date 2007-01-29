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

ROMEHisto::ROMEHisto():TObject()
{
   fActive = true;
   fAccumulation = false;
}

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

const char* ROMEHisto::GetTitle() const{
   if (fTitle.Length()==0)
      return fTitleOrig;
   else
      return fTitle;
}

const char* ROMEHisto::GetFolderTitle() {
   if (fFolderTitle.Length()==0)
      return fFolderTitleOrig;
   else
      return fFolderTitle;
}

const char* ROMEHisto::GetXLabel() {
   if (fXLabel.Length()==0)
      return fXLabelOrig;
   else
      return fXLabel;
}

const char* ROMEHisto::GetYLabel() {
   if (fYLabel.Length()==0)
      return fYLabelOrig;
   else
      return fYLabel;
}

const char* ROMEHisto::GetZLabel() {
   if (fZLabel.Length()==0)
      return fZLabelOrig;
   else
      return fZLabel;
}

int ROMEHisto::GetArraySize() {
   if (fArraySize.Length()==0)
      return fArraySizeOrig;
   else
      return strtol(fArraySize.Data(),&fCstop,10);
}

const char* ROMEHisto::GetArraySizeString(ROMEString &string) {
   if (fArraySize.Length()==0)
      string.SetFormatted("%d",fArraySizeOrig);
   else
      string = fArraySize;
   return string.Data();
}

int ROMEHisto::GetArrayStartIndex() {
   if (fArrayStartIndex.Length()==0)
      return fArrayStartIndexOrig;
   else
      return strtol(fArrayStartIndex.Data(),&fCstop,10);
}

const char* ROMEHisto::GetArrayStartIndexString(ROMEString &string) {
   if (fArrayStartIndex.Length()==0)
      string.SetFormatted("%d",fArrayStartIndexOrig);
   else
      string = fArrayStartIndex;
   return string.Data();
}

int ROMEHisto::GetXNbins() {
   if (fXNbins.Length()==0)
      return fXNbinsOrig;
   else
      return strtol(fXNbins.Data(),&fCstop,10);
}

const char* ROMEHisto::GetXNbinsString(ROMEString &string) {
   if (fXNbins.Length()==0)
      string.SetFormatted("%d",fXNbinsOrig);
   else
      string = fXNbins;
   return string.Data();
}

double ROMEHisto::GetXmin() {
   if (fXmin.Length()==0)
      return fXminOrig;
   else
      return strtod(fXmin.Data(),&fCstop);
}

const char* ROMEHisto::GetXminString(ROMEString &string) {
   if (fXmin.Length()==0)
      string.SetFormatted("%f",fXminOrig);
   else
      string = fXmin;
   return string.Data();
}

double ROMEHisto::GetXmax() {
   if (fXmax.Length()==0)
      return fXmaxOrig;
   else
      return strtod(fXmax.Data(),&fCstop);
}

const char* ROMEHisto::GetXmaxString(ROMEString &string) {
   if (fXmax.Length()==0)
      string.SetFormatted("%f",fXmaxOrig);
   else
      string = fXmax;
   return string.Data();
}

int ROMEHisto::GetYNbins() {
   if (fYNbins.Length()==0)
      return fYNbinsOrig;
   else
      return strtol(fYNbins.Data(),&fCstop,10);
}

const char* ROMEHisto::GetYNbinsString(ROMEString &string) {
   if (fYNbins.Length()==0)
      string.SetFormatted("%d",fYNbinsOrig);
   else
      string = fYNbins;
   return string.Data();
}

double ROMEHisto::GetYmin() {
   if (fYmin.Length()==0)
      return fYminOrig;
   else
      return strtod(fYmin.Data(),&fCstop);
}

const char* ROMEHisto::GetYminString(ROMEString &string) {
   if (fYmin.Length()==0)
      string.SetFormatted("%f",fYminOrig);
   else
      string = fYmin;
   return string.Data();
}

double ROMEHisto::GetYmax() {
   if (fYmax.Length()==0)
      return fYmaxOrig;
   else
      return strtod(fYmax.Data(),&fCstop);
}

const char* ROMEHisto::GetYmaxString(ROMEString &string) {
   if (fYmax.Length()==0)
      string.SetFormatted("%f",fYmaxOrig);
   else
      string = fYmax;
   return string.Data();
}

int ROMEHisto::GetZNbins() {
   if (fZNbins.Length()==0)
      return fZNbinsOrig;
   else
      return strtol(fZNbins.Data(),&fCstop,10);
}

const char* ROMEHisto::GetZNbinsString(ROMEString &string) {
   if (fZNbins.Length()==0)
      string.SetFormatted("%d",fZNbinsOrig);
   else
      string = fZNbins;
   return string.Data();
}

double ROMEHisto::GetZmin() {
   if (fZmin.Length()==0)
      return fZminOrig;
   else
      return strtod(fZmin.Data(),&fCstop);
}

const char* ROMEHisto::GetZminString(ROMEString &string) {
   if (fZmin.Length()==0)
      string.SetFormatted("%f",fZminOrig);
   else
      string = fZmin;
   return string.Data();
}

double ROMEHisto::GetZmax() {
   if (fZmax.Length()==0)
      return fZmaxOrig;
   else
      return strtod(fZmax.Data(),&fCstop);
}

const char* ROMEHisto::GetZmaxString(ROMEString &string) {
   if (fZmax.Length()==0)
      string.SetFormatted("%f",fZmaxOrig);
   else
      string = fZmax;
   return string.Data();
}
