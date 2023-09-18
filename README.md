## Overview
The programming language LISP, developed in 1958, is one of the oldest languages still in common use. The language is famous for: being fully parenthesised (that is, every instruction is inside its own brackets), having a prefix notation (e.g. functions are written (PLUS 1 2) and not (1 PLUS 2)) and its efficient linked-list Car/Cdr structure for (de-)composing lists. Here, I have developed a very simple language inspired by these concepts called NUCLEI and a means to parse or interpret the instructions.
This project includes a recursvie descent parser as well as an interpreter.

The project can be compiled with "make parse" and "make interp"

## The Formal Grammar
    <PROG> ::= "(" <INSTRCTS>

    <INSTRCTS> ::= <INSTRCT> <INSTRCTS> | ")"

    <INSTRCT> ::= "(" <FUNC> ")"

    <FUNC> ::= <RETFUNC> | <IOFUNC> | <IF> | <LOOP>

    <RETFUNC> ::= <LISTFUNC> | <INTFUNC> | <BOOLFUNC>

    <LISTFUNC> ::= "CAR" <LIST> | "CDR" <LIST> | "CONS" <LIST> <LIST>

    <INTFUNC> ::= "PLUS" <LIST> <LIST> | "LENGTH" <LIST>

    <BOOLFUNC> ::= "LESS" <LIST> <LIST> | "GREATER" <LIST> <LIST> | "EQUAL" <LIST> <LIST>

    <IOFUNC> ::= <SET> | <PRINT>
    <SET> ::= "SET" <VAR> <LIST>
    <PRINT> ::= "PRINT" <LIST> | "PRINT" <STRING>

    (IF (CONDITION) ((IF-TRUE-INSTRCTS)) ((IF_FALSE_INSTRCTS)))
    <IF> ::= "IF" "(" <BOOLFUNC> ")" "(" <INSTRCTS> "(" <INSTRCTS>

## Disclaimer
Please note, this work belongs to me and may not be used by students without my permission.
