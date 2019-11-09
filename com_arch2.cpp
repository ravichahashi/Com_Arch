#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <set>
using namespace std;

int main()
{
    vector<string> label;
    vector<int> addr;
    string temp;
    int line = 0;
    ifstream inFile;

    inFile.open("test.txt");
    if (!inFile)
    {
        cerr << "Unable to open file";
        exit(1); // call system to stop
    }

    //--------------------  recognize label

    // while (!inFile.eof())
    // {
    //     if (inFile.peek() != '\t')
    //     {
    //         inFile >> temp;
    //         // --- check duplicate
    //         for (string find : label)
    //         {
    //             cout << find << "  " << temp << endl;
    //             if (temp.compare("neg2") == 0)
    //             {
    //                 cout << "fi\n";
    //                 break;
    //             }
    //             if (find.compare(temp) == 0)
    //             {
    //                 cerr << "Duplicate label.";
    //                 exit(1);
    //             }
    //         }
    //         label.push_back(temp);
    //         addr.push_back(line);
    //     }
    //     inFile.ignore(numeric_limits<streamsize>::max(), '\n');
    //     line++;
    // }
    while (getline(inFile, temp))
    {
        if (temp[0] != '\t')
        {
            cout << "fi \n";
            inFile.seekg(ios::beg);
            inFile >> temp;
            cout<<temp<<endl;
            getline(inFile,temp);
            
        }
        line++;
    }

    inFile.clear();
    inFile.seekg(ios::beg);
    line = 0;
    inFile.close();
    return 0;
}
