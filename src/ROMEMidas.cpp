// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEMidas
//
//  Interface to the Midas System.
//
//  $Log$
//  Revision 1.2  2005/01/27 16:21:06  schneebeli_m
//  print method & no gROME in path
//
//  Revision 1.1  2005/01/24 15:25:09  schneebeli_m
//  Seperated DAQ classes
//
//
//////////////////////////////////////////////////////////////////////////

#if defined( _MSC_VER )
#include <io.h>
#define O_RDONLY_BINARY O_RDONLY | O_BINARY
#endif
#if defined ( __linux__ ) || defined ( __APPLE__ )
#include <unistd.h>
#define O_RDONLY_BINARY O_RDONLY
#endif
#include <fcntl.h>
#include <ROMEMidas.h>

#if defined HAVE_MIDAS
#include <midas.h>
#define MIDAS_DEBUG // define if you want to run the analyzer in the debugger
void ProcessMessage(int hBuf, int id, EVENT_HEADER * pheader, void *message)
{
// This method is called, when a system message from the online system occurs
}
#endif

ROMEMidas::ROMEMidas() {
}

bool ROMEMidas::Initialize() {
   if (gROME->isOnline()) {
#if defined HAVE_MIDAS
      // Connect to the Frontend
      int requestId,i;

      gROME->Println("Program is running online.\n");

      // Connect to the experiment
      if (cm_connect_experiment(gROME->GetOnlineHost(), gROME->GetOnlineExperiment(),gROME->GetProgramName(), NULL) != SUCCESS) {
         gROME->Println("\nCannot connect to experiment");
         return false;
      }

      // open the "system" buffer, 1M size
      bm_open_buffer("SYSTEM", EVENT_BUFFER_SIZE, &fMidasOnlineBuffer);

      // set the buffer cache size
      bm_set_cache_size(fMidasOnlineBuffer, 100000, 0);

      // place a request for a specific event id
      bm_request_event(fMidasOnlineBuffer, 1, TRIGGER_ALL,GET_SOME, &requestId,NULL);

      // place a request for system messages
      cm_msg_register(ProcessMessage);

      // turn off watchdog if in debug mode
#ifdef MIDAS_DEBUG
      cm_set_watchdog_params(TRUE, 0);
#endif

      // Registers a callback function for run transitions.
      if (cm_register_transition(TR_START, NULL ,500) != CM_SUCCESS ||
         cm_register_transition(TR_STOP, NULL, 500) != CM_SUCCESS) {
         gROME->Println("\nCannot connect to experiment");
         return false;
      }

      // Connect to the online database
      if (cm_get_experiment_database(gROME->GetMidasOnlineDataBasePointer(), NULL)!= CM_SUCCESS) {
         gROME->Println("\nCannot connect to the online database");
         return false;
      }

      // Check Run Status
      int state = 0;
      int statesize = sizeof(state);
      if (db_get_value(gROME->GetMidasOnlineDataBase(),0,"/Runinfo/State",&state,&statesize,TID_INT,false)!= CM_SUCCESS) {
         gROME->Println("\nCannot read run status from the online database");
         return false;
      }
      if (state!=3) {
         this->SetBeginOfRun();
         this->SetStopped();
      }

      // Get Runnumber
      int runNumber = 0;
      int size = sizeof(runNumber);
      if (db_get_value(gROME->GetMidasOnlineDataBase(),0,"/Runinfo/Run number",&runNumber,&size,TID_INT,false)!= CM_SUCCESS) {
         gROME->Println("\nCannot read runnumber from the online database");
         return false;
      }
      gROME->SetCurrentRunNumber(runNumber);

      // Initialize the online database
      HNDLE hKey;
      ROMEString str;
      str = "//Trigger/Statistics";
      str.Insert(1,gROME->GetProgramName());
      // Trigger Statistics
      char *triggerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
      db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), triggerStatisticsString, TRUE);
      db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
      if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetTriggerStatistics(), sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         gROME->Println("\nCannot open trigger statistics record, probably other analyzer is using it");
         return false;
      }

      // Scaler Statistics
      char *fScalerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
      str="//Scaler/Statistics";
      str.Insert(1,gROME->GetProgramName());
      db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), fScalerStatisticsString, TRUE);
      db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
      if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetScalerStatistics(), sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         gROME->Println("\nCannot open scaler statistics record, probably other analyzer is using it");
         return false;
      }

      // Tree Switches
      for (i=0;i<gROME->GetTreeObjectEntries();i++) {
         str="//Tree switches/";
         str.Insert(1,gROME->GetProgramName());
         str.Append(gROME->GetTreeObjectAt(i)->GetTree()->GetName());
         db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), gROME->GetTreeObjectAt(i)->GetSwitchesString(), TRUE);
         db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
         if (db_set_record(gROME->GetMidasOnlineDataBase(),hKey,gROME->GetTreeObjectAt(i)->GetSwitches(),gROME->GetTreeObjectAt(i)->GetSwitchesSize(),0) != DB_SUCCESS) {
            gROME->Println("\nCannot write to tree switches record.");
            return false;
         }
         if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetTreeObjectAt(i)->GetSwitches(), gROME->GetTreeObjectAt(i)->GetSwitchesSize(), MODE_READ, NULL, NULL) != DB_SUCCESS) {
            gROME->Println("\nCannot open tree switches record, probably other analyzer is using it");
            return false;
         }
      }

      // Experiment dependent ODB initializations
      this->InitODB();

#else
      gROME->Println("Need Midas support for Online Mode !!");
      gROME->Println("Please link the midas library into this project.");
      return false;
#endif
   }
   else if (gROME->isOffline()) {
      gROME->Println("Program is running offline.\n");
   }
   return true;
}
bool ROMEMidas::Connect() {
   if (gROME->isOffline()) {
      // Open Midas File
      ROMEString runNumberString;
      gROME->GetCurrentRunNumberString(runNumberString);
      ROMEString filename;
      filename.SetFormatted("%srun%s.mid",gROME->GetInputDir(),runNumberString.Data());
      fMidasFileHandle = open(filename.Data(),O_RDONLY_BINARY);
      if (fMidasFileHandle==-1) {
         gROME->Print("Inputfile '");
         gROME->Print(filename.Data());
         gROME->Println("' not found.");
         return false;
      }
      gROME->Print("Reading Midas-File run");
      gROME->Print(runNumberString.Data());
      gROME->Println(".mid");
   }
   return true;
}
bool ROMEMidas::ReadEvent(int event) {
   if (gROME->isOnline()) {
#if defined HAVE_MIDAS
      int runNumber,trans;
      if (cm_query_transition(&trans, &runNumber, NULL)) {
         if (trans == TR_START) {
            gROME->SetCurrentRunNumber(runNumber);
            this->SetBeginOfRun();
            this->SetRunning();
            return true;
         }
         if (trans == TR_STOP) {
            this->SetEndOfRun();
            this->SetStopped();
            return true;
         }
      }
      int status = cm_yield(100);
      if (status == RPC_SHUTDOWN || status == SS_ABORT) {
         this->SetTerminate();
         return false;
      }
      if (this->isStopped()) {
         this->SetContinue();
         return true;
      }
      int size = gROME->GetRawDataEventSize();
      void* mEvent = gROME->GetRawDataEvent();
      status = bm_receive_event(fMidasOnlineBuffer, mEvent, &size, ASYNC);
      if (status != BM_SUCCESS) {
         this->SetContinue();
         return true;
      }

      gROME->SetCurrentEventNumber(((EVENT_HEADER*)mEvent)->serial_number);
      gROME->SetEventID(((EVENT_HEADER*)mEvent)->event_id);
      fTimeStamp = ((EVENT_HEADER*)mEvent)->time_stamp;
#if defined( __ppc__ )
      //byte swapping
      if(((EVENT_HEADER*)mEvent)->event_id != EVENTID_BOR &&
              ((EVENT_HEADER*)mEvent)->event_id != EVENTID_EOR &&
              ((EVENT_HEADER*)mEvent)->event_id != EVENTID_MESSAGE)
              bk_swap((EVENT_HEADER*)mEvent + 1, 0);
#endif
      gROME->InitMidasBanks();

#endif
   }
   else if (gROME->isOffline()) {
      // read event header
      EVENT_HEADER *pevent = (EVENT_HEADER*)gROME->GetRawDataEvent();
      bool readError = false;

      // read event
      int n = read(fMidasFileHandle,pevent, sizeof(EVENT_HEADER));
      
      if (n < (int)sizeof(EVENT_HEADER)) readError = true;
      else {
#if defined( __ppc__ )
         //byte swapping
         ByteSwap((UShort_t *)&pevent->event_id);
         ByteSwap((UShort_t *)&pevent->trigger_mask);
         ByteSwap((UInt_t   *)&pevent->serial_number);
         ByteSwap((UInt_t   *)&pevent->time_stamp);
         ByteSwap((UInt_t   *)&pevent->data_size);
#endif
         n = 0;
         if (pevent->data_size <= 0) readError = true;
         else {
            n = read(fMidasFileHandle, pevent+1, pevent->data_size);
            if (n != (int) pevent->data_size) readError = true;
//            if ((int) ((BANK_HEADER*)(pevent+1))->data_size <= 0) readError = true;
         }
      }
      // check input
      if (readError) {
         if (n > 0) gROME->Println("Unexpected end of file");
         this->SetEndOfRun();
         return true;
      }
      if (pevent->event_id < 0) {
         this->SetContinue();
         return true;
      }
      if (pevent->event_id == EVENTID_EOR) {
         this->SetEndOfRun();
         return true;
      }
#if defined( __ppc__ )
      //byte swapping
      if(pevent->event_id != EVENTID_BOR &&
         pevent->event_id != EVENTID_EOR &&
         pevent->event_id != EVENTID_MESSAGE)
         bk_swap(pevent + 1, 0);
#endif
      if (pevent->data_size<((BANK_HEADER*)(pevent+1))->data_size) {
         this->SetContinue();
         return true;
      }

      // initalize event
      gROME->SetEventID(pevent->event_id);
      gROME->SetCurrentEventNumber(pevent->serial_number);
      fTimeStamp = pevent->time_stamp;

      gROME->InitMidasBanks();
   }
   return true;
}
bool ROMEMidas::Disconnect() {
   if (gROME->isOffline()) {
      close(fMidasFileHandle);
   }
   return true;
}
bool ROMEMidas::Termination() {
   if (gROME->isOnline()) {
#if defined HAVE_MIDAS
      cm_disconnect_experiment();
#endif
   }
   return true;
}

