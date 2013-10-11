/*
 * types.c
 * Most of this is taken from the class slides.
 * I figure it it's already there for us... why should I bother rewriting it?
 */


#define INTSIZE  4	/* integer data */
#define CHARSIZE 1	/* character data */
#define BOOLSIZE 1	/* boolean data */
#define POINTSIZE 4	/* pointer data */
#define NOSIZE   0	/* unknown data */

#define SCOPEDEPTH 3
ID *scope [SCOPEDEPTH];
int currscope = 0;





typedef enum type_kind TY_KIND;
enum type_kind {Integer, Char, Boolean, String, Array, 
                 Record, UndefinedType};
// I'm not sure what all this is about... is ty_next so we have a linked list?
// How am I_supposed to tell as I run down the list which is the one I want?
//
typedef struct type_info TYPE;
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


TYPE *typecreate (int size, TY_KIND kind, ID *id_list, TYPE *type, char* name)
{	TYPE *t;
	t = (TYPE *)malloc(sizeof(TYPE));
	t->ty_name = name;
	t->ty_size = size;
	t->ty_kind = kind;
	t->ty_next = NULL;
	switch (kind)
	{	
		case Array:
			t->ty_form.ty_array.ElementType = type;
			break;
		case Record:
			t->ty_form.ty_record.FirstField = id_list;
			break;
		default:
			break;
	} /* switch */
	return(t);
} /* typecreate */

TYPE *int_type, *bool_type, *char_type, *str_type, *undef_type;


void typeinit (void)
{	int_type = typecreate(INTSIZE, Integer, NULL, NULL, "int");
	bool_type = typecreate(BOOLSIZE, Boolean, NULL, NULL, "bool");
	char_type = typecreate(CHARSIZE, Char, NULL, NULL, "char");
	str_type = typecreate(POINTSIZE, String, NULL, NULL, "string");
	undef_type = typecreate(NOSIZE, UndefinedType, NULL, NULL, "undefinded");
} /* typeinit */

char* getTypeName(TYPE* type) {
    if (type == null) {
        return "void";
    }
    if (type->ty_name == null) {
        return "unnamed-type";
    }
    return type->ty_name;
}



typedef enum identifier_kind ID_KIND;
enum identifier_kind {Constant, Type, Variable, RParameter, VParameter, 
                       Field, Procedure, Function};

char* getIdKindName(ID_KIND kind) {
    switch(kind) {
        case Constant:
            return "Constant";
        case Type:
            return "Type";
        case Variable:
            return "Variable";
        case Procedure:
            return "Procedure";
        case Function:
            return "Function";
        default:
            return "Unknown Kind";
    }
}


// So these have built-in pointers to make a tree.
// I'm not sure how I feel about that, but I'll roll with it.
typedef struct id_info ID;
struct id_info
{	char *id_name;
	int id_addr;
	int id_level;
	TYPE *id_type;
	ID *id_left;
	ID *id_right;
	// id_left and id_right is for a tree of ID's for the symbol table...
	ID_KIND id_kind;
	ID *id_next;
	// id_next is for a linked list of ID's for a record type...
	int id_value;
 }; /* id_info */







ID *newid (char *name)
{	ID *new_id;

	new_id = (ID *)malloc(sizeof(ID));
	new_id->id_name = name;
	new_id->id_addr = 0;
	new_id->id_level = currscope;
	new_id->id_type = undef_type;
	new_id->id_left = NULL;
	new_id->id_right = NULL;
	new_id->id_kind = Variable;
	new_id->id_next = NULL;
	new_id->id_value = 0;
	return(new_id);
 } /* newid */


ID *search (char *name, ID *table)
{	int temp;
 
	if (table == NULL)
		return(NULL);
	temp = strcmp(name, table->id_name);
	if (temp == 0)
		return(table);
	else if (temp < 0)
		return(search(name, table->id_left));
	else
		return(search(name, table->id_right));
 } /* search */

void addIdToTable(ID* newId, ID* table) {
    if (table == NULL) {
        table = newId;
    } 
    int cmp = strcmp(newId->id_name, table->id_name);
    if (cmp < 0) { // newId name < current element name
        addIdToTable(newId, table->id_left);
    } else if (cmp > 0) { // newId name > current element name
        addIdToTable(newId, table->id_right);
    } else {
        // Error
        printf("Error - symbol %s defined twice in the same scope", newId->name);
    }
}

void freeIdTree(ID* tree) {
    if (tree == null) {
        return;
    }
    freeIdTree(tree->id_left);
    freeIdTree(tree->id_right);
    free(tree);
    // TODO - make sure this frees records and arrays properly.
}

void printTypeInfo(TYPE* type) {
    // I'll need to print the size... for arrays maybe just the number of elements
    // for records I need to print the layout.
    // TODO - implement this
}


void printIdTree(ID* tree) {
    if (tree == null) {
        return;
    }
    // Let's do an inorder traversal
    printIdTree(tree->id_left);

    // Do actual printing
    // TODO - print location of everything
    printf("Identifier:\n");
    printf(" Name: %s\n", tree->id_name);
    printf(" Id Kind: %s\n", getIdKindName(tree->id_kind));
    // Should I print the type of a type?  I could just print it's own name...
    printf(" Type: %s\n", getTypeName(tree->id_type));
    if (tree->id_kind == Type) {
        printTypeInfo(tree->id_type);
    }
    if (tree->id_kind == Procedure || tree->id_kind == Function) {
        // TODO - Print function info... 
    }
    if (tree->id_kind == Constant) {
        // TODO - Print out the constant value
    }

    
    // Print right side now
    printIdTree(tree->id_right);
    
}

