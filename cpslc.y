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
#include"slist.h"
}

%union{
    int int_val;
    char char_val;
    char *str_val;

    expr* expr_val;
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
    PROCEDURESYM identifier LPARENSYM formalParameters RPARENSYM SEMICOLONSYM FORWARDSYM SEMICOLONSYM
    | PROCEDURESYM identifier LPARENSYM formalParameters RPARENSYM SEMICOLONSYM body SEMICOLONSYM
    /* TODO - put ID in table as proc (type void), then add level to stack and push parameters. */
    ;
functionDecl:
    FUNCTIONSYM identifier LPARENSYM formalParameters RPARENSYM COLONSYM type SEMICOLONSYM FORWARDSYM SEMICOLONSYM
    | FUNCTIONSYM identifier LPARENSYM formalParameters RPARENSYM COLONSYM type SEMICOLONSYM body SEMICOLONSYM
    /* TODO - put ID in table as func, then add level to stack and push parameters. */
    ;
formalParameters:
    varMaybe identList COLONSYM type formalParameterExt
    /* TODO - make linked list of id names, make ID obj's into list to return... don't push onto table yet... also link up the list from the ext*/
    | /* empty */ %prec EMPTY
    ;
formalParameterExt:
    SEMICOLONSYM varMaybe identList COLONSYM type formalParameterExt
    /* TODO -- do same here as in normal formal parameters */
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
    identifier EQUALSYM type SEMICOLONSYM
    ;
type:
    simpleType
    | recordType
    | arrayType
    ;
simpleType:
    identifier
    ;
recordType:
    RECORDSYM identListsOfTypeStar ENDSYM
    ;
identListsOfTypeStar:
    identList COLONSYM type SEMICOLONSYM identListsOfTypeStar
    | /* empty */ %prec EMPTY
    ;
arrayType:
    ARRAYSYM LBRACKETSYM expression COLONSYM expression RBRACKETSYM OFSYM type
    ;
identList:
    identifier identExt
    ;
identExt:
    COMMASYM identifier identExt
    | /* empty */ %prec EMPTY
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
    identList COLONSYM type SEMICOLONSYM
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


