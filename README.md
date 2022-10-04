# Joy and Joy Based language interpreters
This repository contains 2 language interpreters, first is a joy interpreter, second is an interpreter of Joy based language (modified Joy). 
Joy is a stack based functional programming language. Joy interpreter computes list called expression by iterating over it and evaluating each member of it.
If value of item is name of a function, Interpreter calls the function, then function returns result to a stack.
### Content
- [ Main syntax ](#mains)
- [Joy](#joy)
- [ Joy functions ](#joyf)
- [ Modified Joy ](#modjoy)
- [Modified joy functions](#modjoyf)
- [ How to Run ](#howtorun)
<a name="mains"></a>
## Main syntax
This interpretors operate two data types:
-Words
-Lists

Word is a string without spaces, in both interpreters numbers are also words, all arithmetic functions performed over strings in C.

Word, number:
```
167
```
Word:
```
hello
```
List is a classic linked list. It can be empty, it also can contain words and lists as well.

empty list:
```
()
```
list with word, number and list:
```
(hello 1 (1 2))
```
<a name="joy"></a>
## Joy
Joy is purely [ functional programming language ](https://en.wikipedia.org/wiki/Functional_programming) introduced by Manfred von Thun.

Let's consider two functions:<br> `first` - returns first item from the list.<br>`rest` - return list without first element.
<br>Now let's build a sequence of functions, that will return second item of the list.
<details>
  <summary>Answer</summary> 
  
 ```
 rest first
 ```
</details>

<a name="joyf"></a>
## Joy functions

Funtion alias | What function does | Example
--- | --- | ---
 `+`| Takes 2 top items from stack <br>and attemps to add them. | `1. stack: 2 3; exp: +;`<br>`2. stack: 5; exp:;`
 `-`| Takes 2 top items from stack <br>and attemps to substract first from second. |`1. stack: 2 3; exp: -;`<br>`2. stack: 1; exp:;`
  `*`|Takes 2 top items from stack <br>and attemps to multiply them.|`1. stack: 2 3; exp: *;`<br>`2. stack: 6; exp:;`
  `<`|Checks if second item on the stack is less <br>than first. Returns bool value.|`1. stack: 2 3; exp: <;`<br>`2. stack: false; exp:;`
  `>`|Checks if second item on the stack is bigger <br>than first. Returns bool value.|`1. stack: 2 3; exp: >;`<br>`2. stack: true; exp:;`
  `==`|Checks if second item on the stack is equal <br>to first. Returns bool value.|`1. stack: 2 3; exp: ==;`<br>`2. stack: false; exp:;`
  `dup`|Copies last item and puts it to a stack.|`1. stack: (1 2 3); exp: dup;`<br>`2. stack: (1 2 3) (1 2 3); exp::;`
  `drop`|Frees last item on the stack.|`1. stack: 10; exp: drop;`<br>`2. stack:; exp:;`
  `swap`|Swaps two first items on the stack.|`1. stack: 10 (1); exp: swap;`<br>`2. stack:(1) 10; exp:;`
  `first`|Returns first item of a list to the stack.|`1. stack:(1 2 3); exp: first;`<br>`2. stack: 1; exp:;`
  `rest`|Returns list that contains all items<br>except first to the stack.|`1. stack:(1 2 3); exp: rest;`<br>`2. stack: (2 3); exp:;`
  `cons`|Concatenates second item on the stack<br> with a beginning of a first list on the stack.|`1. stack:(2 3) 1; exp: cons;`<br>`2. stack: (1 2 3); exp:;`
  `null`|Checks if list is emty, returns bool to the stack.|`1. stack:(); exp: null;`<br>`2. stack: true; exp:;`
  `i`|Concatenates content of the first on the stack<br> with a beginning of the expression.|`1. stack:(1 2 +); exp: i;`<br>`2. stack:; exp: 1 2 +;`
  `if`|Examines 3rd item on the stack,<br> it must be a bool value. Then concatenates <br>content of first or second list, depending<br> on the bool value. |`1. stack:(1)(1 2 +) true; exp: if;`<br>`2. stack:; exp: 1 2 +;`
  `dip`|Concatenates 2nd item on the stack with<br> a beginning of expression, then concatenates<br>to it content of the 1st list on the stack.|`1. stack:(1 2 +) 5; exp: dip;`<br>`2. stack:; exp: 1 2 + 5;`
  `def`|Saves 2nd item on the stack as a function name<br> and saves content of a first list on the stack<br>as a fucntion body. Next time when<br> interpreter will evaluate function name, it will<br> concatenate function body with a beginning of<br>expression.|`1. stack:(swap drop) drop2; exp: def drop2;`<br>`2. stack:; exp:drop2;`<br>`3. stack:; exp:swap drop;`

<a name="modjoy"></a>
## Modified Joy
Joy don't have garbage collector, since it is [linear typed language](https://wiki.c2.com/?LinearTypes), but it has some performance issues. For example: If you wish to check if list is empty before proceeding with further operations you must dup(copy) list first, otherwise null will destroy only one example of list and you will get error. Now imagine, if empty list is a base case of your iterator function, you will have to copy list n times, where n is quantity of items in a list. Now imagine 10Mb list.

This modification brings new behaviour for basic Joy's functions, one new function and one coding rule. 
Now arithmetic and logic functions don't drop arguments. Added `uncons` function, now you can easily recombine lists with `cons` and `uncons`. Coding rule: **Functions defined by programmer should not drop arguments from the stack**.

Theese modifications introduced for decreasing usage of `dup` and `drop`. It is not prohibited, but allows programmers to delay it.

<a name="modjoyf"></a>
## Modified joy functions

Funtion alias | What function does | Example
--- | --- | ---
 `+`| Takes 2 top items from stack <br>and attemps to add them. | `1. stack: 2 3; exp: +;`<br>`2. stack: 5 2 3; exp:;`
 `-`| Takes 2 top items from stack <br>and attemps to substract first from second. |`1. stack: 2 3; exp: -;`<br>`2. stack: 1 2 3; exp:;`
  `*`|Takes 2 top items from stack <br>and attemps to multiply them.|`1. stack: 2 3; exp: *;`<br>`2. stack: 6 2 3; exp:;`
  `<`|Checks if second item on the stack is less <br>than first. Returns bool value.|`1. stack: 2 3; exp: <;`<br>`2. stack: false 2 3; exp:;`
  `>`|Checks if second item on the stack is bigger <br>than first. Returns bool value.|`1. stack: 2 3; exp: >;`<br>`2. stack: true 2 3; exp:;`
  `==`|Checks if second item on the stack is equal <br>to first. Returns bool value.|`1. stack: 2 3; exp: ==;`<br>`2. stack: false 2 3; exp:;`
  `dup`|Copies last item and puts it to a stack.|`1. stack: (1 2 3); exp: dup;`<br>`2. stack: (1 2 3) (1 2 3); exp::;`
  `drop`|Frees last item on the stack.|`1. stack: 10; exp: drop;`<br>`2. stack:; exp:;`
  `swap`|Swaps two first items on the stack.|`1. stack: 10 (1); exp: swap;`<br>`2. stack:(1) 10; exp:;`
  `first`|Returns first item of a list to the stack.|`1. stack:(1 2 3); exp: first;`<br>`2. stack: 1; exp:;`
  `rest`|Returns list that contains all items<br>except first to the stack.|`1. stack:(1 2 3); exp: rest;`<br>`2. stack: (2 3); exp:;`
  `cons`|Concatenates second item on the stack<br> with a beginning of a first list on the stack.|`1. stack:(2 3) 1; exp: cons;`<br>`2. stack: (1 2 3); exp:;`
  `uncons`|Inversed cons, puts first item in the list<br> after the list. |`1. stack:(1 2 3); exp: uncons;`<br>`2. stack: (2 3) 1; exp:;`
  `null`|Checks if list is emty, returns bool to the stack.|`1. stack:(); exp: null;`<br>`2. stack: true; exp:;`
  `i`|Concatenates content of the first on the stack<br> with a beginning of the expression.|`1. stack:(1 2 +); exp: i;`<br>`2. stack:; exp: 1 2 +;`
  `if`|Examines 3rd item on the stack,<br> it must be a bool value. Then concatenates <br>content of first or second list, depending<br> on the bool value. |`1. stack:(1)(1 2 +) true; exp: if;`<br>`2. stack:; exp: 1 2 +;`
  `dip`|Concatenates 2nd item on the stack with<br> a beginning of expression, then concatenates<br>to it content of the 1st list on the stack.|`1. stack:(1 2 +) 5; exp: dip;`<br>`2. stack:; exp: 1 2 + 5;`
  `def`|Saves 2nd item on the stack as a function name<br> and saves content of a first list on the stack<br>as a fucntion body. Next time when<br> interpreter will evaluate function name, it will<br> concatenate function body with a beginning of<br>expression.|`1. stack:(swap drop) drop2; exp: def drop2;`<br>`2. stack:; exp:drop2;`<br>`3. stack:; exp:swap drop;`


<a name="howtorun"></a>  
## How to run  
  First go to directory with the desired interpreter and compile it.
  ```
  cd joy
  gcc joy.c
  ```
  ```
  cd modifiedJoy
  gcc joy.c
  ```
  Then run it, but specify as an argument name of a txt file with a code for interpretation. In both directories already exist files program.txt with some examples of joy code.
  ```
  ./a.out program.txt
  ```
