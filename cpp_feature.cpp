#include "cpp_feature.h"

int CppFeature::MULTI_COMMENT_NOT_TERMIN;
int CppFeature::CHAR_ERROR;
string CppFeature::desc_list[CPP_ERROR_N];
map<char,char> CppFeature::escape_map;

void CppFeature::init() {
    init_desc();
    init_escape();
}
bool CppFeature::inEscapeList(const char &ch) {
    return escape_map.find(ch)!=escape_map.end();
}

void CppFeature::init_escape() {
    escape_map={
            {'a','\a'},
            {'b','\b'},
            {'f','\f'},
            {'n','\n'},
            {'r','\r'},
            {'t','\t'},
            {'v','\v'},
            {'\\','\\'},
            {'\'','\''},
            {'\"','\"'},
            {'?','?'},
            {'0','\0'}
    };
}

void CppFeature::init_desc() {
    MULTI_COMMENT_NOT_TERMIN=0;
    CHAR_ERROR=1;
    desc_list[0]="multi-line comment not terminated";
    desc_list[1]="char error";
}

char CppFeature::getCorEscapeChar(const char &ch) {
    return escape_map.find(ch)->second;
}