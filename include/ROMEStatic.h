#ifndef ROMEStatic_H
#define ROMEStatic_H
#include <TArrayI.h>
#include <stdio.h>

#if defined( _MSC_VER )
#include <windows.h>
#ifndef __CINT__
#include <conio.h>
#endif
#endif
#if defined ( __linux__ )
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#endif

#define ALIGN8(x)  (((x)+7) & ~7)

#include "ROME.h"

class ROMEStatic
{
public:
//********************************************************************
//    Routine: decodeRunNumbers
//********************************************************************
   static char* strtok_x(char *str,char* sub,char** tail) {
      int l = strlen(str);
      char* start = strtok(str,sub);
      *tail = str+strlen(str)+1;
      if (*tail>str+l) *tail = NULL;
      return start;
   }

/********************************************************************\
    Routine: ss_kbhit
\********************************************************************/
   static Bool_t ss_kbhit()
   {
#if defined( _MSC_VER )
      return toBool(kbhit());
#endif
#if defined ( __linux__ )
      int n;
      ioctl(0, FIONREAD, &n);
      return (n > 0);
#endif
   }



   static int ss_sleep(int millisec)
   {
      fd_set readfds;
      struct timeval timeout;
      int status;
      static int sock = 0;

      timeout.tv_sec = millisec / 1000;
      timeout.tv_usec = (millisec % 1000) * 1000;

      if (!sock)
         sock = socket(AF_INET, SOCK_DGRAM, 0);

      FD_ZERO(&readfds);
      FD_SET(sock, &readfds);
      do {
         status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

         /* if an alarm signal was cought, restart select with reduced timeout */
         if (status == -1 && timeout.tv_sec >= 1)
            timeout.tv_sec -= 1;

      } while (status == -1);      /* dont return if an alarm signal was cought */

      return 1;
   }

/********************************************************************\
   bk_find
\********************************************************************/

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
   static bool bk_is32(void *event)
   {
      return ((((BANK_HEADER *) event)->flags & (1<<4)) > 0);
   }
   static int bk_find(void* pbkh, const char *name, unsigned long * bklen, unsigned long * bktype,void *pdata)
   {
      int tid_size[] = {0,1,1,1,2,2,4,4,4,4,8,1,0,0,0,0,0};
      BANK *pbk;
      BANK32 *pbk32;
      unsigned long dname;

      if (bk_is32(pbkh)) {
         pbk32 = (BANK32 *) (((BANK_HEADER *)pbkh) + 1);
         strncpy((char *) &dname, name, 4);
         do {
            if (*((unsigned long *) pbk32->name) == dname) {
               *((void **) pdata) = pbk32 + 1;
               if (tid_size[pbk32->type & 0xFF] == 0)
                  *bklen = pbk32->data_size;
               else
                  *bklen = pbk32->data_size / tid_size[pbk32->type & 0xFF];
   
               *bktype = pbk32->type;
               return 1;
            }
            pbk32 = (BANK32 *) ((char *) (pbk32 + 1) + ALIGN8(pbk32->data_size));
         } while ((unsigned long) pbk32 - (unsigned long) pbkh < ((BANK_HEADER *) pbkh)->data_size + sizeof(BANK_HEADER));
      } else {
         pbk = (BANK *) (((BANK_HEADER *)pbkh) + 1);
         strncpy((char *) &dname, name, 4);
         do {
            if (*((unsigned long *) pbk->name) == dname) {
               *((void **) pdata) = pbk + 1;
               if (tid_size[pbk->type & 0xFF] == 0)
                  *bklen = pbk->data_size;
               else
                  *bklen = pbk->data_size / tid_size[pbk->type & 0xFF];
   
               *bktype = pbk->type;
               return 1;
            }
            pbk = (BANK *) ((char *) (pbk + 1) + ALIGN8(pbk->data_size));
         } while ((unsigned long) pbk - (unsigned long) pbkh < ((BANK_HEADER *) pbkh)->data_size + sizeof(BANK_HEADER));
      }
      *((void **) pdata) = NULL;
      return 0;
   }

#define CH_EXT 0x100

#define CH_HOME   (CH_EXT+0)
#define CH_INSERT (CH_EXT+1)
#define CH_DELETE (CH_EXT+2)
#define CH_END    (CH_EXT+3)
#define CH_PUP    (CH_EXT+4)
#define CH_PDOWN  (CH_EXT+5)
#define CH_UP     (CH_EXT+6)
#define CH_DOWN   (CH_EXT+7)
#define CH_RIGHT  (CH_EXT+8)
#define CH_LEFT   (CH_EXT+9)

   static int ss_getchar(bool reset)
   /********************************************************************\

   Routine: ss_getchar

   Purpose: Read a single character

   Input:
      BOOL   reset            Reset terminal to standard mode

   Output:
      <none>

   Function value:
      int             0       for no character available
                     CH_xxs  for special character
                     n       ASCII code for normal character
                     -1      function not available on this OS

   \********************************************************************/
   {
#ifdef OS_UNIX

      static BOOL init = FALSE;
      static struct termios save_termios;
      struct termios buf;
      int i, fd;
      char c[3];

      if (_daemon_flag)
         return 0;

      fd = fileno(stdin);

      if (reset) {
         if (init)
            tcsetattr(fd, TCSAFLUSH, &save_termios);
         init = FALSE;
         return 0;
      }

      if (!init) {
         tcgetattr(fd, &save_termios);
         memcpy(&buf, &save_termios, sizeof(buf));

         buf.c_lflag &= ~(ECHO | ICANON | IEXTEN);

         buf.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON);

         buf.c_cflag &= ~(CSIZE | PARENB);
         buf.c_cflag |= CS8;
         /* buf.c_oflag &= ~(OPOST); */
         buf.c_cc[VMIN] = 0;
         buf.c_cc[VTIME] = 0;

         tcsetattr(fd, TCSAFLUSH, &buf);
         init = TRUE;
      }

      memset(c, 0, 3);
      i = read(fd, c, 1);

      if (i == 0)
         return 0;

      /* check if ESC */
      if (c[0] == 27) {
         i = read(fd, c, 2);
         if (i == 0)               /* return if only ESC */
            return 27;

         /* cursor keys return 2 chars, others 3 chars */
         if (c[1] < 65)
            read(fd, c, 1);

         /* convert ESC sequence to CH_xxx */
         switch (c[1]) {
         case 49:
            return CH_HOME;
         case 50:
            return CH_INSERT;
         case 51:
            return CH_DELETE;
         case 52:
            return CH_END;
         case 53:
            return CH_PUP;
         case 54:
            return CH_PDOWN;
         case 65:
            return CH_UP;
         case 66:
            return CH_DOWN;
         case 67:
            return CH_RIGHT;
         case 68:
            return CH_LEFT;
         }
      }

      /* BS/DEL -> BS */
      if (c[0] == 127)
         return CH_BS;

      return c[0];

#elif defined(OS_WINNT)

      static bool init = false;
      static int repeat_count = 0;
      static int repeat_char;
      HANDLE hConsole;
      unsigned long nCharsRead;
      INPUT_RECORD ir;
      OSVERSIONINFO vi;

      /* find out if we are under W95 */
      vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      GetVersionEx(&vi);

      if (vi.dwPlatformId != VER_PLATFORM_WIN32_NT) {
         /* under W95, console doesn't work properly */
         int c;

         if (!kbhit())
            return 0;

         c = getch();
         if (c == 224) {
            c = getch();
            switch (c) {
            case 71:
               return CH_HOME;
            case 72:
               return CH_UP;
            case 73:
               return CH_PUP;
            case 75:
               return CH_LEFT;
            case 77:
               return CH_RIGHT;
            case 79:
               return CH_END;
            case 80:
               return CH_DOWN;
            case 81:
               return CH_PDOWN;
            case 82:
               return CH_INSERT;
            case 83:
               return CH_DELETE;
            }
         }
         return c;
      }

      hConsole = GetStdHandle(STD_INPUT_HANDLE);

      if (reset) {
         SetConsoleMode(hConsole, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT |
                        ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
         init = FALSE;
         return 0;
      }

      if (!init) {
         SetConsoleMode(hConsole, ENABLE_PROCESSED_INPUT);
         init = TRUE;
      }

      if (repeat_count) {
         repeat_count--;
         return repeat_char;
      }

      PeekConsoleInput(hConsole, &ir, 1, &nCharsRead);

      if (nCharsRead == 0)
         return 0;

      ReadConsoleInput(hConsole, &ir, 1, &nCharsRead);

      if (ir.EventType != KEY_EVENT)
         return ss_getchar(0);

      if (!ir.Event.KeyEvent.bKeyDown)
         return ss_getchar(0);

      if (ir.Event.KeyEvent.wRepeatCount > 1) {
         repeat_count = ir.Event.KeyEvent.wRepeatCount - 1;
         repeat_char = ir.Event.KeyEvent.uChar.AsciiChar;
         return repeat_char;
      }

      if (ir.Event.KeyEvent.uChar.AsciiChar)
         return ir.Event.KeyEvent.uChar.AsciiChar;

      if (ir.Event.KeyEvent.dwControlKeyState & (ENHANCED_KEY)) {
         switch (ir.Event.KeyEvent.wVirtualKeyCode) {
         case 33:
            return CH_PUP;
         case 34:
            return CH_PDOWN;
         case 35:
            return CH_END;
         case 36:
            return CH_HOME;
         case 37:
            return CH_LEFT;
         case 38:
            return CH_UP;
         case 39:
            return CH_RIGHT;
         case 40:
            return CH_DOWN;
         case 45:
            return CH_INSERT;
         case 46:
            return CH_DELETE;
         }

         return ir.Event.KeyEvent.wVirtualKeyCode;
      }

      return ss_getchar(0);

#elif defined(OS_MSDOS)

      int c;

      if (!kbhit())
         return 0;

      c = getch();
      if (!c) {
         c = getch();
         switch (c) {
         case 71:
            return CH_HOME;
         case 72:
            return CH_UP;
         case 73:
            return CH_PUP;
         case 75:
            return CH_LEFT;
         case 77:
            return CH_RIGHT;
         case 79:
            return CH_END;
         case 80:
            return CH_DOWN;
         case 81:
            return CH_PDOWN;
         case 82:
            return CH_INSERT;
         case 83:
            return CH_DELETE;
         }
      }
      return c;

#else
      return -1;
#endif
   }
};


#endif   // ROMEStatic_H
