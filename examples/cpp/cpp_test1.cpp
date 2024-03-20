#include<iostream>
#include<vector>
#include<cmath>
#include<cstring>
using namespace std;
// 判断一个数是否是素数
bool isPrime(const int&a){
    if(a<2)return false;
    for(int i=2;i*i<a;++i){
        if(!(a%i))return false;
    }
    return true;
}
// 得到小于等于n的所有素数
// 埃氏筛法
vector<int> eratosthenes(const int&n){
    vector<int>res;
    bool* is_prime=new bool [n];
    is_prime[0]=false; is_prime[1]=false;
    fill(is_prime+2,is_prime+n-1,true);
    int up=(int) sqrt(n)+1;
    for(int i=2;i<up;++i)
        if(is_prime[i])
            for(int j=i*i;j<n;j+=i)is_prime[j]=false;

    for(int i=2;i<n;++i)
        if(is_prime[i])res.push_back(i);
    delete[]is_prime;
    return res;
}
int main(){
    int a=100;
    auto res=eratosthenes(a);
    return 0;
}