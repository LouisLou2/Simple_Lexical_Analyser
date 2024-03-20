//
// Created by LSK on 2024-03-18.
//
#include "lexical_analyser1.h"
#include "lexical_util.h"
#include "cpp_feature.h"

void LexicalAnalyser::readCodeFile(const std::string &path) {
    std::ifstream input_file(path,ios::binary); // 默认打开方式即为文本模式
    if (!input_file.is_open()) {
        throw std::runtime_error("无法打开文件");
    }
    // 获取文件大小
    input_file.seekg(0, ios::end);
    size = input_file.tellg();
    input_file.seekg(0, ios::beg);

    // 分配缓冲区(需要手动管理内存)
    buffer = new char[size + 1]; // 为终止符'\0'预留空间
    // 读入整个文件到缓冲区
    input_file.read(buffer, size);
    buffer[size] = '\0'; // 添加字符串终止符
    content = string_view(buffer, size+1); // 创建string_view
}

void LexicalAnalyser::initForLanguage() {
    reserved_map=new map<string,int>;
    switch (lang) {
        case C:
            reserved_len=33;
            reserved_words=new string[33]{"define","include","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while"};
            break;
        case CPP:
            initSymbolMap4Cpp();
            CppFeature::init();
            reserved_len=51;
            reserved_words=new string[51]{"using","namespace","define","include","auto","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","bool","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while","class","friend","inline","new","operator","private","protected","public","template","this","throw","try","catch","delete"};
            break;
        case JAVA:
            reserved_len=50;
            reserved_words=new string[50]{"abstract","assert","boolean","break","byte","case","catch","char","class","const","continue","default","do","double","else","enum","extends","final","finally","float","for","goto","if","implements","import","instanceof","int","interface","long","native","new","package","private","protected","public","return","short","static","strictfp","super","switch","synchronized","this","throw","throws","transient","try","void","volatile","while"};
            break;
        case DART:
            reserved_len=65;
            reserved_words=new string[64]{"abstract","as","assert","async","await","break","case","catch","class","const","continue","covariant","default","deferred","do","dynamic","else","enum","export","extends","extension","external","factory","false","final","finally","for","Function","get","hide","if","implements","import","in","inout","interface","is","late","library","mixin","new","null","on","operator","out","part","rethrow","return","set","show","static","super","switch","sync","this","throw","true","try","typedef","var","void","while","with","yield"};
            break;
    }
    for(int i=0;i<reserved_len;++i){
        reserved_map->insert({reserved_words[i],i});
    }
}

void LexicalAnalyser::init() {
    setPointerNullptr();
    initForLanguage();
}

AnalyseResult LexicalAnalyser::analyse(const string &filepath) {
    initWhenAnalyse();
    readCodeFile(filepath);
    cur=0;
    switch(lang){
        case CPP:
            while(cur<size) {
                CPP_analyseSingle();
                token.clear();
            }
            break;
        default:
            throw runtime_error("暂不支持的语言");
    }
    return AnalyseResult(
        lang,
        reserved_map,
        key_words,
        string_table,
        symbol_map,
        lexical_res,
        error_res
    );
}

LexicalAnalyser::~LexicalAnalyser() {
    delete[]reserved_words;
    delete[]buffer;
    // delete reserved_map;
    //delete key_words;
    //delete constant_nums;
    //delete symbol_map;
    delete []symbol_list;
    //delete string_table;
    //delete error_desc_list;
    // delete lexical_res; lexical_res不随着分析器的析构而释放，责任由调用者负责
    // delete error_res; error_res不随着分析器的析构而释放，责任由调用者负责
}

LexicalAnalyser::LexicalAnalyser(language l):lang(l) {
    if(lang != CPP)
        throw runtime_error("不支持的语言");
    init();
}

void LexicalAnalyser::getC() {
    c=content[cur++];
}
void LexicalAnalyser::peekC() {
    // 获取c的下一个字符，但是不移动指针
    if(cur==size+1) throw runtime_error("peekC():不应该超越边界");
    c_peek=content[cur];
}
void LexicalAnalyser::getC_ExceptWhiteSpace() {
    while(LexicalUtil::whiteSpace(c)) getC();
}


void LexicalAnalyser::CPP_analyseSingle() {
    getC();
    getC_ExceptWhiteSpace();
    bool isletter= LexicalUtil::letter(c);
    bool isunderline= LexicalUtil::underline(c);
    if(isletter||isunderline){
        while(LexicalUtil::letter(c) || LexicalUtil::digit(c) || LexicalUtil::underline(c)){
            concatToken();
            getC();
        }
        retract();
        int code=recognizeReserved();
        if(code!=-1){
            lexical_res->emplace_back(code,EMPTY_VALUE);
        }else{
            resolveKeyOrConstant(key_word);
        }
    }else if(LexicalUtil::digit(c)){
        do{
            concatToken();
            getC();
        } while (LexicalUtil::digit(c));
        retract();
        resolveKeyOrConstant(constant_num);
    }else if(LexicalUtil::single_quote(c)){
        getC();
        if(c!='\\'){
            resolveKeyOrConstant(constant_char);
        }else{
            if(CppFeature::inEscapeList(c)){
                // 说明此字符是转义的
                resolveKeyOrConstant(constant_escape_char);
            }else{
                //错误情况
                autoAddError(cur,CppFeature::CHAR_ERROR);
            }
        }
    }else if(LexicalUtil::quote(c)){
        getC();
        while(!LexicalUtil::quote(c)){
            concatToken();
            getC();
        }
        // 这里不需要retract，正好跳过引号
        resolveKeyOrConstant(constant_string);
    }else{
        CPP_analyseForSymbol();
    }
}

void LexicalAnalyser::concatToken() {
    token+=c;
}

void LexicalAnalyser::retract() {
    --cur;
    c=content[cur-1];

}
// 如果kind_code为NOT_DECIDE，则根据token的首字符自动判断,否则根据kind_code判断
void LexicalAnalyser::resolveKeyOrConstant(KindCode kind_code=not_decide) {
    char beginc;
    switch (kind_code) {
        case constant_escape_char:
            autoAddChar(true);
            break;
        case constant_char:
            autoAddChar(false);
            break;
        case constant_num:
            autoAddConstantNum();
            break;
        case constant_string:
            autoAddString();
            break;
        case key_word:
            autoAddKeyWord();
            break;
        case not_decide:
            beginc=token[0];
            if (beginc >= '0' && beginc <= '9') {
                autoAddConstantNum();
            } else if (beginc == '\"') {
                autoAddString();
            } else {
                autoAddKeyWord();
            }
            break;
        default:
            throw runtime_error("不支持的类型");
    }
}

int LexicalAnalyser::recognizeReserved() {
    auto code=reserved_map->find(token);
    if(code==reserved_map->end()) return -1;
    return code->second;
}

void LexicalAnalyser::initSymbolMap4Cpp() {
    //从201开始是符号
    symbol_map=new map<string,int>;
    // 符号表,运算符，定界符
    symbol_len=50;
    symbol_list=new string[50]{
            "++","--","->","<<",">>","<=",">=","==","!=","&&","||","+=","-=","*=","/=","%=","&=","|=","^=","<<=",">>=","!","~","*","/","%","+","-","<",">","&","|","^","=","(",")","[","]","{","}",";",",",":",".","?","::","...","#","##"
    };
    for(int i=0;i<50;++i){
        symbol_map->insert({symbol_list[i],i+201});
    }
}
void LexicalAnalyser::concatAndAddSymbol(){
    concatToken();
    autoAddSymbol();
}
void LexicalAnalyser::retractAndAddSymbol() {
    retract();
    autoAddSymbol();
}
void LexicalAnalyser::setPointerNullptr() {
    buffer= nullptr;
    reserved_words= nullptr;
    reserved_map= nullptr;
    key_words= nullptr;
    symbol_map= nullptr;
    symbol_list= nullptr;
    lexical_res= nullptr;
    error_res=nullptr;
    string_table= nullptr;
}

void LexicalAnalyser::initWhenAnalyse() {
    lexical_res=new vector<word>;
    error_res=new vector<error_info>;
    key_words=new vector<string>;
    string_table=new vector<string>;
}

int LexicalAnalyser::referString() {
    for(int i=0;i<string_table->size();++i){
        if((*string_table)[i]==token) return i;
    }
    string_table->push_back(token);
    return string_table->size()-1;
}
int LexicalAnalyser::referKeyWord() {
    for(int i=0;i<key_words->size();++i){
        if((*key_words)[i]==token) return i;
    }
    key_words->push_back(token);
    return key_words->size()-1;
}

void LexicalAnalyser::autoAddKeyWord() {
    int value=referKeyWord();
    lexical_res->emplace_back(key_word_code,value);
}
void LexicalAnalyser::autoAddConstantNum() {
    int value=stoi(token);
    lexical_res->emplace_back(constant_num_code,value);
}
void LexicalAnalyser::autoAddString() {
    int value=referString();
    lexical_res->emplace_back(constant_string_code,value);
}
void LexicalAnalyser::autoAddSymbol() {
    int value=symbol_map->find(token)->second;
    lexical_res->emplace_back(value,EMPTY_VALUE);
}
void LexicalAnalyser::autoAddChar(bool escape=false) {
    if(!escape){
        lexical_res->emplace_back(constant_char_code,c);
    }else{
        lexical_res->emplace_back(constant_char_code,CppFeature::getCorEscapeChar(c));
    }
}

void LexicalAnalyser::CPP_analyseForSymbol() {
    concatToken();
    switch (c) {
        case '+':
            getC();
            if(c=='+'||c=='='){
                concatAndAddSymbol();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '-':
            getC();
            if(c=='-'||c=='='||c=='>'){
                concatAndAddSymbol();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '*':
            getC();
            if(c=='='){
                concatAndAddSymbol();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '/':
            getC();
            if(c=='='){
                concatAndAddSymbol();
            }else if(c=='/'){
                skipSingleLineComment();
            }else if(c=='*') {
                skipMultiLineComment();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '%':
            getC();
            if(c=='='){
                concatAndAddSymbol();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '<':
            getC();
            if(c=='='){
                concatAndAddSymbol();
            }else if(c=='<'){
                concatToken();
                getC();
                if(c=='='){
                    concatAndAddSymbol();
                }else{
                    retractAndAddSymbol();
                }
            }else{
                retractAndAddSymbol();
            }
            break;
        case '>':
            getC();
            if(c=='='){
                concatAndAddSymbol();
            }else if(c=='>'){
                concatToken();
                getC();
                if(c=='='){
                    concatAndAddSymbol();
                }else{
                    retractAndAddSymbol();
                }
            }else{
                retractAndAddSymbol();
            }
        case '=':
            getC();
            if(c=='='){
                concatAndAddSymbol();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '&':
            getC();
            if(c=='&'||c=='='){
                concatAndAddSymbol();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '|':
            getC();
            if(c=='|'||c=='='){
                concatAndAddSymbol();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '^':
            getC();
            if(c=='='){
                concatAndAddSymbol();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '!':
            getC();
            if(c=='='){
                concatAndAddSymbol();
            }else{
                retractAndAddSymbol();
            }
            break;
        case '~': case ';': case ',': case ':': case '.': case '?': case '(': case ')':
        case '[': case ']': case '{': case '}': case '#':case '\\':
            autoAddSymbol();
            break;
        case '\0':
            break;
        default:
            throw runtime_error("不支持的符号");
    }
}

/*执行完此函数，将会将c指向下一个有效字符*/
void LexicalAnalyser::skipSingleLineComment() {
    // 此时c应该是单行注释符，就是//的第二个/
    getC();
    while(!(c=='\n'||c=='\0')){
        getC();
    }
}

void LexicalAnalyser::skipMultiLineComment() {
    // 此时c应该是/*的*
    do{
        getC();
        if(c=='\0'){
            autoAddError(cur-1, CppFeature::MULTI_COMMENT_NOT_TERMIN);
            return;
        }
        peekC();
        if(c_peek=='\0'){
            autoAddError(cur, CppFeature::MULTI_COMMENT_NOT_TERMIN);
            return;
        }
    }while(!(c=='*'&&c_peek=='/'));
}

void LexicalAnalyser::autoAddError(const int &pos, const int&desc_p) {
    error_res->emplace_back(pos,desc_p);
}