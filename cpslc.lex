%top{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

/* TODO - find a system for error numbers */
#define LEXERRORNUM 58


union YYSTYPE yylval;
char *strUnescape(char *input);
int yywrap();
int intnum();
void lex_error(char *msg);
extern int yylex();

}
letter      [a-zA-Z]
digit       [0-9]
lord        [a-zA-Z0-9]
let_dig_und [a-zA-Z0-9_]
%%
  /* Keywords */
ARRAY               {return(ARRAYSYM);}
array               {return(ARRAYSYM);}
BEGIN               {return(BEGINSYM);}
begin               {return(BEGINSYM);}
CHR                 {return(CHRSYM);}
chr                 {return(CHRSYM);}
CONST               {return(CONSTSYM);}
const               {return(CONSTSYM);}
DO                  {return(DOSYM);}
do                  {return(DOSYM);}
DOWNTO              {return(DOWNTOSYM);}
downto              {return(DOWNTOSYM);}
ELSE                {return(ELSESYM);}
else                {return(ELSESYM);}
ELSEIF              {return(ELSEIFSYM);}
elseif              {return(ELSEIFSYM);}
END                 {return(ENDSYM);}
end                 {return(ENDSYM);}
FOR                 {return(FORSYM);}
for                 {return(FORSYM);}
FORWARD             {return(FORWARDSYM);}
forward             {return(FORWARDSYM);}
FUNCTION            {return(FUNCTIONSYM);}
function            {return(FUNCTIONSYM);}
IF                  {return(IFSYM);}
if                  {return(IFSYM);}
OF                  {return(OFSYM);}
of                  {return(OFSYM);}
ORD                 {return(ORDSYM);}
ord                 {return(ORDSYM);}
PRED                {return(PREDSYM);}
pred                {return(PREDSYM);}
PROCEDURE           {return(PROCEDURESYM);}
procedure           {return(PROCEDURESYM);}
READ                {return(READSYM);}
read                {return(READSYM);}
RECORD              {return(RECORDSYM);}
record              {return(RECORDSYM);}
REPEAT              {return(REPEATSYM);}
repeat              {return(REPEATSYM);}
RETURN              {return(RETURNSYM);}
return              {return(RETURNSYM);}
STOP                {return(STOPSYM);}
stop                {return(STOPSYM);}
SUCC                {return(SUCCSYM);}
succ                {return(SUCCSYM);}
THEN                {return(THENSYM);}
then                {return(THENSYM);}
TO                  {return(TOSYM);}
to                  {return(TOSYM);}
TYPE                {return(TYPESYM);}
type                {return(TYPESYM);}
UNTIL               {return(UNTILSYM);}
until               {return(UNTILSYM);}
VAR                 {return(VARSYM);}
var                 {return(VARSYM);}
WHILE               {return(WHILESYM);}
while               {return(WHILESYM);}
WRITE               {return(WRITESYM);}
write               {return(WRITESYM);}

  /* Identifiers */
{letter}({let_dig_und})*   {yylval.str_val = strdup(yytext); return(IDENTSYM);}

  /* TODO - Predefined identifiers??? */

  /* Operators/Delimiters */
"+"                 {return(PLUSSYM);}
"-"                 {return(MINUSSYM);}
"*"                 {return(STARSYM);}
"/"                 {return(SLASHSYM);}
"&"                 {return(AMPERSANDSYM);}
"|"                 {return(PIPESYM);}
"~"                 {return(TILDESYM);}
"="                 {return(EQUALSYM);}
"<>"                {return(NEQUALSYM);}
"<"                 {return(LTSYM);}
"<="                {return(LTESYM);}
">"                 {return(GTSYM);}
">="                {return(GTESYM);}
"."                 {return(PERIODSYM);}
","                 {return(COMMASYM);}
":"                 {return(COLONSYM);}
";"                 {return(SEMICOLONSYM);}
"("                 {return(LPARENSYM);}
")"                 {return(RPARENSYM);}
"["                 {return(LBRACKETSYM);}
"]"                 {return(RBRACKETSYM);}
"%"                 {return(PERCENTSYM);}
":="                {return(ASSIGNSYM);}

  /* Octal Constants */
0[0-7]*             {yylval.int_val = intnum(); return(NUMERICALSYM);}
  /* Decimal Constants */
({digit})+          {yylval.int_val = intnum(); return(NUMERICALSYM);}
  /* Hexadecimal Constants */
0x[0-9a-fA-F]+      {yylval.int_val = intnum(); return(NUMERICALSYM);}

  /* Character Constants */
\'([\040-\133\135-\177]|\\[\040-\177])\'    {
    char *dupstr = strUnescape(yytext);
    yylval.char_val = dupstr[0];
    free(dupstr);
    return(CHARACTERSYM);} 

  /* String Constants */
\"(([\040-\041\043-\177])|(\\\"))*\"  {yylval.str_val = strUnescape(yytext); return(STRINGSYM);} 

  /* Comments */
\$([^\n])*\n        {++yylineno;}

  /* White Space */
([\r\t\040])+     {}

  /* increment newline */
\n                  {++yylineno;}

  /* Illegal (catch-all) */
.                   {lex_error("Illegal character"); return ERRORSYM;}

%%
int intnum ()
/* convert character string into an integer */
{
    char *atol_end;
    return (int) strtol(yytext, &atol_end, 0);
};  /* intnum */

char getEscapedChar(char in)
/* Returns eg. LF for n, CR for r, TAB for t... */
{
    switch (in) {
        case 'n': return '\n';
        case 'r': return '\r';
        case 'b': return '\b';
        case 't': return '\t';
        case 'f': return '\f';
        default: return in;
    }
}

char *strUnescape (char *input)
/* Like strdup, only it strips the first and last character (quotes) and converts
   escape sequences to literal characters. */
{
    int textlen = strlen(input);
    int unescapedlen = 0;
    char *dupstr;
    dupstr = malloc(sizeof(char) * (textlen+1));
    int escaped = 0;
    for(int i = 1 /*start after opening "*/; i < textlen -1 /*ignore closing quote*/; ++i)
    {
        if(escaped)
        {
            dupstr[unescapedlen++] = getEscapedChar(input[i]);
            escaped = 0;
        }
        else
        {
            if(input[i] == '\\') 
            {
                escaped = 1;
            }
            else
            {
                dupstr[unescapedlen++] = input[i];
            }
        }
    }
    dupstr[unescapedlen++] = 0;
    return dupstr;
}
/*  TODO - when should I free the strings I get with strUnescape or strdup??? 
    Note: There are no memory leaks -- the kernel cleans it up. (While that's facicious
    in this case, that's actually the way I feel for non-daemon, non-interactive programs
    generally.  And my philosophy is that that's where all the work should usually
    be done anyway.)
    But for now I clean them up when the parser hits them.
*/

void lex_error(char *msg)
{
    printf("Error: %s -- on line %i.", msg, yylineno);
    exit(LEXERRORNUM);
}


int yywrap() 
{
    return 1;
}

