// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEStrArray                                                           //
//                                                                      //
//  Handles character string array. 
//                                                                      //
//  $Log$
//  Revision 1.3  2004/11/11 14:07:15  schneebeli_m
//  ROMEStrArray and ROMEStr2DArray change
//
//  Revision 1.2  2004/11/11 13:17:37  schneebeli_m
//  detail
//
//  Revision 1.1  2004/11/11 12:56:43  schneebeli_m
//  Implemented XML database with new path rules
//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <ROMEStrArray.h>
#include "Riostream.h"

ROMEStrArray::ROMEStrArray(Int_t s, Int_t lowerBound)
{
   array = new TObjArray(s,lowerBound);
}
ROMEStrArray::ROMEStrArray(ROMEStrArray& strArray)
{
   array = new TObjArray(strArray.GetEntriesFast());
   for (int i=0;i<strArray.GetEntriesFast();i++) {
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
   for (int i=0;i<array->GetEntriesFast();i++) {
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
   if (array->At(0)!=NULL) {
      this->RemoveAt(0);
   }
   array->AddFirst(new TObjString(str));
}

void ROMEStrArray::AddLast(TString &str)
{
   this->AddLast(str.Data());
}
void ROMEStrArray::AddLast(const char* str)
{
   if (array->At(array->GetLast())!=NULL) {
      this->RemoveAt(array->GetLast());
   }
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
   return ((TObjString*)array->At(i))->GetString();
}
Int_t ROMEStrArray::LowerBound() const
{
   return array->LowerBound();
}
