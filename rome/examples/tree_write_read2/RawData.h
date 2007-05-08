#ifndef RawData_H
#define RawData_H

#include "RConfig.h"
#include "TObject.h"

class RawData : public TObject
{
protected:
   Int_t           fDataSize;  // size of data
   char           *fData;      //[fDataSize] data

public:
#if defined(R__UNIX)
   RawData() {fData = 0;};
   virtual ~RawData() { if(fData) {delete [] fData; fData = 0;} };
#else
   RawData() {};
   virtual ~RawData() {};
#endif

   Int_t    GetDataSize() { return fDataSize; }
   char    *GetData() { return fData; }

   void    SetDataSize(Int_t datasize) { fDataSize = datasize; }
   void    SetData(char *data) { fData = data; };
   ClassDef(RawData,1) // raw data
};

#endif   // RawData_H
