/********************************************************************
  ROME.h, M. Schneebeli PSI

  $Log$
  Revision 1.6  2005/02/21 21:29:07  sawada
  Changed OS specifying macros
  Support for DEC,Ultrix,FreeBSD,Solaris

  Revision 1.5  2004/12/02 17:46:43  sawada
  Macintosh port

  Revision 1.4  2004/10/14 09:53:41  schneebeli_m
  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag

  Revision 1.3  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROME_H
#define ROME_H
#include <Rtypes.h>

#if defined( R__VISUAL_CPLUSPLUS )
#define OS_WINNT
#endif

#if defined( OS_WINNT ) && defined( ROMEDLL_EXPORTS )
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
#define ALIGN8(x)  (((x)+7) & ~7)
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
