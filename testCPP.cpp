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

void gotoLine(ifstream &, int);

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

    /*while (!inFile.eof())
    {
        if (inFile.peek() != '\t')
        {
            inFile >> temp;
            // --- check duplicate
            for (string find : label)
            {
                cout << find << "  " << temp << endl;
                if (temp.compare("neg2") == 0)
                {
                    cout << "fi\n";
                    break;
                }
                if (find.compare(temp) == 0)
                {
                    cerr << "Duplicate label.";
                    exit(1);
                }
            }
            label.push_back(temp);
            addr.push_back(line);
        }
        inFile.ignore(numeric_limits<streamsize>::max(), '\n');
        line++;
    }*/
    while(getline(inFile,temp)){
        if(temp[0] != '\t'){
            inFile.seekg(ios::beg);
            inFile>>temp;
            cout<<temp<<endl;
        }
        line++;
    }

    gotoLine(inFile, 0);
    line = 0;

    //------------------    convert
    while (getline(inFile, temp))
    {
        istringstream splitter(temp);
        char *part;
        char type;
        if (temp[0] != '\t')
            splitter >> part; // skip label
        cout << "0000000";
        // opcode
        splitter >> part;
        if (strcmp(part, "add") == 0)
        {
            cout << "000";
            type = 'R';
        }
        else if (strcmp(part, "nand") == 0)
        {
            cout << "001";
            type = 'R';
        }
        else if (strcmp(part, "lw") == 0)
        {
            cout << "010";
            type = 'I';
        }
        else if (strcmp(part, "sw") == 0)
        {
            cout << "011";
            type = 'I';
        }
        else if (strcmp(part, "beq") == 0)
        {
            cout << "100";
            type = 'I';
        }
        else if (strcmp(part, "jalr") == 0)
        {
            cout << "101";
            type = 'J';
        }
        else if (strcmp(part, "halt") == 0)
        {
            cout << "110";
            type = 'O';
        }
        else if (strcmp(part, "noop") == 0)
        {
            cout << "111";
            type = 'O';
        }
        else if (strcmp(part, ".fill") == 0)
        {
            cout << "000";
            type = '.';
        }
        else
        {
            cerr << "Unrecognized opcode.";
            exit(1);
        }
        // reg A

        cout << endl;
    }

    inFile.close();
    for (int j = 0; j < label.size(); j++)
    {
        cout << label[j] << " " << addr[j] << endl;
    }
    return 0;
}

void gotoLine(ifstream &file, int num)
{
    file.clear();
    file.seekg(ios::beg);
    for (int i = 0; i < num; ++i)
    {
        file.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}