# stcpp

stcpp (super tiny cpp) is a simple and resource efficient C Preprocessor designed to handle macro expansions, file inclusions, and conditional compilations.

## Features

- Macro expansion
- File inclusion
- Conditional compilation
- Token pasting (##) operator
- Stringification (#) operator

## Shortcommings

- no shortcuts for && and ||, may lead to errors in complex header files
- precedence of operators needs a fix
- several other @todo's
- error messages are not yet polished
- no #line used for next compile stages

## Installation

Clone the repository and navigate to the project directory:

```sh
git clone https://github.com/thobo70/stcpp.git
cd stcpp
make
make test
```

## Status

it is getting usable, but has still trouble with very complex header files 

