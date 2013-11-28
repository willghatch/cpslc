/* cpslc.y */

%{
#include<stdio.h>
#include<stdlib.h>
#include"symtab.h"
#include"expression.h"
#include"statement.h"
#include"slist.h"

#define YYDEBUG 1
int yylex(void);
%}

%code requires {
#include"expression.h"
#include"symtab.h"
#include"slist.h"
#include"typedidentlist.h"
#include"statement.h"
#include"mipsout.h"
#include"function.h"

int yyerror(const char *msg);
void addVarsOfType(slist* idents, TYPE* type);
void pushScope();
void popScope();
slist* addVars_and_retTypedIdentLists(slist* idents, TYPE* t);
slist* retTypedIdentLists(slist* idents, TYPE* t);
}

%union{
    int int_val;
    char char_val;
    char *str_val;

    expr* expr_val;
    slist* list_ptr;
    htslist* htslist_ptr;
    ID* ID_val;
    TYPE* type_val;
    openum op_val;
    statement* stmt_val;
    conditional* cond_val;
    inctype inctype_val;
}
/* give verbose errors */
%error-verbose
%defines "parser.h"

/* Tokens go here */


%token            FLEX_EOF_SYM 
%token            ERRORSYM

%token            ARRAYSYM 
%token            ASSIGNSYM 
%token            BEGINSYM 
%token <char_val> CHARACTERSYM 
%token            CHRSYM 
%token            COLONSYM 
%token            COMMASYM 
%token            CONSTSYM 
%token            DOSYM 
%token            DOWNTOSYM 
%token            ELSEIFSYM 
%token            ELSESYM 
%token            ENDSYM 
%token            FORSYM 
%token            FORWARDSYM 
%token            FUNCTIONSYM 
%token            IDENTSYM 
%token            IFSYM 
%token            LBRACKETSYM 
%token            LPARENSYM 
%token <int_val>  NUMERICALSYM 
%token            OFSYM 
%token            ORDSYM 
%token            PREDSYM 
%token            PROCEDURESYM 
%token            RBRACKETSYM 
%token            READSYM 
%token            RECORDSYM 
%token            REPEATSYM 
%token            RETURNSYM 
%token            RPARENSYM 
%token            SEMICOLONSYM 
%token            STOPSYM 
%token <str_val>  STRINGSYM 
%token            SUCCSYM 
%token            THENSYM 
%token            TOSYM 
%token            TYPESYM 
%token            UNTILSYM 
%token            VARSYM 
%token            WHILESYM 
%token            WRITESYM 

%type <expr_val>  expression
%type <list_ptr> expressionList
%type <list_ptr> commaExpressionStar
%type <list_ptr> maybeExpressionsWithCommas
%type <op_val>  binaryOp
%type <op_val>  unaryOp
%type <str_val>   identifier 
%type <type_val>    type
%type <list_ptr>  identListsOfTypeStar
%type <list_ptr>  identList
%type <list_ptr>  identExt 
%type <str_val>   simpleType
%type <type_val>  recordType
%type <type_val>  arrayType
%type <expr_val>   lValue 
%type <list_ptr> commaedLValueList
%type <list_ptr> commaLValueStar
%type <stmt_val> statement
%type <stmt_val> assignment
%type <stmt_val> ifStatement
%type <stmt_val> whileStatement
%type <stmt_val> repeatStatement
%type <stmt_val> forStatement
%type <stmt_val> stopStatement
%type <stmt_val> returnStatement
%type <stmt_val> readStatement
%type <stmt_val> writeStatement
%type <stmt_val> procedureCall
%type <stmt_val> nullStatement
%type <list_ptr> elseifStar
%type <cond_val> elseMaybe
%type <inctype_val> toOrDownto
%type <htslist_ptr> statementSequence
%type <expr_val> expressionMaybe
%type <list_ptr> semicolonStatementStar
%type <list_ptr> formalParameterExt
%type <list_ptr> formalParameters
%type <htslist_ptr> body
%type <htslist_ptr> forwardOrBody
%type <htslist_ptr> block
%type <list_ptr> dotIdentOrExpStar

%nonassoc EMPTY /* to give lowest precedence to empty rules */
%left PIPESYM
%left AMPERSANDSYM
%right TILDESYM
%nonassoc EQUALSYM NEQUALSYM LTSYM LTESYM GTSYM GTESYM
%left PLUSSYM MINUSSYM
%left STARSYM SLASHSYM PERCENTSYM
%right UNARYMINUS
%left PERIODSYM

%%
program:
    constantDeclMaybe typeDeclMaybe varDeclMaybe {reserveGlobals();} procOrFuncDeclStar {m_add_main_label();} block endPeriod {
        eval_stmt_list($7);
    }
    ;
endPeriod:
    PERIODSYM
    | /*empty */
    ;

/* Constant Declarations */
constantDeclMaybe:
    constantDeclPlus
    | /* empty */ %prec EMPTY
    ;
constantDeclPlus:
    constantDecl constantDeclPlus
    | constantDecl
    ;
constantDecl:
    CONSTSYM subConstantDeclPlus
    ;
subConstantDeclPlus:
    subConstantDecl subConstantDeclPlus
    | subConstantDecl
    ;
subConstantDecl:
    identifier EQUALSYM expression SEMICOLONSYM {
        ID* id; 
        id = newid($1); 
        id->id_type = $3->type; 
        id->id_kind = Constant_id; 
        id->const_expr = $3;
        addIdToTable_noAddrMove(id, scope+currscope);
    }
    ;

/* Procedure and Function Declarations */
procOrFuncDeclStar:
    procedureDecl procOrFuncDeclStar
    | functionDecl procOrFuncDeclStar
    | /* empty */ %prec EMPTY
    ;
procedureDecl:
    PROCEDURESYM identifier {pushScope();} LPARENSYM formalParameters RPARENSYM SEMICOLONSYM forwardOrBody SEMICOLONSYM {
        char* name = $2;
        slist* params = $5;
        htslist* bod = $8;
        declareFunc(name, params, bod, NULL);
        // remember to pop the scope!
        popScope();
    }
    ;
functionDecl:
    FUNCTIONSYM identifier {pushScope();} LPARENSYM formalParameters RPARENSYM COLONSYM type {declareFunc($2, $5, NULL, $8);} SEMICOLONSYM forwardOrBody SEMICOLONSYM {
        char* name = $2;
        slist* params = $5;
        htslist* bod = $11;
        TYPE* t = $8;
        declareFunc(name, params, bod, t);
        // remember to pop the scope!
        popScope();
    }
    ;
forwardOrBody:
    FORWARDSYM 
        {$$ = NULL;}
    | body 
        {$$ = $1;}
    ;
formalParameters:
    /* Add vars to scope, and return list of typedidentlists */
    varMaybe identList COLONSYM type formalParameterExt {
        slist* ls = addVars_and_retTypedIdentLists($2, $4);
        slist_concat(ls, $5);
        $$ = ls;
    }
    | /* empty */ %prec EMPTY {
        $$ = NULL;
    }
    ;
formalParameterExt:
    /* Add vars to scope, and return list of typedidentlists */
    SEMICOLONSYM varMaybe identList COLONSYM type formalParameterExt {
        slist* ls = addVars_and_retTypedIdentLists($3, $5);
        slist_concat(ls, $6);
        $$ = ls;
    }
    | /* empty */ %prec EMPTY {
        $$ = NULL;
    }
    ;
varMaybe:
    VARSYM
    | /* empty */ %prec EMPTY
    ;
body:
    constantDeclMaybe typeDeclMaybe varDeclMaybe block {
        $$ = $4;
    }
    ;
block:
    BEGINSYM statementSequence ENDSYM {
        $$ = $2;
    }
    ;

/* Type Declarations */
typeDeclMaybe:
    typeDecl
    | /* empty */ %prec EMPTY
    ;
typeDecl:
    TYPESYM identEqTypePlus
    ;
identEqTypePlus:
    identEqType identEqTypePlus
    | identEqType
    ;
identEqType:
    identifier EQUALSYM type SEMICOLONSYM {
        ID* typeid = newid($1);
        typeid->id_kind = Type;
        TYPE* type = $3;
        type->ty_name = $1;
        typeid->id_type = type;
        addIdToTable_noAddrMove(typeid, scope+currscope);
    }
    ;
type:
    simpleType {
        ID* id = scopeLookup($1);
        if (id == NULL) {
            yyerror("Error: trying to use undefined type");
            exit(1);
        }
        $$ = id->id_type;
    }
    | recordType {
        $$ = $1;
    }
    | arrayType
        {$$ = $1;}
    ;
simpleType:
    identifier
        {$$ = $1;}
    ;
recordType:
    RECORDSYM identListsOfTypeStar ENDSYM {
        slist* tyList = $2;
        int size = 0;
        ID* idlist = NULL;
        while (tyList != NULL) {
            typedidentlist* types = tyList->data;
            TYPE* type = types->type;
            slist* names = types->names;
            while (names != NULL) {
                ID* newId = newid(names->data);
                newId->id_type = type;
                newId->id_next = idlist;
                idlist = newId;
                size += type->ty_size;

                names = names->next;
            }
            tyList = tyList->next;
        }
        TYPE* newType = typecreate(size, Record, idlist, NULL, NULL);
        $$ = newType;
    }
    ;
identListsOfTypeStar:
    /* slist of typedidentlists */
    identList COLONSYM type SEMICOLONSYM identListsOfTypeStar {
        typedidentlist* idents = malloc(sizeof(typedidentlist));
        idents->type = $3;
        idents->names = $1;
        slist* tyList = mkSlist(idents);
        tyList->next = $5;
        $$ = tyList;
    }
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
arrayType:
    ARRAYSYM LBRACKETSYM expression COLONSYM expression RBRACKETSYM OFSYM type {
        int min = 0;
        int max = 0;
        expr* emin = $3;
        expr* emax = $5;
        if (emin != NULL && emin->kind == constant_expr && emin->type == int_type) {
            min = emin->int_val;
        }
        if (emax != NULL && emax->kind == constant_expr && emax->type == int_type) {
            max = emax->int_val;
        }
        // TODO -- get the array bounds better...
        if (min > max) {
            int temp = min;
            min = max;
            max = temp;
        }
        TYPE* elemType = $8;
        int size = elemType->ty_size * (max - min + 1);
        char* name = "array type";
        TYPE* newtype = typecreate(size, Array, NULL, elemType, name);
        newtype->ty_form.ty_array.min = min;
        newtype->ty_form.ty_array.max = max;
        $$ = newtype;
    }
    ;
identList:
    /* slist of identifier names */
    identifier identExt {
        slist* ls = mkSlist($1);
        ls->next = $2;
        $$ = ls;
    }
    ;
identExt:
    /* slist of identifier names */
    COMMASYM identifier identExt {
        slist* ls = mkSlist($2);
        ls->next = $3;
        $$ = ls;
    }
    | /* empty */ %prec EMPTY
        {$$ = NULL;}    
    ;

/* Variable Declarations */
varDeclMaybe:
    varDecl 
    | /* empty */ %prec EMPTY
    ;
varDecl:
    VARSYM varDeclExtPlus
    ;
varDeclExtPlus:
    varDeclExt varDeclExtPlus
    | varDeclExt
    ;
varDeclExt:
    identList COLONSYM type SEMICOLONSYM {
        slist* ls = $1;
        TYPE* type = $3;
        if (type == NULL) {
            yyerror("Error, trying to declare variable of unknown type");
        }
        while (ls != NULL) {
            ID* id = newid(ls->data);
            id->id_type = type;
            addVarToCurTable(id);
            ls = ls->next;
        }
    }
    ;

/* Statements */
statementSequence:
    statement semicolonStatementStar {
        htslist* ls = mkHtslist();
        hts_append(ls, $1);
        slist* starlist = $2;
        while (starlist != NULL) {
            hts_append(ls, starlist->data);
            starlist = starlist->next;
        }
        freeSlist($2);
        $$ = ls;
    }
    ;
semicolonStatementStar:
    SEMICOLONSYM statement semicolonStatementStar {
        slist* l = mkSlist($2);
        l->next = $3;
        $$ = l;
    }
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
statement:
    assignment
        {$$ = $1;}
    | ifStatement
        {$$ = $1;}
    | whileStatement
        {$$ = $1;}
    | repeatStatement
        {$$ = $1;}
    | forStatement
        {$$ = $1;}
    | stopStatement
        {$$ = $1;}
    | returnStatement
        {$$ = $1;}
    | readStatement
        {$$ = $1;}
    | writeStatement
        {$$ = $1;}
    | procedureCall
        {$$ = $1;}
    | nullStatement
        {$$ = $1;}
    ;
assignment:
    lValue ASSIGNSYM expression
        {$$ = mkAssignmentStmt($1, $3);}
    ;
ifStatement:
    IFSYM expression THENSYM statementSequence elseifStar elseMaybe ENDSYM {
        conditional* cond = mkConditional($2, bt_nequal0, $4);
        htslist* conds = mkHtslist();
        hts_append(conds, cond);
        slist* elseifs = $5;
        while (elseifs != NULL) {
            hts_append(conds, elseifs->data);
            elseifs = elseifs->next;
        }
        conditional* elsecond = $6;
        if(elsecond != NULL) {
            hts_append(conds, elsecond);
        }
        // Add a null else block to fix bug where an if statement without
        // an else block always executes.
        conditional* nullcond = mkConditional(NULL, bt_nequal0, NULL);
        hts_append(conds, nullcond);

        $$ = mkIfStmt(conds);
    }
    ;
elseifStar:
    ELSEIFSYM expression THENSYM statementSequence elseifStar {
        conditional* c = mkConditional($2, bt_nequal0, $4);
        slist* ls = mkSlist(c);
        ls->next = $5;
        $$ = ls;
    }
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
elseMaybe:
    ELSESYM statementSequence
        {$$ = mkConditional(NULL, bt_nequal0, $2);}
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
whileStatement:
    WHILESYM expression DOSYM statementSequence ENDSYM {
        $$ = mkWhileStmt($2, $4);
    }
    ;
repeatStatement:
    REPEATSYM statementSequence UNTILSYM expression {
        $$ = mkRepeatStmt($4, $2);
    }
    ;
forStatement:
    FORSYM assignment toOrDownto expression DOSYM statementSequence ENDSYM {
        $$ = mkForStmt($2, $3, $4, $6);
    }
    ;
toOrDownto:
    TOSYM
        {$$ = Increment;}
    | DOWNTOSYM
        {$$ = Decrement;}
    ;
stopStatement:
    STOPSYM {
        $$ = mkStopStmt();
    }
    ;
returnStatement:
    RETURNSYM expressionMaybe {
        $$ = mkReturnStmt($2); 
    }
    ;
expressionMaybe:
    expression
        {$$ = $1;}
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
readStatement:
    READSYM LPARENSYM commaedLValueList RPARENSYM {
        $$ = mkReadStmt($3);
    }
    ;
commaedLValueList:
    lValue commaLValueStar {
        slist* ls = mkSlist($1);
        ls->next = $2;
        $$ = ls;
    }
    ;
commaLValueStar:
    COMMASYM lValue commaLValueStar {
        slist* ls = mkSlist($2);
        ls->next = $3;
        $$ = ls;
    }
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
writeStatement:
    WRITESYM LPARENSYM expressionList RPARENSYM
        {$$ = mkWriteStmt($3);}
    ;
expressionList:
    expression commaExpressionStar {
        slist* ls = mkSlist($1);
        ls->next = $2;
        $$ = ls;
    }
    ;
commaExpressionStar:
    COMMASYM expression commaExpressionStar {
        slist* ls = mkSlist($2);
        ls->next = $3;
        $$ = ls;
    }
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
procedureCall:
    identifier LPARENSYM maybeExpressionsWithCommas RPARENSYM {
        char* name = $1;
        slist* paramExprs = $3;
        $$ = mkProcedureStmt(name, paramExprs);
    }
    ;
maybeExpressionsWithCommas:
    expressionList
        {$$ = $1;}
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
nullStatement:
    /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;

/* Expressions */
expression:
    unaryOp expression
        {$$ = newUnOpExpr($1, $2);}
    | expression binaryOp expression
        {$$ = newBinOpExpr($2, $1, $3);}
    | LPARENSYM expression RPARENSYM
        {$$ = $2;}
    | procedureCall
        {$$ = newFuncCallExpr($1);}
    | CHRSYM LPARENSYM expression RPARENSYM
        {$$ = newCastExpr(char_type, $3);}
    | ORDSYM LPARENSYM expression RPARENSYM
        {$$ = newCastExpr(int_type, $3);}
    | PREDSYM LPARENSYM expression RPARENSYM
        {$$ = newBinOpExpr(op_sub, $3, one_expr);}
    | SUCCSYM LPARENSYM expression RPARENSYM
        {$$ = newBinOpExpr(op_add, $3, one_expr);}
    | lValue 
        {$$ = $1;}
    | NUMERICALSYM
        {$$ = newNumExpr(yylval.int_val);}
    | CHARACTERSYM
        {$$ = newCharExpr(yylval.char_val);}
    | STRINGSYM
        {$$ = newStrExpr(yylval.str_val);}
    ;
unaryOp:
    TILDESYM
        {$$ = op_not;}
    | MINUSSYM %prec UNARYMINUS
        {$$ = op_negate;}
    ;
binaryOp:
    PIPESYM 
        {$$ = op_or;}
    | AMPERSANDSYM 
        {$$ = op_and;}
    | EQUALSYM 
        {$$ = op_equal;}
    | NEQUALSYM 
        {$$ = op_nequal;}
    | LTESYM 
        {$$ = op_lte;}
    | GTESYM 
        {$$ = op_gte;}
    | LTSYM 
        {$$ = op_lt;}
    | GTSYM 
        {$$ = op_gt;}
    | PLUSSYM 
        {$$ = op_add;}
    | MINUSSYM 
        {$$ = op_sub;}
    | STARSYM 
        {$$ = op_mult;}
    | SLASHSYM 
        {$$ = op_div;}
    | PERCENTSYM 
        {$$ = op_mod;}
    ;
lValue:
    identifier dotIdentOrExpStar {
        expr* retval;
        ID* id = scopeLookup($1);
        slist* extlist = $2;
        if (id == NULL) {
            yyerror("Error, trying to look up an identifier which doesn't seem to be declared");
        }
        // Constant case
        if (id->id_kind == Constant_id) {
            expr* e = id->const_expr;
            if (e != NULL && e->kind == constant_expr) {
                retval = e;
            }
        } else if (id->id_kind == Variable && isGlobal(id)) {
            // global var case
            retval = newGlobalVExpr(id);
        } else if (id->id_kind == Variable) { // local variable
            retval = newLocalVExpr(id);
        } else {
            yyerror("It appears you're trying to use an lValue that's not yet supported.  Bummer!");
        }
        TYPE* curType = retval->type;
        expr* offsetExpr = zero_expr;
        while(extlist != NULL) {
            LvalExtension* lvext = extlist->data;
            if (lvext->type == RecordField) {
                ID* recordField = findRecordField(curType, lvext->data.fieldname);
                if (recordField == NULL) {
                    yyerror("Field not found in record");
                }
                int offset = recordField->id_addr;
                offsetExpr = newBinOpExpr(op_add, offsetExpr, newNumExpr(offset));
                curType = recordField->id_type;
            } else { // array type
                expr* index = lvext->data.index;
                TYPE* subtype = curType->ty_form.ty_array.ElementType;
                int elemsize = subtype->ty_size;
                // Adjust index because someone decided it would be a good idea to let it start somewhere other than 0
                expr* adjustedIndex = newBinOpExpr(op_sub, index, newNumExpr(curType->ty_form.ty_array.min));
                expr* offsetInArray = newBinOpExpr(op_mult, adjustedIndex, newNumExpr(elemsize));
                offsetExpr = newBinOpExpr(op_add, offsetExpr, offsetInArray);
                curType = subtype;
            }
            extlist = extlist->next;
        }
        retval->offsetExpr = offsetExpr;
        retval->type = curType;

        $$ = retval;
    }
    ;
dotIdentOrExpStar:
/* returns list of LvalExtensions */
    PERIODSYM identifier dotIdentOrExpStar {
        LvalExtension* lve = mkLvalExtension_field($2);
        slist* ls = mkSlist(lve);
        ls->next = $3;
        $$ = ls;
    }
    | LBRACKETSYM expression RBRACKETSYM dotIdentOrExpStar {
        LvalExtension* lve = mkLvalExtension_array($2);
        slist* ls = mkSlist(lve);
        ls->next = $4;
        $$ = ls;
    }
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
identifier:
    IDENTSYM                        {$$ = yylval.str_val;}
    ;


%%

/* Code */
int yyerror(const char *msg) 
{
    extern int yylineno; /* from flex */
    extern char *yytext; /* from flex */

    printf("ERROR: %s -- at symbol \"%s\" on line %i.\n", msg, yytext, yylineno);
    exit(1);
}


void addVarsOfType(slist* idents, TYPE* type) {
    while(idents) {
        char* name = idents->data;
        ID* id = newid(name);
        id->id_type = type;
        addVarToCurTable(id);
        
        idents = idents->next;
    }
}

void pushScope() {
    ++currscope;
    scopeAddr[currscope] = 0;
}

void popScope() {
    if(verbosity) {
        scopePrint(currscope);
    }
    freeIdTree(scope[currscope]);
    scope[currscope] = NULL;
    --currscope;
}

slist* addVars_and_retTypedIdentLists(slist* idents, TYPE* t) {
        addVarsOfType(idents, t);
        return retTypedIdentLists(idents, t);
}

slist* retTypedIdentLists(slist* idents, TYPE* t) {
        typedidentlist* til = malloc(sizeof(typedidentlist));
        til->type = t;
        til->names = idents;
        slist* ls = mkSlist(til);
        return ls;
}

