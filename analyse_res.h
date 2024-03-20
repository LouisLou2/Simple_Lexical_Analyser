//
// Created by LSK on 2024-03-19.
//

#ifndef LEXICAL_ANALYSE_RES_H
#define LEXICAL_ANALYSE_RES_H
#include "structures.h"
#include "cpp_feature.h"
#include <iostream>
#include <map>
#include <vector>

using namespace std;
struct AnalyseResult{
    language lang;
    int key_word_code;
    int constant_num_code;
    int constant_string_code;
    int constant_char_code;
    // 参照表
    map<string,int>* reserved_words;
    vector<string>* key_word_table;
    vector<string>* string_table;
    string* error_desc_list;
    map<string,int>* symbol_map;
    // 分析结果
    vector<word>* lexical_res;
    vector<error_info>* error_res;

    AnalyseResult(language l,
                  map<string,int>*&reserved_words,
                  vector<string>*&key_word_table,
                  vector<string>*&string_table,
                  map<string,int>*&symbol_map,
                  vector<word>*&lexical_res,
                  vector<error_info>*&error_res
    ):lang(l),reserved_words(reserved_words),key_word_table(key_word_table),string_table(string_table),symbol_map(symbol_map),
    lexical_res(lexical_res),error_res(error_res){
        if(l==CPP){
            key_word_code=101;
            constant_num_code=102;
            constant_string_code=103;
            constant_char_code=104;
            error_desc_list=CppFeature::desc_list;
        }
    }
    ~AnalyseResult(){
        delete reserved_words;
        delete key_word_table;
        delete string_table;
        delete symbol_map;
        delete lexical_res;
        delete error_res;
    }
    void print_refer_table(){
        cout<<"语言类型："<<lang<<endl;
        cout<<"保留字表："<<endl;
        for(auto&[key,value]:*reserved_words){
            cout<<key<<" : "<<value<<endl;
        }
        cout<<"符号表："<<endl;
        for(auto&[key,value]:*symbol_map){
            cout<<key<<" : "<<value<<endl;
        }
        cout<<"标识符表："<<endl;
        for(int i=0;i<key_word_table->size();++i){
            cout<<i<<" : "<<(*key_word_table)[i]<<endl;
        }
        cout<<"常量字符串表："<<endl;
        for(int i=0;i<string_table->size();++i){
            cout<<i<<" : "<<(*string_table)[i]<<endl;
        }
        cout<<"错误描述表："<<endl;
        for(int i=0;i<CPP_ERROR_N;++i){
            cout<<i<<" : "<<error_desc_list[i]<<endl;
        }
    }
    void print() const{
        cout<<"语言类型："<<lang<<endl;
        cout<<"词法分析结果："<<endl;
        for(auto& w:*lexical_res){
            cout<<"< "<<w.code<<" , ";
            if(w.code==constant_char_code){
                cout<<(char)w.p;
            }else{
                cout<<w.p;
            }
            cout<<" >"<<endl;
        }
        cout<<"错误信息："<<endl;
        for(int i=0;i<error_res->size();++i){
            cout<<"ERROR"<<i<<": ";
            cout<<"at"<<(*error_res)[i].pos<<", ";
            int desc_p=(*error_res)[i].error_p;
            cout<<error_desc_list[desc_p]<<endl;
        }
    }
};
#endif //LEXICAL_ANALYSE_RES_H
