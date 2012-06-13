/********************************************************************
  ROMEStrArray.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEStrArray_H
#define ROMEStrArray_H

#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>
#include "ROME.h"

class ROMEStrArray : public TObject
{
protected:
   TObjArray *array;  // The array containing the TStrings

private:
   ROMEStrArray &operator=(const ROMEStrArray &rhs); // not implemented

public:
   ROMEStrArray(Int_t s = TCollection::kInitCapacity, Int_t lowerBound = 0);
   ROMEStrArray(ROMEStrArray& strArray);
   virtual          ~ROMEStrArray();

   virtual void     Delete(Option_t *option="");
   Int_t            GetEntries() const { return array->GetEntries(); }
   Int_t            GetEntriesFast() const { return array->GetEntriesFast(); } //only OK when no gaps
   Int_t            GetLast() const { return array->GetLast(); }
   Bool_t           IsEmpty() const { return array->IsEmpty(); }

   void             AddFormatted(const char* format,...) G_GNUC_PRINTF(2, 3);
   void             Add(TString &str) { AddLast(str); }
   void             Add(const char* str) { AddLast(str); }
   virtual void     AddFirst(TString &str);
   virtual void     AddFirst(const char* str);
   virtual void     AddLast(TString &str);
   virtual void     AddLast(const char* str);
   virtual void     AddAt(TString &str, Int_t idx);
   virtual void     AddAt(const char* str, Int_t idx);
   virtual void     AddAtAndExpand(TString &str, Int_t idx);
   virtual void     AddAtAndExpand(const char* str, Int_t idx);
   virtual Int_t    AddAtFree(TString &str);
   virtual Int_t    AddAtFree(const char* str);
   virtual void     RemoveAt(Int_t idx);
   virtual void     RemoveAll();

   TString          At(Int_t idx) const { return static_cast<TObjString*>(array->At(idx))->GetString(); }
   TString&         At2(Int_t idx) const { return static_cast<TObjString*>(array->At(idx))->String(); }
   TString          UncheckedAt(Int_t i) const { return static_cast<TObjString*>(array->UncheckedAt(i))->GetString(); }
   TString          First() const { return static_cast<TObjString*>(array->First())->GetString(); }
   TString          Last() const { return static_cast<TObjString*>(array->Last())->GetString(); }
   TString&         First2() const { return static_cast<TObjString*>(array->First())->String(); }
   TString&         Last2() const { return static_cast<TObjString*>(array->Last())->String(); }
   virtual TString  operator[](Int_t i) const;
   Int_t            LowerBound() const { return array->LowerBound(); }
   virtual void     Expand(Int_t newSize);
   Int_t            IndexOf(const char *text) const;
   Int_t            IndexOf(const TString &text) const { return IndexOf(text.Data()); }

   void             Sort(Int_t *index, Bool_t down = kTRUE) const;

   ClassDef(ROMEStrArray,1) // 1 dimensional ROMEString array
};

#endif   // ROMEStrArray_H
