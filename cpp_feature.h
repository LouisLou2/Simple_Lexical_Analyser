//
// Created by LSK on 2024-03-19.
//

#ifndef LEXICAL_CPP_FEATURE_H
#define LEXICAL_CPP_FEATURE_H
#define CPP_ERROR_N 2
#include<string>
#include<map>
using namespace std;

class CppFeature{
public:
    static int MULTI_COMMENT_NOT_TERMIN;
    static int CHAR_ERROR;
    static string desc_list[CPP_ERROR_N];
    static map<char,char> escape_map;
    static bool inEscapeList(const char&ch);
    static char getCorEscapeChar(const char&ch);
    static void init();
private:
    static void init_escape();
    static void init_desc();
};
#endif //LEXICAL_CPP_FEATURE_H
