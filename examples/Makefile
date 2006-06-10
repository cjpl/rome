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
               runheader/runhanalyzer.exe

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
ARGUSProjects += argus/folders/romeproject/foframework.exe \
                 argus/folders/fomonitor.exe
endif

all: rome argus
rome: $(ROMEProjects)
argus: $(ARGUSProjects)

multirun/mranalyzer.exe :
	$(ROMESYS)/bin/romebuilder.exe -i multirun/multirun.xml -o multirun -v;

dbexample/rdbframework.exe :
	$(ROMESYS)/bin/romebuilder.exe -i dbexample/RDBframework.xml -o dbexample -v;

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
	find $(ROMESYS)/examples -name "*.obj" -exec rm {} \;

depclean :
	find $(ROMESYS)/examples -name "*.d" -exec rm {} \;

remove :
	rm $(ROMEProjects) $(ARGUSProjects)
