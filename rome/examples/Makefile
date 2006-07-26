RESTRICT_TEST = yes

ifeq ($(RESTRICT_TEST), yes)
export DOOPT    =-Wall -W -Woverloaded-virtual -Werror
export FLOPT    =-Wall -W -Woverloaded-virtual -Werror
export FOOPT    =-Wall -W -Woverloaded-virtual -Werror
export HGOPT    =-Wall -W -Woverloaded-virtual -Werror
export HGUIOPT  =-Wall -W -Woverloaded-virtual -Werror
export HWOPT    =-Wall -W -Woverloaded-virtual -Werror
export MROPT    =-Wall -W -Woverloaded-virtual -Werror
export MIDOPT   =-Wall -W -Woverloaded-virtual -Werror
export NFOPT    =-Wall -W -Woverloaded-virtual -Werror
export PMOPT    =-Wall -W -Woverloaded-virtual -Werror
export RDAQOPT  =-Wall -W -Woverloaded-virtual -Werror
export RDBOPT   =-Wall -W -Woverloaded-virtual -Werror
export RUNHOPT  =-Wall -W -Woverloaded-virtual -Werror
export SBOPT    =-Wall -W -Woverloaded-virtual -Werror
export SBSOPT   =-Wall -W -Woverloaded-virtual -Werror
export SBSHOPT  =-Wall -W -Woverloaded-virtual -Werror
export SDAQOPT  =-Wall -W -Woverloaded-virtual -Werror
export SMDOPT   =-Wall -W -Woverloaded-virtual -Werror
export TBOPT    =-Wall -W -Woverloaded-virtual -Werror
export THOPT    =-Wall -W -Woverloaded-virtual -Werror
export THHOPT   =-Wall -W -Woverloaded-virtual -Werror
export TMOPT    =-Wall -W -Woverloaded-virtual -Werror
export TTOPT    =-Wall -W -Woverloaded-virtual -Werror
export TWROPT   =-Wall -W -Woverloaded-virtual -Werror
endif

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
               taskhierarchyandhistos/thhexample.exe

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
	$(ROMESYS)/bin/romebuilder.exe -i multirun/multirun.xml -o multirun -v;

dbexample/rdbframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i dbexample/RDBframework.xml -o dbexample -v -mysql -sqlite3;

taskhierarchy/thexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i taskhierarchy/taskhierarchy.xml -o taskhierarchy -v;

netfolder/nfexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i netfolder/netfolder.xml -o netfolder -v;

stepbystep/sbsanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystep/stepbystep.xml -o stepbystep -v;

tree_write_read/twrsample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i tree_write_read/tree_write_read.xml -o tree_write_read -v;

stepbystephisto/sbshanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystephisto/stepbystephisto.xml -o stepbystephisto -v;

histoGUI/hguiexample.exe :
	$(ROMESYS)/bin/romebuilder.exe -i histoGUI/histoGUI.xml -o histoGUI -v;

stepbystepdbdaq/sdaqanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystepdbdaq/stepbystepdbdaq.xml -o stepbystepdbdaq -v;

stepbystepmydaq/smdanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i stepbystepmydaq/stepbystepmydaq.xml -o stepbystepmydaq -v;

runheader/runhanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i runheader/runheader.xml -o runheader -v;

rootDAQ/rdaqanalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i rootDAQ/rootDAQ.xml -o rootDAQ -v;

midas/midanalyzer.exe:
	$(ROMESYS)/bin/romebuilder.exe -i midas/midas.xml -o midas -v -midas;

taskhierarchyandhistos/thhexample.exe:
	$(ROMESYS)/bin/romebuilder.exe -i taskhierarchyandhistos/taskhierarchyandhistos.xml -o taskhierarchyandhistos -v;

displayobjects/doexample.exe:
	$(ROMESYS)/bin/romebuilder.exe -i displayobjects/displayObjects.xml -o displayobjects -v;

argus/file/flmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/file/file.xml -o argus/file -v 

argus/timer/ttmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/timer/timer.xml -o argus/timer -v 

argus/processMessage/pmmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/processMessage/processMessage.xml -o argus/processMessage -v

argus/menu/tmmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/menu/menu.xml -o argus/menu -v

argus/thread/ttmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/thread/thread.xml -o argus/thread -v

argus/statusBar/sbmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/statusBar/statusBar.xml -o argus/statusBar -v

argus/tabs/tbmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/tabs/tabs.xml -o argus/tabs -v

argus/helloWorld/hwmonitor.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/helloWorld/HelloWorld.xml -o argus/helloWorld -v

argus/histogram/romeproject/hgframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/histogram/romeproject/histogram.xml -o argus/histogram/romeproject -v

argus/histogram/hgmonitor.exe : argus/histogram/romeproject/hgframework.exe
	$(ROMESYS)/bin/romebuilder.exe -i argus/histogram/histogram.xml -o argus/histogram -v

argus/folders/romeproject/foframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i argus/folders/romeproject/FOframework.xml -o argus/folders/romeproject -v -midas

argus/folders/fomonitor.exe : argus/folders/romeproject/foframework.exe
	$(ROMESYS)/bin/romebuilder.exe -i argus/folders/folders.xml -o argus/folders -v -midas

clean :
	find $(ROMESYS)/examples -name "*.o" -exec rm {} \;

depclean :
	find $(ROMESYS)/examples -name "*.d" -exec rm {} \;

remove :
	rm $(ROMEProjects) $(ARGUSProjects)

