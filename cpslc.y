/* cpslc.y */

%{
#include<stdio.h>
#include<stdlib.h>
int yyerror(const char *msg);
int yylex(void);
%}

%union{
    int int_val;
    char char_val;
    char *str_val;
}
/* give verbose errors */
%error-verbose
%defines "parser.h"

/* Tokens go here */

/* TODO - I think I have the precedence backwards */
%right UNARYMINUS
%left STARSYM SLASHSYM PERCENTSYM
%left PLUSSYM MINUSSYM
%nonassoc EQUALSYM NEQUALSYM LTSYM LTESYM GTSYM GTESYM
%right TILDESYM
%left AMPERSANDSYM
%left PIPESYM

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
%token            PERIODSYM 
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


%%
program:
    constantDeclMaybe typeDeclMaybe varDeclMaybe procOrFuncDeclStar block
    ;

/* Constant Declarations */
constantDeclMaybe:
    constantDeclPlus
    | empty
    ;
constantDeclPlus:
    constantDecl constantDeclPlus
    | constantDecl
    ;
constantDecl:
    CONSTSYM IDENTSYM EQUALSYM constExpression SEMICOLONSYM 
    ;

/* Procedure and Function Declarations */
procOrFuncDeclStar:
    procedureDecl procOrFuncDeclStar
    | functionDecl procOrFuncDeclStar
    | empty
    ;
procedureDecl:
    PROCEDURESYM IDENTSYM LPARENSYM formalParameters RPARENSYM SEMICOLONSYM FORWARDSYM SEMICOLONSYM
    | PROCEDURESYM IDENTSYM LPARENSYM formalParameters RPARENSYM SEMICOLONSYM body SEMICOLONSYM
    ;
functionDecl:
    FUNCTIONSYM IDENTSYM LPARENSYM formalParameters RPARENSYM COLONSYM type SEMICOLONSYM FORWARDSYM SEMICOLONSYM
    | FUNCTIONSYM IDENTSYM LPARENSYM formalParameters RPARENSYM COLONSYM type SEMICOLONSYM body SEMICOLONSYM
    ;
formalParameters:
    empty
    | varMaybe identList COLONSYM type formalParameterExt
    ;
formalParameterExt:
    SEMICOLONSYM varMaybe identList COLONSYM type formalParameterExt
    | empty
    ;
varMaybe:
    VARSYM
    | empty
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
    | empty
    ;
typeDecl:
    TYPESYM identEqTypePlus
    ;
identEqTypePlus:
    identEqType identEqTypePlus
    | identEqType
    ;
identEqType:
    IDENTSYM EQUALSYM type SEMICOLONSYM
    ;
type:
    simpleType
    | recordType
    | arrayType
    ;
simpleType:
    IDENTSYM
    ;
recordType:
    RECORDSYM identListsOfTypeStar ENDSYM
    ;
identListsOfTypeStar:
    identList COLONSYM type SEMICOLONSYM identListsOfTypeStar
    | empty
    ;
arrayType:
    ARRAYSYM LBRACKETSYM constExpression COLONSYM constExpression RBRACKETSYM OFSYM type
    ;
identList:
    IDENTSYM identExt
    ;
identExt:
    COMMASYM IDENTSYM identExt
    | empty
    ;

/* Variable Declarations */
varDeclMaybe:
    varDecl
    | empty
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
    | empty
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
    | empty
    ;
elseMaybe:
    ELSESYM statementSequence
    | empty
    ;
whileStatement:
    WHILESYM expression DOSYM statementSequence ENDSYM
    ;
repeatStatement:
    REPEATSYM statementSequence UNTILSYM expression
    ;
forStatement:
    FORSYM IDENTSYM ASSIGNSYM expression TOSYM expression DOSYM statementSequence ENDSYM
    ;
stopStatement:
    STOPSYM
    ;
returnStatement:
    RETURNSYM expressionMaybe
    ;
expressionMaybe:
    expression
    | empty
    ;
readStatement:
    READSYM LPARENSYM lValue commaLValueStar RPARENSYM
    ;
commaLValueStar:
    COMMASYM lValue commaLValueStar
    | empty
    ;
writeStatement:
    WRITESYM LPARENSYM expression commaExpressionStar RPARENSYM
    ;
commaExpressionStar:
    COMMASYM expression commaExpressionStar
    | empty
    ;
procedureCall:
    IDENTSYM LPARENSYM maybeExpressionsWithCommas RPARENSYM
    ;
maybeExpressionsWithCommas:
    expression commaExpressionStar
    | empty
    ;
nullStatement:
    empty
    ;

/* Expressions */
expression:
    unaryOp expression
    | expression binaryOp expression
    | LPARENSYM expression RPARENSYM
    | procedureCall
    | CHRSYM LPARENSYM expression RPARENSYM
    | ORDSYM LPARENSYM expression RPARENSYM
    | PREDSYM LPARENSYM expression RPARENSYM
    | SUCCSYM LPARENSYM expression RPARENSYM
    | lValue
    | constExpression
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
    IDENTSYM
    | IDENTSYM PERIODSYM IDENTSYM
    | IDENTSYM LBRACKETSYM expression RBRACKETSYM
    ;
constExpression:
    unaryOp constExpression
    | constExpression binaryOp constExpression
    | LPARENSYM constExpression RPARENSYM
    | NUMERICALSYM
    | CHARACTERSYM
    | STRINGSYM
    | IDENTSYM
    ;

empty:
    %empty
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


