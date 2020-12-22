## Run
`make` to build.

### Tests
`./run_tests <mode(+/++/+++)>` to run tests.
Write `+`, `++` or `+++` to run small, small and medium or all the test respectively.

### Tool
`./run <grammar_names>` to run tool.
Put words to check for grammars, `EOF` to exit.


## Grammar format
```
<Grammar name> <Start Symbol>
<From> -> <To>
...
end
```

__'I'__ is reserved by default. Can be changed by constructor parameter.  \
__\EPS__ is reserved as an empty string.

