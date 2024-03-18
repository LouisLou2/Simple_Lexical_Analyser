#include "./lexical_analyser1.h"

int main() {
    ios::sync_with_stdio(false);
    cout.tie(nullptr);

    string path="C:/Users/LSK/CLionProjects/lexical/examples/cpp/cpp_test2.cpp";
    LexicalAnalyser analyser(CPP);
    vector<word>*res=analyser.analyse(path);
    analyser.printResult();
    delete res;
    return 0;
}
