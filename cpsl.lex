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
ARRAY               {return(ARRAYSY);}
array               {return(ARRAYSY);}
BEGIN               {return(BEGINSY);}
begin               {return(BEGINSY);}
CHR                 {return(CHRSY);}
chr                 {return(CHRSY);}
CONST               {return(CONSTSY);}
const               {return(CONSTSY);}
DO                  {return(DOSY);}
do                  {return(DOSY);}
DOWNTO              {return(DOWNTOSY);}
downto              {return(DOWNTOSY);}
ELSE                {return(ELSESY);}
else                {return(ELSESY);}
ELSEIF              {return(ELSEIFSY);}
elseif              {return(ELSEIFSY);}
END                 {return(ENDSY);}
end                 {return(ENDSY);}
FOR                 {return(FORSY);}
for                 {return(FORSY);}
FORWARD             {return(FORWARDSY);}
forward             {return(FORWARDSY);}
FUNCTION            {return(FUNCTIONSY);}
function            {return(FUNCTIONSY);}
IF                  {return(IFSY);}
if                  {return(IFSY);}
OF                  {return(OFSY);}
of                  {return(OFSY);}
ORD                 {return(ORDSY);}
ord                 {return(ORDSY);}
PRED                {return(PREDSY);}
pred                {return(PREDSY);}
PROCEDURE           {return(PROCEDURESY);}
procedure           {return(PROCEDURESY);}
READ                {return(READSY);}
read                {return(READSY);}
RECORD              {return(RECORDSY);}
record              {return(RECORDSY);}
REPEAT              {return(REPEATSY);}
repeat              {return(REPEATSY);}
RETURN              {return(RETURNSY);}
return              {return(RETURNSY);}
STOP                {return(STOPSY);}
stop                {return(STOPSY);}
SUCC                {return(SUCCSY);}
succ                {return(SUCCSY);}
THEN                {return(THENSY);}
then                {return(THENSY);}
TO                  {return(TOSY);}
to                  {return(TOSY);}
TYPE                {return(TYPESY);}
type                {return(TYPESY);}
UNTIL               {return(UNTILSY);}
until               {return(UNTILSY);}
VAR                 {return(VARSY);}
var                 {return(VARSY);}
WHILE               {return(WHILESY);}
while               {return(WHILESY);}
WRITE               {return(WRITESY);}
write               {return(WRITESY);}

  /* Identifiers */
{letter}({lord})*   {yylval.name_ptr = strdup(yytext); return(IDENTSY);}

  /* Octal Constants */
0[0-7]*             {yylval.int_val = intnum(); return(CONSTANTSY);}
  /* Decimal Constants */
({digit})+          {yylval.int_val = intnum(); return(CONSTANTSY);}
  /* Hexadecimal Constants */
0x[0-9a-fA-F]+      {yylval.int_val = intnum(); return(CONSTANTSY);}


  /* Operators */
":="                {return(ASSIGNSY);}
":"                 {return(COLONSY);}
.                   {error("Illegal character");}
%%
int intnum ()
/* convert character string into an integer */
{
   /* remember to deal with octal, decimal, and hexadecimal (or make it actually 3 functions) */
};  /* intnum */
