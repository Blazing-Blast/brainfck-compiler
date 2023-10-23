# BFC

> A brainf*ck compiler written in C++

## Building

In order to build BFC run

```bash
make
```

otherwise, if you wish to also compile a few test programs, run

```bash
make check
```

## Usage

The usage of BFC is as follows

```bash
bfc in out
```

`in` -- The file with the brainf*ck code. It should be terminated with a non-brainf*ck character. This cannot be a whitespace or newline. It is commonly suffixed with .bf  
`out` -- The file to write the output assembly to. It is usually suffixed with .asm or .S.

The Makefile can also compile, assemble and link the brainf*ck code. In order to build `filename.bf`, run

```bash
make filename
```

## What is brainf*ck?

Brainf*ck is an esoteric language consisting of 5 legal characters: +, -, <, >, [, ], . and an infinite array of bytes.
In the backend it also stores an index into the array. The < and > charcters move that pointer around by one, +/- increment/dectrement the current byte by one and anything between [] gets looped over while the currect cell is not 0.
