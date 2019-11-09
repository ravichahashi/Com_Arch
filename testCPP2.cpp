#include<iostream>
using namespace std;

int main(){
    char *s="1f";
    int i=6;
    cout<<isdigit(s[0])<<s[0];
    cout<<atoi(s)+i;
    return 0;    
}