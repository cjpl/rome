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
               macros/mcranalyzer.exe

ARGUSProjects = argus/file/flmonitor.exe \
                argus/timer/ttmonitor.exe \
                argus/processMessage/pmmonitor.exe \
                argus/menu/tmmonitor.exe \
                argus/thread/ttmonitor.exe \
                argus/statusBar/sbmonitor.exe \
                argus/tabs/tbmonitor.exe \
                argus/helloWorld/hwmonitor.exe \
                argus/histogram/romeproject/hgframework.exe \
                argus/histogram/hgmonitor.exe
ifdef MIDASSYS
ROMEProjects  += midas/midanalyzer.exe
ARGUSProjects += argus/folders/romeproject/foframework.exe \
                 argus/folders/fomonitor.exe
endif

all: rome argus
rome: $(ROMEProjects)
argus: $(ARGUSProjects)

multirun/mranalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i multirun/multirun.xml -o multirun -nl;
	make $(JOBSFLAG) -C multirun/

dbexample/rdbframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i dbexample/RDBframework.xml -o dbexample -nl -mysql -sqlite3;
	make $(JOBSFLAG) -C dbexample/

taskhierarchy/thexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i taskhierarchy/taskhierarchy.xml -o taskhierarchy -nl;
	make $(JOBSFLAG) -C taskhierarchy/

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

argus/statusBar/sbmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/statusBar/statusBar.xml -o argus/statusBar -nl
	make $(JOBSFLAG) -C argus/statusBar/

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

clean :
	find $(ROMESYS)/examples -name "*.o" -exec rm {} \;

depclean :
	find $(ROMESYS)/examples -name "*.d" -exec rm {} \;

remove :
	rm $(ROMEProjects) $(ARGUSProjects)

