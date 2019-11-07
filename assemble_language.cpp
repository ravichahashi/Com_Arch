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
    string temp;
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

    vector<string> tokens;
    while (getline(inFile, temp))
    {
        istringstream iss(temp);
        copy(istream_iterator<string>(iss),istream_iterator<string>(),back_inserter(tokens));
        if(tokens[0] == "beq" || tokens[0] == "lw" || tokens[0] == "add"){
            for(int i = 0 ; i < tokens.size() ; i++){
                cout << tokens[i] << " ";
            }
        }
        //----------------- clear vector
        tokens.clear();
    }

    //------------------------- Stop Lauching file .txt -------------------------
    inFile.close();
    return 0;
}

