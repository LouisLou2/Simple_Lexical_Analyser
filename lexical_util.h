//
// Created by LSK on 2024-03-19.
//

#ifndef LEXICAL_LEXICAL_UTIL_H
#define LEXICAL_LEXICAL_UTIL_H
class LexicalUtil{
public :
    static bool whiteSpace(const char& ch);
    static bool letter(const char& ch);
    static bool digit(const char&ch);
    static bool underline(const char&ch);
    static bool quote(const char&ch);
    static bool single_quote(const char&ch);
};
#endif //LEXICAL_LEXICAL_UTIL_H
