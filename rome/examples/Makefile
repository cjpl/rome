-include ../Makefile.arch

RESTRICT_TEST = yes

# parallel job option
#JOBSFLAG = -j2

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

ROMEProjects = multirun/mranalyzer.exe \
               dbexample/rdbframework.exe \
               taskhierarchy/thexample.exe \
               netfolder/nfexample.exe \
               stepbystep/sbsanalyzer.exe \
               tree_write_read/twrsample.exe \
               stepbystephisto/sbshanalyzer.exe \
               histoGUI/hguiexample.exe \
               stepbystepdbdaq/sdaqanalyzer.exe \
               stepbystepmydaq/smdanalyzer.exe \
               runheader/runhanalyzer.exe \
               rootDAQ/rdaqanalyzer.exe \
               displayobjects/doexample.exe \
               taskhierarchyandhistos/thhexample.exe \
               taskhierarchydependence/thexample.exe \
               macros/mcranalyzer.exe

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
ifeq ($(OSTYPE),linux)
	make -C romedbwrapper
endif

multirun/mranalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i multirun/multirun.xml -o multirun -nl;
	make $(JOBSFLAG) -C multirun/

dbexample/rdbframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i dbexample/RDBframework.xml -o dbexample -nl -mysql -sqlite3;
	make $(JOBSFLAG) -C dbexample/

taskhierarchy/thexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i taskhierarchy/taskhierarchy.xml -o taskhierarchy -nl;
	make $(JOBSFLAG) -C taskhierarchy/

taskhierarchydependence/thexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i taskhierarchydependence/taskhierarchy.xml -o taskhierarchydependence -nl;
	make $(JOBSFLAG) -C taskhierarchydependence/

netfolder/nfexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i netfolder/netfolder.xml -o netfolder -nl;
	make $(JOBSFLAG) -C netfolder/

stepbystep/sbsanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystep/stepbystep.xml -o stepbystep -nl;
	make $(JOBSFLAG) -C stepbystep/

tree_write_read/twrsample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i tree_write_read/tree_write_read.xml -o tree_write_read -nl;
	make $(JOBSFLAG) -C tree_write_read/

stepbystephisto/sbshanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystephisto/stepbystephisto.xml -o stepbystephisto -nl;
	make $(JOBSFLAG) -C stepbystephisto/

histoGUI/hguiexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i histoGUI/histoGUI.xml -o histoGUI -nl;
	make $(JOBSFLAG) -C histoGUI/

stepbystepdbdaq/sdaqanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystepdbdaq/stepbystepdbdaq.xml -o stepbystepdbdaq -nl;
	make $(JOBSFLAG) -C stepbystepdbdaq/

stepbystepmydaq/smdanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystepmydaq/stepbystepmydaq.xml -o stepbystepmydaq -nl;
	make $(JOBSFLAG) -C stepbystepmydaq/

runheader/runhanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i runheader/runheader.xml -o runheader -nl;
	make $(JOBSFLAG) -C runheader/

rootDAQ/rdaqanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i rootDAQ/rootDAQ.xml -o rootDAQ -nl;
	make $(JOBSFLAG) -C rootDAQ/

midas/midanalyzer.exe:
	$(ROMESYS)/bin/romebuilder.exe -i midas/midas.xml -o midas -nl -midas;
	make $(JOBSFLAG) -C midas/

taskhierarchyandhistos/thhexample.exe:
	$(ROMESYS)/bin/romebuilder.exe -i taskhierarchyandhistos/taskhierarchyandhistos.xml -o taskhierarchyandhistos -nl;
	make $(JOBSFLAG) -C taskhierarchyandhistos/

displayobjects/doexample.exe:
	$(ROMESYS)/bin/romebuilder.exe -i displayobjects/displayObjects.xml -o displayobjects -nl;
	make $(JOBSFLAG) -C displayobjects/

macros/mcranalyzer.exe:
	$(ROMESYS)/bin/romebuilder.exe -i macros/macros.xml -o macros -nl;
	make $(JOBSFLAG) -C macros/

argus/file/flmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/file/file.xml -o argus/file -nl 
	make $(JOBSFLAG) -C argus/file/

argus/timer/ttmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/timer/timer.xml -o argus/timer -nl 
	make $(JOBSFLAG) -C argus/timer/

argus/processMessage/pmmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/processMessage/processMessage.xml -o argus/processMessage -nl
	make $(JOBSFLAG) -C argus/processMessage/

argus/menu/tmmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/menu/menu.xml -o argus/menu -nl
	make $(JOBSFLAG) -C argus/menu/

argus/thread/ttmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/thread/thread.xml -o argus/thread -nl
	make $(JOBSFLAG) -C argus/thread/

argus/tabs/tbmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/tabs/tabs.xml -o argus/tabs -nl
	make $(JOBSFLAG) -C argus/tabs/

argus/helloWorld/hwmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/helloWorld/HelloWorld.xml -o argus/helloWorld -nl
	make $(JOBSFLAG) -C argus/helloWorld/

argus/histogram/romeproject/hgframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/histogram/romeproject/histogram.xml -o argus/histogram/romeproject -nl
	make $(JOBSFLAG) -C argus/histogram/romeproject/

argus/histogram/hgmonitor.exe : argus/histogram/romeproject/hgframework.exe
	$(ROMESYS)/bin/romebuilder.exe -i argus/histogram/histogram.xml -o argus/histogram -nl
	make $(JOBSFLAG) -C argus/histogram/

argus/folders/romeproject/foframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/folders/romeproject/FOframework.xml -o argus/folders/romeproject -nl -midas
	make $(JOBSFLAG) -C argus/folders/romeproject/

argus/folders/fomonitor.exe : argus/folders/romeproject/foframework.exe
	$(ROMESYS)/bin/romebuilder.exe -i argus/folders/folders.xml -o argus/folders -nl -midas
	make $(JOBSFLAG) -C argus/folders/

check : $(ROMEProjects) $(ARGUSProjects) dbwrapper

	cd romedbwrapper; ./test_c; ./test_f

	cd multirun; ./mranalyzer.exe -i romeConfig.xml -q

	cd dbexample; \
	cp sqlDataBase/RDBDataBase.sqlite3 .; \
	./rdbframework.exe -i sqlite3Config.xml -q -r 1; \
	mv -f RDBDataBase.sqlite3 sqlDataBase/; \

	cd taskhierarchy; ./thexample.exe -i romeConfig.xml -q -e 0-9999 -r 1

	cd netfolder; ./nfexample.exe -i singleProcessConfig.xml -q -e 0-19

	cd stepbystep;	./sbsanalyzer.exe -i romeConfig.xml -q

	cd tree_write_read; ./twrsample.exe -i writeTreeConfig.xml -q
	cd tree_write_read; ./twrsample.exe -i writeTreeWithFileName.xml -q
	cd tree_write_read; ./twrsample.exe -i readTreeConfig.xml -q
	cd tree_write_read; ./twrsample.exe -i readTreeWithFileName.xml -q

	cd stepbystephisto; ./sbshanalyzer.exe -i romeConfig.xml -q -e 0-99

#	cd histoGUI; ./hguiexample.exe -i romeConfig.xml -q -e 0-9 & \
#	./hguiexample.exe -i histoReadConfig.xml -q -e 0-9

	cd stepbystepdbdaq; ./sdaqanalyzer.exe -i xmlConfig.xml -q

	cd stepbystepmydaq; ./smdanalyzer.exe -i romeConfig.xml -q

	cd runheader; ./runhanalyzer.exe -i writeConfig.xml -q -b && \
	./runhanalyzer.exe -i readConfig.xml -q

	cd rootDAQ; root -b -q treeCreationMakro.C+; ./rdaqanalyzer.exe -i romeConfig.xml -q

#	cd midas; ./midanalyzer.exe -i romeConfig.xml -q

	cd taskhierarchyandhistos; ./thhexample.exe -i romeConfig.xml -q -e 0-99 -r 1

	cd taskhierarchydependence; ./thexample.exe -i romeConfig.xml -q -e 0-99 -r 1

#	cd displayobjects; ./doexample.exe -i romeConfig.xml -q -e 0-99 -r 1

	cd macros; ./mcranalyzer.exe -i romeConfig.xml -q

	cd argus/file; ./flmonitor.exe -i romeConfig.xml -q -e 0-99

	cd argus/timer; ./ttmonitor.exe -i romeConfig.xml -q

	cd argus/processMessage; ./pmmonitor.exe -i romeConfig.xml -q

	cd argus/menu; ./tmmonitor.exe -i romeConfig.xml -q

	cd argus/thread; ./ttmonitor.exe -i romeConfig.xml -q

	cd argus/tabs;	./tbmonitor.exe -i romeConfig.xml -q

	cd argus/helloWorld; ./hwmonitor.exe -i romeConfig.xml -q

	cd argus/histogram/romeproject; ./hgframework.exe -i romeConfig.xml -q && \
	cd ..; ./hgmonitor.exe -i argusConfig.xml  -q

	cd argus/folders/romeproject; ./foframework.exe -i romeConfig.xml -q -r 1 && \
	cd ..; ./fomonitor.exe -i argusConfig.xml -q

clean :
	find $(ROMESYS)/examples -name "*.o" | xargs $(RM)
	find $(ROMESYS)/examples -name "*Dict*.cpp" | xargs $(RM)
	find $(ROMESYS)/examples -name "*Dict*.h" | xargs $(RM)
	$(MAKE) -C romedbwrapper clean

depclean :
	find $(ROMESYS)/examples -name "*.d" -exec $(RM) {} \;

remove :
	$(RM) $(ROMEProjects) $(ARGUSProjects) romedbwrapper/test_c romedbwrapper/test_f

