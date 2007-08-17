/********************************************************************
  ROMEMidasDAQ.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEMidasDAQ_H
#define ROMEMidasDAQ_H

#include <zlib.h>
#include <RConfig.h>
#include <TArrayL.h>
#include "ROMEDAQSystem.h"

#if defined ( HAVE_MIDAS ) && !defined ( __MAKECINT__ )
#   include "midas.h"
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
   Bool_t        fByteSwap;
   char          fRawDataEvent[2][0x400000];               //! Midas Inputdata Stack for the current Event and the last Event
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
   Bool_t        fRequestAll;                              //! True if one request type requests all events

   EVENT_HEADER *fOdbOffline;                              //! Handle to the Midas Online Data Base (Offline)

   Int_t         fTimeStamp;                               //! Current time stamp

   TArrayL*      fEventFilePositions;                      //! File pointer to event
   Long64_t      fValidEventFilePositions;                 //! Max number of valid index of fEventFilePositions
   Long64_t      fCurrentPosition;                         //! Current position in file [event]
   Long64_t      fMaxDataEvent;                            //! Maximum number of sequential number of data events in the run.

   Bool_t        fByteSwapFlagMightBeWrong;                //! Flag if <MidasByteSwap> might be wrongly specified.

private:
   ROMEMidasDAQ(const ROMEMidasDAQ &daq); // not implemented
   ROMEMidasDAQ &operator=(const ROMEMidasDAQ &rhs); // not implemented

public:
   ROMEMidasDAQ();
   virtual ~ROMEMidasDAQ();

   // Online Database
//   HNDLE          GetMidasOnlineDataBase() const { return fMidasOnlineDataBase; }
//   HNDLE         *GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; }

   // Raw Data
   void          *GetRawDataEvent() { return fRawDataEvent[fCurrentRawDataEvent]; }
   void          *GetLastRawDataEvent() { return fRawDataEvent[1-fCurrentRawDataEvent]; }
   size_t         GetRawDataEventSize() const { return sizeof(fRawDataEvent[fCurrentRawDataEvent]); }
   void           SwitchRawDataBuffer() { fCurrentRawDataEvent = 1-fCurrentRawDataEvent; }

   // Event Requests
   Int_t          GetNumberOfEventRequests() const { return fNumberOfEventRequests; }
   Short_t        GetEventRequestID(Int_t i) const { return fEventRequestID[i]; }
   Short_t        GetEventRequestMask(Int_t i) const { return fEventRequestMask[i]; }
   Int_t          GetEventRequestRate(Int_t i) const { return fEventRequestRate[i]; }

   void           SetNumberOfEventRequests(Int_t value) { fNumberOfEventRequests = value; }
   void           SetEventRequestID(Int_t i,Short_t value)    { fEventRequestID[i] = value; }
   void           SetEventRequestMask(Int_t i,Short_t value)  { fEventRequestMask[i] = value; }
   void           SetEventRequestRate(Int_t i,Int_t value)  { fEventRequestRate[i] = value; }

   // Byte swap flag
   void           SetByteSwap(Bool_t flag = kTRUE) { fByteSwap = flag; }
   Bool_t         GetByteSwap() const { return fByteSwap; }

   // Additional Getters
   Int_t          GetTimeStamp() { return fTimeStamp; }
   const char    *GetName() const { return "midas"; }

   Bool_t         Init();
   Bool_t         BeginOfRun();
   Long64_t       Seek(Long64_t event);
   Bool_t         Event(Long64_t event);
   Bool_t         EndOfRun();
   Bool_t         Terminate();

   Bool_t         ReadODBOffline();

   virtual Bool_t IsActiveEventID(Int_t /*id*/) const { return true; }
#if defined( HAVE_MIDAS )
   virtual Bool_t InitODB() = 0;
#endif // HAVE_MIDAS
   virtual Bool_t InitHeader() = 0;
   virtual void   InitMidasBanks() = 0;

   //byte swapping
#if !defined HAVE_MIDAS && !defined ( __MAKECINT__ )
   INT            bk_swap(void *event, BOOL force) const;
   static BOOL    bk_is32(void *event);
   static INT     bk_find(BANK_HEADER* pbkh, const char *name, DWORD* bklen, DWORD* bktype,void *pdata);
#endif
   virtual void  *ByteSwapStruct( char* /*aName*/, void* aData ) const { return aData; }       // Must be overwritten by analyzermidas code.
   Bool_t         ConnectExperiment();

   ClassDef(ROMEMidasDAQ, 0) // Base DAQ class for Midas system
};

#endif   // ROMEMidasDAQ_H
