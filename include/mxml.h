/********************************************************************\

   Name:         mxml.h
   Created by:   Stefan Ritt

   Contents:     Header file for mxml.c

   $Log$
   Revision 1.1  2005/03/23 09:04:17  schneebeli_m
   new xml parser

   Revision 1.4  2005/03/22 15:06:11  ritt
   Added mxml_add_node_at()

   Revision 1.3  2005/03/22 14:26:22  ritt
   Implemented mxml_find_nodes()

   Revision 1.2  2005/03/03 15:34:40  ritt
   Implemented mxml_debug_tree()

   Revision 1.1  2005/03/01 23:48:18  ritt
   Implemented MXML for password file

\********************************************************************/

/*------------------------------------------------------------------*/

#define MXML_NAME_LENGTH 64

typedef struct {
   int  fh;
   int  level;
   int element_is_open;
   int data_was_written;
   char **stack;
} MXML_WRITER;

typedef struct mxml_struct *PMXML_NODE;

typedef struct mxml_struct {
   char       name[MXML_NAME_LENGTH];  // name of element    <[name]>[value]</[name]>
   char       *value;                  // value of element
   int        n_attributes;            // list of attributes
   char       *attribute_name;
   char       **attribute_value;
   int        line_number;             // line number for source file
   PMXML_NODE parent;                  // pointer to parent element
   int        n_children;              // list of children
   PMXML_NODE child;
} MXML_NODE;

/*------------------------------------------------------------------*/

MXML_WRITER *mxml_open_document(const char *file_name);
int mxml_start_element(MXML_WRITER *writer, const char *name);
int mxml_end_element(MXML_WRITER *writer); 
int mxml_write_attribute(MXML_WRITER *writer, const char *name, const char *value);
int mxml_write_value(MXML_WRITER *writer, const char *value);
int mxml_close_document(MXML_WRITER *writer);

int mxml_get_number_of_children(PMXML_NODE pnode);
PMXML_NODE mxml_subnode(PMXML_NODE pnode, int index);
PMXML_NODE mxml_find_node(PMXML_NODE tree, char *xml_path);
int mxml_find_nodes(PMXML_NODE tree, char *xml_path, PMXML_NODE **nodelist);
char *mxml_get_value(PMXML_NODE pnode);
char *mxml_get_attribute(PMXML_NODE pnode, char *name);

int mxml_add_attribute(PMXML_NODE pnode, char *attrib_name, char *attrib_value);
PMXML_NODE mxml_add_node(PMXML_NODE parent, char *node_name, char *value);
PMXML_NODE mxml_add_node_at(PMXML_NODE parent, char *node_name, char *value, int index);

int mxml_replace_node_name(PMXML_NODE pnode, char *new_name);
int mxml_replace_node_value(PMXML_NODE pnode, char *value);
int mxml_replace_subvalue(PMXML_NODE pnode, char *name, char *value);
int mxml_replace_attribute_name(PMXML_NODE pnode, char *old_name, char *new_name);
int mxml_replace_attribute_value(PMXML_NODE pnode, char *attrib_name, char *attrib_value);

int mxml_delete_node(PMXML_NODE pnode);
int mxml_delete_attribute(PMXML_NODE, char *attrib_name);

PMXML_NODE mxml_create_root_node();
PMXML_NODE mxml_parse_file(char *file_name, char *error, int error_size);
int mxml_write_tree(char *file_name, PMXML_NODE tree);
void mxml_debug_tree(PMXML_NODE tree, int level);
void mxml_free_tree(PMXML_NODE tree);

/*------------------------------------------------------------------*/
