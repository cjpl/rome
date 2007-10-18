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
#include "ROMEUtilities.h"
#include "ROMEAnalyzer.h"
#include "ROMEODBOfflineDataBase.h"
#include "ROMEMidasDAQ.h"

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
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::Init()
{
   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      // Connect to the Frontend
      if (!ConnectExperiment()) {
         return kFALSE;
      }

      ROMEPrint::Print("Program is running online.\n");

      // Check Run Status
      int state = 0;
      int statesize = sizeof(state);
      if (db_get_value(gROME->GetMidasOnlineDataBase(),0,const_cast<char*>("/Runinfo/State"), &state,
                       &statesize,TID_INT, false) != CM_SUCCESS) {
         ROMEPrint::Error("\nCan not read run status from the online database\n");
         return false;
      }
      if (state != 3) {
         this->SetBeginOfRun();
         this->SetStopped();
      }

      // Get Runnumber
      Int_t runNumber = 0;
      Int_t size = sizeof(runNumber);
      if (db_get_value(gROME->GetMidasOnlineDataBase(), 0, const_cast<char*>("/Runinfo/Run number"),
                       &runNumber, &size, TID_INT, false) != CM_SUCCESS) {
         ROMEPrint::Error("\nCan not read runnumber from the online database\n");
         return false;
      }
      gROME->SetCurrentRunNumber(runNumber);

      // Initialize the online database
      HNDLE hKey;
      ROMEString str;
      // Tree Switches
      const Int_t nTree = gROME->GetTreeObjectEntries();
      Int_t i;
      for (i = 0;i <nTree; i++) {
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
      this->InitODB();

#else
      ROMEPrint::Error("Need Midas support for Online Mode !!\n");
      ROMEPrint::Error("Please link the midas library into this project.\n");
      ROMEPrint::Error("--> Run the ROMEBuilder with the '-midas' option.\n");
      return false;
#endif
   } else if (gROME->isOffline()) {
      if (!gROME->IsRunNumberBasedIO()) {
         ROMEPrint::Print("The midas DAQ does not yet support InputFileNames.\n");
         return false;
      }
      ROMEPrint::Print("Program is running offline.\n");
   }
   return true;
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
         fGZippedMidasFile = false;
         filename.SetFormatted("%srun%s%s", gROME->GetInputDir(), runNumberString.Data(), fileExtension.Data());
         gzfilename.SetFormatted("%srun%s%s", gROME->GetInputDir(), runNumberString.Data(), gzfileExtension.Data());
         fMidasFileHandle = open(filename.Data(), O_RDONLY_BINARY);
         if (fMidasFileHandle == -1) {
            fMidasGzFileHandle = gzopen(gzfilename.Data(), "rb");
            if (fMidasGzFileHandle == 0) {
               ROMEPrint::Error("Failed to open input file '%s[.gz]'.\n", filename.Data());
               return false;
            }
            fGZippedMidasFile = true;
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
            return false;
         }
      }
      SetAnalyze();
   }
   return true;
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
      INT size;
      void* mEvent;
      int status;

      if (!RespondOnlineRequest()) {
         return false;
      }
      if (isBeginOfRun() || isEndOfRun() || isContinue()) {
         return true;
      }

      if (!fReadExistingRawData) {
         // read an event
         if (fValidRawDataEvent != kRawDataEvents) {
            fValidRawDataEvent = fCurrentRawDataEvent + 1;
         }
         size = this->GetRawDataEventSize();
         mEvent = this->GetRawDataEvent();
         status = bm_receive_event(fMidasOnlineBuffer, mEvent, &size, ASYNC);
         if (status != BM_SUCCESS) {
            this->SetContinue();
            return true;
         }

         if (reinterpret_cast<EVENT_HEADER*>(mEvent)->event_id != kTriggerEventID) {
            gROME->SetFillEvent(false);
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
            gROME->SetFillEvent(false);
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
         return true;
      }

      // read event header
      EVENT_HEADER *pevent = reinterpret_cast<EVENT_HEADER*>(this->GetRawDataEvent());
      bool readError = false;
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
         readError = true;
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
            readError = true;
         } else {
            if(!fGZippedMidasFile) {
               n = read(fMidasFileHandle, pevent + 1, pevent->data_size);
            } else {
               n = gzread(fMidasGzFileHandle, pevent + 1, pevent->data_size);
            }
            if (n != static_cast<Long_t>(pevent->data_size)) {
               readError = true;
            }
#if 0
            if (static_cast<Int_t>(reinterpret_cast<BANK_HEADER*>(pevent + 1)->data_size) <= 0) {
               readError = true;
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
         return true;
      }
      // Get Handle to ODB header
      if (pevent->event_id == EVENTID_BOR) {
         if (gROME->isDataBaseActive("ODB")) {
            static_cast<ROMEODBOfflineDataBase*>(gROME->GetDataBase("ODB"))->
                  SetBuffer(reinterpret_cast<char*>(pevent + 1));
         }
         this->SetBeginOfRun();
         return true;
      }
      if (pevent->event_id == EVENTID_EOR) {
         this->SetEndOfRun();
         return true;
      }
      if (pevent->event_id < 0) {
         this->SetContinue();
         return true;
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
         return true;
      }

      // initalize event
      gROME->SetEventID(pevent->event_id);
      if (pevent->event_id != kTriggerEventID) {
         gROME->SetFillEvent(false);
      } else {
         gROME->SetCurrentEventNumber(pevent->serial_number);
      }
      fTimeStamp = pevent->time_stamp;

      this->InitMidasBanks();
   }
   this->InitHeader();
   return true;
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
      bool readError = false;
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
            readError = true;
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
               readError = true;
            } else {
               if(!fGZippedMidasFile) {
                  n = read(fMidasFileHandle, pevent + 1, pevent->data_size);
               } else {
                  n = gzread(fMidasGzFileHandle, pevent + 1, pevent->data_size);
               }
               if (n != static_cast<Long_t>(pevent->data_size)) {
                  readError = true;
               }
#if 0
               if (static_cast<Int_t>(reinterpret_cast<BANK_HEADER*>(pevent + 1)->data_size) <= 0) {
                  readError = true;
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
   return true;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::Terminate()
{
   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      cm_disconnect_experiment();
#endif
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::ReadODBOffline()
{
   if (gROME->isOffline()) {
      EVENT_HEADER *pevent = reinterpret_cast<EVENT_HEADER*>(this->GetRawDataEvent());
      bool readError = false;
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
         readError = true;
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
            readError = true;
         } else {
            if(!fGZippedMidasFile) {
               n = read(fMidasFileHandle, pevent + 1, pevent->data_size);
            } else {
               n = gzread(fMidasGzFileHandle, pevent + 1, pevent->data_size);
            }
            if (n != static_cast<Long_t>(pevent->data_size)) {
               readError = true;
            }
            //            if (static_cast<int>(reinterpret_cast<BANK_HEADER*>(pevent + 1)->data_size) <= 0) readError = true;
         }
      }
      // check input
      if (readError) {
         if (n > 0) {
            ROMEPrint::Warning("Unexpected end of file\n");
         }
         this->SetEndOfRun();
         return true;
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
   return true;
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
Bool_t ROMEMidasDAQ::ConnectExperiment()
{
   // Connect to the experiment
#if defined( HAVE_MIDAS )
   if (cm_connect_experiment(const_cast<char*>(gROME->GetOnlineHost()),
                             const_cast<char*>(gROME->GetOnlineExperiment()),
                             const_cast<char*>(gROME->GetOnlineAnalyzerName()), 0) != SUCCESS) {
      ROMEPrint::Error("\nCan not connect to experiment\n");
      return kFALSE;
   }
   // regesters a disconnection to be executed when the program terminates normally.
   atexit(reinterpret_cast<void (*)(void)>(&cm_disconnect_experiment));

   INT requestId;
   Int_t i;

   // open the "system" buffer, 1M size
   bm_open_buffer(const_cast<char*>(gROME->GetOnlineMemoryBuffer()), 2 * MAX_EVENT_SIZE, &fMidasOnlineBuffer);

   // set the buffer cache size
   bm_set_cache_size(fMidasOnlineBuffer, 100000, 0);

   // place a request for a specific event id
   if (this->GetNumberOfEventRequests() <= 0) {
#if 0 // Is no event request invalid ?
      ROMEPrint::Error("\nNo Events Requests for online mode!\n");
      ROMEPrint::Error("\nPlace Events Requests into the ROME configuration file.\n");
      return false;
#endif
   }
   const int nRequest = this->GetNumberOfEventRequests();
   for (i = 0; i < nRequest; i++) {
      if (this->GetEventRequestRate(i) == 1) {
         fRequestAll = true;
      }
      bm_request_event(fMidasOnlineBuffer, this->GetEventRequestID(i),
                       this->GetEventRequestMask(i),this->GetEventRequestRate(i), &requestId,0);
   }

   // place a request for system messages
   cm_msg_register(ProcessMessage);

   // turn off watchdog if in debug mode
#if defined( MIDAS_DEBUG )
   cm_set_watchdog_params(TRUE, 0);
#endif

   // Registers a callback function for run transitions.
   if (!gROME->IsStandAloneARGUS()) {
      if(cm_register_transition(TR_START, 0 ,500) != CM_SUCCESS ||
         cm_register_transition(TR_STOP, 0, 500) != CM_SUCCESS) {
         ROMEPrint::Error("\nCan not connect to experiment\n");
         return false;
      }
   }

   // Connect to the online database
   if (cm_get_experiment_database(gROME->GetMidasOnlineDataBasePointer(), 0)!= CM_SUCCESS) {
      ROMEPrint::Error("\nCan not connect to the online database\n");
      return false;
   }
#endif

   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEMidasDAQ::RespondOnlineRequest()
{
#if defined( HAVE_MIDAS )
   INT size;
   void* mEvent;
   int status;
   int runNumber,trans; // use int instead of INT or Int_t

   if (cm_query_transition(&trans, &runNumber, 0)) {
      if (trans == TR_START) {
         gROME->SetCurrentRunNumber(runNumber);
         this->SetBeginOfRun();
         this->SetRunning();
         return true;
      }
      if (trans == TR_STOP) {
         if (fRequestAll) {
            INT numberOfBytes;
            bm_get_buffer_level(fMidasOnlineBuffer, &numberOfBytes);
            while (numberOfBytes <= 0) {
               size = this->GetRawDataEventSize();
               mEvent = this->GetRawDataEvent();
               status = bm_receive_event(fMidasOnlineBuffer, mEvent, &size, ASYNC);
               if (status != BM_SUCCESS)
                  break;
               bm_get_buffer_level(fMidasOnlineBuffer, &numberOfBytes);
            }
         }
         this->SetEndOfRun();
         this->SetStopped();
         return true;
      }
   }
   status = cm_yield(100);
   if (status == RPC_SHUTDOWN || status == SS_ABORT) {
      this->SetTerminate();
      return false;
   }
   if (this->isStopped()) {
      this->SetContinue();
      return true;
   }
#endif
   return true;
}
