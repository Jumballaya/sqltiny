# Plans for SQLTiny

1. Finish Btree/B+tree implementation (with CRUD API)
2. Plan out SQL syntax and how it works with the database
  - List of SQL types and how they will be serialized
  - List of SQL commands that will be supported and the API commands they map to
4. Plan out how the VM is going to work and how the machine code will look
5. Create SQL tokenizer-\>parser-\>vm code
6. Create VM that interprets the code from the SQL parser and acts on the database (as planned above)


Right now the first thing to do is complete the tree code and refactor the exposed API to be easy to work with. The refactor should allow for a given schema instead of the currently hardcoded `id username email` schema.

The next thing to do is to build the SQL parser. To start I will write down every supported SQL type and command and figure out how it is going to work from tokenizer to btree operation. Next I will plan out what the VM and machine code is going to look like.

After that I will build the lexer/tokenizer and then design the AST and parser. Once I am satisfied with the AST I will create the compiler that compiles the AST to machine code for the VM I will build.

One of the last steps to having a working prototype of a SQL database is building the VM that takes the machine code and runs operations on the btree, and eventually the database file.
