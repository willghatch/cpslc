/*
 * symtab.c
 * Most of this is taken from the class slides.
 * I figure it it's already there for us... why should I bother rewriting it?
 */

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include"symtab.h"
#include"expression.h"
#include"mipsout.h"


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

ID* mkBoolId(char* name, expr* expression) {
    ID* boolid = newid(name);
    boolid->id_kind = Constant_id;
    boolid->id_type = bool_type;
    boolid->const_expr = expression;
    return boolid;
}

void symtabInit(void) {
    typeinit();
    // Initialize pre-defined level of symbol table
    ID** s = scope;
    addIdToTable_noAddrMove(typeIdCreate(int_type, "integer"), s);
    addIdToTable_noAddrMove(typeIdCreate(int_type, "INTEGER"), s);
    addIdToTable_noAddrMove(typeIdCreate(char_type, "char"), s);
    addIdToTable_noAddrMove(typeIdCreate(char_type, "CHAR"), s);
    addIdToTable_noAddrMove(typeIdCreate(bool_type, "boolean"), s);
    addIdToTable_noAddrMove(typeIdCreate(bool_type, "BOOLEAN"), s);
    addIdToTable_noAddrMove(typeIdCreate(str_type, "string"), s);
    addIdToTable_noAddrMove(typeIdCreate(str_type, "STRING"), s);
    
    // add true and false (upper and lower) to table
    true_expr = newBoolExpr(1);
    false_expr = newBoolExpr(0);

    ID* boolid = mkBoolId("true", true_expr);
    addIdToTable_noAddrMove(boolid, s);
    boolid = mkBoolId("TRUE", true_expr);
    addIdToTable_noAddrMove(boolid, s);
    boolid = mkBoolId("false", false_expr);
    addIdToTable_noAddrMove(boolid, s);
    boolid = mkBoolId("FALSE", false_expr);
    addIdToTable_noAddrMove(boolid, s);
    
    false_str_expr = newStrExpr("false");
    true_str_expr = newStrExpr("true");
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
        case Constant_id:
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

void addIdToTable_noAddrMove(ID* newId, ID** table) {
    if (*table == NULL) {
        *table = newId;
        return;
    } 
    int cmp = strcmp(newId->id_name, (*table)->id_name);
    if (cmp < 0) { // newId name < current element name
        ID** p = &((*table)->id_left);
        addIdToTable_noAddrMove(newId, p);
    } else if (cmp > 0) { // newId name > current element name
        ID** p = &((*table)->id_right);
        addIdToTable_noAddrMove(newId, p);
    } else {
        // Error
        printf("Error - symbol %s defined twice in the same scope\n", newId->id_name);
        // TODO - Exit or something here probably
    }
}

void addVarToCurTable(ID* var) {
    var->id_addr = scopeAddr[currscope];
    scopeAddr[currscope] += var->id_type->ty_size;
    addIdToTable_noAddrMove(var, scope+currscope);
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
        printf(" Elem Type: %s\n", type->ty_form.ty_array.ElementType->ty_name);
        printf(" Min index: %i\n", type->ty_form.ty_array.min);
        printf(" Max index: %i\n", type->ty_form.ty_array.max);
    }
}

void printConstInfo(ID* c) {
    if(c == NULL || c->id_kind != Constant_id) {
        return;
    }
    expr* e = c->const_expr;
    if(e->kind == constant_expr) {
        if(c->id_type == int_type) {
            printf(" Value: %i\n", e->int_val);
        } else if (c->id_type == char_type) {
            printf(" Value: %c\n", e->char_val);
        } else if (c->id_type == str_type) {
            printf(" Value: \"%s\"", e->str_val);
        }
    }
}

void printVarInfo(ID* v) {
    printf(" Size: %i\n", v->id_type->ty_size);
    printf(" Offset in scope space: %i\n", v->id_addr);
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
    if (tree->id_kind == Constant_id) {
        printConstInfo(tree);
    }
    if (tree->id_kind == Variable) {
        printVarInfo(tree);
    }

    // Print right side now
    printIdTree(tree->id_right);
}

void scopePrint(int s) {
    printf("Printing symbol table at scope level %i:\n", s);
    printIdTree(scope[s]);
}

void reserveGlobal(ID* gvar) {
    int size = gvar->id_type->ty_size;
    int reservation = m_reserve_global_var(size);
    gvar->id_label = reservation;
}

void _reserveGlobals_recursive(ID* id) {
    if (id == NULL) {
        return;
    }
    _reserveGlobals_recursive(id->id_left);
    if(id->id_kind == Variable && isGlobal(id)) {
        reserveGlobal(id);
    }
    _reserveGlobals_recursive(id->id_right);
}

void reserveGlobals() {
// output to mips the global variables.
    _reserveGlobals_recursive(scope[0]);
    _reserveGlobals_recursive(scope[1]);
}

int isGlobal(ID* id) {
// returns 1 if global, 0 if not
    if(id->id_level < 2)
        return 1;
    return 0;
}

// Everybody loves global variables
ID *scope [SCOPEDEPTH];
int currscope = 0;
TYPE *int_type, *bool_type, *char_type, *str_type, *undef_type;
int scopeAddr [SCOPEDEPTH] = {0,0,0}; // TODO - Ok, ideally I should initialize
    // this in a loop in case the scope depth changes... but for now... bleh.
int verbosity = 0;
expr* true_expr;
expr* false_expr;
expr* true_str_expr;
expr* false_str_expr;


