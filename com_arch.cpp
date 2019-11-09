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
    ifstream inFile;
    vector<string> label;
    vector<int> addr;
    string temp;
    int line = 0;

    inFile.open("test.txt");
    if (!inFile)
    {
        cerr << "Unable to open file";
        exit(1); // call system to stop
    }

    //--------------------  recognize label
    while (!inFile.eof())
    {
        if (inFile.peek() != '\t')
        {
            inFile >> temp;
            // --- check duplicate
            for (string s : label)
            {
                if (temp.compare(s) == 0)
                {
                    cerr << "Duplicate label.\n";
                    exit(1);
                }
            }
            label.push_back(temp);
            addr.push_back(line);
        }
        inFile.ignore(numeric_limits<streamsize>::max(), '\n');
        line++;
    }

    inFile.clear();
    inFile.seekg(ios::beg);
    line = 0;
    ;
    //------------------    convert
    while (!inFile.eof())
    {
        char type;
        if (inFile.peek() != '\t')
            inFile >> temp; // skip label
        cout << "0000000";
        // ---------------- opcode
        inFile >> temp;
        if (temp.compare("add") == 0)
        {
            cout << "000";
            type = 'R';
        }
        else if (temp.compare("nand") == 0)
        {
            cout << "001";
            type = 'R';
        }
        else if (temp.compare("lw") == 0)
        {
            cout << "010";
            type = 'I';
        }
        else if (temp.compare("sw") == 0)
        {
            cout << "011";
            type = 'I';
        }
        else if (temp.compare("beq") == 0)
        {
            cout << "100";
            type = 'I';
        }
        else if (temp.compare("jalr") == 0)
        {
            cout << "101";
            type = 'J';
        }
        else if (temp.compare("halt") == 0)
        {
            cout << "1100000000000000000000000\n";
            type = 'O';
            continue;
        }
        else if (temp.compare("noop") == 0)
        {
            cout << "1110000000000000000000000\n";
            type = 'O';
            continue;
        }
        else if (temp.compare(".fill") == 0)
        {
            cout << "000000000";
            type = '.';
        }
        else
        {
            cerr << "\nUnrecognized opcode.\n";
            exit(1);
        }
        // ---------------- field 1
        if (type != '.')
        {
            inFile >> temp;
            int regA = temp[0] - '0';
            int tempR = regA;
            string convert = "";
            while (regA > 0)
            {
                if (regA % 2)
                {
                    convert = "1" + convert;
                }
                else
                {
                    convert = "0" + convert;
                }
                regA >>= 1;
            }
            if (tempR == 0)
            {
                convert = "000" + convert;
            }
            else if (tempR < 2)
            {
                convert = "00" + convert;
            }
            else if (tempR < 4)
            {
                convert = "0" + convert;
            }
            cout << convert;
        }

        // ---------------  field 2
        if (type != '.')
        {
            inFile >> temp;
            int regB = temp[0] - '0';
            int tempR = regB;
            string convert = "";
            while (regB > 0)
            {
                if (regB % 2)
                {
                    convert = "1" + convert;
                }
                else
                {
                    convert = "0" + convert;
                }
                regB >>= 1;
            }
            if (tempR == 0)
            {
                convert = "000" + convert;
            }
            else if (tempR < 2)
            {
                convert = "00" + convert;
            }
            else if (tempR < 4)
            {
                convert = "0" + convert;
            }
            cout << convert;
        }

        // -------------    field 3
        inFile >> temp;
        if (type == 'R')
        {
            cout << "0000000000000";
            int dest = temp[0] - '0';
            int tempR = dest;
            string convert = "";
            while (dest > 0)
            {
                if (dest % 2)
                {
                    convert = "1" + convert;
                }
                else
                {
                    convert = "0" + convert;
                }
                dest >>= 1;
            }
            if (tempR == 0)
            {
                convert = "000" + convert;
            }
            else if (tempR < 2)
            {
                convert = "00" + convert;
            }
            else if (tempR < 4)
            {
                convert = "0" + convert;
            }
            cout << convert;
        }
        else if (type == 'I' || type == '.')
        {
            int num = temp[0] - '0';
            if (temp[0] == '-')
            {
                num = temp[1] - '0';
            }
            bool isLabel = false;
            if (num < 0 || num > 9)
            {
                isLabel = true;
                bool undefine = true;
                for (int i = 0; i < label.size(); i++)
                {
                    if (temp.compare(label[i]) == 0)
                    {
                        num = addr[i];
                        undefine = false;
                    }
                }
                if (undefine)
                {
                    cerr << "\nUndefined Label.\n";
                    exit(1);
                }
            }
            if (!isLabel)
            {
                num = stoi(temp);
            }
            if (num < 0)
            {
                num += 65536;
            }
            string convert = "";
            while (num > 0)
            {
                if (num % 2)
                {
                    convert = "1" + convert;
                }
                else
                {
                    convert = "0" + convert;
                }
                num >>= 1;
            }
            while (convert.size() < 16)
            {
                convert = "0" + convert;
            }
            cout << convert;
        }
        else
        {
            cout << "0000000000000000";
        }

        inFile.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << endl;
    }
    inFile.close();
    return 0;
}
