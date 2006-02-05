// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEString                                                           //
//                                                                      //
//  Handles character strings.
//  Derived from TString.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <TString.h>
#include <TMath.h>
#include <ROMEString.h>
#include "Riostream.h"

ROMEString& ROMEString::AppendFormatted(const char* format,...)
{
   if (format==NULL)
      return *this;
   bool res;
   va_list ap;
   va_start(ap,format);
   res = FormatString(this,format,ap);
   va_end(ap);
   return *this;
}

ROMEString& ROMEString::InsertFormatted(Ssiz_t position,const char* format,...)
{
   if (format==NULL)
      return *this;
   bool res;
   va_list ap;
   va_start(ap,format);
   ROMEString tmp;
   res = FormatString(&tmp,format,ap);
   va_end(ap);
   if (res)
      this->Insert(position,tmp);
   return *this;
}

ROMEString& ROMEString::SetFormatted(const char* format,...)
{
   if (format==NULL)
      return *this;
   bool res;
   va_list ap;
   va_start(ap,format);
   this->Resize(0);
   res = FormatString(this,format,ap);
   va_end(ap);
   return *this;
}

Bool_t ROMEString::FormatString(ROMEString* str,const char* format,va_list parameters)
{
   char* cstop;
   char* tmp;
   const char* pstart;
   const char* pactual;
   char* form;
   char* pp;
   bool    asterisk;
   int     additionalDigits;
   int     numberOfDigits=0;
   int     precision;
   char*   stringValue;
   char    characterValue;
   int     integerValue;
   double  doubleValue;
   int ind=0;
   pactual = format;
   pstart = strstr(pactual,"%");
   while (pstart!=NULL) {
      if (strlen(pstart)==1)
         return false;
      str->Append(pactual,pstart-pactual);
      if (pstart[1]=='%') {
         str->Append("%");
         pactual = pstart+2;
         pstart = strstr(pactual,"%");
         continue;
      }
      ind = SearchFormatType(pstart);
      if (ind==-1)
         return false;
      form = new char[ind+2];
      strncpy(form,pstart,ind+1);
      form[ind+1] = 0;
      pp = form;
      asterisk = false;
      additionalDigits = 0;
      for (;;) {
         pp++;
         if (pp[0]=='-'||pp[0]=='0'||pp[0]==' ') {
            continue;
         }
         if (pp[0]=='+'||pp[0]=='#') {
            additionalDigits++;
            continue;
         }
         break;
      }
      if (pp[0]=='*') {
         asterisk = true;
         pp++;
      }
      else {
         numberOfDigits = strtol(pp,&cstop,10);
         pp = cstop;
      }
      precision = -1;
      if (pp[0]=='.') {
         precision = strtol(pp+1,&cstop,10);
      }
      switch (pstart[ind]) {
         case 'c' :
            characterValue = va_arg(parameters,int);
            str->Append(characterValue);
            break;
         case 's' :
            if (asterisk)
               numberOfDigits = va_arg(parameters,int);
            else
               numberOfDigits = TMath::Max(numberOfDigits,precision);
            stringValue = va_arg(parameters,char*);
            if (stringValue==NULL)
               return false;
            tmp = new char[TMath::Max(numberOfDigits,(int)strlen(stringValue))+2+additionalDigits];
            if (asterisk)
               sprintf(tmp,form,numberOfDigits,stringValue);
            else
               sprintf(tmp,form,stringValue);
            str->Append(tmp);
            delete [] tmp;
            break;
         case 'd' :
         case 'i' :
         case 'x' :
            if (asterisk)
               numberOfDigits = va_arg(parameters,int);
            else
               numberOfDigits = TMath::Max(numberOfDigits,precision);
            integerValue = va_arg(parameters,int);
            tmp = new char[TMath::Max(numberOfDigits,(Int_t)TMath::Log10(TMath::Abs(integerValue))+1)+2+additionalDigits];
            if (asterisk)
               sprintf(tmp,form,numberOfDigits,integerValue);
            else
               sprintf(tmp,form,integerValue);
            str->Append(tmp);
            delete [] tmp;
            break;
         case 'e' :
         case 'f' :
         case 'g' :
            if (precision==-1)
               precision = 6;
            if (asterisk)
               numberOfDigits = va_arg(parameters,int);
            doubleValue = va_arg(parameters,double);
            tmp = new char[TMath::Max(numberOfDigits+precision+1,(Int_t)TMath::Log10(TMath::Abs((Int_t)doubleValue))+1+precision+1)+2+additionalDigits];
            if (asterisk)
               sprintf(tmp,form,numberOfDigits,doubleValue);
            else
               sprintf(tmp,form,doubleValue);
            str->Append(tmp);
            delete [] tmp;
            break;
            break;
         default :
            delete [] form;
            return false;
            break;
      }
      pactual = pstart+ind+1;
      pstart = strstr(pactual,"%");
      delete [] form;
   }
   str->Append(pactual);
   return true;
}

void ROMEString::Write() {
   cout << this->Data();
}


void ROMEString::WriteLine() {
   cout << this->Data() << endl;
}

Int_t ROMEString::NumberOfOccurrence(ROMEString& subString)
{
   return this->NumberOfOccurrence(subString.Data());
}

Int_t ROMEString::NumberOfOccurrence(const char* subString)
{
   Int_t numberOfOccurrence = 0;
   const char* str = this->Data();
   str = strstr(str,subString);
   while (str!=NULL) {
      str++;
      numberOfOccurrence++;
      str = strstr(str,subString);
   }
   return numberOfOccurrence;
}

Int_t ROMEString::SearchFormatType(const char* str)
{
   const char numberOfTypes = 17;
   int i,j;
   char c[numberOfTypes] = {'c','C','d','i','o','u','x','X','e','E','f','g','G','n','p','s','S'};
   for (i=0;i<(int)strlen(str);i++) {
      for (j=0;j<numberOfTypes;j++) {
         if (str[i]==c[j]) return i;;
      }
   }
   return -1;
}

istream& ROMEString::ReadFile(istream& str) {
   this->Resize(0);
   char *buffer = "";
   int bufferSize = 2000;
   int bufferLength = bufferSize-1;
   while (bufferLength==bufferSize-1) {
      bufferSize*=2;
      buffer = new char[bufferSize];
      str.get(buffer,bufferSize,0);
      bufferLength = strlen(buffer);
      this->Append(buffer);
      delete buffer;
   }
   return str;
}

istream& ROMEString::ReadLine(istream& str) {
   this->Resize(0);
   char *buffer = "";
   int bufferSize = 2000;
   int bufferLength = bufferSize-1;
   while (bufferLength==bufferSize-1) {
      bufferSize*=2;
      buffer = new char[bufferSize];
      str.getline(buffer,bufferSize);
      bufferLength = strlen(buffer);
      this->Append(buffer);
      delete buffer;
   }
   return str;
}

Int_t ROMEString::ToInteger()
{
   char *cstop;
   return strtol(this->Data(),&cstop,10);
}

Double_t ROMEString::ToDouble()
{
   char *cstop;
   return strtod(this->Data(),&cstop);
}

void ROMEString::ToLower() {
   ((TString*)this)->ToLower();
}

const char* ROMEString::ToLower(ROMEString& destination)
{
   destination = this->Data();
   destination.ToLower();
   return destination.Data();
}

void ROMEString::ToUpper() {
   ((TString*)this)->ToUpper();
}

const char* ROMEString::ToUpper(ROMEString& destination)
{
   destination = this->Data();
   destination.ToUpper();
   return destination.Data();
}

// Strip space,tab and new line at both sides
ROMEString& ROMEString::StripSpaces(){
   Ssiz_t start = 0;             // Index of first character
   Ssiz_t end = Length();        // One beyond last character
   const char *direct = Data();  // Avoid a dereference w dumb compiler

   while (start < end && direct[start] <= ' ')
      ++start;

   while (start < end && direct[end] <= ' ')
      --end;

   this->Remove(end+1,Length()-end-1);
   this->Remove(0,start);
   return *this;
}
