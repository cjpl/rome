#ifndef ROMEIndexObject_H
#define ROMEIndexObject_H

#include <TArrayI.h>
#include <TObject.h>

class ROMEIndexObject  : public TObject
{
private:
   TArrayI* fIndex; //-> index
public:
   ROMEIndexObject(int size) { fIndex = new TArrayI(size); }

   int   GetIndexAt(int i) { return fIndex->At(i); }

   void  SetIndexAt(int i,int index) { fIndex->AddAt(index,i); }

   ClassDef(ROMEIndexObject,1)
};

#endif   // ROMEIndexObject_H
