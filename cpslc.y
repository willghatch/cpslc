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
    identifier EQUALSYM expression SEMICOLONSYM
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
    ;
functionDecl:
    FUNCTIONSYM identifier LPARENSYM formalParameters RPARENSYM COLONSYM type SEMICOLONSYM FORWARDSYM SEMICOLONSYM
    | FUNCTIONSYM identifier LPARENSYM formalParameters RPARENSYM COLONSYM type SEMICOLONSYM body SEMICOLONSYM
    ;
formalParameters:
    varMaybe identList COLONSYM type formalParameterExt
    | /* empty */ %prec EMPTY
    ;
formalParameterExt:
    SEMICOLONSYM varMaybe identList COLONSYM type formalParameterExt
    | /* empty */ %prec EMPTY
    ;
varMaybe:
    VARSYM
    | /* empty */ %prec EMPTY
    ;
body:
    constantDeclMaybe typeDeclMaybe varDeclMaybe block
    { printf("In Body\n");}
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
    | expression binaryOp expression
    | LPARENSYM expression RPARENSYM
    | procedureCall
    | CHRSYM LPARENSYM expression RPARENSYM
    | ORDSYM LPARENSYM expression RPARENSYM
    | PREDSYM LPARENSYM expression RPARENSYM
    | SUCCSYM LPARENSYM expression RPARENSYM
    | lValue
    | NUMERICALSYM
    | CHARACTERSYM
    | STRINGSYM                                     {free(yylval.str_val);}
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
    IDENTSYM                        {free(yylval.str_val);}
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


