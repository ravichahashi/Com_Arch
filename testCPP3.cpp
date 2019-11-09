#include <iostream>
#include <set>
#include <vector>
using namespace std;

int main()
{
    char *s = "1f";
    int i = 6;
    set<int> ii;
    vector<int> ll;
    for (int j = 0; j < 10000000; j++)
    {
        ll.push_back(j);
    }
    for (int j = 0; j < ll.size(); j++)
    {
        if (ll[j] == 5000000){
            cout<<"finddd";
            break;
        }
    }
    return 0;
}