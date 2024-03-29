#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
using namespace std;

int main()
{
    ifstream inFile;
    vector<string> label;
    vector<int> addr;
    string temp;

    inFile.open("test.txt");
    if (!inFile)
    {
        cerr << "Unable to open file";
        exit(1); // call system to stop
    }

    //--------------------  recognize label
    int line = 0;
    while (!inFile.eof())
    {
        if (inFile.peek() != '\t' && inFile.peek() != ' ')
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

    //------------------    convert
    while (!inFile.eof())
    {
        char type;
        if (inFile.peek() != '\t' && inFile.peek() != ' ')
            inFile >> temp; // skip label
        // ---------------- opcode
        inFile >> temp;
        if (temp.compare("add") == 0)
        {
            cout << "0000000000";
            type = 'R';
        }
        else if (temp.compare("nand") == 0)
        {
            cout << "0000000001";
            type = 'R';
        }
        else if (temp.compare("lw") == 0)
        {
            cout << "0000000010";
            type = 'I';
        }
        else if (temp.compare("sw") == 0)
        {
            cout << "0000000011";
            type = 'I';
        }
        else if (temp.compare("beq") == 0)
        {
            cout << "0000000100";
            type = 'I';
        }
        else if (temp.compare("jalr") == 0)
        {
            cout << "0000000101";
            type = 'J';
        }
        else if (temp.compare("halt") == 0)
        {
            cout << "00000001100000000000000000000000\n";
            type = 'O';
            inFile.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        else if (temp.compare("noop") == 0)
        {
            cout << "00000001110000000000000000000000\n";
            type = 'O';
            inFile.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        else if (temp.compare(".fill") == 0)
        {
            type = '.';
        }
        else
        {
            cerr << "\nUnrecognized opcode.\n";
            exit(1);
        }

        // ---------------- field 1-2
        if (type != '.')
        {
            for (int round = 0; round < 2; round++)
            {
                inFile >> temp;
                int reg = temp[0] - '0';
                int tempR = reg;
                string convert = "";
                while (reg > 0)
                {
                    if (reg % 2)
                    {
                        convert = "1" + convert;
                    }
                    else
                    {
                        convert = "0" + convert;
                    }
                    reg >>= 1;
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
            if (type == 'J')
            {
                cout << "0000000000000000";
                inFile.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
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
        else
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
            int digit = 32;
            if (type == 'I')
            {
                digit = 16;
                if (num < 0)
                {
                    num += 65536;
                }
                else if (num > 65535)
                {
                    num -= 65536;
                }
            }
            string convert = "";
            unsigned int num2 = num;
            while (num2 > 0)
            {
                if (num2 % 2)
                {
                    convert = "1" + convert;
                }
                else
                {
                    convert = "0" + convert;
                }
                num2 >>= 1;
            }
            int zero = digit - convert.size();
            for (int i = 0; i < zero; i++)
            {
                convert = "0" + convert;
            }
            cout << convert;
        }

        inFile.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << endl;
    }
    inFile.close();
    return 0;
}
