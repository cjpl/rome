/********************************************************************
  ROMEMidas.h, M. Schneebeli PSI

  $Log$
  Revision 1.2  2005/02/06 01:26:55  sawada
  moved byte swapping functions to ROMEMidas

  Revision 1.1  2005/01/24 15:25:06  schneebeli_m
  Seperated DAQ classes


********************************************************************/
#ifndef ROMEMidas_H
#define ROMEMidas_H

#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>

class ROMEMidas : public ROMEDAQSystem {
protected:
   int        fMidasOnlineDataBase;             //! Handle to the Midas Online Data Base
   int        fMidasOnlineBuffer;               //! Midas Online Buffer
   int        fMidasFileHandle;                 //! Handle to Midas Inputfile

   int        fTimeStamp;                       //! Current time stamp

public:
   ROMEMidas::ROMEMidas();

   int   GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
   int*  GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; };

   int   GetTimeStamp() { return fTimeStamp; };

   bool Initialize();
   bool Connect();
   bool ReadEvent(int event);
   bool Disconnect();
   bool Termination();

   virtual bool InitODB() = 0;

   //byte swapping
#if defined( __ppc__ )
#if !defined(HAVE_MIDAS)
   void bk_swap(void *event, bool force);
#endif
   void ByteSwap(UShort_t  *x);
   void ByteSwap(Short_t   *x){ByteSwap((UShort_t* )x);};
   void ByteSwap(UInt_t    *x);
   void ByteSwap(Int_t     *x){ByteSwap((UInt_t*   )x);};
   void ByteSwap(Float_t   *x){ByteSwap((UInt_t*   )x);};
   void ByteSwap(ULong64_t *x);
   void ByteSwap(Long64_t  *x){ByteSwap((ULong64_t*)x);};
   void ByteSwap(Double_t  *x){ByteSwap((ULong64_t*)x);};
#endif
};

#endif   // ROMEMidas_H
