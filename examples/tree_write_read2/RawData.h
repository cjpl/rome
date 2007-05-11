#ifndef RawData_H
#define RawData_H

#include "RConfig.h"
#include "TObject.h"

#define RAWDATA_WITH_COPY

class RawData : public TObject
{
protected:
   Int_t           fDataLength; // size of data
   char           *fData;       //[fDataLength] data

public:
   RawData();
#if defined(RAWDATA_WITH_COPY)
   RawData(const RawData &c);
   RawData &operator=(const RawData &c);
#endif
   virtual ~RawData();

   Int_t    GetDataLength() { return fDataLength; }
   char    *GetData() { return fData; }

   void    SetDataLength(Int_t length) { fDataLength = length; }
   void    SetData(char *data) { fData = data; };
   void    SetDataCopy(const char *data, Int_t n) { memcpy(fData, data, n); }
   void    SetDataSize(Int_t size);

   ClassDef(RawData,1) // raw data
};

#endif   // RawData_H
