#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
using namespace std;

int main(int argc, char ** argv)
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

    //--------------------  recognize label ------------------------
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

    //------------------    convert -----------------------------
    while (!inFile.eof())
    {
        int reg[8];
        int dec = 0;
        char type;
        if (inFile.peek() != '\t' && inFile.peek() != ' ')
            inFile >> temp; // skip label
        // ---------------- opcode
        inFile >> temp;
        if (temp.compare("add") == 0)
        {
            type = 'R';
        }
        else if (temp.compare("nand") == 0)
        {
            dec += 1 << 22;
            type = 'R';
        }
        else if (temp.compare("lw") == 0)
        {
            dec += 2 << 22;
            type = 'I';
        }
        else if (temp.compare("sw") == 0)
        {
            dec += 3 << 22;
            type = 'I';
        }
        else if (temp.compare("beq") == 0)
        {
            dec += 4 << 22;
            type = 'I';
        }
        else if (temp.compare("jalr") == 0)
        {
            dec += 5 << 22;
            type = 'J';
        }
        else if (temp.compare("halt") == 0)
        {
            dec += 6 << 22;
            type = 'O';
            cout << dec << endl;
            inFile.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        else if (temp.compare("noop") == 0)
        {
            dec += 7 << 22;
            type = 'O';
            cout << dec << endl;
            inFile.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        else if (temp.compare(".fill") == 0)
        {
            type = '.';
        }
        else
        {
            cerr << "\nUnrecognized opcode: " << temp << endl;
            exit(1);
        }

        // ---------------- field 1-2
        if (type != '.')
        {

            inFile >> temp;
            int reg = temp[0] - '0';
            dec += reg << 19;
            inFile >> temp;
            reg = temp[0] - '0';
            dec += reg << 16;
            if (type == 'J')
            {
                cout << dec << endl;
                inFile.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
        }

        // -------------    field 3
        inFile >> temp;
        if (type == 'R')
        {
            int dest = temp[0] - '0';
            dec += dest;
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
                        break;
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
            if (type == 'I')
            {
                while (num < -32768 || num > 32767)
                {
                    if (num < -32768)
                    {
                        num += 65536;
                    }
                    else
                    {
                        num -= 65536;
                    }
                }
            }
            dec += num;
        }
        cout << dec << endl;

        inFile.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    inFile.close();
    return 0;
}
