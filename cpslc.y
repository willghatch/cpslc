/* cpslc.y */

%{
#include<stdio.h>
#include<stdlib.h>
#include"symtab.h"
#include"expression.h"
int yyerror(const char *msg);
int yylex(void);
%}

%code requires {
#include"expression.h"
#include"symtab.h"
#include"slist.h"
#include"typedidentlist.h"

void addIdentsOfType(slist* idents, TYPE* type);
}

%union{
    int int_val;
    char char_val;
    char *str_val;

    expr* expr_val;
    slist* list_ptr;
    ID* ID_val;
    TYPE* type_val;
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
%type <str_val>   identifier 
%type <type_val>    type
%type <list_ptr>  identListsOfTypeStar
%type <list_ptr>  identList
%type <list_ptr>  identExt 
%type <str_val>   simpleType
%type <type_val>  recordType
%type <type_val>  arrayType

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
    constantDeclMaybe typeDeclMaybe varDeclMaybe procOrFuncDeclStar block endPeriod
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
        id->id_kind = Constant; 
        id->const_expr = $3;
        addIdToTable(id, scope+currscope);
    }
    ;

/* Procedure and Function Declarations */
procOrFuncDeclStar:
    procedureDecl procOrFuncDeclStar
    | functionDecl procOrFuncDeclStar
    | /* empty */ %prec EMPTY
    ;
procedureDecl:
    PROCEDURESYM identifier {++currscope;} LPARENSYM formalParameters RPARENSYM SEMICOLONSYM FORWARDSYM {--currscope;} SEMICOLONSYM
    | PROCEDURESYM identifier {++currscope;} LPARENSYM formalParameters RPARENSYM SEMICOLONSYM body {--currscope;} SEMICOLONSYM
    ;
functionDecl:
    FUNCTIONSYM identifier {++currscope;} LPARENSYM formalParameters RPARENSYM COLONSYM type SEMICOLONSYM FORWARDSYM {--currscope;} SEMICOLONSYM
    | FUNCTIONSYM identifier {++currscope;} LPARENSYM formalParameters RPARENSYM COLONSYM type SEMICOLONSYM body {--currscope;} SEMICOLONSYM
    ;
formalParameters:
    varMaybe identList COLONSYM type formalParameterExt {
        addIdentsOfType($2, $4);
    }
    | /* empty */ %prec EMPTY
    ;
formalParameterExt:
    SEMICOLONSYM varMaybe identList COLONSYM type formalParameterExt {
        addIdentsOfType($3, $5);
    }
    | /* empty */ %prec EMPTY
    ;
varMaybe:
    VARSYM
    | /* empty */ %prec EMPTY
    ;
body:
    constantDeclMaybe typeDeclMaybe varDeclMaybe block
    ;
block:
    BEGINSYM statementSequence ENDSYM
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
        addIdToTable(typeid, scope+currscope);
    }
    ;
type:
    /* TODO - return real TYPE* */
    simpleType {
        ID* id = scopeLookup($1);
        if (id == NULL) {
            printf("Error: trying to use undefined type %s\n", $1);
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
            printf("identList with field with type %s\n", $3->ty_name);
        $$ = tyList;
    }
    | /* empty */ %prec EMPTY
        {$$ = NULL;}
    ;
arrayType:
    ARRAYSYM LBRACKETSYM expression COLONSYM expression RBRACKETSYM OFSYM type {
        int min = 0;
        int max = 5;
        if (min > max) {
            int temp = min;
            min = max;
            max = temp;
        }
        // TODO - get the real min and max
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
            printf("Error, trying to declare variable %s of unknown type\n", ls->data);
        }
        while (ls != NULL) {
            ID* id = newid(ls->data);
            id->id_type = type;
            addIdToTable(id, scope+currscope);
            ls = ls->next;
        }
    }
    ;

/* Statements */
statementSequence:
    statement semicolonStatementStar
    ;
semicolonStatementStar:
    SEMICOLONSYM statement semicolonStatementStar
    | /* empty */ %prec EMPTY
    ;
statement:
    assignment
    | ifStatement
    | whileStatement
    | repeatStatement
    | forStatement
    | stopStatement
    | returnStatement
    | readStatement
    | writeStatement
    | procedureCall
    | nullStatement
    ;
assignment:
    lValue ASSIGNSYM expression
    ;
ifStatement:
    IFSYM expression THENSYM statementSequence elseifStar elseMaybe ENDSYM
    ;
elseifStar:
    ELSEIFSYM expression THENSYM statementSequence elseifStar
    | /* empty */ %prec EMPTY
    ;
elseMaybe:
    ELSESYM statementSequence
    | /* empty */ %prec EMPTY
    ;
whileStatement:
    WHILESYM expression DOSYM statementSequence ENDSYM
    ;
repeatStatement:
    REPEATSYM statementSequence UNTILSYM expression
    ;
forStatement:
    FORSYM identifier ASSIGNSYM expression toOrDownto expression DOSYM statementSequence ENDSYM
    ;
toOrDownto:
    TOSYM
    | DOWNTOSYM
    ;
stopStatement:
    STOPSYM
    ;
returnStatement:
    RETURNSYM expressionMaybe
    ;
expressionMaybe:
    expression
    | /* empty */ %prec EMPTY
    ;
readStatement:
    READSYM LPARENSYM lValue commaLValueStar RPARENSYM
    ;
commaLValueStar:
    COMMASYM lValue commaLValueStar
    | /* empty */ %prec EMPTY
    ;
writeStatement:
    WRITESYM LPARENSYM expression commaExpressionStar RPARENSYM
    ;
commaExpressionStar:
    COMMASYM expression commaExpressionStar
    | /* empty */ %prec EMPTY
    ;
procedureCall:
    identifier LPARENSYM maybeExpressionsWithCommas RPARENSYM
    ;
maybeExpressionsWithCommas:
    expression commaExpressionStar
    | /* empty */ %prec EMPTY
    ;
nullStatement:
    /* empty */ %prec EMPTY
    ;

/* Expressions */
expression:
    unaryOp expression
        {$$ = NULL;} /* TODO - fix*/
    | expression binaryOp expression
        {$$ = NULL;} /* TODO - fix*/
    | LPARENSYM expression RPARENSYM
        {$$ = NULL;} /* TODO - fix*/
    | procedureCall
        {$$ = NULL;} /* TODO - fix*/
    | CHRSYM LPARENSYM expression RPARENSYM
        {$$ = NULL;} /* TODO - fix*/
    | ORDSYM LPARENSYM expression RPARENSYM
        {$$ = NULL;} /* TODO - fix*/
    | PREDSYM LPARENSYM expression RPARENSYM
        {$$ = NULL;} /* TODO - fix*/
    | SUCCSYM LPARENSYM expression RPARENSYM
        {$$ = NULL;} /* TODO - fix*/
    | lValue
        {$$ = NULL;} /* TODO - fix*/
    | NUMERICALSYM
        {$$ = newNumExpr(yylval.int_val);}
    | CHARACTERSYM
        {$$ = newCharExpr(yylval.char_val);}
    | STRINGSYM
        {$$ = newStrExpr(yylval.str_val);}
    ;
unaryOp:
    TILDESYM
    | MINUSSYM %prec UNARYMINUS
    ;
binaryOp:
    PIPESYM 
    | AMPERSANDSYM 
    | EQUALSYM 
    | NEQUALSYM 
    | LTESYM 
    | GTESYM 
    | LTSYM 
    | GTSYM 
    | PLUSSYM 
    | MINUSSYM 
    | STARSYM 
    | SLASHSYM 
    | PERCENTSYM 
    ;
lValue:
    identifier dotIdentOrExpStar
    ;
dotIdentOrExpStar:
    PERIODSYM identifier dotIdentOrExpStar
    | LBRACKETSYM expression RBRACKETSYM dotIdentOrExpStar
    | /* empty */ %prec EMPTY
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


void addIdentsOfType(slist* idents, TYPE* type) {
    while(idents) {
        char* name = idents->data;
        ID* id = newid(name);
        id->id_type = type;
        addIdToTable(id, scope+currscope);
        
        idents = idents->next;
    }
}


