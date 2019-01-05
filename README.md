# ![](doc/img/logo.png)
Shi is a fast and tiny [Forth](https://en.wikipedia.org/wiki/Forth_(programming_language)) implementation written for the [Thumb-2 instruction set](https://infocenter.arm.com/help/topic/com.arm.doc.qrc0001m/QRC0001_UAL.pdf) which runs on ARMv7-M architecture and newer. It currently fits into 8kB of flash and 320B of ram. Some of it's features are
* A
* B

# Introduction
Forth is an imperative explicitly stack-based language mostly known for its quirky [RPN](https://en.wikipedia.org/wiki/Reverse_Polish_notation) syntax. Explicitly stack-based means that everything in Forth either consumes or creates elements on a gobal data-stack upon execution. While other languages differ between expressions, operators, functions and so on Forth only has the notion of *words*. A word can be any combination of characters but a whitespace and simply denotes an executable entry in a collection of words called a *dictionary*. A word not found by the parser must be a number or results in an *undefined word* error. The dictionary is extensible and allows for the addition of new words.

For further information or a comprehensive tutorial refer to
* [Starting Forth](doc/books/Starting-FORTH.pdf)
* [Thinking Forth](doc/books/thinking-forth-color.pdf)

## "Reverse Polish" ![](doc/img/polish_flag.png)
Although Forth is quite a unique language in all of its aspects it is the complementary RPN and stack which really let it shine in very restricted environments. To understand why let's create a real world example by writing a linear interpolation function we'd like to parse and interpret or even compile.

In common infix notation languages like C we'd probably write something like
```c
int lerp(int x, int x1, int x2, int y1, int y2) {
  return y1 + ((y2 - y1) * (x - x1)) / (x2 - x1);
}
```

From a programmers point of view this expression is easy to read and understand but it actually puts a lot of responsibility into the hands of the interpreter/compiler. The intended order of operations given by parentheses and precedence rules has to be discovered. In case the language ain't stack-based there might be a calling convention the interpreter/compiler needs to obey. It needs to figure out which registers are used for passing and returning arguments. Typically an expression has to be parsed multiple times in order to build up whats called an [AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree), a representation of the input the compiler can work with. This is a time and memory consuming task.

In Forth the same function could be written as
```
: lerp ( x x1 x2 y1 y2 -- y )
  over - 4 roll 4 pick - * 2swap swap - / +
;
```

This is inarguably harder to read and shifts responsibility from the interpreter/compiler to the programmer but it has one major advantage. It allows for handling everything in a single pass. Every single *word* from the input stream can be interpreted or compiled without any prior or subsequent knowledge. New *words* such as *lerp* simply use calls (or inlined copies of the assembly) to other fine grained *words* such as *over* or *-* to achieve a very high level of reusability. A Forth parser basically degenerates to a linked list look-up which easily fits into a few kB of memory.

If you don't immediately understand the *words* between the actual operators don't worry. These are sometimes refered to as "stack jugglers" and simply put stack arguments in the right order. The right order can be found by converting the original algorithm from infix to postfix notation using the [shunting-yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm).

```c
// Infix notation
y1 + ((y2 - y1) * (x - x1)) / (x2 - x1)

// Reverse Polish notation (postfix notation)
y1 y2 y1 - x x1 - * x2 x1 - / +
```

A more detailed explanation of how lerp works can be found in the [documentation](/doc/html/page_lerp.html).

## Performance
MicroPython, Lua(eLua), Partcl https://github.com/zserge/partcl

## Usage
Blabla so und so.