// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEString                                                           //
//                                                                      //
//  Handles character strings. 
//  Derived from TString.
//                                                                      //
//  $Log$
//  Revision 1.8  2005/03/13 08:43:14  sawada
//  removed or comment out unused variables.
//  made virtual destructor of ROMEConfig and ROMEDataBase.
//
//  Revision 1.7  2004/11/16 16:14:01  schneebeli_m
//  implemented task hierarchy
//
//  Revision 1.6  2004/09/30 13:08:21  schneebeli_m
//  ...
//
//  Revision 1.5  2004/09/25 01:34:48  schneebeli_m
//  implemented FW dependent EventLoop and DataBase classes
//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <TString.h>
#include <TMath.h>
#include <ROMEString.h>
#include "Riostream.h"

bool ROMEString::AppendFormatted(char* format,...) 
{
   bool res;
   va_list ap;
   va_start(ap,format);
   res = FormatString(this,format,ap);
   va_end(ap);
   return res;
}
bool ROMEString::InsertFormatted(int position,char* format,...) 
{
   bool res;
   va_list ap;
   va_start(ap,format);
   ROMEString tmp;
   res = FormatString(&tmp,format,ap);
   va_end(ap);
   if (res)
      this->Insert(position,tmp);
   return res;
}
bool ROMEString::SetFormatted(char* format,...) 
{
   bool res;
   va_list ap;
   va_start(ap,format);
   this->Resize(0);
   res = FormatString(this,format,ap);
   va_end(ap);
   return res;
}
bool ROMEString::FormatString(ROMEString* string,char* format,va_list parameters) 
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
            tmp = new char[TMath::Max(numberOfDigits,(int)strlen(stringValue))+2+additionalDigits];
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
            tmp = new char[TMath::Max(numberOfDigits,(Int_t)TMath::Log10(TMath::Abs(integerValue))+1)+2+additionalDigits];
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
            tmp = new char[TMath::Max(numberOfDigits+precision+1,(Int_t)TMath::Log10(TMath::Abs((Int_t)doubleValue))+1+precision+1)+2+additionalDigits];
            if (asterisk)
               sprintf(tmp,form,numberOfDigits,doubleValue);
            else
               sprintf(tmp,form,doubleValue);
            string->Append(tmp);
            delete [] tmp;
            break;
            break;
         default : 
            delete form;
            return false;
            break;
      }
      pactual = pstart+ind+1;
      pstart = strstr(pactual,"%");
      delete form;
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
