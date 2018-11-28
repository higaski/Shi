# Shi
Shi is a fast and tiny [Forth](https://en.wikipedia.org/wiki/Forth_(programming_language)) implementation written for the  [Thumb-2 instruction set](https://infocenter.arm.com/help/topic/com.arm.doc.qrc0001m/QRC0001_UAL.pdf) (ARMv7-M and newer). It currently fits into 8kB of flash and 320B of ram.

# Issue (with) Forth
Nowadays its almost like a rule of thumb that someone writes a Forth engine for every new device that gets released. And while this is certainly an entertaining coding challenge I simply don't see a whole lot of use cases to be honest. Why run Forth on a powerful device that easily fits [Lua](https://www.lua.org/) or [MicroPython](https://micropython.org/)? Both offer modern languages, a unified environment and a properly defined interface to C. In comparison Forth is outdated, error-prone, ugly, completely lacking object-oriented and functional paradigms and the often lauded interactivity is neither something exclusive anymore nor that useful in my opinion. Now I know this might seem like a weird thing to read considering the fact you're currently looking at the introduction of an actual implementation, but I just want you to know what you might be getting into here.

The **single only reason** to favor Forth is its capability to self-compile down to assembly. There are two factors coming toghether which let Forth really excel in this area:
1. It is entirely stack-based
2. It uses [reverse Polish notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation) to utilize that (which is why Forth is ugly btw)

### An example
Lets create a real world example by using the pythagorean theorem:

In C or practically any other language you'd write something like
```c
c² = a² + b²;
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

# Yet another...
What I gernerally dislike about most scripting languages apart from Lua is that they mostly talk about "interfacing C", not about "getting interfaced from C". This bugs me specially when talking about the embedded domain. These languages promote themself as system level language and tell you to call C if necessary but in my opinion thats mostly the wrong way round. A large part of embedded software is about writing device drivers at very low level, talking to registers and what not. I want a scripting language to drive general high-level behavior of whole devices not output pin levels of processors. Also C programmers tend to be extremely conservative about using anything else apart from C so don't make them. I suggest watching Dan Saks (sad) talk [“extern c: Talking to C Programmers about C++”](https://www.youtube.com/watch?v=D7Sd8A6_fYU) from CppCon 2016 on that matter.

Suprisingly to me the best known Forth implementations for ARM also choose to play system rather than library. The standard way of doing so is implementing the words [key](http://forth-standard.org/standard/core/KEY) and [emit](http://forth-standard.org/standard/core/EMIT) to constantly reading and writing from a serial interface. The otherwise very promising [CoreForth](https://github.com/ekoeppen/CoreForth) and [Mecrisp-Stellaris](http://mecrisp.sourceforge.net/>Mecrisp-Stellaris) are both following this path. While both implementations fit into ~16-20kB of flash the latter really stands out. Not only is the [documentation](http://hightechdoc.net/mecrisp-stellaris/_build/html/index.html) superb, but it also comes with amazing features like literal folding, compilation to flash and inline optimizations. Needless to say that I had to borrow a lot of their ideas. 

HMMM der Teil taugt ma no ned so ->

Shi has arisen from the quest for a suitable library Forth. Suitable can be summarized by at least soft real-time capable and space-saving. At the time I was working for a company producing small model railroad electronics, among others thumbnail-sized digital decoders with 3x3mm small Cortex-M4 processors. One of the key features of digital decoders is real-time audio processing based on user defined behavior which basically means running a bunch of scripts in between processing two consecutive audio buffers. For having noticeable latency free audio output at 44.1kHz those buffers are usually between 64 and 256 samples long. This results in a timeframe of 1.5 to 5.8ms for running all scripts.