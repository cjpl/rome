/********************************************************************
  ROMEStr2DArray.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEStr2DArray_H
#define ROMEStr2DArray_H

#include <TObjArray.h>
#include <TString.h>

class ROMEStrArray;

class ROMEStr2DArray : public TObject
{
protected:
   TObjArray *array;   // Array containing the ROMEStrArrays
   Int_t      fSizeX;  // Initial array size
   Int_t      fSizeY;  // Initial sub array size

private:
   ROMEStr2DArray(const ROMEStr2DArray &str2darray); // not implemented
   ROMEStr2DArray &operator=(const ROMEStr2DArray &rhs); // not implemented

public:
   ROMEStr2DArray(Int_t sizeX = TCollection::kInitCapacity, Int_t sizeY = TCollection::kInitCapacity);
   virtual          ~ROMEStr2DArray();

   Int_t            GetEntries() const;
   Int_t            GetEntriesFast() const;  //only OK when no gaps
   Int_t            GetEntriesAt(Int_t idx) const;
   Int_t            GetEntriesFastAt(Int_t idx) const;  //only OK when no gaps

   virtual void     SetAt(TString &str, Int_t idx, Int_t idy);
   virtual void     SetAt(const char* str, Int_t idx, Int_t idy);
   virtual void     SetAt(ROMEStrArray *str, Int_t idx, Int_t idy);
   virtual void     RemoveAt(Int_t idx, Int_t idy);
   virtual void     RemoveAllAt(Int_t idx);
   virtual void     RemoveAll();

   TString          At(Int_t idx, Int_t idy) const;
   TString&         At2(Int_t idx, Int_t idy) const;

   ClassDef(ROMEStr2DArray, 1) // Two dimensional ROMEString array
};

#endif   // ROMEStr2DArray_H
