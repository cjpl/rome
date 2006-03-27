// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEStrArray                                                           //
//                                                                      //
//  Handles character string array.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <Riostream.h>
#include "ROMEString.h"
#include "ROMEStrArray.h"

ROMEStrArray::ROMEStrArray(Int_t s, Int_t lowerBound)
{
   array = new TObjArray(s,lowerBound);
}

ROMEStrArray::ROMEStrArray(ROMEStrArray& strArray)
{
   const Int_t nStr = strArray.GetEntriesFast();
   array = new TObjArray(nStr);
   for (int i=0;i<nStr;i++) {
      this->AddAt(strArray[i],i);
   }
}

ROMEStrArray::~ROMEStrArray()
{
   this->RemoveAll();
   delete array;
}

void ROMEStrArray::Delete(Option_t *option)
{
   const Int_t nStr = array->GetEntriesFast(); 
   for (int i=0;i<nStr;i++) {
      delete array->At(i);
   }
   array->RemoveAll();
}

Int_t ROMEStrArray::GetEntries() const
{
   return array->GetEntries();
}

Int_t ROMEStrArray::GetEntriesFast() const
{
   return array->GetEntriesFast();
}

Int_t ROMEStrArray::GetLast() const
{
   return array->GetLast();
}

Bool_t ROMEStrArray::IsEmpty() const
{
   return array->IsEmpty();
}

void ROMEStrArray::AddFormatted(const char* va_(fmt),...)
{
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   this->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

void ROMEStrArray::Add(TString &str)
{
   this->AddLast(str);
}

void ROMEStrArray::Add(const char* str)
{
   this->AddLast(str);
}

void ROMEStrArray::AddFirst(TString &str)
{
   this->AddFirst(str.Data());
}

void ROMEStrArray::AddFirst(const char* str)
{
   if (array->GetEntriesFast()>0) {
      if (array->At(0)!=NULL) {
         this->RemoveAt(0);
      }
   }
   array->AddFirst(new TObjString(str));
}

void ROMEStrArray::AddLast(TString &str)
{
   this->AddLast(str.Data());
}

void ROMEStrArray::AddLast(const char* str)
{
   array->AddLast(new TObjString(str));
}

void ROMEStrArray::AddAt(TString &str, Int_t idx)
{
   this->AddAt(str.Data(),idx);
}

void ROMEStrArray::AddAt(const char* str, Int_t idx)
{
   if (idx>=0&&idx<array->GetEntriesFast()) {
      if (array->At(idx)!=NULL) {
         this->RemoveAt(idx);
      }
   }
   array->AddAt(new TObjString(str),idx);
}

void ROMEStrArray::AddAtAndExpand(TString &str, Int_t idx)
{
   this->AddAtAndExpand(str.Data(),idx);
}

void ROMEStrArray::AddAtAndExpand(const char* str, Int_t idx)
{
   if (idx>=0&&idx<array->GetEntriesFast()) {
      if (array->At(idx)!=NULL) {
         this->RemoveAt(idx);
      }
   }
   array->AddAtAndExpand(new TObjString(str),idx);
}

Int_t ROMEStrArray::AddAtFree(TString &str)
{
   return array->AddAtFree(new TObjString(str));
}

Int_t ROMEStrArray::AddAtFree(const char* str)
{
   return array->AddAtFree(new TObjString(str));
}

void ROMEStrArray::RemoveAt(Int_t idx)
{
   delete array->At(idx);
   array->RemoveAt(idx);
}

void ROMEStrArray::RemoveAll()
{
   this->Delete();
}

TString ROMEStrArray::At(Int_t idx) const
{
   return ((TObjString*)array->At(idx))->GetString();
}

TString ROMEStrArray::UncheckedAt(Int_t i) const
{
   return ((TObjString*)array->UncheckedAt(i))->GetString();
}

TString ROMEStrArray::First() const
{
   return ((TObjString*)array->First())->GetString();
}

TString ROMEStrArray::Last() const
{
   return ((TObjString*)array->Last())->GetString();
}

TString ROMEStrArray::operator[](Int_t i) const
{
   TObjString* objstr = (TObjString*)(array->At(i));
   return objstr->GetString();
}

Int_t ROMEStrArray::LowerBound() const
{
   return array->LowerBound();
}

void ROMEStrArray::Expand(Int_t newSize) {
   array->Expand(newSize);
}
