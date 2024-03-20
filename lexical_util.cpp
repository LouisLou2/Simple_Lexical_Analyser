//
// Created by LSK on 2024-03-19.
//
#include "lexical_util.h"

bool LexicalUtil::whiteSpace(const char& ch){
    return ch==' '||ch=='\n'||ch=='\r'||ch=='\t';
}
bool LexicalUtil::letter(const char& ch){
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}
bool LexicalUtil::digit(const char&ch){
    return (ch >= '0' && ch <= '9');
}
bool LexicalUtil::underline(const char&ch){
    return ch=='_';
}
bool LexicalUtil::quote(const char&ch){
    return ch=='\"';
}

bool LexicalUtil::single_quote(const char &ch) {
    return ch=='\'';
}