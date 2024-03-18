//
// Created by LSK on 2024-03-18.
//
#include "lexical_analyser1.h"

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
    reserved_map=new unordered_map<string,int>;
    switch (lang) {
        case C:
            reserved_len=33;
            reserved_words=new string[33]{"define","include","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while"};
            break;
        case CPP:
            initSymbolMap4Cpp();
            reserved_len=36;
            reserved_words=new string[36]{"using","namespace","define","include","auto","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while"};
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

vector<word>* LexicalAnalyser::analyse(const string &filepath) {
    initWhenAnalyse();
    readCodeFile(filepath);
    cur=0;
    while(cur<size) {
        analyseSingle();
        token.clear();
    }
    return lexical_res;
}

LexicalAnalyser::~LexicalAnalyser() {
    delete[]reserved_words;
    delete[]buffer;
    delete reserved_map;
    delete key_words;
    delete constant_nums;
    delete symbol_map;
    delete []symbol_list;
    delete string_table;
    // delete lexical_res; lexical_res不随着分析器的析构而释放，责任由调用者负责
}

LexicalAnalyser::LexicalAnalyser(language l):lang(l) {
    if(!(lang==C||lang==CPP||lang==JAVA||lang==DART))
        throw runtime_error("不支持的语言");
    init();
}

void LexicalAnalyser::getC() {
    c=content[cur++];
}
void LexicalAnalyser::getC_ExceptWhiteSpace() {
    while(whiteSpace(c)) getC();
}

bool LexicalAnalyser::whiteSpace(const char& ch) {
    return ch==' '||ch=='\n'||ch=='\r'||ch=='\t';
}
bool LexicalAnalyser::letter(const char& ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}
bool LexicalAnalyser::digit(const char &ch) {
    return (ch >= '0' && ch <= '9');
}
bool LexicalAnalyser::underline(const char &ch) {
    return ch=='_';
}
bool LexicalAnalyser::quote(const char &ch) {
    return ch=='\"';
}

void LexicalAnalyser::analyseSingle() {
    getC();
    getC_ExceptWhiteSpace();
    bool isletter= letter(c);
    bool isunderline= underline(c);
    if(isletter||isunderline){
        while(letter(c) || digit(c) || underline(c)){
            concatToken();
            getC();
        }
        retract();
        int code=recognizeReserved();
        if(code!=-1){
            lexical_res->emplace_back(code,-1);
        }else{
            resolveKeyOrConstant(key_word);
        }
    }else if(digit(c)){
        do{
            concatToken();
            getC();
        } while (digit(c));
        retract();
        resolveKeyOrConstant(constant_num);
    }else if(quote(c)){
        getC();
        while(!quote(c)){
            concatToken();
            getC();
        }
        // 这里不需要retract，正好跳过引号
        resolveKeyOrConstant(constant_string);
    }else{
        analyseForSymbol();
    }
}

void LexicalAnalyser::concatToken() {
    token+=c;
}

void LexicalAnalyser::retract() {
    --cur;
    c=content[cur++];
}
// 如果kind_code为NOT_DECIDE，则根据token的首字符自动判断,否则根据kind_code判断
void LexicalAnalyser::resolveKeyOrConstant(KindCode kind_code=not_decide) {
    char beginc;
    switch (kind_code) {
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
    symbol_map=new unordered_map<string,int>;
    // 符号表,运算符，定界符
    symbol_len=50;
    symbol_list=new string[50]{
            "++","--","->","<<",">>","<=",">=","==","!=","&&","||","+=","-=","*=","/=","%=","&=","|=","^=","<<=",">>=","!","~","*","/","%","+","-","<",">","&","|","^","=","(",")","[","]","{","}",";",",",":",".","?","::","...","#","##"
    };
    for(int i=0;i<50;++i){
        symbol_map->insert({symbol_list[i],i+201});
    }
}

void LexicalAnalyser::setPointerNullptr() {
    buffer= nullptr;
    reserved_words= nullptr;
    reserved_map= nullptr;
    key_words= nullptr;
    constant_nums= nullptr;
    symbol_map= nullptr;
    symbol_list= nullptr;
    lexical_res= nullptr;
    string_table= nullptr;
}

void LexicalAnalyser::initWhenAnalyse() {
    lexical_res=new vector<word>;
    constant_nums=new vector<int>;
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
int LexicalAnalyser::referConstantNum() {
    int num=stoi(token);
    for(int i=0;i<constant_nums->size();++i){
        if((*constant_nums)[i]==num) return i;
    }
    constant_nums->push_back(num);
    return constant_nums->size()-1;
}

void LexicalAnalyser::autoAddKeyWord() {
    int value=referKeyWord();
    lexical_res->emplace_back(key_word_code,value);
}
void LexicalAnalyser::autoAddConstantNum() {
    int value=referConstantNum();
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
void LexicalAnalyser::analyseForSymbol() {
    concatToken();
    switch (c) {
        case '+':
            getC();
            if(c=='+'||c=='='){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '-':
            getC();
            if(c=='-'||c=='='||c=='>'){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '*':
            getC();
            if(c=='='){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '/':
            getC();
            if(c=='='){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '%':
            getC();
            if(c=='='){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                concatToken();
                autoAddSymbol();
            }
            break;
        case '<':
            getC();
            if(c=='='){
                concatToken();
                autoAddSymbol();
            }else if(c=='<'){
                concatToken();
                getC();
                if(c=='='){
                    concatToken();
                    autoAddSymbol();
                }else{
                    retract();
                    autoAddSymbol();
                }
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '>':
            getC();
            if(c=='='){
                concatToken();
                autoAddSymbol();
            }else if(c=='>'){
                concatToken();
                getC();
                if(c=='='){
                    concatToken();
                    autoAddSymbol();
                }else{
                    retract();
                    autoAddSymbol();
                }
            }else{
                retract();
                autoAddSymbol();
            }
        case '=':
            getC();
            if(c=='='){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '&':
            getC();
            if(c=='&'||c=='='){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '|':
            getC();
            if(c=='|'||c=='='){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '^':
            getC();
            if(c=='='){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '!':
            getC();
            if(c=='='){
                concatToken();
                autoAddSymbol();
            }else{
                retract();
                autoAddSymbol();
            }
            break;
        case '~': case ';': case ',': case ':': case '.': case '?': case '(': case ')':
        case '[': case ']': case '{': case '}': case '#':
            autoAddSymbol();
            break;
        case '\0':
            break;
        default:
            throw runtime_error("不支持的符号");
    }
}

void LexicalAnalyser::printResult() {
    cout<<"词法分析结果:\n";
    cout<<"201: 标识符，202: 常数，203: 字符串\n";
    cout<<"以上三种内部码值对应的值是 指向的是标识符表，常数表，字符串表的指针位置\n";
    cout<<"可使用printReferTable查看对应的表\n";
    int const_num;
    for(auto& w:*lexical_res){
        cout<<"< "<<w.code<<" , ";
        if(w.p==EMPTY_VALUE){
            cout<<"__";
        }else{
            switch (w.code) {
                case 102:
                    const_num=(*constant_nums)[w.p];
                    cout<<const_num;
                    break;
                case 103:
                    cout<<(*string_table)[w.p];
                    break;
                case 101:
                    cout<<(*key_words)[w.p];
                    break;
                default:
                    throw runtime_error("不支持的类型");
            }
        }
        cout<<" >\n";
    }
}