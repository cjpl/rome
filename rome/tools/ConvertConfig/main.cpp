#include <iostream>
#include <mxml.h>
using std::cout;
using std::endl;

bool convertConfig(PMXML_NODE configNode,bool runConfig);
void deleteComments(PMXML_NODE node);

//______________________________________________________________________________
int main(int argc, char *argv[])
{
   char *inputFile;

   if (argc!=3) {
      cout << "  -i      xml configuration file" << endl;
      return false;
   }
   else if (strcmp(argv[1],"-i")) {
      cout << "  -i      xml configuration file" << endl;
      return false;
   }
   else
      inputFile = argv[2];

   char error[80];
   char *name = "";
   char *value;
   PMXML_NODE rootNode,configurationNode,node;
   int nChild;
   int i;
   bool rome,argus;

   rootNode = mxml_parse_file(inputFile, error, sizeof(error));
   if (rootNode==NULL) {
      cout << "File not found!" << endl;
      return 1;
   }
   configurationNode = mxml_find_node(rootNode,"/Configuration");
   if (configurationNode==NULL) {
      cout << "This configuration file has not the correct format.\n'Configuration' not found!\nPlease edit by hand." << endl;
      return 0;
   }

   // ProgramConfiguration
   rome = false;
   argus = false;
   node = mxml_find_node(rootNode,"/Configuration/ProgramConfiguration/ROME/Active");
   if (node!=NULL) {
      value = mxml_get_value(node);
      if (!strcmp(value,"true"))
         rome = true;
      node = mxml_find_node(rootNode,"/Configuration/ProgramConfiguration/ARGUS/Active");
      value = mxml_get_value(node);
      if (!strcmp(value,"true"))
         argus = true;
      node = mxml_find_node(rootNode,"/Configuration/ProgramConfiguration");
      nChild = mxml_get_number_of_children(node);
      for (i=0;i<nChild;i++)
         mxml_delete_node(mxml_subnode(node,0));
      if (rome && !argus)
         mxml_add_node(node,"ProgramMode","0");
      else if (!rome && argus)
         mxml_add_node(node,"ProgramMode","1");
      else if (rome && argus)
         mxml_add_node(node,"ProgramMode","2");
      else
         mxml_add_node(node,"ProgramMode","0");
   }
   else {
      node = mxml_add_node(configurationNode,"ProgramConfiguration","");
      mxml_add_node(node,"ProgramMode","0");
   }

   node = mxml_find_node(rootNode,"/Configuration/MainConfiguration");
   if (node!=NULL) {
      if (!convertConfig(node,false))
         return 0;
   }
   else {
      cout << "This configuration file has not the correct format.\n'Configuration->MainConfiguration' not found!\nPlease edit by hand." << endl;
      return 0;
   }

   configurationNode = mxml_find_node(rootNode,"/Configuration");
   if (configurationNode!=NULL) {
      nChild = mxml_get_number_of_children(configurationNode);
      for (i=0;i<nChild;i++) {
         node = mxml_subnode(configurationNode,i);
         name = mxml_get_name(node);
         if (!strcmp(name,"RunConfiguration")) {
            if (!convertConfig(node,true))
               return 0;
         }
      }
   }
   else {
      cout << "This configuration file has not the correct format.\n'Configuration' not found!\nPlease edit by hand." << endl;
      return 0;
   }

   deleteComments(rootNode);

   mxml_write_tree(inputFile,rootNode);

   return 0;
}
void deleteComments(PMXML_NODE node)
{
   int i;
   for (i=mxml_get_number_of_children(node)-1;i>=0;i--) {
      deleteComments(&node->child[i]);
      if (node->child[i].node_type==COMMENT_NODE) {
         mxml_delete_node(&node->child[i]);
      }
   }
}


bool convertConfig(PMXML_NODE configNode,bool runConfig)
{
   PMXML_NODE node,tmpNode;
   PMXML_NODE modeNode;
   PMXML_NODE commonNode;
   PMXML_NODE analyzerNode;
   PMXML_NODE monitorNode;
   PMXML_NODE socketServerNode;

   // Modes
   modeNode = mxml_find_node(configNode,"/Modes");
   if (modeNode!=NULL) {
      mxml_replace_node_name(modeNode,"Settings");
      modeNode = mxml_add_node_at(configNode,"Modes","",0);

      node = mxml_find_node(configNode,"/Settings/AnalyzingMode");
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(modeNode,tmpNode,0);

      node = mxml_find_node(configNode,"/Settings/DAQSystem");
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(modeNode,tmpNode,1);
   }
   else {
      if (!runConfig) {
         cout << "This configuration file has not the correct format.\n'Modes' not found!\nPlease edit by hand." << endl;
         return false;
      }
      else
         return true;
   }

   // Offline
   node = mxml_find_node(configNode,"/RunParameters");
   if (node!=NULL) {
      mxml_replace_node_name(node,"Offline");
   }
   else {
      if (!runConfig) {
         cout << "This configuration file has not the correct format.\n'RunParameters' not found!\nPlease edit by hand." << endl;
         return false;
      }
      else
         return true;
   }

   // Online
   node = mxml_find_node(configNode,"/Online");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(configNode,tmpNode,2);
   }
   else {
      if (!runConfig) {
         cout << "This configuration file has not the correct format.\n'Online' not found!\nPlease edit by hand." << endl;
         return false;
      }
      else
         return true;
   }

   // Paths
   node = mxml_find_node(configNode,"/Paths");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(configNode,tmpNode,3);
   }
   else {
      if (!runConfig) {
         cout << "This configuration file has not the correct format.\n'Paths' not found!\nPlease edit by hand." << endl;
         return false;
      }
      else
         return true;
   }

   // Common
   commonNode = mxml_add_node_at(configNode,"Common","",4);

   // Common/Settings
   node = mxml_find_node(configNode,"/Settings");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(commonNode,tmpNode,mxml_get_number_of_children(commonNode));
   }

   // Common/SocketServer
   node = mxml_find_node(configNode,"/SocketInterface");
   if (node!=NULL) {
      mxml_replace_node_name(node,"SocketServer");
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(commonNode,tmpNode,mxml_get_number_of_children(commonNode));

      socketServerNode = mxml_find_node(commonNode,"/SocketServer");
      node = mxml_find_node(commonNode,"/SocketServer/AvailableOffline");
      mxml_replace_node_name(node,"Active");
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(socketServerNode,tmpNode,0);
   }

   // Common/Databases
   node = mxml_find_node(configNode,"/DataBases");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(commonNode,tmpNode,mxml_get_number_of_children(commonNode));
   }

   // Common/Trees
   node = mxml_find_node(configNode,"/Trees");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(commonNode,tmpNode,mxml_get_number_of_children(commonNode));
   }

   // Common/GlobalSteeringParameters
   node = mxml_find_node(configNode,"/GlobalSteeringParameters");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(commonNode,tmpNode,mxml_get_number_of_children(commonNode));
   }

   // Analyzer
   analyzerNode = mxml_add_node_at(configNode,"Analyzer","",5);

   // Analyzer/HistogramRead
   node = mxml_find_node(configNode,"/HistogramRead");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(analyzerNode,tmpNode,mxml_get_number_of_children(analyzerNode));
   }

   // Analyzer/Macros
   node = mxml_find_node(configNode,"/Macros");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(analyzerNode,tmpNode,mxml_get_number_of_children(analyzerNode));
   }

   // Analyzer/Tasks
   node = mxml_find_node(configNode,"/Tasks");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(analyzerNode,tmpNode,mxml_get_number_of_children(analyzerNode));
   }

   // Monitor
   monitorNode = mxml_add_node_at(configNode,"Monitor","",6);

   // Monitor/Display
   node = mxml_find_node(configNode,"/Argus");
   if (node!=NULL) {
      mxml_replace_node_name(node,"Display");
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(monitorNode,tmpNode,mxml_get_number_of_children(monitorNode));
   }

   // Monitor/SocketClient
   node = mxml_find_node(monitorNode,"/Display/SocketToROME");
   if (node!=NULL) {
      mxml_replace_node_name(node,"SocketClient");
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(monitorNode,tmpNode,mxml_get_number_of_children(monitorNode));
      node = mxml_find_node(monitorNode,"/SocketClient/Active");
      mxml_delete_node(node);
   }

   // Monitor/Tabs
   node = mxml_find_node(configNode,"/Tabs");
   if (node!=NULL) {
      tmpNode = mxml_clone_tree(node);
      mxml_delete_node(node);
      mxml_add_tree_at(monitorNode,tmpNode,mxml_get_number_of_children(monitorNode));
   }
   return true;
}

