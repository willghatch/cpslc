cpslc 
=====

the Compilers Project Source Language Compiler

By William Hatch (willghatch@gmail.com)

An exciting class project for CS5300 at Utah State University.

Compiles CPSL to mips assembly suitable for running on spim or MARS simulators.

Built to comply with the specification document included as CPSL.pdf (also available
in source from https://github.com/ksundberg/CS5300).
The test files from the class are also included in the ClassTestFiles directory (copied
in case future classes change the specifications of the compiler, etc).  They are also
available from Dr. Sundberg's git repository given above.

The executable `cpslc` is the compiler.
It prints a lovely help message if run with -h, --help, or an unknown argument.
Syntax is:
`cpslc [-hv] [-f infile] [-o outfile]`
where the flags can be replaced by --help, --verbose, --file, and --output respectively.


The executable `lextest` is just a lexical analyzer.  It prints int values for the symbols found, and their yylval
value if they're an identifier or constant (int, string, or char).  It takes the name of a file or
it reads from stdin.


Just run `make` to build, and there should be no problems as long as you have:
- bison
- flex
- c/unix standard libraries

Comments
--------

Since I'm not sure what libraries the grader will have installed, and to not have
him install any more, I rolled some of my own stuff (linked list, etc) that I would
otherwise have used a library for.  I wanted to use pure C instead of C++ to get
some more experience with just C.

The style of the code is pretty bad -- I mixed a hodgepodge of styles, as I was
kinda trying to find what style I liked for straight C.  Some of the code in
symtab.c and symtab.h I took from the class slides on the symbol table stuff.
I'm not really sure why I re-used class code for that and nothing else... but that's
what happened.

It was fun and fulfilling to make.  I'm really glad I took this class and wrote this
compiler.

I'll probably put this on github, because hey, I built a compiler.  If random people
from the internet inspect this code remember:  It was a school project.  Corners were
cut.  Documentation was ignored.  It's mostly awful.  But it works... so... pedagogical
goal completed I suppose.

Known Issues
------------

If you have a long expression of the right type, you might run out of registers.
There is no register spilling code.  But you should be safe unless you have something
like a + (b + (c + (d + (e + f)))).  And it would have to be longer.  So I've never
actually run into the problem.

Error messages are pretty bad.  Oh well.


