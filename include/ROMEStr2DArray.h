/********************************************************************
  ROMEStr2DArray.h, M. Schneebeli PSI

  $Log$
  Revision 1.2  2004/11/11 13:17:37  schneebeli_m
  detail

  Revision 1.1  2004/11/11 12:57:58  schneebeli_m
  Implemented XML database with new path rules


********************************************************************/
#ifndef ROMEStr2DArray_H
#define ROMEStr2DArray_H

#include<TObjArray.h>
#include<TObjString.h>

class ROMEStr2DArray : public TObject
{
protected:
   TObjArray* array;   // Array containing the ROMEStrArrays
   Int_t      fSizeX;  // Initial array size
   Int_t      fSizeY;  // Initial sub array size
public:
   ROMEStr2DArray(Int_t sizeX = TCollection::kInitCapacity, Int_t sizeY = TCollection::kInitCapacity);
   virtual          ~ROMEStr2DArray();
   Int_t            GetEntries() const;
   Int_t            GetEntriesFast() const;  //only OK when no gaps
   Int_t            GetEntriesAt(Int_t idx) const;
   Int_t            GetEntriesFastAt(Int_t idx) const;  //only OK when no gaps

   virtual void     SetAt(TString &str, Int_t idx, Int_t idy);
   virtual void     SetAt(TSubString &str, Int_t idx, Int_t idy);
   virtual void     SetAt(const char* str, Int_t idx, Int_t idy);
   virtual void     RemoveAt(Int_t idx, Int_t idy);
   virtual void     RemoveAllAt(Int_t idx);
   virtual void     RemoveAll();

   TString          At(Int_t idx, Int_t idy) const;
};

#endif   // ROMEStr2DArray_H
