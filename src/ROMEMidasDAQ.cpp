// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEMidasDAQ
//
//  Interface to the Midas System.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#include <stdlib.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   include <io.h>
#   include <SYS\STAT.H>
#endif
#if defined( R__UNIX )
#   include <unistd.h>
#endif
#include <fcntl.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#include <TThread.h>
#include <TTimer.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROMEUtilities.h"
#include "ROMEAnalyzer.h"
#include "ROMEODBOfflineDataBase.h"
#include "ROMEMidasDAQ.h"
#include "ROMEPath.h"

#if defined( R__VISUAL_CPLUSPLUS )
#   define O_RDONLY_BINARY O_RDONLY | O_BINARY
#else
#   if defined( R__SEEK64 )
#      define O_RDONLY_BINARY O_RDONLY | O_LARGEFILE
#   else
#      define O_RDONLY_BINARY O_RDONLY
#   endif
#endif

const Int_t kEventFilePositionsResizeIncrement = 100000;
const Int_t kTriggerEventID = 1;

namespace {
   enum EOnlineThreadRequest {
      kNoRequest = 0,
      kReadEvent,
      kReadDB,
      kWriteDB,
      kCheckTransition
   };
   // inter thread communication
   Int_t           fgOnlineThreadRequest      = 0;
   Bool_t          fgReadEventReturn          = kFALSE;
   Bool_t          fgDBReadReturn             = kFALSE;
   Bool_t          fgDBWriteReturn            = kFALSE;
   ROMEStr2DArray* fgDBValues                 = 0;
   const char *    fgDBPath                   = 0;
   Long64_t        fgDBRunNumber              = 0;
   Long64_t        fgDBEventNumber            = 0;
   TVirtualMutex  *fgMutex                    = 0;
   Long64_t        fgCurrentRunNumber         = -1;
   Int_t           fgEventStatus              = -1;
   Int_t           fgRunStatus                = -1;
}

//#define MIDAS_DEBUG

#if defined( HAVE_MIDAS )
#   define MIDAS_DEBUG // define if you want to run the analyzer in the debugger
void ProcessMessage(Int_t /*hBuf*/, Int_t /*id*/, EVENT_HEADER * /*pheader*/, void * /*message*/)
{
// This method is called, when a system message from the online system occurs
}
#endif

ClassImp(ROMEMidasDAQ)

//______________________________________________________________________________
ROMEMidasDAQ::ROMEMidasDAQ()
:ROMEDAQSystem()
#if defined( R__BYTESWAP )
,fByteSwap(kFALSE)
#else
,fByteSwap(kTRUE)
#endif
,fNumberOfRawDataEvent(0)
,fCurrentRawDataEvent(-1)
,fValidRawDataEvent(0)
,fReadExistingRawData(kFALSE)
,fNumberOfEventRequests(0)
,fMidasOnlineBuffer(0)
,fMidasFileHandle(0)
,fMidasGzFileHandle()
,fGZippedMidasFile(kFALSE)
,fRequestAll(kFALSE)
,fOdbOffline(0)
,fTimeStamp(0)
,fEventFilePositions(new TArrayL(kEventFilePositionsResizeIncrement))
,fValidEventFilePositions(0)
,fCurrentPosition(0)
,fMaxDataEvent(0)
,fByteSwapFlagMightBeWrong(kFALSE)
,fOnlineHandlerThread(0)
,fOnlineHandlerTimer(0)
,fOnlineConnection(kFALSE)
,fOnlineThread(kFALSE)
,fODBBufferSize(5000)
,fODBBuffer(new char[fODBBufferSize])
{
   Int_t i;
   for (i = 0; i < kMaxMidasEventTypes; i++) {
      fEventRequestRate[i] = 2;
      fEventRequestMask[i] = -1;
   }

   if (gROME->isOffline()){
      fNumberOfRawDataEvent = 2;
   } else {
      fNumberOfRawDataEvent = kRawDataEvents;
   }
   for (i = 0; i < kRawDataEvents; i++) {
      if (i < fNumberOfRawDataEvent) {
         fRawDataEvent[i] = new char[MAX_EVENT_SIZE];
      } else {
         fRawDataEvent[i] = 0;
      }
   }
}

//______________________________________________________________________________
ROMEMidasDAQ::~ROMEMidasDAQ()
{
   Int_t i;
   for (i = 0; i< kRawDataEvents; i++) {
      SafeDeleteArray(fRawDataEvent[i]);
   }
   SafeDelete(fOdbOffline);
   SafeDelete(fEventFilePositions);
   StopOnlineCommunication(this);
   SafeDeleteArray(fODBBuffer);
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::Init()
{
   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      ROMEPrint::Print("Program is running online.\n");

      // Connect to the Frontend
      StartOnlineCommunication(this);
      while (fgCurrentRunNumber < 0) {
         gSystem->Sleep(10);
      }
      if (fgCurrentRunNumber >= 0) {
         gROME->SetCurrentRunNumber(fgCurrentRunNumber);
         fgCurrentRunNumber = -1;
      }
      if (fgEventStatus >= 0) {
         fEventStatus = fgEventStatus;
         fgEventStatus = -1;
      }
      if (fgRunStatus >= 0) {
         fRunStatus = fgRunStatus;
         fgRunStatus = -1;
      }
   } else if (gROME->isOffline()) {
      if (!gROME->IsRunNumberBasedIO()) {
         ROMEPrint::Print("The midas DAQ does not yet support InputFileNames.\n");
         return kFALSE;
      }
      ROMEPrint::Print("Program is running offline.\n");
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::BeginOfRun()
{
   if (gROME->isOffline()) {
      ROMEString filename;
      ROMEString gzfilename;
      ROMEString fileExtension = ".mid";
      ROMEString gzfileExtension = ".mid.gz";
      ROMEString runNumberString;

      this->SetAnalyze();
      this->SetRunning();
      // Open Midas File
      if (gROME->IsRunNumberBasedIO()) {
         gROME->GetCurrentRunNumberString(runNumberString);
         fGZippedMidasFile = kFALSE;
         filename.SetFormatted("%srun%s%s", gROME->GetInputDir(), runNumberString.Data(), fileExtension.Data());
         gzfilename.SetFormatted("%srun%s%s", gROME->GetInputDir(), runNumberString.Data(), gzfileExtension.Data());
         fMidasFileHandle = open(filename.Data(), O_RDONLY_BINARY);
         if (fMidasFileHandle == -1) {
            fMidasGzFileHandle = gzopen(gzfilename.Data(), "rb");
            if (fMidasGzFileHandle == 0) {
               ROMEPrint::Error("Failed to open input file '%s[.gz]'.\n", filename.Data());
               return kFALSE;
            }
            fGZippedMidasFile = kTRUE;
         }
      }

      ROMEPrint::Print("Reading Midas-File ");
      if(!fGZippedMidasFile){
         ROMEPrint::Print("%s\n", filename.Data());
      } else {
         ROMEPrint::Print("%s\n", gzfilename.Data());
      }

      fMaxDataEvent = kMaxLong64;

      fCurrentPosition = 0;
      fValidEventFilePositions = 0;
      fEventFilePositions->Set(0);

      while (!isBeginOfRun() && !isEndOfRun() && !isTerminate()) {
         if (!ReadODBOffline()) {
            return kFALSE;
         }
      }
      SetAnalyze();
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::Event(Long64_t event)
{
   // Switch Raw Data Buffer
   if (!fReadExistingRawData) {
      this->SwitchRawDataBuffer();
   }

   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      void* mEvent;

      // check transition
      if (fOnlineThread) {
         if (!fOnlineHandlerThread) {
            return kFALSE;
         }
         {
            ROME_LOCKGUARD(fgMutex);
            fgOnlineThreadRequest = kCheckTransition;
         }
         while (fgOnlineThreadRequest) {
            // wait until thread finish process
            gSystem->Sleep(10);
         }
      } else {
         if (fOnlineHandlerTimer) {
            // call CheckTransition
            fOnlineHandlerTimer->Emit("Timeout()");
         }
      }
      if (fgCurrentRunNumber >= 0) {
         gROME->SetCurrentRunNumber(fgCurrentRunNumber);
         fgCurrentRunNumber = -1;
      }
      if (fgEventStatus >= 0) {
         fEventStatus = fgEventStatus;
         fgEventStatus = -1;
      }
      if (fgRunStatus >= 0) {
         fRunStatus = fgRunStatus;
         fgRunStatus = -1;
      }

      if (isBeginOfRun() || isEndOfRun() || isContinue()) {
         return kTRUE;
      }

      if (!fReadExistingRawData) {
         // read an event
         if (fValidRawDataEvent != kRawDataEvents) {
            fValidRawDataEvent = fCurrentRawDataEvent + 1;
         }

         if (fOnlineThread) {
            if (!fOnlineHandlerThread) {
               return kFALSE;
            }
            {
               ROME_LOCKGUARD(fgMutex);
               fgOnlineThreadRequest = kReadEvent;
            }
            while (fgOnlineThreadRequest) {
               // wait until thread finish process
               gSystem->Sleep(10);
            }
            if (!fgReadEventReturn) {
               this->SetContinue();
               return kTRUE;
            }
         } else {
            if (!ReadOnlineEvent(this)) {
               this->SetContinue();
               return kTRUE;
            }
         }

         mEvent = this->GetRawDataEvent();
         if (reinterpret_cast<EVENT_HEADER*>(mEvent)->event_id != kTriggerEventID) {
            gROME->SetFillEvent(kFALSE);
         } else {
            gROME->SetCurrentEventNumber(reinterpret_cast<EVENT_HEADER*>(mEvent)->serial_number);
         }
         gROME->SetEventID(reinterpret_cast<EVENT_HEADER*>(mEvent)->event_id);
         fTimeStamp = reinterpret_cast<EVENT_HEADER*>(mEvent)->time_stamp;
         if (fByteSwap) {
            //byte swapping
            if(reinterpret_cast<EVENT_HEADER*>(mEvent)->event_id != EVENTID_BOR &&
               reinterpret_cast<EVENT_HEADER*>(mEvent)->event_id != EVENTID_EOR &&
               reinterpret_cast<EVENT_HEADER*>(mEvent)->event_id != EVENTID_MESSAGE)
               if(IsActiveEventID(reinterpret_cast<EVENT_HEADER*>(mEvent)->event_id)) {
                  bk_swap(reinterpret_cast<EVENT_HEADER*>(mEvent) + 1, 0);
               }
         }
      } else {
         mEvent = this->GetRawDataEvent();
         if (reinterpret_cast<EVENT_HEADER*>(mEvent)->event_id != kTriggerEventID) {
            gROME->SetFillEvent(kFALSE);
         } else {
            gROME->SetCurrentEventNumber(reinterpret_cast<EVENT_HEADER*>(mEvent)->serial_number);
         }
         gROME->SetEventID(reinterpret_cast<EVENT_HEADER*>(mEvent)->event_id);
         fTimeStamp = reinterpret_cast<EVENT_HEADER*>(mEvent)->time_stamp;
         fReadExistingRawData = kFALSE;
      }
      this->InitMidasBanks();
#endif
   } else if (gROME->isOffline()) {
      if (event > fMaxDataEvent) {
         this->SetEndOfRun();
         return kTRUE;
      }

      // read event header
      EVENT_HEADER *pevent = reinterpret_cast<EVENT_HEADER*>(this->GetRawDataEvent());
      bool readError = kFALSE;
      Long_t n;

      // seek position
      if (Seek(event) < 0) {
         return kTRUE;
      }

      // read event header
      if(!fGZippedMidasFile) {
         n = read(fMidasFileHandle, pevent, sizeof(EVENT_HEADER));
      } else {
         n = gzread(fMidasGzFileHandle, pevent, sizeof(EVENT_HEADER));
      }

      if (n < static_cast<Long_t>(sizeof(EVENT_HEADER))) {
         readError = kTRUE;
      } else {
         // read data
#if !defined( R__BYTESWAP )
         ROMEUtilities::ByteSwap(&pevent->event_id);
         ROMEUtilities::ByteSwap(&pevent->trigger_mask);
         ROMEUtilities::ByteSwap(&pevent->serial_number);
         ROMEUtilities::ByteSwap(&pevent->time_stamp);
         ROMEUtilities::ByteSwap(&pevent->data_size);
#endif
         n = 0;
         if (pevent->data_size <= 0) {
            readError = kTRUE;
         } else {
            if(!fGZippedMidasFile) {
               n = read(fMidasFileHandle, pevent + 1, pevent->data_size);
            } else {
               n = gzread(fMidasGzFileHandle, pevent + 1, pevent->data_size);
            }
            if (n != static_cast<Long_t>(pevent->data_size)) {
               readError = kTRUE;
            }
#if 0
            if (static_cast<Int_t>(reinterpret_cast<BANK_HEADER*>(pevent + 1)->data_size) <= 0) {
               readError = kTRUE;
            }
#endif
         }
         fCurrentPosition++;
      }

      // check input
      if (readError) {
         if (n > 0) {
            ROMEPrint::Warning("Unexpected end of file\n");
         }
         this->SetEndOfRun();
         return kTRUE;
      }
      // Get Handle to ODB header
      if (pevent->event_id == EVENTID_BOR) {
         if (gROME->isDataBaseActive("ODB")) {
            static_cast<ROMEODBOfflineDataBase*>(gROME->GetDataBase("ODB"))->
                  SetBuffer(reinterpret_cast<char*>(pevent + 1));
         }
         this->SetBeginOfRun();
         return kTRUE;
      }
      if (pevent->event_id == EVENTID_EOR) {
         this->SetEndOfRun();
         return kTRUE;
      }
      if (pevent->event_id < 0) {
         this->SetContinue();
         return kTRUE;
      }
      if (fByteSwap) {
         //byte swapping
         if(pevent->event_id != EVENTID_BOR &&
            pevent->event_id != EVENTID_EOR &&
            pevent->event_id != EVENTID_MESSAGE) {
            if(IsActiveEventID( pevent->event_id )) {
               bk_swap(pevent + 1, 0);
            }
         }
      }
      if (pevent->data_size < reinterpret_cast<BANK_HEADER*>(pevent + 1)->data_size) {
         this->SetContinue();
         fByteSwapFlagMightBeWrong = kTRUE;
         return kTRUE;
      }

      // initalize event
      gROME->SetEventID(pevent->event_id);
      if (pevent->event_id != kTriggerEventID) {
         gROME->SetFillEvent(kFALSE);
      } else {
         gROME->SetCurrentEventNumber(pevent->serial_number);
      }
      fTimeStamp = pevent->time_stamp;

      this->InitMidasBanks();
   }
   this->InitHeader();
   return kTRUE;
}

//______________________________________________________________________________
Long64_t ROMEMidasDAQ::Seek(Long64_t event)
{
   // Move file pointer to specified event
   // Event number is not necessarily same with serial number when
   // there are multiple type of events in a Midas file.
   // In online mode, Seek moves in ring buffer forward or backward,
   // instead of jump to specified event number

   if (event < 0) {
      Warning("Seek", "Event number "R_LLD" was not found.", event);
      event = gROME->GetCurrentEventNumber();
      gROME->SetDontReadNextEvent();
      SetContinue();
      return -1;
   }

   if (gROME->isOnline()) {
      Int_t i;
      Bool_t found = kFALSE;
      Int_t rawDataEventOld = fCurrentRawDataEvent;
      Long64_t currentEventNumber = gROME->GetCurrentEventNumber();
      Long64_t newEventNumber = currentEventNumber;

      if (event == currentEventNumber) {
         fReadExistingRawData = kTRUE;
         return event;
      }
      for (i = 0; i < fValidRawDataEvent; i++) {
         if (event < currentEventNumber) {
            fCurrentRawDataEvent = (fCurrentRawDataEvent - 1) % fValidRawDataEvent;
            if (fCurrentRawDataEvent < 0) {
               fCurrentRawDataEvent += fValidRawDataEvent;
            }
            if (reinterpret_cast<EVENT_HEADER*>(this->GetRawDataEvent())->event_id == kTriggerEventID) {
               newEventNumber = reinterpret_cast<EVENT_HEADER*>(this->GetRawDataEvent())->serial_number;
               if (newEventNumber < currentEventNumber) {
                  found = kTRUE;
                  break;
               }
            }
         } else {
            fCurrentRawDataEvent = (fCurrentRawDataEvent + 1) % fValidRawDataEvent;
            if (reinterpret_cast<EVENT_HEADER*>(this->GetRawDataEvent())->event_id == kTriggerEventID) {
               newEventNumber = reinterpret_cast<EVENT_HEADER*>(this->GetRawDataEvent())->serial_number;
               if (newEventNumber > currentEventNumber) {
                  found = kTRUE;
                  break;
               }
            }
         }
      }
      if (!found) {
         fCurrentRawDataEvent = rawDataEventOld;
         newEventNumber = currentEventNumber;
         if (event > currentEventNumber) {
            return newEventNumber;
         }
      }
      fReadExistingRawData = kTRUE;
      return newEventNumber;
   } else if (gROME->isOffline()) {
      if (event < fValidEventFilePositions) {
         // use stored position
         if(fEventFilePositions->At(static_cast<Int_t>(event)) != -1) {
            if(!fGZippedMidasFile) {
               lseek(fMidasFileHandle, fEventFilePositions->At(static_cast<Int_t>(event)), SEEK_SET);
            } else {
               gzseek(fMidasGzFileHandle, fEventFilePositions->At(static_cast<Int_t>(event)), SEEK_SET);
            }
            fCurrentPosition = event;
            return event;
         } else {
            return -1;
         }
      }

      // seek event
      EVENT_HEADER *pevent = reinterpret_cast<EVENT_HEADER*>(this->GetRawDataEvent());
      bool readError = kFALSE;
      Long_t n;
      while (fCurrentPosition != event) {
         if (fEventFilePositions->GetSize() <= fCurrentPosition) {
            fEventFilePositions->Set(fEventFilePositions->GetSize() + kEventFilePositionsResizeIncrement);
         }
         if(!fGZippedMidasFile) {
            fEventFilePositions->AddAt(lseek(fMidasFileHandle, 0L, SEEK_CUR), static_cast<Int_t>(fCurrentPosition));
         } else {
            fEventFilePositions->AddAt(gzseek(fMidasGzFileHandle, 0L, SEEK_CUR), static_cast<Int_t>(fCurrentPosition));
         }
         fValidEventFilePositions = fCurrentPosition + 1;
         if(!fGZippedMidasFile) {
            n = read(fMidasFileHandle,pevent, sizeof(EVENT_HEADER));
         } else {
            n = gzread(fMidasGzFileHandle,pevent, sizeof(EVENT_HEADER));
         }

         if (n < static_cast<Long_t>(sizeof(EVENT_HEADER))) {
            readError = kTRUE;
         } else {
#if !defined( R__BYTESWAP )
            ROMEUtilities::ByteSwap(&pevent->event_id);
            ROMEUtilities::ByteSwap(&pevent->trigger_mask);
            ROMEUtilities::ByteSwap(&pevent->serial_number);
            ROMEUtilities::ByteSwap(&pevent->time_stamp);
            ROMEUtilities::ByteSwap(&pevent->data_size);
#endif
            n = 0;
            if (pevent->data_size <= 0) {
               readError = kTRUE;
            } else {
               if(!fGZippedMidasFile) {
                  n = read(fMidasFileHandle, pevent + 1, pevent->data_size);
               } else {
                  n = gzread(fMidasGzFileHandle, pevent + 1, pevent->data_size);
               }
               if (n != static_cast<Long_t>(pevent->data_size)) {
                  readError = kTRUE;
               }
#if 0
               if (static_cast<Int_t>(reinterpret_cast<BANK_HEADER*>(pevent + 1)->data_size) <= 0) {
                  readError = kTRUE;
               }
#endif
            }
            fCurrentPosition++;
         }
         if (pevent->event_id == EVENTID_EOR) {
            this->SetEndOfRun();
            return -1;
         }
         // check input
         if (readError) {
            if (readError && n > 0) {
               ROMEPrint::Warning("Unexpected end of file\n");
               fMaxDataEvent = fCurrentPosition - 2;
               SetEndOfRun();
            } else {
               Warning("Seek", "Event number "R_LLD" was not found.", event);
               SetContinue();
            }
            fEventFilePositions->AddAt(-1, static_cast<Int_t>(fCurrentPosition - 1));
            gROME->SetDontReadNextEvent();
            return -1;
         }
         if (pevent->data_size < reinterpret_cast<BANK_HEADER*>(pevent + 1)->data_size) {
            continue;
         }
      }

      // we are now at the correct position.
      if (fCurrentPosition >= fValidEventFilePositions) {
         if (fEventFilePositions->GetSize() <= fCurrentPosition) {
            fEventFilePositions->Set(fEventFilePositions->GetSize() + kEventFilePositionsResizeIncrement);
         }
         if(!fGZippedMidasFile) {
            fEventFilePositions->AddAt(lseek(fMidasFileHandle, 0L, SEEK_CUR), static_cast<Int_t>(fCurrentPosition));
         } else {
            fEventFilePositions->AddAt(gzseek(fMidasGzFileHandle, 0L, SEEK_CUR), static_cast<Int_t>(fCurrentPosition));
         }
         fValidEventFilePositions = fCurrentPosition + 1;
      }
      return event;
   }
   return -1;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::EndOfRun()
{
   if (gROME->isOffline()) {
      if(!fGZippedMidasFile) {
         close(fMidasFileHandle);
      } else {
         gzclose(fMidasGzFileHandle);
      }

      if (fByteSwapFlagMightBeWrong && gROME->GetProcessedEvents() < 0.5) {
         ROMEPrint::Warning("\nWarning : A flag <MidasByteSwap> in your config XML file might be wrong.\n\n");
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::Terminate()
{
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::ActualReadODBOnline(ROMEStr2DArray *values, const char *dataBasePath,
                                         Long64_t runNumber, Long64_t eventNumber)
{
#if defined ( HAVE_MIDAS )
   if (gROME->isOnline()) {
      int i, iv;
      HNDLE hKey;
      char name[500];
      int type, num_values, item_size;
      int buffersize = fODBBufferSize;
      char value[500];
      ROMEString odbPath;
      ROMEString errMsg;

      ROMEPath *path = new ROMEPath();

      try {
         // decode path
         if (!path->Decode(dataBasePath, runNumber, eventNumber)) {
            errMsg.SetFormatted("Path decode error : %s", dataBasePath);
            throw errMsg.Data();
         }
         for (i = 0; i < path->GetNumberOfTables(); i++) {
            odbPath += "/";
            odbPath += path->GetTableNameAt(i);
         }
         odbPath += "/";
         odbPath += path->GetFieldName();
         if (db_find_key(gROME->GetMidasOnlineDataBase(), 0, const_cast<char*>(odbPath.Data()), &hKey) != CM_SUCCESS) {
            errMsg.SetFormatted("Cannot read online database\n");
            throw errMsg.Data();
         }
         if (db_get_key_info(gROME->GetMidasOnlineDataBase(), hKey, name, sizeof(name), &type, &num_values, &item_size) !=
             CM_SUCCESS) {
            errMsg.SetFormatted("Cannot read online database");
            throw errMsg.Data();
         }
         if (num_values * item_size > fODBBufferSize) {
            SetODBBufferSize(num_values * item_size);
         }
         if (db_get_data(gROME->GetMidasOnlineDataBase(), hKey, fODBBuffer, &buffersize, type) != CM_SUCCESS) {
            errMsg.SetFormatted("Cannot read online database");
            throw errMsg.Data();
         }
         if (path->GetFieldIndexAt(1) == -1) {
            // single field
            if (db_sprintf(value, fODBBuffer, buffersize, 0, type) != CM_SUCCESS) {
               errMsg.SetFormatted("Cannot read online database");
               throw errMsg.Data();
            }
            values->SetAt(value, 0, 0);
         } else {
            // array field
            iv = 0;
            for (i = path->GetFieldIndexAt(0); i < path->GetFieldIndexAt(1); i += path->GetFieldIndexAt(2)) {
               if (db_sprintf(value, fODBBuffer, buffersize, i, type) != CM_SUCCESS) {
                  errMsg.SetFormatted("Cannot read online database");
                  throw errMsg.Data();
               }
               values->SetAt(value, iv, 0);
               iv++;
            }
         }
      }
      catch(const char* err) {
         if (err) {
            ROMEPrint::Error("\n%s\n", err);
         }
         delete path;
         return kFALSE;
      }
      delete path;
      return kTRUE;
   }
#endif // HAVE_MIDAS
   WarningSuppression(values);
   WarningSuppression(dataBasePath);
   WarningSuppression(runNumber);
   WarningSuppression(eventNumber);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::WriteODBOnline(ROMEStr2DArray* values, const char * dataBasePath,
                                    Long64_t runNumber, Long64_t eventNumber)
{
   if (fOnlineThread) {
      if (!fOnlineHandlerThread) {
         return kFALSE;
      }
      {
         ROME_LOCKGUARD(fgMutex);
         fgDBValues = values;
         fgDBPath = dataBasePath;
         fgDBRunNumber = runNumber;
         fgDBEventNumber = eventNumber;
         fgOnlineThreadRequest = kWriteDB;
      }
      while (fgOnlineThreadRequest) {
         // wait until thread finish process
         gSystem->Sleep(10);
      }
      return fgDBWriteReturn;
   } else {
      return ActualWriteODBOnline(values, dataBasePath, runNumber, eventNumber);
   }
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::ReadODBOnline(ROMEStr2DArray* values, const char * dataBasePath,
                                   Long64_t runNumber, Long64_t eventNumber)
{
   if (fOnlineThread) {
      if (!fOnlineHandlerThread) {
         return kFALSE;
      }
      {
         ROME_LOCKGUARD(fgMutex);
         fgDBValues = values;
         fgDBPath = dataBasePath;
         fgDBRunNumber = runNumber;
         fgDBEventNumber = eventNumber;
         fgOnlineThreadRequest = kReadDB;
      }
      while (fgOnlineThreadRequest) {
         // wait until thread finish process
         gSystem->Sleep(10);
      }
      return fgDBReadReturn;
   } else {
      return ActualReadODBOnline(values, dataBasePath, runNumber, eventNumber);
   }
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::ActualWriteODBOnline(ROMEStr2DArray* values, const char * dataBasePath,
                                          Long64_t runNumber, Long64_t eventNumber)
{
#if defined ( HAVE_MIDAS )
   if (gROME->isOnline()) {
      int i, iv;
      HNDLE hKey;
      char name[500];
      int type, num_values, item_size;
      int buffersize = fODBBufferSize;
      ROMEString odbPath;
      ROMEString errMsg;

      ROMEPath *path = new ROMEPath();
      try {
         // decode path
         if (!path->Decode(dataBasePath, runNumber, eventNumber)) {
            errMsg.SetFormatted("\nPath decode error : %s\n", dataBasePath);
            throw errMsg.Data();
         }
         for (i = 0;i < path->GetNumberOfTables(); i++) {
            odbPath += "/";
            odbPath += path->GetTableNameAt(i);
         }
         odbPath += "/";
         odbPath += path->GetFieldName();

         if (db_find_key(gROME->GetMidasOnlineDataBase(), 0, const_cast<char*>(odbPath.Data()), &hKey) != CM_SUCCESS) {
            errMsg.SetFormatted("\nCannot write to online database\n");
            throw errMsg.Data();
         }
         if (db_get_key_info(gROME->GetMidasOnlineDataBase(), hKey, name, sizeof(name), &type, &num_values, &item_size) != CM_SUCCESS) {
            errMsg.SetFormatted("\nCannot write to online database\n");
            throw errMsg.Data();
         }
         if (num_values * item_size > fODBBufferSize) {
            SetODBBufferSize(num_values * item_size);
         }
         if (path->GetFieldIndexAt(1) == -1) {
            // single field
            if (db_sscanf(const_cast<char*>(values->At(0, 0).Data()), fODBBuffer, &buffersize, 0, type) != CM_SUCCESS) {
               errMsg.SetFormatted("\nCannot write to online database\n");
               throw errMsg.Data();
            }
            if (db_set_data(gROME->GetMidasOnlineDataBase(), hKey, fODBBuffer, buffersize, 1, type) != CM_SUCCESS) {
               errMsg.SetFormatted("\nCannot write to online database\n");
               throw errMsg.Data();
            }
         } else {
            // array field
            iv = 0;
            for (i = path->GetFieldIndexAt(0); i < path->GetFieldIndexAt(1); i += path->GetFieldIndexAt(2)) {
               if (db_sscanf(const_cast<char*>(values->At(iv, 0).Data()), fODBBuffer, &buffersize, 0, type) != CM_SUCCESS) {
                  errMsg.SetFormatted("\nCannot write to online database\n");
                  throw errMsg.Data();
               }
               if (db_set_data_index(gROME->GetMidasOnlineDataBase(), hKey, fODBBuffer, buffersize, i, type) != CM_SUCCESS) {
                  errMsg.SetFormatted("\nCannot write to online database\n");
                  throw errMsg.Data();
               }
               iv++;
            }
         }
      }
      catch(const char* err) {
         if (err) {
            ROMEPrint::Error("\n%s\n", err);
         }
         delete path;
         return kFALSE;
      }
      delete path;
   }
#endif // HAVE_MIDAS
   return kTRUE;
   WarningSuppression(values);
   WarningSuppression(dataBasePath);
   WarningSuppression(runNumber);
   WarningSuppression(eventNumber);
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::ReadODBOffline()
{
   if (gROME->isOffline()) {
      EVENT_HEADER *pevent = reinterpret_cast<EVENT_HEADER*>(this->GetRawDataEvent());
      bool readError = kFALSE;
      Long_t posOld = -1;

      // store current position
      if(!fGZippedMidasFile) {
         posOld = lseek(fMidasFileHandle, 0L, SEEK_CUR);
      } else {
         posOld = gzseek(fMidasGzFileHandle, 0L, SEEK_CUR);
      }

      Long_t n;
      if(!fGZippedMidasFile) {
         n = read(fMidasFileHandle,pevent, sizeof(EVENT_HEADER));
      } else {
         n = gzread(fMidasGzFileHandle,pevent, sizeof(EVENT_HEADER));
      }
      if (n < static_cast<Long_t>(sizeof(EVENT_HEADER))) {
         readError = kTRUE;
      } else {
#if !defined( R__BYTESWAP )
         ROMEUtilities::ByteSwap(&pevent->event_id);
         ROMEUtilities::ByteSwap(&pevent->trigger_mask);
         ROMEUtilities::ByteSwap(&pevent->serial_number);
         ROMEUtilities::ByteSwap(&pevent->time_stamp);
         ROMEUtilities::ByteSwap(&pevent->data_size);
#endif
         n = 0;
         if (pevent->data_size <= 0) {
            readError = kTRUE;
         } else {
            if(!fGZippedMidasFile) {
               n = read(fMidasFileHandle, pevent + 1, pevent->data_size);
            } else {
               n = gzread(fMidasGzFileHandle, pevent + 1, pevent->data_size);
            }
            if (n != static_cast<Long_t>(pevent->data_size)) {
               readError = kTRUE;
            }
            //            if (static_cast<int>(reinterpret_cast<BANK_HEADER*>(pevent + 1)->data_size) <= 0) readError = kTRUE;
         }
      }
      // check input
      if (readError) {
         if (n > 0) {
            ROMEPrint::Warning("Unexpected end of file\n");
         }
         this->SetEndOfRun();
         return kTRUE;
      }

      // Get Handle to ODB header
      fCurrentPosition = 0;
      if (fEventFilePositions->GetSize() < 1) {
         fEventFilePositions->Set(kEventFilePositionsResizeIncrement);
      }
      if (pevent->event_id == EVENTID_BOR) {
         if (gROME->isDataBaseActive("ODB")) {
            static_cast<ROMEODBOfflineDataBase*>(gROME->GetDataBase("ODB"))->
                  SetBuffer(reinterpret_cast<char*>(pevent + 1));
         }
         if(!fGZippedMidasFile) {
            fEventFilePositions->AddAt(lseek(fMidasFileHandle, 0L, SEEK_CUR), static_cast<Int_t>(fCurrentPosition));
         } else {
            fEventFilePositions->AddAt(gzseek(fMidasGzFileHandle, 0L, SEEK_CUR), static_cast<Int_t>(fCurrentPosition));
         }
         fValidEventFilePositions = 1;
      } else {
         if(posOld != -1) {
            if(!fGZippedMidasFile) {
               lseek(fMidasFileHandle, posOld, SEEK_SET);
            } else {
               gzseek(fMidasGzFileHandle, posOld, SEEK_SET);
            }
         }
         fEventFilePositions->AddAt(posOld, static_cast<Int_t>(fCurrentPosition));
         fValidEventFilePositions = 1;
      }
      this->SetBeginOfRun();
   }
   return kTRUE;
}

#if !defined( HAVE_MIDAS )
/**
Swaps bytes from little endian to big endian or vice versa for a whole event.

An event contains a flag which is set by bk_init() to identify
the endian format of an event. If force is FALSE, this flag is evaluated and
the event is only swapped if it is in the "wrong" format for this system.
An event can be swapped to the "wrong" format on purpose for example by a
front-end which wants to produce events in a "right" format for a back-end
analyzer which has different byte ordering.
@param event pointer to data area of event
@param force If TRUE, the event is always swapped, if FALSE, the event
is only swapped if it is in the wrong format.
@return 1==event has been swap, 0==event has not been swapped.
*/
//______________________________________________________________________________
INT ROMEMidasDAQ::bk_swap(void *event, BOOL force) const
{
   BANK_HEADER  *pbh;
   BANK         *pbk;
   BANK32       *pbk32;
   char         name[ 4 ];
   void         *pdata;
   UShort_t     type;
   bool         b32;

   pbh = static_cast<BANK_HEADER*>(event);

   // only swap if flags in high 16-bit
   if (pbh->flags < 0x10000 && !force) {
      return 0;
   }

   // swap bank header
   ROMEUtilities::ByteSwap((UInt_t*)&pbh->data_size);
   ROMEUtilities::ByteSwap((UInt_t*)&pbh->flags);

   // check for 32bit banks
   b32 = ((pbh->flags & (1<<4)) > 0);

   pbk = reinterpret_cast<BANK*>(pbh + 1);
   pbk32 = reinterpret_cast<BANK32*>(pbk);

   // scan event
   const size_t pend = pbh->data_size + sizeof(BANK_HEADER);
   while (reinterpret_cast<size_t>(pbk) - reinterpret_cast<size_t>(pbh) < pend) {
      // swap bank header
      if (b32) {
         ROMEUtilities::ByteSwap(&pbk32->type);
         ROMEUtilities::ByteSwap(&pbk32->data_size);
         pdata = pbk32 + 1;
         type = static_cast<UShort_t>(pbk32->type);
         for (Int_t i = 0; i < 4; i++) {
            name[i] = pbk32->name[i];
         }
      } else {
         ROMEUtilities::ByteSwap(&pbk->type);
         ROMEUtilities::ByteSwap(&pbk->data_size);
         pdata = pbk + 1;
         type = pbk->type;
         for (Int_t i = 0; i < 4; i++) {
            name[i] = pbk->name[i];
         }
      }

      // pbk points to next bank
      if (b32) {
         pbk32 = reinterpret_cast<BANK32*>(reinterpret_cast<char*>(pbk32 + 1) + ALIGN8(pbk32->data_size));
         pbk = reinterpret_cast<BANK*>(pbk32);
      } else {
         pbk = reinterpret_cast<BANK*>(reinterpret_cast<char*>(pbk + 1) + ALIGN8(pbk->data_size));
         pbk32 = reinterpret_cast<BANK32*>(pbk);
      }

      switch (type) {
      case TID_WORD:
      case TID_SHORT:
         while (reinterpret_cast<size_t>(pdata) < reinterpret_cast<size_t>(pbk)) {
            ROMEUtilities::ByteSwap(static_cast<UShort_t*>(pdata));
            pdata = static_cast<void*>(static_cast<UShort_t*>(pdata) + 1);
         }
         break;
      case TID_DWORD:
      case TID_INT:
      case TID_BOOL:
      case TID_FLOAT:
         while (reinterpret_cast<size_t>(pdata) < reinterpret_cast<size_t>(pbk)) {
            ROMEUtilities::ByteSwap(static_cast<UInt_t*>(pdata));
            pdata = static_cast<void*>(static_cast<UInt_t*>(pdata) + 1);
         }
         break;
      case TID_DOUBLE:
         while (reinterpret_cast<size_t>(pdata) < reinterpret_cast<size_t>(pbk)) {
            ROMEUtilities::ByteSwap(static_cast<ULong64_t*>(pdata));
            pdata = static_cast<void*>(static_cast<ULong64_t*>(pdata) + 1);
         }
         break;
      case TID_STRUCT:
         while (reinterpret_cast<size_t>(pdata) < reinterpret_cast<size_t>(pbk)) {
            pdata = ByteSwapStruct( &name[0], pdata );
         }
         break;
      }
   }
   return CM_SUCCESS;
}
#endif

#if !defined( HAVE_MIDAS )
//______________________________________________________________________________
BOOL ROMEMidasDAQ::bk_is32(void *event)
{
   return ((static_cast<BANK_HEADER*>(event)->flags & BANK_FORMAT_32BIT) > 0);
}

//______________________________________________________________________________
INT ROMEMidasDAQ::bk_find(BANK_HEADER* pbkh, const char *name, DWORD* bklen, DWORD* bktype,void *pdata)
{
   Int_t tid_size[] = {0,1,1,1,2,2,4,4,4,4,8,1,0,0,0,0,0};
   BANK *pbk;
   BANK32 *pbk32;
   UInt_t dname;

   if (bk_is32(pbkh)) {
      pbk32 = reinterpret_cast<BANK32*>(pbkh + 1);
      strncpy(reinterpret_cast<char*>(&dname), name, 4);
      do {
         if (*reinterpret_cast<UInt_t*>(pbk32->name) == dname) {
            *reinterpret_cast<void**>(pdata) = pbk32 + 1;
            if (tid_size[pbk32->type & 0xFF] == 0) {
               *bklen = pbk32->data_size;
            } else {
               *bklen = pbk32->data_size / tid_size[pbk32->type & 0xFF];
            }

            *bktype = pbk32->type;
            return 1;
         }
         pbk32 = reinterpret_cast<BANK32*>(reinterpret_cast<char*>(pbk32 + 1) + ALIGN8(pbk32->data_size));
      } while (reinterpret_cast<size_t>(pbk32) - reinterpret_cast<size_t>(pbkh) <
               reinterpret_cast<BANK_HEADER*>(pbkh)->data_size + sizeof(BANK_HEADER));
   } else {
      pbk = reinterpret_cast<BANK*>(reinterpret_cast<BANK_HEADER*>(pbkh) + 1);
      strncpy(reinterpret_cast<char*>(&dname), name, 4);
      do {
         if (*reinterpret_cast<UInt_t*>(pbk->name) == dname) {
            *reinterpret_cast<void**>(pdata) = pbk + 1;
            if (tid_size[pbk->type & 0xFF] == 0) {
               *bklen = pbk->data_size;
            } else {
               *bklen = pbk->data_size / tid_size[pbk->type & 0xFF];
            }

            *bktype = pbk->type;
            return 1;
         }
         pbk = reinterpret_cast<BANK*>(reinterpret_cast<char*>(pbk + 1) + ALIGN8(pbk->data_size));
      } while (reinterpret_cast<size_t>(pbk) - reinterpret_cast<size_t>(pbkh) <
               reinterpret_cast<BANK_HEADER*>(pbkh)->data_size + sizeof(BANK_HEADER));
   }
   *reinterpret_cast<void**>(pdata) = 0;
   return 0;
}
#endif

//______________________________________________________________________________
void ROMEMidasDAQ::StartOnlineCommunication(ROMEMidasDAQ* localThis)
{
   if (localThis->fOnlineThread) {
      if (!localThis->fOnlineHandlerThread) {
         localThis->fOnlineConnection = kTRUE;
         localThis->fOnlineHandlerThread = new TThread("ODBHandle", ROMEMidasDAQ::OnlineConnectionLoop, localThis);
         localThis->fOnlineHandlerThread->Run();
      }
   } else {
      if (!localThis->fOnlineHandlerTimer) {
         ConnectExperiment(localThis);
         InitOnlineCommunication(localThis);
         localThis->fOnlineHandlerTimer = new TTimer(100);
         localThis->fOnlineHandlerTimer->Connect("Timeout()", "ROMEMidasDAQ", localThis, "CheckTransition()");
         localThis->fOnlineHandlerTimer->TurnOn();
      }
   }
}

//______________________________________________________________________________
void ROMEMidasDAQ::StopOnlineCommunication(ROMEMidasDAQ* localThis)
{
   if (localThis->fOnlineHandlerThread) {
      localThis->fOnlineConnection = kFALSE;
      localThis->fOnlineHandlerThread->Join();
      SafeDelete(localThis->fOnlineHandlerThread);
   }
   if (localThis->fOnlineHandlerTimer) {
      SafeDelete(localThis->fOnlineHandlerTimer);
      cm_disconnect_experiment();
   }
}

//______________________________________________________________________________
THREADTYPE ROMEMidasDAQ::OnlineConnectionLoop(void *arg)
{
   ROMEMidasDAQ *localThis = static_cast<ROMEMidasDAQ*>(arg);

   ConnectExperiment(localThis);
   InitOnlineCommunication(localThis);
   Bool_t ret;

   while (localThis->fOnlineConnection) {
      CheckTransition();
      switch (fgOnlineThreadRequest) {
      case kCheckTransition:
         CheckTransition();
         break;
      case kReadEvent:
         ret = ReadOnlineEvent(localThis);
         {
            ROME_LOCKGUARD(fgMutex);
            fgReadEventReturn = ret;
         }
      break;
      case kReadDB:
         ret = localThis->ActualReadODBOnline(fgDBValues, fgDBPath, fgDBRunNumber, fgDBEventNumber);
         {
            ROME_LOCKGUARD(fgMutex);
            fgDBReadReturn = ret;
         }
         break;
      case kWriteDB:
         ret = localThis->ActualWriteODBOnline(fgDBValues, fgDBPath, fgDBRunNumber, fgDBEventNumber);
         {
            ROME_LOCKGUARD(fgMutex);
            fgDBWriteReturn = ret;
         }
         break;
      default:
         break;
      };
      if (fgOnlineThreadRequest) {
         ROME_LOCKGUARD(fgMutex);
         fgOnlineThreadRequest = kNoRequest;
      }
   }

   cm_disconnect_experiment();

   return THREADRETURN;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::ConnectExperiment(ROMEMidasDAQ *localThis)
{
   // Connect to the experiment
#if defined( HAVE_MIDAS )
   if (cm_connect_experiment(const_cast<char*>(gROME->GetOnlineHost()),
                             const_cast<char*>(gROME->GetOnlineExperiment()),
                             const_cast<char*>(gROME->GetOnlineAnalyzerName()), 0) != SUCCESS) {
      ROMEPrint::Error("\nCan not connect to experiment\n");
      return kFALSE;
   }

   INT requestId;
   Int_t i;

   // open the "system" buffer, 1M size
   bm_open_buffer(const_cast<char*>(gROME->GetOnlineMemoryBuffer()), 2 * MAX_EVENT_SIZE, &localThis->fMidasOnlineBuffer);

   // set the buffer cache size
   bm_set_cache_size(localThis->fMidasOnlineBuffer, 100000, 0);

   // place a request for a specific event id
   if (localThis->GetNumberOfEventRequests() <= 0) {
#if 0 // Is no event request invalid ?
      ROMEPrint::Error("\nNo Events Requests for online mode!\n");
      ROMEPrint::Error("\nPlace Events Requests into the ROME configuration file.\n");
      return kFALSE;
#endif
   }
   const int nRequest = localThis->GetNumberOfEventRequests();
   for (i = 0; i < nRequest; i++) {
      if (localThis->GetEventRequestRate(i) == 1) {
         localThis->fRequestAll = kTRUE;
      }
      bm_request_event(localThis->fMidasOnlineBuffer, localThis->GetEventRequestID(i),
                       localThis->GetEventRequestMask(i), localThis->GetEventRequestRate(i), &requestId, 0);
   }

   // place a request for system messages
   cm_msg_register(ProcessMessage);

   // turn off watchdog if in debug mode
#if defined( MIDAS_DEBUG )
   cm_set_watchdog_params(TRUE, 0);
#endif

   // Registers a callback function for run transitions.
   if (!gROME->IsStandAloneARGUS() && !gROME->IsROMEMonitor()) {
      if(cm_register_transition(TR_START, 0 ,500) != CM_SUCCESS ||
         cm_register_transition(TR_STOP, 0, 500) != CM_SUCCESS) {
         ROMEPrint::Error("\nCan not connect to experiment\n");
         return kFALSE;
      }
   }

   // Connect to the online database
   if (cm_get_experiment_database(gROME->GetMidasOnlineDataBasePointer(), 0)!= CM_SUCCESS) {
      ROMEPrint::Error("\nCan not connect to the online database\n");
      return kFALSE;
   }
#endif

   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::InitOnlineCommunication(ROMEMidasDAQ *localThis)
{
      // Check Run Status
      int state = 0;
      int statesize = sizeof(state);

      if (db_get_value(gROME->GetMidasOnlineDataBase(), 0, const_cast<char*>("/Runinfo/State"), &state,
                       &statesize, TID_INT, kFALSE) != CM_SUCCESS) {
         ROMEPrint::Error("\nCan not read run status from the online database\n");
         return kFALSE;
      }
      if (state != 3) {
         ROME_LOCKGUARD(fgMutex);
         fgEventStatus = kBeginOfRun;
         fgRunStatus = kStopped;
      }

      // Get Runnumber
      Int_t runNumber = 0;
      Int_t size = sizeof(runNumber);
      if (db_get_value(gROME->GetMidasOnlineDataBase(), 0, const_cast<char*>("/Runinfo/Run number"),
                       &runNumber, &size, TID_INT, kFALSE) != CM_SUCCESS) {
         ROMEPrint::Error("\nCan not read runnumber from the online database\n");
         return kFALSE;
      }
      {
         ROME_LOCKGUARD(fgMutex);
         fgCurrentRunNumber = runNumber;
      }

      // Initialize the online database
      HNDLE hKey;
      ROMEString str;
      // Tree Switches
      const Int_t nTree = gROME->GetTreeObjectEntries();
      Int_t i;
      for (i = 0; i <nTree; i++) {
         str = "//Tree switches/";
         str.Insert(1, gROME->GetOnlineAnalyzerName());
         str.Append(gROME->GetTreeObjectAt(i)->GetTree()->GetName());
         db_check_record(gROME->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()),
                         const_cast<char*>(gROME->GetTreeObjectAt(i)->GetSwitchesString()), TRUE);
         db_find_key(gROME->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()), &hKey);
         if (db_set_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetTreeObjectAt(i)->GetSwitches(),
                           gROME->GetTreeObjectAt(i)->GetSwitchesSize(), 0) != DB_SUCCESS) {
            ROMEPrint::Warning("\nCan not write to tree switches record.\n");
         }
         if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetTreeObjectAt(i)->GetSwitches(),
                            gROME->GetTreeObjectAt(i)->GetSwitchesSize(), MODE_READ, 0, 0) != DB_SUCCESS) {
            ROMEPrint::Warning("\nCan not open tree switches record, probably other analyzer is using it\n");
         }
      }

      // Experiment dependent ODB initializations
      localThis->InitODB();
      return kTRUE;
#else
      ROMEPrint::Error("Need Midas support for Online Mode !!\n");
      ROMEPrint::Error("Please link the midas library into this project.\n");
      ROMEPrint::Error("--> Run the ROMEBuilder with the '-midas' option.\n");
      return kFALSE;
#endif
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::ReadOnlineEvent(ROMEMidasDAQ *localThis)
{
   INT size;
   void* mEvent;
   int status;

   size = localThis->GetRawDataEventSize();
   mEvent = localThis->GetRawDataEvent();
   status = bm_receive_event(localThis->fMidasOnlineBuffer, mEvent, &size, ASYNC);
   if (status != BM_SUCCESS) {
      return kFALSE;
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::CheckTransition()
{
#if defined( HAVE_MIDAS )
   ROMEMidasDAQ *localThis = static_cast<ROMEMidasDAQ*>(gROME->GetActiveDAQ());

   INT size;
   void* mEvent;
   int status;
   int runNumber,trans; // use int instead of INT or Int_t

   static Long_t selfId = 0;
   if (!selfId) {
      // this is the first time to be called
      selfId = TThread::SelfId();
   }
   if (selfId != TThread::SelfId()) {
      // ODB communication must be done in a thread
      return kFALSE;
   }

   // ODB update
   db_send_changed_records();

   if (cm_query_transition(&trans, &runNumber, 0)) {
      if (trans == TR_START) {
         ROME_LOCKGUARD(fgMutex);
         fgCurrentRunNumber = runNumber;
         fgEventStatus = kBeginOfRun;
         fgRunStatus = kRunning;
         return kTRUE;
      }
      if (trans == TR_STOP) {
         if (localThis->fRequestAll) {
            INT numberOfBytes;
            bm_get_buffer_level(localThis->fMidasOnlineBuffer, &numberOfBytes);
            while (numberOfBytes <= 0) {
               size = localThis->GetRawDataEventSize();
               mEvent = localThis->GetRawDataEvent();
               status = bm_receive_event(localThis->fMidasOnlineBuffer, mEvent, &size, ASYNC);
               if (status != BM_SUCCESS) {
                  break;
               }
               bm_get_buffer_level(localThis->fMidasOnlineBuffer, &numberOfBytes);
            }
         }
         ROME_LOCKGUARD(fgMutex);
         fgEventStatus = kEndOfRun;
         fgRunStatus = kStopped;
         return kTRUE;
      }
   }
   status = cm_yield(100);
   if (status == RPC_SHUTDOWN || status == SS_ABORT) {
      cm_disconnect_experiment(); // this is needed because calling thead can not cancel itself in ROMERint::Terminate => ROMEAnalyzer::Cleaning
      gROME->GetApplication()->Terminate(1);
   }
   if (localThis->isStopped()) {
      ROME_LOCKGUARD(fgMutex);
      fgEventStatus = kContinue;
      return kTRUE;
   }
#endif
   return kTRUE;
}

//______________________________________________________________________________
void ROMEMidasDAQ::SetODBBufferSize(Int_t size)
{
   if (size > 0) {
      fODBBufferSize = size;
      SafeDeleteArray(fODBBuffer);
      fODBBuffer = new char[fODBBufferSize];
   }
}
