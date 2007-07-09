#ifndef TCUTGTFillHisto_H
#define TCUTGTFillHisto_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TCUTGTFillHisto                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "generated/TCUTGTFillHisto_Base.h"

class TCUTGTFillHisto : public TCUTGTFillHisto_Base
{

private:
   TCUTGTFillHisto(const TCUTGTFillHisto &c); // not implemented
   TCUTGTFillHisto &operator=(const TCUTGTFillHisto &c); // not implemented

public:
   TCUTGTFillHisto(const char *name = 0, const char *title = 0, int level = 0, const char *histoSuffix = 0, TFolder *histoFolder = 0)
   :TCUTGTFillHisto_Base(name,title,level,histoSuffix,histoFolder) {}
   virtual ~TCUTGTFillHisto() {}

protected:
   // Event Methods
   virtual void Init();
   virtual void BeginOfRun();
   virtual void Event();
   virtual void EndOfRun();
   virtual void Terminate();


   ClassDef(TCUTGTFillHisto,0)
};

#endif   // TCUTGTFillHisto_H
