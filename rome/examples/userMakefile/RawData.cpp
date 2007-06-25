#include "RawData.h"

ClassImp(RawData)

//______________________________________________________________________________
RawData::RawData()
:TObject()
,fDataLength(0)
,fData(0)
{}

#if defined(RAWDATA_WITH_COPY)
//______________________________________________________________________________
RawData::RawData(const RawData &c)
:TObject(c)
,fDataLength(c.fDataLength)
,fData(0)
{
   if (fDataLength) {
      fData = new char[fDataLength];
   }
   memcpy(fData, c.fData, fDataLength);
}

//______________________________________________________________________________
RawData &RawData::operator=(const RawData &c)
{
   if (this != &c) {
      if (fDataLength != c.fDataLength) {
         fDataLength = c.fDataLength;
         delete [] fData;
         if (fDataLength) {
            fData = new char[fDataLength];
         } else {
            fData = 0;
         }
      }
      if (fDataLength) {
         memcpy(fData, c.fData, fDataLength);
      }
   }
   return *this;
}
#endif

//______________________________________________________________________________
RawData::~RawData()
{
   if(fData) {delete [] fData; fData = 0;}
}

//______________________________________________________________________________
void RawData::SetDataSize(Int_t size)
{
   if (fData) {
      delete [] fData;
   }
   if (size) {
      fData = new char[size];
   } else {
      fData = 0;
   }
}
