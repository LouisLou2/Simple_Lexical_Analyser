/*目前只支持C++*/

/*出现的问题*/
/*1. 在windows下使用文本模式解析文件，seek和tellg均是不准确的*/

/*TODO*/
//1. #define的处理
//2. #include的处理
//3. 注释的剔除
//4. 转义字符

#ifndef LEXICAL_LEXICAL_ANALYSER1_H
#define LEXICAL_LEXICAL_ANALYSER1_H

#define EMPTY_VALUE -1
#define NOT_DECIDE -2

#include <iostream>
#include<string_view>
#include <fstream>
#include <map>
#include <vector>
#include "analyse_res.h"

using namespace std;

class LexicalAnalyser {
private:
    /*错误信息描述*/
    //string*multi_conmment_not_termin= new string("Unterminated /* comment");
    //string

    /*解析文本要用的变量*/
    language lang;
    char*buffer;
    std::streamsize size;
    string_view content;
    int cur;
    char c;
    char c_peek;
    string token;

    /*内部码值定义*/
    /*保留字的内部码值直接使用eserved_map中的second*/
    int const key_word_code=101;
    int const constant_num_code=102;
    int const constant_string_code=103;
    int const constant_char_code=104;

    /*保留字*/
    int reserved_len;
    string*reserved_words;
    map<string,int>*reserved_map;/*key: 保留字，value:其内部码值*/
    /*标识符表*/
    vector<string>*key_words;
    /*字符串表*/
    vector<string>*string_table;
    /*符号表*//*设置此字段位置建立符号与码值的映射*/
    int symbol_len;
    string*symbol_list;
    map<string,int>*symbol_map;

    vector<word>*lexical_res;
    vector<error_info>* error_res;

    /*私有函数*/
    void initForLanguage();
    void initSymbolMap4Cpp();
    void initWhenAnalyse();
    void setPointerNullptr();
    void init();

    void readCodeFile(const string&path);
    void getC();
    void peekC();
    void getC_ExceptWhiteSpace();
    void retract();
    void concatToken();
    void concatAndAddSymbol();
    void retractAndAddSymbol();

    int referKeyWord();
    int referString();

    void autoAddKeyWord();
    void autoAddConstantNum();
    void autoAddString();
    void autoAddSymbol();
    void autoAddChar(bool escape);
    void autoAddError(const int&pos,const int&desc_p);
    // 此方法从注释的第一个字符开始，直至c指向注释之后的第一个字符，这里注释的第一个字符开始就是“ ”;
    void skipSingleLineComment();
    // 此方法从注释的第一个字符开始，直至c指向注释之后的第一个字符，这里注释的第一个字符开始就是“ ”;
    void skipMultiLineComment();

    void resolveKeyOrConstant(KindCode);// 解析标识符或常数或字符串
    int recognizeReserved();//识别保留字

    void CPP_analyseSingle();
    void CPP_analyseForSymbol();
public:
    //构造函数
    explicit LexicalAnalyser(language l);
    AnalyseResult analyse(const string& filepath);
    //析构函数
    ~LexicalAnalyser();
};
#endif //LEXICAL_LEXICAL_ANALYSER1_H
