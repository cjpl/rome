// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEStr2DArray                                                       //
//                                                                      //
//  Handles character string array of array. 
//                                                                      //
//  $Log$
//  Revision 1.4  2004/11/12 17:35:18  schneebeli_m
//  fast xml database
//
//  Revision 1.3  2004/11/11 14:07:15  schneebeli_m
//  ROMEStrArray and ROMEStr2DArray change
//
//  Revision 1.2  2004/11/11 13:17:37  schneebeli_m
//  detail
//
//  Revision 1.1  2004/11/11 12:56:50  schneebeli_m
//  Implemented XML database with new path rules
//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <ROMEStr2DArray.h>
#include "Riostream.h"

ROMEStr2DArray::ROMEStr2DArray(Int_t sizeX,Int_t sizeY)
{
   fSizeX = sizeX;
   fSizeY = sizeY;
   array = new TObjArray(fSizeX);
}

ROMEStr2DArray::~ROMEStr2DArray()
{
   this->RemoveAll();
   delete array;
}


Int_t ROMEStr2DArray::GetEntries() const
{
   return array->GetEntries();
}
Int_t ROMEStr2DArray::GetEntriesFast() const
{
   return array->GetEntriesFast();
}
Int_t ROMEStr2DArray::GetEntriesAt(Int_t idx) const
{
   if (idx<0||idx>=array->GetEntriesFast())
      return 0;
   if (array->At(idx)==NULL)
      return 0;
   return ((TObjArray*)array->At(idx))->GetEntries();
}
Int_t ROMEStr2DArray::GetEntriesFastAt(Int_t idx) const
{
   if (idx<0||idx>=array->GetEntriesFast())
      return 0;
   if (array->At(idx)==NULL)
      return 0;
   return ((TObjArray*)array->At(idx))->GetEntriesFast();
}

void ROMEStr2DArray::SetAt(TString &str, Int_t idx, Int_t idy)
{
   this->SetAt(str.Data(),idx,idy);
}
void ROMEStr2DArray::SetAt(const char* str, Int_t idx, Int_t idy)
{
   if (idx<0||idx>=array->GetEntriesFast()) {
      array->AddAtAndExpand(new TObjArray(idy+1),idx);
   }
   if (array->At(idx)==NULL) {
      array->AddAtAndExpand(new TObjArray(idy+1),idx);
   }
   TObjArray *subArray = (TObjArray*)array->At(idx);
   if (idy>=0&&idy<subArray->GetEntriesFast()) {
      if (subArray->At(idy)!=NULL) {
         this->RemoveAt(idx,idy);
      }
   }
   subArray->AddAtAndExpand(new TObjString(str),idy);
}
void ROMEStr2DArray::SetAt(ROMEStrArray *str, Int_t idx, Int_t idy)
{
   if (idx<0||idx>=array->GetEntriesFast()) {
      array->AddAtAndExpand(new TObjArray(idy+1),idx);
   }
   if (array->At(idx)==NULL) {
      array->AddAtAndExpand(new TObjArray(idy+1),idx);
   }
   TObjArray *subArray = (TObjArray*)array->At(idx);
   for (int i=0;i<str->GetEntriesFast();i++) {
      if (str->At(i).Length()>0) {
         if (i+idy>=0&&i+idy<subArray->GetEntriesFast()) {
            if (subArray->At(i+idy)!=NULL) {
               this->RemoveAt(idx,i+idy);
            }
         }
         subArray->AddAtAndExpand(new TObjString(str->At(i)),i+idy);
      }
   }
}

void ROMEStr2DArray::RemoveAt(Int_t idx, Int_t idy)
{
   if (idx<0||idx>=array->GetEntriesFast())
      return;
   if (array->At(idx)==NULL)
      return;
   TObjArray *subArray = (TObjArray*)array->At(idx);
   if (idy>=subArray->GetEntriesFast()||idy<0)
      return;
   if (subArray->At(idy)==NULL)
      return;
   delete subArray->At(idy);
   subArray->RemoveAt(idy);
}
void ROMEStr2DArray::RemoveAllAt(Int_t idx)
{
   if (idx<0||idx>=array->GetEntriesFast())
      return;
   if (array->At(idx)==NULL)
      return;
   TObjArray *subArray = (TObjArray*)array->At(idx);
   for (int i=0;i<subArray->GetEntriesFast();i++) {
      delete subArray->At(i);
   }
   subArray->RemoveAll();
   delete array->At(idx);
   array->RemoveAt(idx);
}
void ROMEStr2DArray::RemoveAll()
{
   for (int i=0;i<array->GetEntriesFast();i++) {
      TObjArray *subArray = (TObjArray*)array->At(i);
      if (subArray==NULL)
         continue;
      for (int j=0;j<subArray->GetEntriesFast();j++) {
         delete subArray->At(j);
      }
      subArray->RemoveAll();
      delete subArray;
   }
   array->RemoveAll();
}

TString ROMEStr2DArray::At(Int_t idx, Int_t idy) const
{
   if (idx<0||idx>=array->GetEntriesFast())
      return "";
   if (array->At(idx)==NULL)
      return "";
   TObjArray *subArray = (TObjArray*)array->At(idx);
   if (idy<0||idy>=subArray->GetEntriesFast())
      return "";
   if (subArray->At(idy)==NULL)
      return "";
   return ((TObjString*)subArray->At(idy))->GetString();
}
