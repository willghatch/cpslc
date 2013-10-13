/*
 * symtab.h
 * 
 * Contains structs, enums, and function prototypes mostly from the class slides.
 *
 */
#ifndef SYMTAB_H
#define SYMTAB_H

#define INTSIZE  4	/* integer data */
#define CHARSIZE 1	/* character data */
#define BOOLSIZE 1	/* boolean data */
#define POINTSIZE 4	/* pointer data */
#define NOSIZE   0	/* unknown data */

#define SCOPEDEPTH 3


typedef struct id_info ID;
typedef struct type_info TYPE;
typedef enum type_kind TY_KIND;
typedef enum identifier_kind ID_KIND;

enum type_kind {Integer, Char, Boolean, String, Array, 
                 Record, UndefinedType};
// I'm not sure what all this is about... is ty_next so we have a linked list?
// How am I_supposed to tell as I run down the list which is the one I want?
//
struct type_info
 {	int ty_size;
        char* ty_name;
 	TY_KIND ty_kind;
 	TYPE *ty_next;
 	union
 	{	struct
		{	TYPE *RangeType;
			int min;
			int max;
		} ty_subrange;
		struct
		{	TYPE *ElementType;
			TYPE *IndexType;
		} ty_array;
		struct
		{	ID *FirstField;
		} ty_record;
	} ty_form;
}; /* type_info */

enum identifier_kind {Constant, Type, Variable, RParameter, VParameter, 
                       Field, Procedure, Function};



// So these have built-in pointers to make a tree.
// I'm not sure how I feel about that, but I'll roll with it.
struct id_info
{	char *id_name;
	int id_addr;
	int id_level;
	TYPE *id_type;
	ID_KIND id_kind;
	ID *id_left;
	ID *id_right;
	// id_left and id_right is for a tree of ID's for the symbol table...
	ID *id_next;
	// id_next is for a linked list of ID's for a record type...
	int id_value;
 }; /* id_info */


// prototypes...
TYPE *typecreate (int size, TY_KIND kind, ID *id_list, TYPE *elem_type, char* name);
void typeinit (void);
char* getTypeName(TYPE* type);
char* getIdKindName(ID_KIND kind);
ID *newid (char *name);
ID *search (char *name, ID *table);
void addIdToTable(ID* newId, ID* table);
void freeIdTree(ID* tree);
void printTypeInfo(TYPE* type);
void printIdTree(ID* tree);



// A couple global vars, because everybody loves them.
extern ID *scope [SCOPEDEPTH];
extern int currscope;
extern TYPE *int_type, *bool_type, *char_type, *str_type, *undef_type;

#endif /*SYMTAB_H*/

