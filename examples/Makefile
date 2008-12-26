-include ../Makefile.arch

RESTRICT_TEST = yes
MEMORY_TEST   = no

# parallel job option
#JOBSFLAG = -j2

ifeq ($(MEMORY_TEST), yes)
   VALGRIND ?= valgrind --suppressions=${ROOTSYS}/etc/valgrind-root.supp --tool=memcheck --leak-check=yes
else
   VALGRIND =
endif

ifeq ($(RESTRICT_TEST), yes)
OPT = -Wall -W -Werror
endif
export DOOPT    =$(OPT)
export FLOPT    =$(OPT)
export FOOPT    =$(OPT)
export HGOPT    =$(OPT)
export HGUIOPT  =$(OPT)
export HWOPT    =$(OPT)
export MCROPT   =$(OPT)
export MROPT    =$(OPT)
export MIDOPT   =$(OPT)
export NFOPT    =$(OPT)
export PMOPT    =$(OPT)
export RDAQOPT  =$(OPT)
export RDBOPT   =$(OPT)
export RUNHOPT  =$(OPT)
export SBOPT    =$(OPT)
export SBSOPT   =$(OPT)
export SBSHOPT  =$(OPT)
export SDAQOPT  =$(OPT)
export SMDOPT   =$(OPT)
export TBOPT    =$(OPT)
export THOPT    =$(OPT)
export THHOPT   =$(OPT)
export TMOPT    =$(OPT)
export TTOPT    =$(OPT)
export TWROPT   =$(OPT)
export TWR2OPT  =$(OPT)
export TWR3OPT  =$(OPT)
export TCUTGOPT =$(OPT)
export BFOPT    =$(OPT)
export MSGOPT   =$(OPT)
export DOCOPT   =$(OPT)
export REFOPT   =$(OPT)

ROMEProjects = multirun/mranalyzer.exe \
               dbexample/rdbframework.exe \
               taskhierarchy/thexample.exe \
               netfolder/nfexample.exe \
               stepbystep/sbsanalyzer.exe \
               tree_write_read/twrsample.exe \
               tree_write_read2/twr2sample.exe \
               tree_write_read3/twr3sample.exe \
               stepbystephisto/sbshanalyzer.exe \
               histoGUI/hguiexample.exe \
               stepbystepdbdaq/sdaqanalyzer.exe \
               stepbystepmydaq/smdanalyzer.exe \
               runheader/runhanalyzer.exe \
               rootDAQ/rdaqanalyzer.exe \
               displayobjects/doexample.exe \
               taskhierarchyandhistos/thhexample.exe \
               taskhierarchydependence/thexample.exe \
               macros/mcranalyzer.exe \
               cutg/tcutgexample.exe \
               basefolder/bfsample.exe \
               message/msganalyzer.exe \
               document/docanalyzer.exe \
               reference/refanalyzer.exe

ARGUSProjects = argus/file/flmonitor.exe \
                argus/timer/ttmonitor.exe \
                argus/processMessage/pmmonitor.exe \
                argus/menu/tmmonitor.exe \
                argus/thread/ttmonitor.exe \
                argus/tabs/tbmonitor.exe \
                argus/helloWorld/hwmonitor.exe \
                argus/histogram/romeproject/hgframework.exe \
                argus/histogram/hgmonitor.exe
ifdef MIDASSYS
ROMEProjects  += midas/midanalyzer.exe
ARGUSProjects += argus/folders/romeproject/foframework.exe \
                 argus/folders/fomonitor.exe
endif

all: rome argus dbwrapper
rome: $(ROMEProjects)
argus: $(ARGUSProjects)

dbwrapper :
	make -C romedbwrapper

multirun/mranalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i multirun/multirun.xml -o multirun -nl -pch;
	make $(JOBSFLAG) -C multirun/

dbexample/rdbframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i dbexample/RDBframework.xml -o dbexample -nl -pch -mysql -sqlite3;
	make $(JOBSFLAG) -C dbexample/

taskhierarchy/thexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i taskhierarchy/taskhierarchy.xml -o taskhierarchy -nl -pch;
	make $(JOBSFLAG) -C taskhierarchy/

taskhierarchydependence/thexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i taskhierarchydependence/taskhierarchy.xml -o taskhierarchydependence -nl -pch;
	make $(JOBSFLAG) -C taskhierarchydependence/

netfolder/nfexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i netfolder/netfolder.xml -o netfolder -nl -pch;
	make $(JOBSFLAG) -C netfolder/

stepbystep/sbsanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystep/stepbystep.xml -o stepbystep -nl -pch;
	make $(JOBSFLAG) -C stepbystep/

tree_write_read/twrsample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i tree_write_read/tree_write_read.xml -o tree_write_read -nl -pch;
	make $(JOBSFLAG) -C tree_write_read/

tree_write_read2/twr2sample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i tree_write_read2/tree_write_read2.xml -o tree_write_read2 -nl -pch;
	make $(JOBSFLAG) -C tree_write_read2/

tree_write_read3/twr3sample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i tree_write_read3/tree_write_read3.xml -o tree_write_read3 -nl -pch;
	make $(JOBSFLAG) -C tree_write_read3/

stepbystephisto/sbshanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystephisto/stepbystephisto.xml -o stepbystephisto -nl -pch;
	make $(JOBSFLAG) -C stepbystephisto/

histoGUI/hguiexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i histoGUI/histoGUI.xml -o histoGUI -nl -pch;
	make $(JOBSFLAG) -C histoGUI/

stepbystepdbdaq/sdaqanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystepdbdaq/stepbystepdbdaq.xml -o stepbystepdbdaq -nl -pch;
	make $(JOBSFLAG) -C stepbystepdbdaq/

stepbystepmydaq/smdanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystepmydaq/stepbystepmydaq.xml -o stepbystepmydaq -nl -pch;
	make $(JOBSFLAG) -C stepbystepmydaq/

runheader/runhanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i runheader/runheader.xml -o runheader -nl -pch;
	make $(JOBSFLAG) -C runheader/

rootDAQ/rdaqanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i rootDAQ/rootDAQ.xml -o rootDAQ -nl -pch;
	make $(JOBSFLAG) -C rootDAQ/

midas/midanalyzer.exe:
	$(ROMESYS)/bin/romebuilder.exe -i midas/midas.xml -o midas -nl -pch -midas;
	make $(JOBSFLAG) -C midas/

taskhierarchyandhistos/thhexample.exe:
	$(ROMESYS)/bin/romebuilder.exe -i taskhierarchyandhistos/taskhierarchyandhistos.xml -o taskhierarchyandhistos -nl -pch;
	make $(JOBSFLAG) -C taskhierarchyandhistos/

displayobjects/doexample.exe:
	$(ROMESYS)/bin/romebuilder.exe -i displayobjects/displayObjects.xml -o displayobjects -nl -pch;
	make $(JOBSFLAG) -C displayobjects/

macros/mcranalyzer.exe:
	$(ROMESYS)/bin/romebuilder.exe -i macros/macros.xml -o macros -nl -pch;
	make $(JOBSFLAG) -C macros/

cutg/tcutgexample.exe:
	$(ROMESYS)/bin/romebuilder.exe -i cutg/cutg.xml -o cutg -nl -pch;
	make $(JOBSFLAG) -C cutg/

basefolder/bfsample.exe:
	$(ROMESYS)/bin/romebuilder.exe -i basefolder/basefolder.xml -o basefolder -nl -pch;
	make $(JOBSFLAG) -C basefolder/

message/msganalyzer.exe:
	$(ROMESYS)/bin/romebuilder.exe -i message/message.xml -o message -nl -pch;
	make $(JOBSFLAG) -C message/

document/docanalyzer.exe:
	$(ROMESYS)/bin/romebuilder.exe -i document/document.xml -o document -nl -pch;
	make $(JOBSFLAG) -C document/

reference/refanalyzer.exe:
	$(ROMESYS)/bin/romebuilder.exe -i reference/reference.xml -o reference -nl -pch;
	make $(JOBSFLAG) -C reference/

argus/file/flmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/file/file.xml -o argus/file -nl -pch 
	make $(JOBSFLAG) -C argus/file/

argus/timer/ttmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/timer/timer.xml -o argus/timer -nl -pch 
	make $(JOBSFLAG) -C argus/timer/

argus/processMessage/pmmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/processMessage/processMessage.xml -o argus/processMessage -nl -pch
	make $(JOBSFLAG) -C argus/processMessage/

argus/menu/tmmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/menu/menu.xml -o argus/menu -nl -pch
	make $(JOBSFLAG) -C argus/menu/

argus/thread/ttmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/thread/thread.xml -o argus/thread -nl -pch
	make $(JOBSFLAG) -C argus/thread/

argus/tabs/tbmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/tabs/tabs.xml -o argus/tabs -nl -pch
	make $(JOBSFLAG) -C argus/tabs/

argus/helloWorld/hwmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/helloWorld/HelloWorld.xml -o argus/helloWorld -nl -pch
	make $(JOBSFLAG) -C argus/helloWorld/

argus/histogram/romeproject/hgframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/histogram/romeproject/histogram.xml -o argus/histogram/romeproject -nl -pch
	make $(JOBSFLAG) -C argus/histogram/romeproject/

argus/histogram/hgmonitor.exe : argus/histogram/romeproject/hgframework.exe
	$(ROMESYS)/bin/romebuilder.exe -i argus/histogram/histogram.xml -o argus/histogram -nl -pch
	make $(JOBSFLAG) -C argus/histogram/

argus/folders/romeproject/foframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/folders/romeproject/FOframework.xml -o argus/folders/romeproject -nl -pch -midas
	make $(JOBSFLAG) -C argus/folders/romeproject/

argus/folders/fomonitor.exe : argus/folders/romeproject/foframework.exe
	$(ROMESYS)/bin/romebuilder.exe -i argus/folders/folders.xml -o argus/folders -nl -pch -midas
	make $(JOBSFLAG) -C argus/folders/

check : remove $(ROMEProjects) $(ARGUSProjects) dbwrapper

	cd romedbwrapper; $(VALGRIND) ./test_c; $(VALGRIND) ./test_f

	cd multirun; $(VALGRIND) ./mranalyzer.exe -i romeConfig.xml -q -nc

	cd dbexample; \
	cp sqlDataBase/RDBDataBase.sqlite3 .; \
	$(VALGRIND) ./rdbframework.exe -i sqlite3Config.xml -q -r 1 -nc; \
	mv -f RDBDataBase.sqlite3 sqlDataBase/; \

	cd taskhierarchy; $(VALGRIND) ./thexample.exe -i romeConfig.xml -q -e 0-9999 -r 1 -nc

	cd netfolder; $(VALGRIND) ./nfexample.exe -i singleProcessConfig.xml -q -e 0-19 -nc

	cd stepbystep;	$(VALGRIND) ./sbsanalyzer.exe -i romeConfig.xml -q -nc

	cd tree_write_read; $(VALGRIND) ./twrsample.exe -i writeTreeConfig.xml -q -nc
	cd tree_write_read; $(VALGRIND) ./twrsample.exe -i writeTreeWithFileName.xml -q -nc
	cd tree_write_read; $(VALGRIND) ./twrsample.exe -i readTreeConfig.xml -q -nc
	cd tree_write_read; $(VALGRIND) ./twrsample.exe -i readTreeWithFileName.xml -q -nc

	cd tree_write_read2; $(VALGRIND) ./twr2sample.exe -i writeTreeConfig.xml -q -e 0-9 -nc
	cd tree_write_read2; $(VALGRIND) ./twr2sample.exe -i readTreeConfig.xml -q -nc

	cd tree_write_read3; $(VALGRIND) ./twr3sample.exe -i writeTreeConfig.xml -q -e 0-9 -nc
	cd tree_write_read3; $(VALGRIND) ./twr3sample.exe -i readTreeConfig.xml -q -nc

	cd stepbystephisto; $(VALGRIND) ./sbshanalyzer.exe -i romeConfig.xml -q -e 0-99 -nc

	cd histoGUI; $(VALGRIND) ./hguiexample.exe -i romeConfig.xml -q -e 0-9 -nc
	cd histoGUI; $(VALGRIND) ./hguiexample.exe -i histoReadConfig.xml -q -e 0-9 -nc

	cd stepbystepdbdaq; $(VALGRIND) ./sdaqanalyzer.exe -i xmlConfig.xml -q -nc

	cd stepbystepmydaq; $(VALGRIND) ./smdanalyzer.exe -i romeConfig.xml -q -nc -e 0-99

	cd runheader; $(VALGRIND) ./runhanalyzer.exe -i writeConfig.xml -q -b -nc && \
	$(VALGRIND) ./runhanalyzer.exe -i readConfig.xml -q -nc

	cd rootDAQ; root -b -q treeCreationMakro.C+; $(VALGRIND) ./rdaqanalyzer.exe -i romeConfig.xml -q -nc

	cd midas; $(VALGRIND) ./midanalyzer.exe -i romeConfig.xml -q -m offline -nc -e 0-99

	cd taskhierarchyandhistos; $(VALGRIND) ./thhexample.exe -i romeConfig.xml -q -e 0-99 -r 1 -nc

	cd taskhierarchydependence; $(VALGRIND) ./thexample.exe -i romeConfig.xml -q -e 0-99 -r 1 -nc
	cd taskhierarchydependence; $(VALGRIND) ./thexample.exe -i romeConfig2nd.xml -q -r 1 -ng

#	cd displayobjects; ./doexample.exe -i romeConfig.xml -q -e 0-99 -r 1 -nc

	cd macros; $(VALGRIND) ./mcranalyzer.exe -i romeConfig.xml -q -nc

	cd cutg; $(VALGRIND) ./tcutgexample.exe -i romeConfig.xml -q -e 0-9 -nc

	cd basefolder; $(VALGRIND) ./bfsample.exe -i romeConfig.xml -q -nc

	cd message; $(VALGRIND) ./msganalyzer.exe -i romeConfig.xml -q -nc

	cd reference; $(VALGRIND) ./refanalyzer.exe -i writeConfig.xml -q -nc
	cd reference; $(VALGRIND) ./refanalyzer.exe -i readConfig.xml -q -nc

	cd argus/file; $(VALGRIND) ./flmonitor.exe -i romeConfig.xml -q -e 0-99 -nc

	cd argus/timer; $(VALGRIND) ./ttmonitor.exe -i romeConfig.xml -q -nc & \
	sleep 10; killall -9 ttmonitor.exe

	cd argus/processMessage; $(VALGRIND) ./pmmonitor.exe -i romeConfig.xml -q -nc & \
	sleep 10; killall -9 pmmonitor.exe

	cd argus/menu; $(VALGRIND) ./tmmonitor.exe -i romeConfig.xml -q -nc & \
	sleep 10; killall -9 tmmonitor.exe

	cd argus/thread; $(VALGRIND) ./ttmonitor.exe -i romeConfig.xml -q -nc & \
	sleep 10; killall -9 ttmonitor.exe

	cd argus/tabs;	$(VALGRIND) ./tbmonitor.exe -i romeConfig.xml -q -nc & \
	sleep 10; killall -9 tbmonitor.exe

	cd argus/helloWorld; $(VALGRIND) ./hwmonitor.exe -i romeConfig.xml -q -nc & \
	sleep 10; killall -9 hwmonitor.exe

	cd argus/histogram/romeproject; $(VALGRIND) ./hgframework.exe -i romeConfig.xml -b & \
	cd ..; $(VALGRIND) ./hgmonitor.exe -i argusConfig.xml -q -p 3 -nc & sleep 10; killall -9 hgmonitor.exe; \
	killall -9 hgframework.exe

	cd argus/folders/romeproject; $(VALGRIND) ./foframework.exe -i romeConfig.xml -b -r 1 & \
	cd ..; $(VALGRIND) ./fomonitor.exe -i argusConfig.xml -q -p 3 -nc & sleep 10; killall -9 fomonitor.exe; \
	killall -9 foframework.exe

clean :
	find $(ROMESYS)/examples -name "*.o" | xargs $(RM)
	find $(ROMESYS)/examples -name "*.so" | xargs $(RM)
	find $(ROMESYS)/examples -name "*.gch" | xargs $(RM)
	find $(ROMESYS)/examples -name "*Dict*.cpp" | xargs $(RM)
	find $(ROMESYS)/examples -name "*Dict*.h" | xargs $(RM)
	$(MAKE) -C romedbwrapper clean

depclean :
	find $(ROMESYS)/examples -name "*.d" -exec $(RM) {} \;

remove :
	$(RM) $(ROMEProjects) $(ARGUSProjects) romedbwrapper/test_c romedbwrapper/test_f

