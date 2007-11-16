      program main

      integer*8    sqldb
      integer*8    textdb
      integer      ret
      integer      int_value
      integer      int_value2
      character*80 str_value
      integer      irow

ccccc SQL ccccc
c     establish connection to a database
      call romedb_init('mysqldb', 1,
     $     'sqlite3://./RDBDataBase.sqlite3', sqldb)
      if (sqldb .eq. 0) then
         write(*,*) 'database connection error'
      else

c     read an integer
         call romedb_read(sqldb, '/AAA[id=1]/BBB/bbb',
     $        0, 0, ret)
         call romedb_get_int_value(sqldb, 1, 1, int_value)
         write(*,*) int_value

c     read a string
         call romedb_read(sqldb, '/AAA[id=1]/CCC[idx=0]/ccc',
     $        0, 0, ret)
         call romedb_get_string_value(sqldb, 1, 1, str_value)
         write(*,*) str_value

c     read array
         call romedb_read(sqldb, '/III[id=0](0,2)/JJJ_idx;III/idx(0,2)',
     $        0, 0, ret)
         do irow = 1, 3
            call romedb_get_int_value(sqldb, irow, 1, int_value)
            write(*,*) int_value
         enddo

c     disconnect and free memory of the working space
         call romedb_disconnect(sqldb)

      endif

ccccc Text ccccc
c     establish connection to a database
      call romedb_init('mytextdb', 2, './', textdb)
      if (textdb .eq. 0) then
         write(*,*) 'database connection error'
      else

c     read an integer
         call romedb_read(textdb, 'text.db/val1',
     $        0, 0, ret)
         call romedb_get_int_value(textdb, 1, 1, int_value)
         write(*,*) int_value

c     read a string
         call romedb_read(textdb, 'text.db/val2',
     $        0, 0, ret)
         call romedb_get_string_value(textdb, 1, 1, str_value)
         write(*,*) str_value

c     read array
         call romedb_read(textdb, 'text.db/val3',
     $        0, 0, ret)
         do irow = 1, 10
            call romedb_get_int_value(textdb, irow, 1, int_value)
            call romedb_get_int_value(textdb, irow, 2, int_value2)
            write(*,*) int_value, int_value2
         enddo

c     disconnect and free memory of the working space
         call romedb_disconnect(textdb)

      endif

      stop
      end
