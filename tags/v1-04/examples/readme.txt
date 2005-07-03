There are some example. Before compiling exmaples, you need to compile
argusbuilder. To know more details, please see ARGUS web page.

http://midas.psi.ch/argus 

After you compile argusbuilder, you can have an executable which shows
hello world with following commands.

shell> cd examples/helloWorld/
shell> ../../bin/argusbuilder HelloWorld.xml
shell> hwmonitor



helloWorld
      This is the simplest example. Just shows a text in canvas.

tabs
      This shows how you create several tabs. Tabs can be nested.

menu
      This shows how you create menus. You can asign any function to
      menus.

statusBar
      This shows how you put messages in satus bar.

thread
      This shows how you have thread functions.

folders
      This show how you create and update folders. Folders can have
      several variables in it, and they are accessible from all
      tabs. This example shows how you get informations from SQL/XML
      database, ODB and ROME over network.

histogram
      This shows how you get ROOT object from ROME or MIDAS analyzer.

processMessage
      This show how you create buttons. Buttons will publish signal
      when it is clicked. You can assign any function to the signal.

analyzerControl
      This example shows how you control analysis program such as
      ROME. Analyzer controller is still under development.

Ryu, Sawada
