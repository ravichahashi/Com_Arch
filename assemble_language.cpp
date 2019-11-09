#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
using namespace std;

int main()
{
    //------------------------- Lauching file .txt -------------------------
    ifstream inFile;
    inFile.open("test.txt");
    //------------------------- Check Lauching file .txt -------------------------
    if (!inFile)
    {
        cerr << "Unable to open file";
        exit(1); // call system to stop
    }
    //------------------------- Reading file .txt -------------------------
    //-------------------- Count line -------------------------
    vector<string> label;
    vector<int> addr;
    string temp;
    int line=0;
    while (!inFile.eof())
    {
        if (inFile.peek() != '\t')
        {
            inFile >> temp;
            for (string s : label)
            {
                cout << s << " ";
                if(temp.compare(s) == 0){
                    cerr<<"Duplicate Label.\n";
                    exit(1);
                }
            }
            label.push_back(temp);
            addr.push_back(line);
            cout << endl;
        }
        inFile.ignore(numeric_limits<streamsize>::max(), '\n');
        line++;
    }

    //------------------------- Stop Lauching file .txt -------------------------
    inFile.close();
    return 0;
}
