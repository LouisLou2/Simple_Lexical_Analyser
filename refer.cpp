//
// Created by LSK on 2024-03-18.
//
#include<stdio.h>
#include<stdlib.h>
#include<cstring>
#include<string>
#include<map>
#include<set>
#include<vector>
#include<iostream>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
string reserved_Word[41] = {"","while","if","else","switch","case","int","main","using","namespace","std","printf"};
int reserved_Len = 40;
FILE* fin, * fout;//文件流
char character;//字符变量，存放最新读入的源程序字符
string token;
int number_table[50];//常数表，存放构成的常数
int number_point;//常数表指针
map<string, int> identifier_table;
map<string, int> string_table;
int id_point = 0;
int string_point = 0;
int cur_point=0;//当前指针
void getch() {
    if (fin)
        character = fgetc(fin);
}
void getbe() {
    while (character == ' '||character=='\n'||character=='\r'||character=='\t')
        getch();
}
void concatenation() {
    token += character;
}
bool letter() {
    if ((character >= 'a') && (character <= 'z') || (character >= 'A') && (character <= 'Z'))
        return 1;
    else
        return 0;
}
bool underline() {
    if (character == '_')
        return 1;
    else
        return 0;
}
bool digit() {
    if ((character >= '0') && (character <= '9'))
        return 1;
    else
        return 0;
}
int reserve() {
    for (int i = 1; i <=reserved_Len; i++) {
        if (reserved_Word[i] == token)
            return i;
    }
    return 0;
}
void retract() {
    fseek(fin, -1, SEEK_CUR);
    character = ' ';
}
void buildlist() {
    char temp = token[0];
    int temp_num = 0;//存放转换后的常数值
    if (temp >= '0' && temp <= '9') {//如果是数字
        temp_num = stoi(token);//则字符串变为常数
        int i;
        for (i = 0; i < number_point; i++) {
            if (temp_num == number_table[i]) {
                cur_point = i;
                break;
            }
        }
        if (i == number_point) {//如果常数表中没有该常数，则登记该常数
            number_table[number_point] = temp_num;
            cur_point = number_point;
            number_point++;
        }
    }
    else if (temp == '\"') {//登记该字符串
        if (string_table.count(token))
            cur_point = string_table[token];
        else {
            string_table[token] = string_point++;
            cur_point = string_table[token];
        }
    }
    else {
        if (identifier_table.count(token))
            cur_point=identifier_table[token];
        else {
            identifier_table[token] = id_point++;
            cur_point = identifier_table[token];
        }
    }
}
void error() {
    printf("非法字符:%c\n", character);
}
void test() {
    token = "";
    getch();//获取字符
    getbe();  //滤除空格
    switch (character) {
        case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':
        case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':
        case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':case 'M':
        case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':
        case '_':
        {
            while (letter() || digit() || underline()) {
                concatenation();    //将当前读入的字符送入token数组
                getch();
            }
            retract();            //扫描指针回退一个字符
            int c = reserve();
            if (c == 0) {
                buildlist();        //将标识符登录到符号表中
                //return(id, 指向id的符号表入口指针);
                printf("(id,%s)id表入口指针:%d\n", token.c_str(), cur_point);
            }
            else {
                //return(保留字码，null);
                printf("(reserved word,%s)种别编码:%d\n", token.c_str(),c);
            }
            break;
        }
        case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
            while (digit()) {
                concatenation();
                getch();
            }
            retract();
            buildlist();
            printf("(num,%d)常数表入口指针:%d\n", number_table[cur_point], cur_point);
            break;
        case '\"': {
            do {
                concatenation();
                getch();
            } while (character != '\"');
            concatenation();
            getch();
            retract();
            buildlist();
            printf("(string,%s)string表入口指针:%d\n", token.c_str(), cur_point);
            break;
        }
        case '+':
            printf("(%c,-)\n", '+');
            break;
        case '-':
            printf("(%c,-)\n", '-');
            break;
        case '*':
            printf("(%c,-)\n", '*');
            break;
        case '<':
            getch();
            if (character == '=')
                printf("(relop,LE)\n");
            else {
                retract();
                printf("(relop,LT)\n");
            }
            break;
        case '>':
            getch();
            if (character == '=')
                printf("(relop,GE)\n");
            else {
                retract();
                printf("(relop,GT)\n");
            }
            break;
        case '=':
            getch();
            if (character == '=')
                printf("(relop,EQ)\n");
            else {
                retract();
                printf("(assign,=)\n");
                //return('=', null);
            }
            break;
        case ';':
            printf("(semicolon,;)\n");
            //return(';', null);
            break;
        case ':':
            printf("(colon,:)\n");
            //return(';', null);
            break;
        case '#': {
            while (character!='\n') {
                concatenation();    //将当前读入的字符送入token数组
                getch();
            }
            printf("(head,%s)\n",token.c_str());
            break;
        }
        case '(':case ')':case '{':case '}':
            printf("(bracket,%c)\n", character);
            break;
        default:
            if (character == '\n' || character == '\t' || character == EOF)
                break;
            error();
    }
}
int main() {
    fin=fopen(R"(C:\Users\LSK\CLionProjects\lexical_analysis\examples\cpp\cpp_test1.cpp)", "r");
    while (character != EOF)
        test();
    if (fin)
        fclose(fin);
    if (fout)
        fclose(fout);
    return 0;
}