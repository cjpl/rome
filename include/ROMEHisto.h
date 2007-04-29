/********************************************************************
  ROMEHisto.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEHisto_H
#define ROMEHisto_H

#include <TObject.h>
#include "ROMEString.h"

class ROMEHisto : public TObject {
private:
   char*      fCstop; //!

protected:
   Bool_t     fActive;                  // Active flag
   ROMEString fTitle;                   // Title
   ROMEString fTitleOrig;               // Title Original
   ROMEString fFolderTitle;             // Title of the Folder
   ROMEString fFolderTitleOrig;         // Title of the Folder Original
   ROMEString fArraySize;               // Array Size
   Int_t      fArraySizeOrig;           // Array Size Original
   ROMEString fArrayStartIndex;         // Array Start Index
   Int_t      fArrayStartIndexOrig;     // Array Start Index Original
   ROMEString fXLabel;                  // Label of the X axis
   ROMEString fXLabelOrig;              // Label of the X axis Original
   ROMEString fYLabel;                  // Label of the Y axis
   ROMEString fYLabelOrig;              // Label of the Y axis Original
   ROMEString fZLabel;                  // Label of the Z axis
   ROMEString fZLabelOrig;              // Label of the Z axis Original
   ROMEString fXNbins;                  // Number of bins on the X axis
   Int_t      fXNbinsOrig;              // Number of bins on the X axis Original
   ROMEString fXmin;                    // Minimum value on the X axis
   Double_t   fXminOrig;                // Minimum value on the X axis Original
   ROMEString fXmax;                    // Maximum value on the X axis
   Double_t   fXmaxOrig;                // Maximum value on the X axis Original
   ROMEString fYNbins;                  // Number of bins on the Y axis
   Int_t      fYNbinsOrig;              // Number of bins on the Y axis Original
   ROMEString fYmin;                    // Minimum value on the Y axis
   Double_t   fYminOrig;                // Minimum value on the Y axis Original
   ROMEString fYmax;                    // Maximum value on the Y axis
   Double_t   fYmaxOrig;                // Maximum value on the Y axis Original
   ROMEString fZNbins;                  // Number of bins on the Z axis
   Int_t      fZNbinsOrig;              // Number of bins on the Z axis Original
   ROMEString fZmin;                    // Minimum value on the Z axis
   Double_t   fZminOrig;                // Minimum value on the Z axis Original
   ROMEString fZmax;                    // Maximum value on the Z axis
   Double_t   fZmaxOrig;                // Maximum value on the Z axis Original
   Bool_t     fAccumulation;            // Accumulation Flag

private:
   ROMEHisto(const ROMEHisto &histo); // not implemented
   ROMEHisto &operator=(const ROMEHisto &rhs); // not implemented

public:
   ROMEHisto();
   virtual ~ROMEHisto() {};

   void SetActive(bool value) { fActive = value; };
   void SetTitle(const char* value) { fTitle = value; };
   void SetFolderTitle(const char* value) { fFolderTitle = value; };
   void SetArraySize(const char* value) { fArraySize = value; };
   void SetArrayStartIndex(const char* value) { fArrayStartIndex = value; };
   void SetXLabel(const char* value) { fXLabel = value; };
   void SetYLabel(const char* value) { fYLabel = value; };
   void SetZLabel(const char* value) { fZLabel = value; };
   void SetXNbins(const char* value) { fXNbins = value; };
   void SetXmin(const char* value) { fXmin = value; };
   void SetXmax(const char* value) { fXmax = value; };
   void SetYNbins(const char* value) { fYNbins = value; };
   void SetYmin(const char* value) { fYmin = value; };
   void SetYmax(const char* value) { fYmax = value; };
   void SetZNbins(const char* value) { fZNbins = value; };
   void SetZmin(const char* value) { fZmin = value; };
   void SetZmax(const char* value) { fZmax = value; };
   void SetOriginal(const char* title,const char* folderTitle,Int_t arraySize,Int_t arrayStartIndex,const char* xLabel,const char* yLabel,const char* zLabel,Int_t xNbins,Double_t xmin,Double_t xmax,Int_t yNbins,Double_t ymin,Double_t ymax,Int_t zNbins,Double_t zmin,Double_t zmax);
   void SetTitleOriginal(const char* value) { fTitleOrig = value; };
   void SetFolderTitleOriginal(const char* value) { fFolderTitleOrig = value; };
   void SetArraySizeOriginal(Int_t value) { fArraySizeOrig = value; };
   void SetArrayStartIndexOriginal(Int_t value) { fArrayStartIndexOrig = value; };
   void SetXLabelOriginal(const char* value) { fXLabelOrig = value; };
   void SetYLabelOriginal(const char* value) { fYLabelOrig = value; };
   void SetZLabelOriginal(const char* value) { fZLabelOrig = value; };
   void SetXNbinsOriginal(Int_t value) { fXNbinsOrig = value; };
   void SetXminOriginal(Double_t value) { fXminOrig = value; };
   void SetXmaxOriginal(Double_t value) { fXmaxOrig = value; };
   void SetYNbinsOriginal(Int_t value) { fYNbinsOrig = value; };
   void SetYminOriginal(Double_t value) { fYminOrig = value; };
   void SetYmaxOriginal(Double_t value) { fYmaxOrig = value; };
   void SetZNbinsOriginal(Int_t value) { fZNbinsOrig = value; };
   void SetZminOriginal(Double_t value) { fZminOrig = value; };
   void SetZmaxOriginal(Double_t value) { fZmaxOrig = value; };
   bool IsActive() { return fActive; };
   const char* GetTitle() const;
   const char* GetFolderTitle();
   const char* GetXLabel();
   const char* GetYLabel();
   const char* GetZLabel();
   int GetArraySize();
   const char* GetArraySizeString(ROMEString &string);
   int GetArrayStartIndex();
   const char* GetArrayStartIndexString(ROMEString &string);
   int GetXNbins();
   const char* GetXNbinsString(ROMEString &string);
   double GetXmin();
   const char* GetXminString(ROMEString &string);
   double GetXmax();
   const char* GetXmaxString(ROMEString &string);
   int GetYNbins();
   const char* GetYNbinsString(ROMEString &string);
   double GetYmin();
   const char* GetYminString(ROMEString &string);
   double GetYmax();
   const char* GetYmaxString(ROMEString &string);
   int GetZNbins();
   const char* GetZNbinsString(ROMEString &string);
   double GetZmin();
   const char* GetZminString(ROMEString &string);
   double GetZmax();
   const char* GetZmaxString(ROMEString &string);

   Bool_t isAccumulation() { return fAccumulation; };
   void SetAccumulation(Bool_t flag) { fAccumulation = flag; };

   ClassDef(ROMEHisto,0) // ROMEHisto
};

#endif   // ROMEHisto_H
