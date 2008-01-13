/********************************************************************
  ROMEGraph.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEGraph_H
#define ROMEGraph_H

#include <TObject.h>
#include "ROMEString.h"

class ROMEGraph : public TNamed {
protected:
   Bool_t     fActive;                  // Active flag
   Bool_t     fWrite;                   // Write flag
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
   ROMEString fXmin;                    // Minimum value on the X axis
   Double_t   fXminOrig;                // Minimum value on the X axis Original
   ROMEString fXmax;                    // Maximum value on the X axis
   Double_t   fXmaxOrig;                // Maximum value on the X axis Original
   ROMEString fYmin;                    // Minimum value on the Y axis
   Double_t   fYminOrig;                // Minimum value on the Y axis Original
   ROMEString fYmax;                    // Maximum value on the Y axis
   Double_t   fYmaxOrig;                // Maximum value on the Y axis Original
   ROMEString fZmin;                    // Minimum value on the Z axis
   Double_t   fZminOrig;                // Minimum value on the Z axis Original
   ROMEString fZmax;                    // Maximum value on the Z axis
   Double_t   fZmaxOrig;                // Maximum value on the Z axis Original
   Bool_t     fAccumulate;              // Accumulation Flag

private:
   ROMEGraph(const ROMEGraph &graph); // not implemented
   ROMEGraph &operator=(const ROMEGraph &rhs); // not implemented

public:
   ROMEGraph();
   virtual ~ROMEGraph() {};

   void        SetActive(bool value) { fActive = value; };
   void        SetWrite(bool value) { fWrite = value; };
   void        SetTitle(const char* value) { fTitle = value; };
   void        SetFolderTitle(const char* value) { fFolderTitle = value; };
   void        SetArraySize(const char* value) { fArraySize = value; };
   void        SetArrayStartIndex(const char* value) { fArrayStartIndex = value; };
   void        SetXLabel(const char* value) { fXLabel = value; };
   void        SetYLabel(const char* value) { fYLabel = value; };
   void        SetZLabel(const char* value) { fZLabel = value; };
   void        SetXmin(const char* value) { fXmin = value; };
   void        SetXmax(const char* value) { fXmax = value; };
   void        SetYmin(const char* value) { fYmin = value; };
   void        SetYmax(const char* value) { fYmax = value; };
   void        SetZmin(const char* value) { fZmin = value; };
   void        SetZmax(const char* value) { fZmax = value; };
   void        SetOriginal(const char* title,const char* folderTitle,Int_t arraySize,Int_t arrayStartIndex,
                           const char* xLabel,const char* yLabel,const char* zLabel,Double_t xmin,Double_t xmax,
                           Double_t ymin,Double_t ymax,Double_t zmin,Double_t zmax);
   void        SetTitleOriginal(const char* value) { fTitleOrig = value; };
   void        SetFolderTitleOriginal(const char* value) { fFolderTitleOrig = value; };
   void        SetArraySizeOriginal(Int_t value) { fArraySizeOrig = value; };
   void        SetArrayStartIndexOriginal(Int_t value) { fArrayStartIndexOrig = value; };
   void        SetXLabelOriginal(const char* value) { fXLabelOrig = value; };
   void        SetYLabelOriginal(const char* value) { fYLabelOrig = value; };
   void        SetZLabelOriginal(const char* value) { fZLabelOrig = value; };
   void        SetXminOriginal(Double_t value) { fXminOrig = value; };
   void        SetXmaxOriginal(Double_t value) { fXmaxOrig = value; };
   void        SetYminOriginal(Double_t value) { fYminOrig = value; };
   void        SetYmaxOriginal(Double_t value) { fYmaxOrig = value; };
   void        SetZminOriginal(Double_t value) { fZminOrig = value; };
   void        SetZmaxOriginal(Double_t value) { fZmaxOrig = value; };
   bool        IsActive() const { return fActive; }
   bool        IsWrite() const { return fWrite; }
   const char *GetTitle() const;
   const char *GetFolderTitle() const;
   const char *GetXLabel() const;
   const char *GetYLabel() const;
   const char *GetZLabel() const;
   int         GetArraySize() const;
   const char *GetArraySizeString(ROMEString &string) const;
   int         GetArrayStartIndex() const;
   const char *GetArrayStartIndexString(ROMEString &string) const;
   double      GetXmin() const;
   const char *GetXminString(ROMEString &string) const;
   double      GetXmax() const;
   const char *GetXmaxString(ROMEString &string) const;
   double      GetYmin() const;
   const char *GetYminString(ROMEString &string) const;
   double      GetYmax() const;
   const char *GetYmaxString(ROMEString &string) const;
   double      GetZmin() const;
   const char *GetZminString(ROMEString &string) const;
   double      GetZmax() const;
   const char *GetZmaxString(ROMEString &string) const;

   Bool_t      IsAccumulate() const { return fAccumulate; };
   void        SetAccumulate(Bool_t flag) { fAccumulate = flag; };

   ClassDef(ROMEGraph,0) // ROMEGraph
};

#endif   // ROMEGraph_H
