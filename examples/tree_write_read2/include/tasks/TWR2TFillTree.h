#ifndef TWR2TFillTree_H
#define TWR2TFillTree_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TWR2TFillTree                                                              //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "generated/TWR2TFillTree_Base.h"
#include "RawData.h"

const Int_t kNRawData = 10;

class TWR2TFillTree : public TWR2TFillTree_Base
{
protected:
   RawData **fRawData;

private:
   TWR2TFillTree(const TWR2TFillTree &c); // not implemented
   TWR2TFillTree &operator=(const TWR2TFillTree &c); // not implemented

public:
   TWR2TFillTree(const char *name = 0,const char *title = 0,int level = 0,const char *histoSuffix = 0,TFolder *histoFolder = 0)
   :TWR2TFillTree_Base(name,title,level,histoSuffix,histoFolder)
   ,fRawData(fRawData = new RawData*[kNRawData])
   {
      Int_t i;
      for(i = 0; i < kNRawData; i++) {
         fRawData[i] = new RawData();
      }
   }

   virtual ~TWR2TFillTree()
   {
      Int_t i;
      for(i = 0; i < kNRawData; i++) {
         SafeDelete(fRawData[i]);
      }
      SafeDeleteArray(fRawData);
   }

protected:
   // Event Methods
   virtual void Init();
   virtual void BeginOfRun();
   virtual void Event();
   virtual void EndOfRun();
   virtual void Terminate();


   ClassDef(TWR2TFillTree,0)
};

#endif   // TWR2TFillTree_H
