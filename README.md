# ukkonen
personal implementation of ukkonen's suffix tree algorithm, with helpful comments

This implementation made specifically for the hackerrank challenge https://www.hackerrank.com/challenges/how-many-substrings/problem

## TODO
need to implement support for custom char2rank function

## Example


    #include "ukkonen.h"
    int main(){
        char *test = "foo";
        return substrings_ukkonen_implicit(test, 3, 26);
    }
