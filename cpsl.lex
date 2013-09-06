%{
#include <string.h>
#include "utility.h"
#include "pascal.tab.h"
%}
letter      [a-zA-Z]
digit       [0-9]
lord        [a-zA-Z0-9]
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
{letter}({lord})*   {yylval.name_ptr = strdup(yytext); return(IDENTSYM);}

  /* TODO - Predefined identifiers??? */

  /* Operators/Delimiters */
"+"                 {return(PLUSSYM);}
"-"                 {return(MINUSSYM);}
"*"                 {return(STARSYMM);}
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
\'([\040-\133\135-\177]|\\[\040-\177])\'    {return(CHARACTERSYM);} /* TODO - read the character */

  /* String Constants */
\"([\040-\177])*\"  {return(STRINGSYM);} /* TODO - read the string */

  /* Comments */
\$([^\n])*\n        {}

  /* White Space */
([\r\n\t\040])+     {}

  /* Illegal (catch-all) */
.                   {error("Illegal character");}

%%
int intnum ()
/* convert character string into an integer */
{
   /* remember to deal with octal, decimal, and hexadecimal (or make it actually 3 functions) */
};  /* intnum */
