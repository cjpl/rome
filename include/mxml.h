/********************************************************************\

   Name:         mxml.h
   Created by:   Stefan Ritt

   Contents:     Header file for mxml.c

   $Log$
   Revision 1.11  2005/10/08 09:30:56  sawada
   added mxml_dirname,mxml_basename

   Revision 1.10  2005/10/07 22:32:49  sawada
   relative path to follow external entity.

   Revision 1.9  2005/10/07 19:53:12  sawada
   ENTITY is always inside DOCTYPE.

   Revision 1.8  2005/10/07 13:16:26  sawada
   parse ENTITY

   Revision 1.7  2005/07/12 09:04:15  schneebeli_m
   ifndef mxml_H removed

   Revision 1.6  2005/07/12 08:43:28  schneebeli_m
   ifndef mxml_H added

   Revision 1.5  2005/07/12 06:42:21  sawada
   Bug fix. Matched the name of method (IsActiveID and IsActiveEventID)

   Revision 1.4  2005/05/11 12:50:01  schneebeli_m
   added strlcpy

   Revision 1.5  2005/04/19 21:43:33  ritt
   Implemented tree cloning and adding

   Revision 1.4  2005/04/06 11:17:02  ritt
   Nodes can now have values AND subnodes

   Revision 1.3  2005/03/29 14:48:54  ritt
   Implemented mxml_write_comment()

   Revision 1.2  2005/03/29 14:14:38  ritt
   Implemented mxml_set_translate

   Revision 1.1.1.1  2005/03/29 08:19:45  ritt
   Imported sources

\********************************************************************/

/*------------------------------------------------------------------*/
#define MXML_NAME_LENGTH 64

#define ELEMENT_NODE                  1
#define TEXT_NODE                     2
#define PROCESSING_INSTRUCTION_NODE   3
#define COMMENT_NODE                  4
#define DOCUMENT_NODE                 5

#define INTERNAL_ENTITY               0
#define EXTERNAL_ENTITY               1
#define MXML_MAX_ENTITY            1000

typedef struct {
   int  fh;
   char *buffer;
   int  buffer_size;
   int  buffer_len;
   int  level;
   int element_is_open;
   int data_was_written;
   char **stack;
   int  translate;
} MXML_WRITER;

typedef struct mxml_struct *PMXML_NODE;

typedef struct mxml_struct {
   char       name[MXML_NAME_LENGTH];  // name of element    <[name]>[value]</[name]>
   int        node_type;               // type of node XXX_NODE
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

MXML_WRITER *mxml_open_file(const char *file_name);
MXML_WRITER *mxml_open_buffer(void);
int mxml_set_translate(MXML_WRITER *writer, int flag);
int mxml_start_element(MXML_WRITER *writer, const char *name);
int mxml_start_element_noindent(MXML_WRITER *writer, const char *name);
int mxml_end_element(MXML_WRITER *writer);
int mxml_write_comment(MXML_WRITER *writer, const char *string);
int mxml_write_attribute(MXML_WRITER *writer, const char *name, const char *value);
int mxml_write_value(MXML_WRITER *writer, const char *value);
char *mxml_close_buffer(MXML_WRITER *writer);
int mxml_close_file(MXML_WRITER *writer);

int mxml_get_number_of_children(PMXML_NODE pnode);
PMXML_NODE mxml_subnode(PMXML_NODE pnode, int index);
PMXML_NODE mxml_find_node(PMXML_NODE tree, char *xml_path);
int mxml_find_nodes(PMXML_NODE tree, char *xml_path, PMXML_NODE **nodelist);
char *mxml_get_name(PMXML_NODE pnode);
char *mxml_get_value(PMXML_NODE pnode);
char *mxml_get_attribute(PMXML_NODE pnode, char *name);

int mxml_add_attribute(PMXML_NODE pnode, char *attrib_name, char *attrib_value);
PMXML_NODE mxml_add_special_node(PMXML_NODE parent, int node_type, char *node_name, char *value);
PMXML_NODE mxml_add_special_node_at(PMXML_NODE parent, int node_type, char *node_name, char *value, int index);
PMXML_NODE mxml_add_node(PMXML_NODE parent, char *node_name, char *value);
PMXML_NODE mxml_add_node_at(PMXML_NODE parent, char *node_name, char *value, int index);

PMXML_NODE mxml_clone_tree(PMXML_NODE tree);
int mxml_add_tree(PMXML_NODE parent, PMXML_NODE tree);
int mxml_add_tree_at(PMXML_NODE parent, PMXML_NODE tree, int index);

int mxml_replace_node_name(PMXML_NODE pnode, char *new_name);
int mxml_replace_node_value(PMXML_NODE pnode, char *value);
int mxml_replace_subvalue(PMXML_NODE pnode, char *name, char *value);
int mxml_replace_attribute_name(PMXML_NODE pnode, char *old_name, char *new_name);
int mxml_replace_attribute_value(PMXML_NODE pnode, char *attrib_name, char *attrib_value);

int mxml_delete_node(PMXML_NODE pnode);
int mxml_delete_attribute(PMXML_NODE, char *attrib_name);

PMXML_NODE mxml_create_root_node();
PMXML_NODE mxml_parse_file(char *file_name, char *error, int error_size);
PMXML_NODE mxml_parse_buffer(char *buffer, char *error, int error_size);
PMXML_NODE mxml_parse_entity(char **buf, char* file_name, char *error, int error_size);
int mxml_write_tree(char *file_name, PMXML_NODE tree);
void mxml_debug_tree(PMXML_NODE tree, int level);
void mxml_free_tree(PMXML_NODE tree);

char* mxml_dirname(char* path);
char* mxml_basename(char *path);
/*------------------------------------------------------------------*/
