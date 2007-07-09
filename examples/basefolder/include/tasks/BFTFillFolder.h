#ifndef BFTFillFolder_H
#define BFTFillFolder_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BFTFillFolder                                                              //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "generated/BFTFillFolder_Base.h"

class BFTFillFolder : public BFTFillFolder_Base
{

private:
   BFTFillFolder(const BFTFillFolder &c); // not implemented
   BFTFillFolder &operator=(const BFTFillFolder &c); // not implemented

public:
   BFTFillFolder(const char *name = 0, const char *title = 0, int level = 0, const char *histoSuffix = 0, TFolder *histoFolder = 0)
   :BFTFillFolder_Base(name,title,level,histoSuffix,histoFolder) {}
   virtual ~BFTFillFolder() {}

protected:
   // Event Methods
   virtual void Init();
   virtual void BeginOfRun();
   virtual void Event();
   virtual void EndOfRun();
   virtual void Terminate();


   ClassDef(BFTFillFolder,0)
};

#endif   // BFTFillFolder_H
