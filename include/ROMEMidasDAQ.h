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
#else
#if defined( R__VISUAL_CPLUSPLUS )
typedef unsigned long   DWORD;
typedef int    BOOL;
#else
typedef UInt_t   DWORD;
typedef DWORD    BOOL;
const BOOL TRUE = 1;
const BOOL FALSE = 0;
#endif
typedef Int_t    HNDLE;
typedef Int_t    INT;
typedef UShort_t WORD;
const Int_t CM_SUCCESS = 1;
#   define BANK_FORMAT_32BIT   (1<<4)
#   define EVENTID_BOR      ((short) 0x8000)  /**< Begin-of-run      */
#   define EVENTID_EOR      ((short) 0x8001)  /**< End-of-run        */
#   define EVENTID_MESSAGE  ((short) 0x8002)  /**< Message events    */
typedef struct {
   Short_t event_id;
   Short_t trigger_mask;
   DWORD   serial_number;
   DWORD   time_stamp;
   DWORD   data_size;
} EVENT_HEADER;
typedef struct {
   DWORD   data_size;
   DWORD   flags;
} BANK_HEADER;
typedef struct {
   char    name[4];
   WORD    type;
   WORD    data_size;
} BANK;
typedef struct {
   char    name[4];
   DWORD   type;
   DWORD   data_size;
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

const Int_t kMaxMidasEventTypes = 5;

class ROMEMidasDAQ : public ROMEDAQSystem {
protected:
   char          fRawDataEvent[2][0x80000];                //! Midas Inputdata Stack for the current Event and the last Event
   Int_t         fCurrentRawDataEvent;                     //! Index of the current event buffer

   Int_t         fNumberOfEventRequests;                   //! Number of Event Requests
   Short_t       fEventRequestID[kMaxMidasEventTypes];     //! IDs of Event Requests
   Short_t       fEventRequestMask[kMaxMidasEventTypes];   //! Trigger Masks of Event Requests
   Int_t         fEventRequestRate[kMaxMidasEventTypes];   //! Sampling Rates of Event Requests

//   HNDLE         fMidasOnlineDataBase;                     //! Handle to the Midas Online Data Base (Online)
   INT           fMidasOnlineBuffer;                       //! Midas Online Buffer
   Seek_t        fMidasFileHandle;                         //! Handle to a un-gzipped Midas Inputfile
   gzFile        fMidasGzFileHandle;                       //! Handle to Midas gzipped Inputfile
   Bool_t        fGZippedMidasFile;                        //! True if input file is gzipped.
   Bool_t        fStopRequest;                             //! True if a Stop transition message was received

   EVENT_HEADER *fOdbOffline;                              //! Handle to the Midas Online Data Base (Offline)

   Int_t         fTimeStamp;                               //! Current time stamp

public:
   ROMEMidasDAQ::ROMEMidasDAQ();

   // Online Database
//   HNDLE          GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
//   HNDLE         *GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; };

   // Raw Data
   void          *GetRawDataEvent() { return fRawDataEvent[fCurrentRawDataEvent]; };
   void          *GetLastRawDataEvent() { return fRawDataEvent[1-fCurrentRawDataEvent]; };
   size_t         GetRawDataEventSize() { return sizeof(fRawDataEvent[fCurrentRawDataEvent]); };
   void           SwitchRawDataBuffer() { fCurrentRawDataEvent = 1-fCurrentRawDataEvent; };

   // Event Requests
   Int_t          GetNumberOfEventRequests() { return fNumberOfEventRequests; };
   Short_t        GetEventRequestID(Int_t i) { return fEventRequestID[i]; };
   Short_t        GetEventRequestMask(Int_t i) { return fEventRequestMask[i]; };
   Int_t          GetEventRequestRate(Int_t i) { return fEventRequestRate[i]; };

   void           SetNumberOfEventRequests(Int_t value) { fNumberOfEventRequests = value; };
   void           SetEventRequestID(Int_t i,Short_t value)    { fEventRequestID[i] = value; };
   void           SetEventRequestMask(Int_t i,Short_t value)  { fEventRequestMask[i] = value; };
   void           SetEventRequestRate(Int_t i,Int_t value)  { fEventRequestRate[i] = value; };

   // Additional Getters
   Int_t          GetTimeStamp() { return fTimeStamp; };
   const char    *GetName() { return "midas"; };

   Bool_t         Init();
   Bool_t         BeginOfRun();
   Bool_t         Event(Long64_t event);
   Bool_t         EndOfRun();
   Bool_t         Terminate();

   virtual Bool_t IsActiveEventID(Int_t id){ return true; }
   virtual Bool_t InitODB() = 0;
   virtual Bool_t InitHeader() = 0;
   virtual void   InitMidasBanks() = 0;

   //byte swapping
#ifndef R__BYTESWAP
#   ifndef HAVE_MIDAS
   INT            bk_swap(void *event, BOOL force);
#   endif
   virtual void  *ByteSwapStruct( char* aName, void* aData ) { return aData; };       // Must be overwritten by analyzermidas code.
#endif
#ifndef HAVE_MIDAS
   BOOL           bk_is32(void *event);
   INT            bk_find(BANK_HEADER* pbkh, const char *name, DWORD* bklen, DWORD* bktype,void *pdata);
#endif
};

#endif   // ROMEMidasDAQ_H
