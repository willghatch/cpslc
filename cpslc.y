/* cpslc.y */

%{
#include "foobar"
int yyerror(char *msg);
int yylex(void);
%}

%union{
    int int_val;
    char char_val;
    char *str_val;
}

/* Tokens go here */

%token            FLEX_EOF_SYM 
%token            AMPERSANDSYM 
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
%token            EQUALSYM 
%token            FORSYM 
%token            FORWARDSYM 
%token            FUNCTIONSYM 
%token            GTESYM 
%token            GTSYM 
%token            IDENTSYM 
%token            IFSYM 
%token            LBRACKETSYM 
%token            LPARENSYM 
%token            LTESYM 
%token            LTSYM 
%token            MINUSSYM 
%token            NEQUALSYM 
%token <int_val>  NUMERICALSYM 
%token            OFSYM 
%token            ORDSYM 
%token            PERCENTSYM 
%token            PERIODSYM 
%token            PIPESYM 
%token            PLUSSYM 
%token            PREDSYM 
%token            PROCEDURESYM 
%token            RBRACKETSYM 
%token            READSYM 
%token            RECORDSYM 
%token            REPEATSYM 
%token            RETURNSYM 
%token            RPARENSYM 
%token            SEMICOLONSYM 
%token            SLASHSYM 
%token            STARSYM 
%token            STOPSYM 
%token <str_val>  STRINGSYM 
%token            SUCCSYM 
%token            THENSYM 
%token            TILDESYM 
%token            TOSYM 
%token            TYPESYM 
%token            UNTILSYM 
%token            VARSYM 
%token            WHILESYM 
%token            WRITESYM 

%%
program:
    constantDeclMaybe typeDeclMaybe varDeclMaybe procOrFuncDecls block
    ;
constantDeclMaybe:
    constantDecls
    | empty
    ;
constantDecls:
    constantDecl constantDecls
    | constantDecl
    ;
constantDecl:
    CONSTSYM IDENTSYM EQUALSYM constExpression SEMICOLONSYM constantDecl
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
    | varMaybe identList COLONSYM formalParameterExt
    ;
formalParameterExt:
    SEMICOLONSYM varMaybe identList COLONSYM type formalParameterExt
    | empty
    ;

