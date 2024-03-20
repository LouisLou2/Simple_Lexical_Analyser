#include "./lexical_analyser1.h"

int main() {
    ios::sync_with_stdio(false);
    cout.tie(nullptr);

    string path="C:/Users/LSK/CLionProjects/lexical_original/examples/cpp/cpp_test1.cpp";
    LexicalAnalyser analyser(CPP);
    AnalyseResult res=analyser.analyse(path);
    res.print();
    res.print_refer_table();
    return 0;
}