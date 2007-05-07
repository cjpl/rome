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
#include "ROMEiostream.h"
#include <TObjString.h>
#include "ROMEString.h"
#include "ROMEStrArray.h"

ClassImp(ROMEStrArray)

//______________________________________________________________________________
ROMEStrArray::ROMEStrArray(Int_t s, Int_t lowerBound)
:TObject()
,array(new TObjArray(s,lowerBound))
{
}

//______________________________________________________________________________
ROMEStrArray::ROMEStrArray(ROMEStrArray& strArray)
:TObject(strArray)
,array(new TObjArray(strArray.GetEntriesFast()))
{
   const Int_t nStr = strArray.GetEntriesFast();
  
   for (int i=0;i<nStr;i++) {
      this->AddAt(strArray[i],i);
   }
}

//______________________________________________________________________________
ROMEStrArray::~ROMEStrArray()
{
   this->RemoveAll();
   SafeDelete(array);
}

//______________________________________________________________________________
void ROMEStrArray::Delete(Option_t * /*option*/)
{
   const Int_t nStr = array->GetEntriesFast();
   for (int i=0;i<nStr;i++) {
      delete array->At(i);
   }
   array->RemoveAll();
}

//______________________________________________________________________________
Int_t ROMEStrArray::GetEntries() const
{
   return array->GetEntries();
}

//______________________________________________________________________________
Int_t ROMEStrArray::GetEntriesFast() const
{
   return array->GetEntriesFast();
}

//______________________________________________________________________________
Int_t ROMEStrArray::GetLast() const
{
   return array->GetLast();
}

//______________________________________________________________________________
Bool_t ROMEStrArray::IsEmpty() const
{
   return array->IsEmpty();
}

//______________________________________________________________________________
void ROMEStrArray::AddFormatted(const char* va_(fmt),...)
{
   if (va_(fmt)==NULL) {
      return;
   }
   va_list ap;
   va_start(ap,va_(fmt));
   this->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

//______________________________________________________________________________
void ROMEStrArray::Add(TString &str)
{
   this->AddLast(str);
}

//______________________________________________________________________________
void ROMEStrArray::Add(const char* str)
{
   this->AddLast(str);
}

//______________________________________________________________________________
void ROMEStrArray::AddFirst(TString &str)
{
   this->AddFirst(str.Data());
}

//______________________________________________________________________________
void ROMEStrArray::AddFirst(const char* str)
{
   if (array->GetEntriesFast()>0) {
      if (array->At(0)!=NULL) {
         this->RemoveAt(0);
      }
   }
   array->AddFirst(new TObjString(str));
}

//______________________________________________________________________________
void ROMEStrArray::AddLast(TString &str)
{
   this->AddLast(str.Data());
}

//______________________________________________________________________________
void ROMEStrArray::AddLast(const char* str)
{
   array->AddLast(new TObjString(str));
}

//______________________________________________________________________________
void ROMEStrArray::AddAt(TString &str, Int_t idx)
{
   this->AddAt(str.Data(),idx);
}

//______________________________________________________________________________
void ROMEStrArray::AddAt(const char* str, Int_t idx)
{
   if (idx>=0&&idx<array->GetEntriesFast()) {
      if (array->At(idx)!=NULL) {
         this->RemoveAt(idx);
      }
   }
   array->AddAt(new TObjString(str),idx);
}

//______________________________________________________________________________
void ROMEStrArray::AddAtAndExpand(TString &str, Int_t idx)
{
   this->AddAtAndExpand(str.Data(),idx);
}

//______________________________________________________________________________
void ROMEStrArray::AddAtAndExpand(const char* str, Int_t idx)
{
   if (idx>=0&&idx<array->GetEntriesFast()) {
      if (array->At(idx)!=NULL) {
         this->RemoveAt(idx);
      }
   }
   array->AddAtAndExpand(new TObjString(str),idx);
}

//______________________________________________________________________________
Int_t ROMEStrArray::AddAtFree(TString &str)
{
   return array->AddAtFree(new TObjString(str));
}

//______________________________________________________________________________
Int_t ROMEStrArray::AddAtFree(const char* str)
{
   return array->AddAtFree(new TObjString(str));
}

//______________________________________________________________________________
void ROMEStrArray::RemoveAt(Int_t idx)
{
   delete array->At(idx);
   array->RemoveAt(idx);
}

//______________________________________________________________________________
void ROMEStrArray::RemoveAll()
{
   this->Delete();
}

//______________________________________________________________________________
TString ROMEStrArray::At(Int_t idx) const
{
   return ((TObjString*)array->At(idx))->GetString();
}

//______________________________________________________________________________
TString ROMEStrArray::UncheckedAt(Int_t i) const
{
   return ((TObjString*)array->UncheckedAt(i))->GetString();
}

//______________________________________________________________________________
TString ROMEStrArray::First() const
{
   return ((TObjString*)array->First())->GetString();
}

//______________________________________________________________________________
TString ROMEStrArray::Last() const
{
   return ((TObjString*)array->Last())->GetString();
}

//______________________________________________________________________________
TString ROMEStrArray::operator[](Int_t i) const
{
   TObjString* objstr = (TObjString*)(array->At(i));
   return objstr->GetString();
}

//______________________________________________________________________________
Int_t ROMEStrArray::LowerBound() const
{
   return array->LowerBound();
}

//______________________________________________________________________________
void ROMEStrArray::Expand(Int_t newSize) {
   array->Expand(newSize);
}

//______________________________________________________________________________
Int_t ROMEStrArray::IndexOf(const char *text) const
{
   // obj != 0 Return index of object in array.
   //          Returns -1 in case array doesn't contain the obj.
   //
   // obj == 0 return -1.
   if (!text) {
      return -1;
   }

   Int_t i;
   for (i = 0; i < array->GetEntriesFast(); i++) {
      if (static_cast<TObjString*>(array->At(i))->GetString() == text) {
         return i;
      }
   }

   return -1;
}
