/********************************************************************
  ROMEStrArray.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2004/11/11 12:57:49  schneebeli_m
  Implemented XML database with new path rules


********************************************************************/
#ifndef ROMEStrArray_H
#define ROMEStrArray_H

#include<TObjArray.h>
#include<TObjString.h>

class ROMEStrArray : public TObject
{
protected:
   TObjArray* array;  // The array containing the TStrings
public:
   ROMEStrArray(Int_t s = TCollection::kInitCapacity, Int_t lowerBound = 0);
   ROMEStrArray(ROMEStrArray& strArray);
   virtual          ~ROMEStrArray();
   virtual void     Delete(Option_t *option="");
   Int_t            GetEntries() const;
   Int_t            GetEntriesFast() const;  //only OK when no gaps
   Int_t            GetLast() const;
   Bool_t           IsEmpty() const;

   void             Add(TString &str);
   void             Add(TSubString &str);
   void             Add(const char* str);
   virtual void     AddFirst(TString &str);
   virtual void     AddFirst(TSubString &strr);
   virtual void     AddFirst(const char* str);
   virtual void     AddLast(TString &str);
   virtual void     AddLast(TSubString &str);
   virtual void     AddLast(const char* str);
   virtual void     AddAt(TString &str, Int_t idx);
   virtual void     AddAt(TSubString &str, Int_t idx);
   virtual void     AddAt(const char* str, Int_t idx);
   virtual void     AddAtAndExpand(TString &str, Int_t idx);
   virtual void     AddAtAndExpand(TSubString &str, Int_t idx);
   virtual void     AddAtAndExpand(const char* str, Int_t idx);
   virtual Int_t    AddAtFree(TString &str);
   virtual Int_t    AddAtFree(TSubString &str);
   virtual Int_t    AddAtFree(const char* str);
   virtual void     RemoveAt(Int_t idx);
   virtual void     RemoveAll();

   TString         *At(Int_t idx) const;
   TString         *UncheckedAt(Int_t i) const;
   TString         *First() const;
   TString         *Last() const;
   virtual TString  operator[](Int_t i) const;
   Int_t            LowerBound() const;

};

#endif   // ROMEStrArray_H
