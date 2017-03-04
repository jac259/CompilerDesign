# Compiler Design Course

Compiler
----------------------------------
 * This program is an in-progress compiler for Dr. Sutton's Compiler Design course.
 * Currently this program can
   * create abstract syntax trees containing expressions of integer and boolean types,
   * perform lexical analysis on a text file and build tokens based on the characters in the text,
   * ignore text after a comment symbol '#',
   * accept integer inputs in decimal, hexadecimal (0x prefix), and binary (0b prefix),
   * print integer token values in decimal, hexadecimal, or binary, and
   * read from standard input (line by line or through redirection) and print to standard output.
 * The program can be run with options such as:
   * ./build (command line inputs)
   * ./build < inputfile.txt
   * ./build < inputfile.txt > outputfile.txt
   * ./build -* < inputfile.txt > outputfile.txt
     * where * is replaced with
       * h for hexadecimal output,
       * b for binary output, or
       * d for decimal output (optional, this is the default).