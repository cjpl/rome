/********************************************************************
  ROME.h, M. Schneebeli PSI

  $Log$
  Revision 1.4  2004/10/14 09:53:41  schneebeli_m
  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag

  Revision 1.3  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROME_H
#define ROME_H

#if defined( _MSC_VER )
#define OS_WINNT
#endif

#if defined(OS_WINNT) && defined(ROMEDLL_EXPORTS)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/**
toBool */
#ifndef toBool
#define toBool(a)            (a != 0)
#endif

// midas
#ifndef HAVE_MIDAS
#define EVENTID_BOR      ((short) 0x8000)  /**< Begin-of-run      */
#define EVENTID_EOR      ((short) 0x8001)  /**< End-of-run        */
#define EVENTID_MESSAGE  ((short) 0x8002)  /**< Message events    */
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
#endif

// General
#define gDebugLevel 0

#define gShowTime 1
#define gPrintProgress 0

#define gFileNameLength 300

#define gTaskInit       "init"
#define gTaskBeginOfRun "bof"
#define gTaskEvent      "evt"
#define gTaskEndOfRun   "eof"
#define gTaskTerminate  "term"

#endif   // ROME_H
