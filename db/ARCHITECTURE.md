# DB

# Flow

[frontend]

1. Tokenizer
2. Parser
3. Code Generator

[backend]

4. Virtual Machine
5. B-Tree
6. Pager
7. OS Interface

# Tokenizer

-   break down input queries into smaller components called tokens
-   scans the query string from left to right, recognizers token based on rules or reg-ex

# Parser

-   takes tokens from the Tokenizer and validates them against the grammar rules of SQL
-   builds an AST

# Code Generator

-   Translates the parse tree or AST into an intermediate representation or bytecode, which the backend can execute

# Virtual Machine

-   responsible for executing the bytecode or IR gernerated by Code Generator
-   processes bytecode instruction, performaing acitons like retriving data, applying filters, and joining tables

# B-Tree

-   used for indexing and organizing data efficiently on disk, allowing for fast search, insert, delete, and update

# Pager

-   manages reading and writing to disk pages, acting as an interface between in-memery data nad on-disk storage, ensuring efficient data transfer and managing memmoery efficiently.

# OS Interface

-   responsible for interacting with the operating system for tasks like file I/O, memeroy allcoation, locking mechanims,s and hanlding system resouces
