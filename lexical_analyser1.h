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
#include <unordered_map>
#include <vector>

using namespace std;

enum language{
    C,CPP,JAVA,DART
};

enum KindCode{
    constant_num,constant_string,key_word,not_decide
};

struct word{
    int code;//内部码值
    int p;// 常量表指针
    word(int c,int p):code(c),p(p){}
};

class LexicalAnalyser {
private:
    /*解析文本要用的变量*/
    language lang;
    char*buffer;
    std::streamsize size;
    string_view content;
    int cur;
    char c;
    string token;

    /*内部码值定义*/
    /*保留字的内部码值直接使用eserved_map中的second*/
    int const key_word_code=101;
    int const constant_num_code=102;
    int const constant_string_code=103;

    /*保留字*/
    int reserved_len;
    string*reserved_words;
    unordered_map<string,int>*reserved_map;/*key: 保留字，value:其内部码值*/
    /*标识符表*/
    vector<string>*key_words;
    /*常数表*/
    vector<int>*constant_nums;
    /*字符串表*/
    vector<string>*string_table;
    /*符号表*//*设置此字段位置建立符号与码值的映射*/
    int symbol_len;
    string*symbol_list;
    unordered_map<string,int>*symbol_map;

    vector<word>*lexical_res;

    /*私有函数*/
    void initForLanguage();
    void initSymbolMap4Cpp();
    void initWhenAnalyse();
    void setPointerNullptr();
    void init();

    void readCodeFile(const string&path);
    void getC();
    void getC_ExceptWhiteSpace();
    void retract();
    void concatToken();

    int referKeyWord();
    int referConstantNum();
    int referString();

    void autoAddKeyWord();
    void autoAddConstantNum();
    void autoAddString();
    void autoAddSymbol();

    void resolveKeyOrConstant(KindCode);// 解析标识符或常数或字符串
    int recognizeReserved();//识别保留字

    void analyseSingle();
    void analyseForSymbol();

    void printReferTable(int code);
    vector<int>* getConstantNumTable(){ return constant_nums;}
    vector<string>* getKeyWordTable(){ return key_words;}
    vector<string>* getStringTable(){return string_table;}
public:
    //构造函数
    explicit LexicalAnalyser(language l);
    static bool whiteSpace(const char& ch);
    static bool letter(const char& ch);
    static bool digit(const char&ch);
    static bool underline(const char&ch);
    static bool quote(const char&ch);
    vector<word>* analyse(const string& filepath);
    void printResult();
    //析构函数
    ~LexicalAnalyser();
};
#endif //LEXICAL_LEXICAL_ANALYSER1_H
