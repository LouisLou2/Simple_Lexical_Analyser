//
// Created by LSK on 2024-03-19.
//

#ifndef LEXICAL_STRUCTURES_H
#define LEXICAL_STRUCTURES_H
enum language{
    C,CPP,JAVA,DART
};

enum KindCode{
    constant_char,constant_escape_char,constant_num,constant_string,key_word,not_decide
};

struct word{
    int code;//内部码值
    int p;// 常量表指针, 当code指示为单个字符的字面量，p用来直接存储char的值，而不是索引
    word(const int& c,const int& p):code(c),p(p){}
    word(const int&c,const char&p):code(c),p((int)p){}
};
struct error_info{
    int pos;
    int error_p;// 错误信息描述表指针
    error_info(const int&pos,const int& error_p):pos(pos),error_p(error_p){};
};
#endif //LEXICAL_STRUCTURES_H
