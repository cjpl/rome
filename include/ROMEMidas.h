/********************************************************************
  ROMEMidas.h, M. Schneebeli PSI

  $Log$
  Revision 1.14  2005/06/16 02:52:54  sawada
  added virtual destructer.

  Revision 1.13  2005/06/15 07:34:14  sawada
  bug fix on IsActiveEventID.

  Revision 1.12  2005/06/14 14:43:08  sawada
  bk_swap only when the event is active.

  Revision 1.11  2005/06/13 15:49:04  schneebeli_m
  changed name of DAQ user functions

  Revision 1.10  2005/05/13 23:51:14  sawada
  code cleanup.

  Revision 1.9  2005/05/13 10:47:18  sawada
  GZipped midas files on linux.

  Revision 1.8  2005/04/15 16:44:26  schneebeli_m
  odb, zlib

  Revision 1.7  2005/04/15 14:44:40  schneebeli_m
  gzopen implemented

  Revision 1.6  2005/04/14 07:56:46  schneebeli_m
  Implemented odb database (offline)

  Revision 1.5  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.4  2005/03/21 17:29:47  schneebeli_m
  minor changes

  Revision 1.3  2005/02/21 21:29:07  sawada

  Changed OS specifying macros
  Support for DEC,Ultrix,FreeBSD,Solaris

  Revision 1.2  2005/02/06 01:26:55  sawada
  moved byte swapping functions to ROMEMidas

  Revision 1.1  2005/01/24 15:25:06  schneebeli_m
  Seperated DAQ classes


********************************************************************/
#ifndef ROMEMidas_H
#define ROMEMidas_H

#include <RConfig.h>
#include <zlib.h>
#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>
#include <ROMEODBOfflineDataBase.h>

#ifdef HAVE_MIDAS
#   include <midas.h>
#endif

#ifndef HAVE_MIDAS
#   define EVENTID_BOR      ((short) 0x8000)  /**< Begin-of-run      */
#   define EVENTID_EOR      ((short) 0x8001)  /**< End-of-run        */
#   define EVENTID_MESSAGE  ((short) 0x8002)  /**< Message events    */
typedef struct {
   short int event_id;
   short int trigger_mask;
   unsigned long int serial_number;
   unsigned long int time_stamp;
   unsigned long int data_size;
} EVENT_HEADER;
typedef struct {
   unsigned long int data_size;
   unsigned long int flags;
} BANK_HEADER;
typedef struct {
   char name[4];
   unsigned short int type;
   unsigned short int data_size;
} BANK;
typedef struct {
   char name[4];
   unsigned long int type;
   unsigned long int data_size;
} BANK32;
#   define ALIGN8(x)  (((x)+7) & ~7)
//Data types Definition                         min      max
const UShort_t TID_BYTE      = 1;       //< unsigned byte         0       255
const UShort_t TID_SBYTE     = 2;       //< signed byte         -128      127
const UShort_t TID_CHAR      = 3;       //< single character      0       255
const UShort_t TID_WORD      = 4;       //< two bytes             0      65535
const UShort_t TID_SHORT     = 5;       //< signed word        -32768    32767I
const UShort_t TID_DWORD     = 6;       //< four bytes            0      2^32-1
const UShort_t TID_INT       = 7;       //< signed dword        -2^31    2^31-1
const UShort_t TID_BOOL      = 8;       //< four bytes bool       0        1
const UShort_t TID_FLOAT     = 9;       //< 4 Byte float format
const UShort_t TID_DOUBLE    = 10;      //< 8 Byte float format
const UShort_t TID_BITFIELD  = 11;      //< 32 Bits Bitfield      0  111... (32)
const UShort_t TID_STRING    = 12;      //< zero terminated string
const UShort_t TID_ARRAY     = 13;      //< array with unknown contents
const UShort_t TID_STRUCT    = 14;      //< structure with fixed length
const UShort_t TID_KEY       = 15;      //< key in online database
const UShort_t TID_LINK      = 16;      //< link in online database
const UShort_t TID_LAST      = 17;      //< end of TID list indicator
#endif

class ROMEMidas : public ROMEDAQSystem {
protected:
   char       fRawDataEvent[2][0x80000];        //! Midas Inputdata Stack for the current Event and the last Event
   int        fCurrentRawDataEvent;             //! Index of the current event buffer

   int        fNumberOfEventRequests;           //! Number of Event Requests
   int        fEventRequestID[5];               //! IDs of Event Requests
   int        fEventRequestMask[5];             //! Trigger Masks of Event Requests
   int        fEventRequestRate[5];             //! Sampling Rates of Event Requests

   int        fMidasOnlineDataBase;             //! Handle to the Midas Online Data Base (Online)
   int        fMidasOnlineBuffer;               //! Midas Online Buffer
   int        fMidasFileHandle;                 //! Handle to a un-gzipped Midas Inputfile
   gzFile     fMidasGzFileHandle;               //! Handle to Midas gzipped Inputfile
   bool       fGZippedMidasFile;                //! True if input file is gzipped.
   bool       fStopRequest;                     //! True if a Stop transition message was received

   EVENT_HEADER *fOdbOffline;                    //! Handle to the Midas Online Data Base (Offline)

   int        fTimeStamp;                       //! Current time stamp

public:
   ROMEMidas::ROMEMidas();
   virtual ROMEMidas::~ROMEMidas() {};

   // Online Database
   int   GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
   int*  GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; };

   // Raw Data
   void*      GetRawDataEvent() { return fRawDataEvent[fCurrentRawDataEvent]; };
   void*      GetLastRawDataEvent() { return fRawDataEvent[1-fCurrentRawDataEvent]; };
   int        GetRawDataEventSize() { return sizeof(fRawDataEvent[fCurrentRawDataEvent]); };
   void       SwitchRawDataBuffer() { fCurrentRawDataEvent = 1-fCurrentRawDataEvent; };

   // Event Requests
   int        GetNumberOfEventRequests() { return fNumberOfEventRequests; };
   int        GetEventRequestID(int i) { return fEventRequestID[i]; };
   int        GetEventRequestMask(int i) { return fEventRequestMask[i]; };
   int        GetEventRequestRate(int i) { return fEventRequestRate[i]; };

   void       SetNumberOfEventRequests(int value) { fNumberOfEventRequests = value; };
   void       SetEventRequestID(int i,int value)    { fEventRequestID[i] = value; };
   void       SetEventRequestMask(int i,int value)  { fEventRequestMask[i] = value; };
   void       SetEventRequestRate(int i,int value)  { fEventRequestRate[i] = value; };


   // Additional Getters
   int   GetTimeStamp() { return fTimeStamp; };
   const char* GetName() { return "midas"; };

   bool Init();
   bool BeginOfRun();
   bool Event(int event);
   bool EndOfRun();
   bool Terminate();

   virtual bool IsActiveEventID(int id){ return true; }
   virtual bool InitODB() = 0;
   virtual bool InitHeader() = 0;
   virtual void InitMidasBanks() = 0;

   //byte swapping
#ifndef R__BYTESWAP
#   ifndef HAVE_MIDAS
   void bk_swap(void *event, bool force);
#   endif
   void ByteSwap(UShort_t  *x);
   void ByteSwap(Short_t   *x){ByteSwap((UShort_t* )x);};
   void ByteSwap(UInt_t    *x);
   void ByteSwap(Int_t     *x){ByteSwap((UInt_t*   )x);};
   void ByteSwap(Float_t   *x){ByteSwap((UInt_t*   )x);};
   void ByteSwap(ULong64_t *x);
   void ByteSwap(Long64_t  *x){ByteSwap((ULong64_t*)x);};
   void ByteSwap(Double_t  *x){ByteSwap((ULong64_t*)x);};
#endif
#ifndef HAVE_MIDAS
   bool bk_is32(void *event);
   int bk_find(void* pbkh, const char *name, unsigned long * bklen, unsigned long * bktype,void *pdata);
#endif
};

#endif   // ROMEMidas_H
