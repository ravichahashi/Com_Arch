#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <limits>
using namespace std;

int main(int argc, char **argv)
{
    ifstream inFile;
    ofstream outFile;
    vector<string> label;
    vector<int> addr;
    vector<int> fill;
    int reg[8] = {0};
    stack<int> stack;
    string temp;

    inFile.open("test02.txt");
    outFile.open("output.txt", ofstream::out);
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
            if (isdigit(temp[0]))
            {
                cerr << "First char is digit.";
                exit(1);
            }
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
            inFile >> temp;
            if (temp.compare(".fill") == 0)
            {
                inFile >> temp;
                bool isLabel = !isdigit(temp[0]);
                if (temp[0] == '-')
                    isLabel = !isdigit(temp[1]);
                if (isLabel)
                {
                    bool undefine = true;
                    for (int i = 0; i < label.size(); i++)
                    {
                        if (temp.compare(label[i]) == 0)
                        {
                            fill.push_back(addr[i]);
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
                else
                {
                    fill.push_back(stoi(temp));
                }
            }
            else
            {
                fill.push_back(0);
            }
        }
        inFile.ignore(numeric_limits<streamsize>::max(), '\n');
        line++;
    }

    line = 0;
    inFile.clear();
    inFile.seekg(ios::beg);

    //------------------    convert -----------------------------
    while (!inFile.eof())
    {
        int dec = 0;

        if (inFile.peek() != '\t' && inFile.peek() != ' ')
            inFile >> temp; // skip label

        inFile >> temp;
        if (temp.compare("add") == 0)
        {
            int regA, regB, dest;
            inFile >> temp;
            regA = temp[0] - '0';
            dec += regA << 19;

            inFile >> temp;
            regB = temp[0] - '0';
            dec += regB << 16;

            inFile >> temp;
            dest = temp[0] - '0';
            dec += dest;
            reg[dest] = reg[regA] + reg[regB];
        }
        else if (temp.compare("nand") == 0)
        {
            dec += 1 << 22;

            int regA, regB, dest;
            inFile >> temp;
            regA = temp[0] - '0';
            dec += regA << 19;

            inFile >> temp;
            regB = temp[0] - '0';
            dec += regB << 16;

            inFile >> temp;
            dest = temp[0] - '0';
            dec += dest;
        }
        else if (temp.compare("lw") == 0)
        {
            dec += 2 << 22;

            int regA, regB;
            inFile >> temp;
            regA = temp[0] - '0';
            dec += regA << 19;

            inFile >> temp;
            regB = temp[0] - '0';
            dec += regB << 16;

            inFile >> temp;
            int num;
            bool isLabel = !isdigit(temp[0]);
            if (temp[0] == '-')
                isLabel = !isdigit(temp[1]);
            if (isLabel)
            {
                bool undefine = true;
                for (int i = 0; i < label.size(); i++)
                {
                    if (temp.compare(label[i]) == 0)
                    {
                        num = addr[i];
                        if (temp.compare("stack") == 0)
                        {
                            addr[i]--;
                            reg[regB] = stack.top();
                            stack.pop();
                        }
                        else
                        {
                            reg[regB] = fill[i];
                        }
                        undefine = false;
                        break;
                    }
                }
                if (undefine)
                {
                    cerr << "\nUndefined Label.\n";
                    exit(1);
                }
                num -= reg[regA];
                cout<<"of:"<<num<<" ";
            }
            else
            {
                num = stoi(temp);
            }
            while (num < 0 || num > 65535)
            {
                if (num < 0)
                {
                    num += 65536;
                }
                else
                {
                    num -= 65536;
                }
            }
            dec += num;
        }
        else if (temp.compare("sw") == 0)
        {
            dec += 3 << 22;

            int regA, regB;
            inFile >> temp;
            regA = temp[0] - '0';
            dec += regA << 19;

            inFile >> temp;
            regB = temp[0] - '0';
            dec += regB << 16;

            inFile >> temp;
            int num;
            bool isLabel = !isdigit(temp[0]);
            if (temp[0] == '-')
                isLabel = !isdigit(temp[1]);
            if (isLabel)
            {
                bool undefine = true;
                for (int i = 0; i < label.size(); i++)
                {
                    if (temp.compare(label[i]) == 0)
                    {
                        num = addr[i];
                        if (temp.compare("stack") == 0)
                        {
                            addr[i]++;
                            stack.push(reg[regB]);
                        }
                        undefine = false;
                        break;
                    }
                }
                if (undefine)
                {
                    cerr << "\nUndefined Label.\n";
                    exit(1);
                }
                num -= reg[regA];
                cout<<"of:"<<num<<" ";
            }
            else
            {
                num = stoi(temp);
            }
            while (num < 0 || num > 65535)
            {
                if (num < 0)
                {
                    num += 65536;
                }
                else
                {
                    num -= 65536;
                }
            }
            dec += num;
        }
        else if (temp.compare("beq") == 0)
        {
            dec += 4 << 22;

            int regA, regB;
            inFile >> temp;
            regA = temp[0] - '0';
            dec += regA << 19;

            inFile >> temp;
            regB = temp[0] - '0';
            dec += regB << 16;

            inFile >> temp;
            int num;
            bool isLabel = !isdigit(temp[0]);
            if (temp[0] == '-')
                isLabel = !isdigit(temp[1]);
            if (isLabel)
            {
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
                num -= (line + 1);
                cout<<"of:"<<num<<" ";
            }
            else
            {
                num = stoi(temp);
            }
            while (num < 0 || num > 65535)
            {
                if (num < 0)
                {
                    num += 65536;
                }
                else
                {
                    num -= 65536;
                }
            }
            dec += num;
        }
        else if (temp.compare("jalr") == 0)
        {
            dec += 5 << 22;

            int regA, regB;
            inFile >> temp;
            regA = temp[0] - '0';
            dec += regA << 19;

            inFile >> temp;
            regB = temp[0] - '0';
            dec += regB << 16;
        }
        else if (temp.compare("halt") == 0)
        {
            dec += 6 << 22;
        }
        else if (temp.compare("noop") == 0)
        {
            dec += 7 << 22;
        }
        else if (temp.compare(".fill") == 0)
        {
            inFile >> temp;
            int num;
            bool isLabel = !isdigit(temp[0]);
            if (temp[0] == '-')
                isLabel = !isdigit(temp[1]);
            if (isLabel)
            {
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
            else
            {
                num = stoi(temp);
            }
            dec += num;
        }
        else
        {
            cerr << "\nUnrecognized opcode: " << temp << endl;
            exit(1);
        }

        inFile.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << dec << endl;
        outFile << dec << endl;
        line++;
    }
    inFile.close();
    outFile.close();
    return 0;
}
