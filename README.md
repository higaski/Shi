# ![alt text][logo]
Shi is a fast and tiny [Forth](https://en.wikipedia.org/wiki/Forth_(programming_language)) implementation written for the [Thumb-2 instruction set](https://infocenter.arm.com/help/topic/com.arm.doc.qrc0001m/QRC0001_UAL.pdf) which runs on ARMv7-M architecture and newer. It currently fits into 8kB of flash and 320B of ram. Some of it's features are:
* A
* B

[logo]: doc/img/logo.png

# Introduction
Forth is an imperative explicitly stack-based language mostly known for its quirky [RPN](https://en.wikipedia.org/wiki/Reverse_Polish_notation) syntax. Explicitly stack-based means that everything in Forth either consumes or creates elements on a gobal data-stack upon execution. While other languages differ between expressions, operators, functions and so on Forth only has the notion of *words*. A word can be any combination of characters but a whitespace and simply denotes an executable entry in a collection of words called a *dictionary*. A word not found by the parser must be a number or results in an *undefined word* error. The dictionary is extensible and allows for the addition of new words.

For further information or a comprehensive tutorial refer to:
* [Starting Forth](doc/books/Starting-FORTH.pdf)
* [Thinking Forth](doc/books/thinking-forth-color.pdf)

## "Reverse Polish" ![alt text][polish_flag]
Although Forth is quite a unique language in all of its aspects it is the complementary RPN and stack which really let it shine in very restricted environments.

[polish_flag]: doc/img/polish_flag.png

RPN && stack-based == WINNER WINNER

Let's create a real world example by writing a linear interpolation function.


```c
int lerp(int x, int x1, int x2, int y1, int y2) {
  return y1 + ((y2 - y1) * (x - x1)) / (x2 - x1);
}
```


'y1 y2 y1 - x x1 - * x2 x1 - / +'

'x x1 x2 y1 y2'

over

'x x1 x2 y1 y2 y1'

-

'x x1 x2 y1 (y2-y1)'

4 roll

'x1 x2 y1 (y2-y1) x'

4 pick

'x1 x2 y1 (y2-y1) x x1'

-

'x1 x2 y1 (y2-y1) (x-x1)'

*

'x1 x2 y1 (y2-y1)*(x-x1)'

2swap

'y1 (y2-y1)*(x-x1) x1 x2'

swap

'y1 (y2-y1)*(x-x1) x2 x1'

-

'y1 (y2-y1)*(x-x1) (x2-x1)'

/

'y1 (y2-y1)*(x-x1)/(x2-x1)'

+

... done

: lerp over - 4 roll 4 pick - * 2swap swap - / + ;

## Performance
MicroPython, Lua(eLua), Partcl https://github.com/zserge/partcl

### Pythagorean theorem
Lets create a real world example by using the pythagorean theorem:

In C or practically any other language you'd write something like
```
c = a * a + b * b;
```

This expression actually puts a lot of responsibility into the hands of the interpreter/compiler. First it would need to keep track of where the variables are coming from. Are those variables function parameters or are they coming from somewhere else? Also the whole expression has to get parsed till the end before even a single machine instruction can be executed. The interpreter/compiler can't say for certain that b² is the last part of the expression before the semicolon has been detected. 

In Forth on the other hand the same expression would be written as
```
a @ dup * b @ dup * +
```

This tells the interpreter/compiler to
1. put a on the stack
2. duplicate it
3. multiply the last two elements on the stack (a * a)
4. put b on the stack
5. duplicate it
6. multiply the last two elements on the stack (b * b)
7. add the last two elements on the stack (a + b) 

The interpreter/compiler doesn't have to be really smart to evaluate such an expression. It just takes the next token from the input string and executes it without any prior or subsequent knowledge about the other tokens. The example could be further refactored by taking the squaring part out of the expression and creating its own function (called a "word" in Forth) for it.
```
: square dup * ;
a @ square b @ square +
```

The key is that new words such as "square" would simply link calls to other fine grained words such as "dup" together or even copy the underlying definition in assembly (basically inlining it). This low level of code reuse is an uniqueness of stack-oriented languages and makes Forth extremely small and fast.