// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEString                                                           //
//                                                                      //
//  Handles character strings. 
//  Derived from TString.
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdarg.h>
#include <TString.h>
#include <TMath.h>
#include <ROMEString.h>
#include "Riostream.h"

bool ROMEString::AppendFormated(char* format,...) 
{
   bool res;
   va_list ap;
   va_start(ap,format);
   res = FormateString(this,format,ap);
   va_end(ap);
   return res;
}
bool ROMEString::InsertFormated(int position,char* format,...) 
{
   bool res;
   va_list ap;
   va_start(ap,format);
   ROMEString tmp;
   res = FormateString(&tmp,format,ap);
   va_end(ap);
   if (res)
      this->Insert(position,tmp);
   return res;
}
bool ROMEString::SetFormated(char* format,...) 
{
   bool res;
   va_list ap;
   va_start(ap,format);
   this->Resize(0);
   res = FormateString(this,format,ap);
   va_end(ap);
   return res;
}
bool ROMEString::FormateString(ROMEString* string,char* format,va_list parameters) 
{
   char* cstop;
   char* tmp;
   char* pstart;
   char* pactual;
   char* form;
   char* pp;
   bool    asterisk;
   int     additionalDigits;
   int     numberOfDigits;
   int     precision;
   char*   stringValue;
   int     integerValue;
   double  doubleValue;
   int ind=0;
   pactual = format;
   pstart = strstr(pactual,"%");
   while (pstart!=NULL) {
      if (strlen(pstart)==1)
         return false;
      string->Append(pactual,pstart-pactual);
      if (pstart[1]=='%') {
         string->Append("%");
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
         case 's' : 
            if (asterisk)
               numberOfDigits = va_arg(parameters,int);
            else
               numberOfDigits = TMath::Max(numberOfDigits,precision);
            stringValue = va_arg(parameters,char*);
            if (stringValue==NULL) 
               return false;
            tmp = new char[TMath::Max(numberOfDigits,(int)strlen(stringValue))+1+additionalDigits];
            if (asterisk)
               sprintf(tmp,form,numberOfDigits,stringValue);
            else
               sprintf(tmp,form,stringValue);
            string->Append(tmp);
            delete [] tmp;
            break;
         case 'd' :
            if (asterisk)
               numberOfDigits = va_arg(parameters,int);
            else
               numberOfDigits = TMath::Max(numberOfDigits,precision);
            integerValue = va_arg(parameters,int);
            tmp = new char[TMath::Max(numberOfDigits,TMath::Abs((int)TMath::Log10(integerValue))+1)+1+additionalDigits];
            if (asterisk)
               sprintf(tmp,form,numberOfDigits,integerValue);
            else
               sprintf(tmp,form,integerValue);
            string->Append(tmp);
            delete [] tmp;
            break;
         case 'f' : 
            if (precision==-1)
               precision = 6;
            if (asterisk)
               numberOfDigits = va_arg(parameters,int);
            doubleValue = va_arg(parameters,double);
            tmp = new char[TMath::Max(numberOfDigits,TMath::Abs((int)TMath::Log10(doubleValue))+1+precision+1)+1+additionalDigits];
            if (asterisk)
               sprintf(tmp,form,numberOfDigits,doubleValue);
            else
               sprintf(tmp,form,doubleValue);
            string->Append(tmp);
            delete [] tmp;
            break;
            break;
         default : 
            return false;
            break;
      }
      pactual = pstart+ind+1;
      pstart = strstr(pactual,"%");
   }
   string->Append(pactual);
   return true;
}
void ROMEString::Write() {
   cout << this->Data();
}

void ROMEString::WriteLine() {
   cout << this->Data() << endl;
}

int ROMEString::NumberOfOccurrence(ROMEString& subString) 
{
   return this->NumberOfOccurrence((char*)subString.Data());
}

int ROMEString::NumberOfOccurrence(char* subString) 
{
   int numberOfOccurrence = 0;
   const char* str = this->Data();
   str = strstr(str,subString);
   cout << str << endl;
   while (str!=NULL) {
      str++;
      numberOfOccurrence++;
      str = strstr(str,subString);
   }
   return numberOfOccurrence;
}

int ROMEString::SearchFormatType(char* str)
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
