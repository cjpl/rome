#ifndef ROMENameObject_H
#define ROMENameObject_H

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TObject.h>

class ROMENameObject  : public TObject
{
private:
   TObjArray* fNames; //-> names
public:
   ROMENameObject() { fNames = new TObjArray(0); }

   char* GetNameAt(int i) { return (char*)((TObjString*)fNames->At(i))->GetString().Data(); }
   int   GetSize()        { return fNames->GetEntriesFast(); }

   void  AddName(char* name) { fNames->AddLast(new TObjString(name)); }

   void  SetNameAt(int i,char* name) { fNames->AddAtAndExpand(new TObjString(name),i); }

   ClassDef(ROMENameObject,1)
};

#endif   // ROMENameObject_H
