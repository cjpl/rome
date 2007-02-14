      program main

      integer romedb
      integer ret
      integer int_value;
      character*80 str_value;

c     establish connection to a database
      call romesqldb_init('mydb\0',
     $     'sqlite3://./RDBDataBase.sqlite3\0', romedb);
      if (romedb .eq. 0) then
         write(*,*) 'database connection error'
      else

c     read an integer
         call romesqldb_read(romedb, '/AAA[id=1]/BBB/bbb\0',
     $        0, 0, ret)
         call romesqldb_get_int_value(romedb, 0, 0, int_value)
         write(*,*) int_value

c     read a string
         call romesqldb_read(romedb, '/AAA[id=1]/CCC[idx=0]/ccc\0',
     $        0, 0, ret)
         call romesqldb_get_string_value(romedb, 0, 0, str_value)
         write(*,*) str_value

c     disconnect and free memory of the working space
         call romesqldb_disconnect(romedb)

      endif

      stop
      end
