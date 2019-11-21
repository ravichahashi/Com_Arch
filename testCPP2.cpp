#include <iostream>
#include <set>
#include <vector>
#include <string.h>
#include <stack>
using namespace std;

class gg
{
public:
  int i = 5;
};

void hh(int = 99, string = "gg", int = 77);

int main()
{
  stack<int> d;
  gg o;
  hh();
  return 0;
}

void hh(int io, string tt, int e)
{
  cout << io << " " << tt << " " << e;
}