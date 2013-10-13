/*
 * symtab.c
 * Most of this is taken from the class slides.
 * I figure it it's already there for us... why should I bother rewriting it?
 */

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include"symtab.h"


TYPE *typecreate (int size, TY_KIND kind, ID *id_list, TYPE *elem_type, char* name)
{	TYPE *t;
	t = (TYPE *)malloc(sizeof(TYPE));
	t->ty_name = name;
	t->ty_size = size;
	t->ty_kind = kind;
	t->ty_next = NULL;
	switch (kind)
	{	
		case Array:
			t->ty_form.ty_array.ElementType = elem_type;
			break;
		case Record:
			t->ty_form.ty_record.FirstField = id_list;
			break;
		default:
			break;
	} /* switch */
	return(t);
} /* typecreate */

ID* typeIdCreate(TYPE* type, char* name) {
    ID* id = newid(name);
    id->id_kind = Type;
    id->id_type = type;
    return id;
}

void typeinit (void)
{	int_type = typecreate(INTSIZE, Integer, NULL, NULL, "integer");
	bool_type = typecreate(BOOLSIZE, Boolean, NULL, NULL, "boolean");
	char_type = typecreate(CHARSIZE, Char, NULL, NULL, "char");
	str_type = typecreate(POINTSIZE, String, NULL, NULL, "string");
	undef_type = typecreate(NOSIZE, UndefinedType, NULL, NULL, "undefinded");
} /* typeinit */

void symtabInit(void) {
    typeinit();
    // Initialize pre-defined level of symbol table
    ID** s = scope;
    addIdToTable(typeIdCreate(int_type, "integer"), s);
    addIdToTable(typeIdCreate(int_type, "INTEGER"), s);
    addIdToTable(typeIdCreate(char_type, "char"), s);
    addIdToTable(typeIdCreate(char_type, "CHAR"), s);
    addIdToTable(typeIdCreate(bool_type, "boolean"), s);
    addIdToTable(typeIdCreate(bool_type, "BOOLEAN"), s);
    addIdToTable(typeIdCreate(str_type, "string"), s);
    addIdToTable(typeIdCreate(str_type, "STRING"), s);
    
    // TODO - add true and false (upper and lower) to table

}

char* getTypeName(TYPE* type) {
    if (type == NULL) {
        return "void";
    }
    if (type->ty_name == NULL) {
        return "unnamed-type";
    }
    return type->ty_name;
}

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
        // TODO - RParameter, VParameter, and Field are kinds from the slides
        // I bet I'll actually need to use them, but I'm not quite sure how yet.
        default:
            return "Unknown Kind";
    }
}

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
	//new_id->id_value = 0;
	new_id->const_expr = NULL;
	return(new_id);
} /* newid */

ID *IDsearch (char *name, ID *table)
{	int temp;
 
	if (table == NULL)
		return(NULL);
	temp = strcmp(name, table->id_name);
	if (temp == 0)
		return(table);
	else if (temp < 0)
		return(IDsearch(name, table->id_left));
	else
		return(IDsearch(name, table->id_right));
} /* search */

ID* scopeLookup(char* name) {
    ID* id;
    for (int i = currscope; i >= 0; --i) {
        id = IDsearch(name, scope[i]);
        if(id) {
            return id;
        }
    }
    return NULL;
}

void addIdToTable(ID* newId, ID** table) {
    if (*table == NULL) {
        *table = newId;
        return;
    } 
    int cmp = strcmp(newId->id_name, (*table)->id_name);
    if (cmp < 0) { // newId name < current element name
        ID** p = &((*table)->id_left);
        addIdToTable(newId, p);
    } else if (cmp > 0) { // newId name > current element name
        ID** p = &((*table)->id_right);
        addIdToTable(newId, p);
    } else {
        // Error
        printf("Error - symbol %s defined twice in the same scope\n", newId->id_name);
        // TODO - Exit or something here probably
    }
}

void freeIdTree(ID* tree) {
    if (tree == NULL) {
        return;
    }
    freeIdTree(tree->id_left);
    freeIdTree(tree->id_right);
    free(tree);
    // TODO - make sure this frees records and arrays properly, and strings inside them, etc.
}

char* getTypeKindName(TY_KIND kind) {
    switch(kind) {
        case Integer:
            return "Integer";
        case Char:
            return "Char";
        case Boolean:
            return "Boolean";
        case String:
            return "String";
        case Array:
            return "Array";
        case Record:
            return "Record";
        case UndefinedType:
            return "UndefinedType";
    }
}

void printTypeInfo(TYPE* type) {
    // I'll need to print the size... for arrays maybe just the number of elements
    // for records I need to print the layout.
    // TODO - implement this
    printf(" Type kind: %s\n", getTypeKindName(type->ty_kind));
    printf(" Type size: %i\n", type->ty_size);
    if(type->ty_kind == Record) {
        printf(" Layout:\n");
        ID* id = type->ty_form.ty_record.FirstField;
        while(id != NULL) {
            printf("  field %s of type %s\n", id->id_name, id->id_type->ty_name);
            id = id->id_next;
        }
    }
    if(type->ty_kind == Array) {
        printf("Note -- Array types are only partially supported so far.  The array bounds are hard coded, I haven't yet written the stuff to read the expressions.\n");
        printf(" Elem Type: %s\n", type->ty_form.ty_array.ElementType->ty_name);
        printf(" Min index: %i\n", type->ty_form.ty_array.min);
        printf(" Max index: %i\n", type->ty_form.ty_array.max);
    }
}

void printIdTree(ID* tree) {
    if (tree == NULL) {
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

void scopePrint(int s) {
    printf("Printing symbol table at scope level %i:\n", s);
    printIdTree(scope[s]);
}

// Everybody loves global variables
ID *scope [SCOPEDEPTH];
int currscope = 0;
TYPE *int_type, *bool_type, *char_type, *str_type, *undef_type;
