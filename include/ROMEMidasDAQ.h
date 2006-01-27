/********************************************************************
  ROMEMidasDAQ.h, M. Schneebeli PSI

  $Id: ROMEMidasDAQ.h 480 2005-10-17 12:12:54Z schneebeli_m $

********************************************************************/
#ifndef ROMEMidasDAQ_H
#define ROMEMidasDAQ_H

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
   Short_t event_id;
   Short_t trigger_mask;
   UInt_t serial_number;
   UInt_t time_stamp;
   UInt_t data_size;
} EVENT_HEADER;
typedef struct {
   UInt_t data_size;
   UInt_t flags;
} BANK_HEADER;
typedef struct {
   Char_t name[4];
   UShort_t type;
   UShort_t data_size;
} BANK;
typedef struct {
   Char_t name[4];
   UInt_t type;
   UInt_t data_size;
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

class ROMEMidasDAQ : public ROMEDAQSystem {
protected:
   char       fRawDataEvent[2][0x80000];        //! Midas Inputdata Stack for the current Event and the last Event
   int        fCurrentRawDataEvent;             //! Index of the current event buffer

   int        fNumberOfEventRequests;           //! Number of Event Requests
   int        fEventRequestID[5];               //! IDs of Event Requests
   int        fEventRequestMask[5];             //! Trigger Masks of Event Requests
   int        fEventRequestRate[5];             //! Sampling Rates of Event Requests

   int        fMidasOnlineDataBase;             //! Handle to the Midas Online Data Base (Online)
   int        fMidasOnlineBuffer;               //! Midas Online Buffer
   Long64_t   fMidasFileHandle;                 //! Handle to a un-gzipped Midas Inputfile
   gzFile     fMidasGzFileHandle;               //! Handle to Midas gzipped Inputfile
   bool       fGZippedMidasFile;                //! True if input file is gzipped.
   bool       fStopRequest;                     //! True if a Stop transition message was received

   EVENT_HEADER *fOdbOffline;                    //! Handle to the Midas Online Data Base (Offline)

   int        fTimeStamp;                       //! Current time stamp

public:
   ROMEMidasDAQ::ROMEMidasDAQ();

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
   virtual void* ByteSwapStruct( char* aName, void* aData ) { return aData; };       // Must be overwritten by analyzermidas code.
#endif
#ifndef HAVE_MIDAS
   bool bk_is32(void *event);
   int bk_find(void* pbkh, const char *name, UInt_t* bklen, UInt_t* bktype,void *pdata);
#endif
};

#endif   // ROMEMidasDAQ_H
