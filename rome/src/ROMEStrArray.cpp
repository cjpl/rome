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

//______________________________________________________________________________
void ROMEStrArray::Sort(Int_t *indx, Bool_t down)
{
   // Sort strings in an instance.
   // In output the array indx contains the indices of the sorted array.
   // If down is false sort in increasing order (default is decreasing order).
   // This is a translation of the CERNLIB routine sortzv (M101)
   // based on the quicksort algorithm.
   // NOTE that the array indx must be created with a length >= n1
   // before calling this function.

   const Int_t n1 = GetEntries();
   Int_t i,i1,n,i2,i3,i33,i222,iswap,n2;
   Int_t i22 = 0;
   TString ai;
   n = n1;
   if (n <= 0) return;
   if (n == 1) {indx[0] = 0; return;}
   for (i=0;i<n;i++) indx[i] = i+1;
   for (i1=2;i1<=n;i1++) {
      i3 = i1;
      i33 = indx[i3-1];
      ai  = At(i33-1);
      while(1) {
         i2 = i3/2;
         if (i2 <= 0) break;
         i22 = indx[i2-1];
         if (strcmp(ai.Data(), At(i22-1).Data()) <= 0) break;
         indx[i3-1] = i22;
         i3 = i2;
      }
      indx[i3-1] = i33;
   }

   while(1) {
      i3 = indx[n-1];
      indx[n-1] = indx[0];
      ai = At(i3-1);
      n--;
      if(n-1 < 0) {indx[0] = i3; break;}
      i1 = 1;
      while(2) {
         i2 = i1+i1;
         if (i2 <= n) i22 = indx[i2-1];
         if (i2-n > 0) {indx[i1-1] = i3; break;}
         if (i2-n < 0) {
            i222 = indx[i2];
            if (strcmp(At(i22-1).Data(), At(i222-1).Data()) < 0) {
               i2++;
               i22 = i222;
            }
         }
         if (strcmp(ai.Data(), At(i22-1).Data()) > 0) {indx[i1-1] = i3; break;}
         indx[i1-1] = i22;
         i1 = i2;
      }
   }
   for (i=0;i<n1;i++) indx[i]--;
   if (!down) return;
   n2 = n1/2;
   for (i=0;i<n2;i++) {
      iswap        = indx[i];
      indx[i]      = indx[n1-i-1];
      indx[n1-i-1] = iswap;
   }
}
