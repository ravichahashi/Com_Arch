#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <limits>
using namespace std;

bool haveLabel(ifstream &);
int getReg(ifstream &);
bool isLabel(string);
void rewindFile(ifstream &);
void nextLine(ifstream &);
void to16Bits(int &);
void errNotify(char);
int transOC(string);

int main(int argc, char **argv)
{
    ifstream inFile;
    ofstream outFile;
    vector<string> label;
    vector<int> addr;
    vector<int> fill;
    int reg[8] = {0};
    stack<int> stack;
    int maxStack = 0, countStack = 0;
    string temp;

    if (argc != 3)
    {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
               argv[0]);
        exit(1);
    }

    inFile.open(argv[1]);
    outFile.open(argv[2], ofstream::out);
    if (!inFile.is_open())
        errNotify('O');

    //------------------------- recognize label ------------------------
    int PC = 0;
    while (!inFile.eof())
    {
        if (haveLabel(inFile))
        {
            inFile >> temp;
            if (isdigit(temp[0]))
                errNotify('F');
            // --- check duplicate
            for (string s : label)
                if (temp.compare(s) == 0)
                    errNotify('D');
            label.push_back(temp);
            addr.push_back(PC);
        }
        nextLine(inFile);
        PC++;
    }

    PC = 0;
    rewindFile(inFile);

    // -------------------------- get .fill with label ---------------------------
    while (!inFile.eof())
    {
        if (haveLabel(inFile))
        {
            inFile >> temp;
            inFile >> temp;
            if (temp.compare(".fill") == 0)
            {
                inFile >> temp;
                if (isLabel(temp))
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
                        errNotify('L');
                }
                else
                    fill.push_back(stoi(temp));
            }
            else
                fill.push_back(0);
        }
        nextLine(inFile);
        PC++;
    }

    PC = 0;
    rewindFile(inFile);

    // -------------------------- convert -----------------------------
    while (!inFile.eof())
    {
        int dec = 0;
        int opcode;
        if (haveLabel(inFile))
            inFile >> temp; // skip label

        inFile >> temp;
        opcode = transOC(temp);
        dec += opcode << 22;
        if (temp.compare(".fill") == 0)
        {
            inFile >> temp;
            int num;
            if (isLabel(temp))
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
                    errNotify('L');
            }
            else
                num = stoi(temp);
            dec += num;
        }
        else if (opcode != 6 && opcode != 7)
        {
            int regA, regB, dest;
            regA = getReg(inFile);
            dec += regA << 19;

            regB = getReg(inFile);
            dec += regB << 16;

            if (opcode <= 1)
            {
                dest = getReg(inFile);
                dec += dest;
                if (opcode == 0)
                    reg[dest] = reg[regA] + reg[regB];
            }
            else if (opcode == 2)
            {
                inFile >> temp;
                int num;
                if (isLabel(temp))
                {
                    bool undefine = true;
                    for (int i = 0; i < label.size(); i++)
                    {
                        if (temp.compare(label[i]) == 0)
                        {
                            if (temp.compare("stack") == 0)
                            {
                                addr[i]--;
                                reg[regB] = stack.top();
                                stack.pop();
                                countStack--;
                            }
                            else
                                reg[regB] = fill[i];
                            num = addr[i];
                            undefine = false;
                            break;
                        }
                    }
                    if (undefine)
                        errNotify('L');
                    num -= reg[regA];
                }
                else
                    num = stoi(temp);
                to16Bits(num);
                dec += num;
            }
            else if (opcode == 3)
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
                            if (temp.compare("stack") == 0)
                            {
                                addr[i]++;
                                stack.push(reg[regB]);
                                countStack++;
                                countStack > maxStack ? maxStack = countStack : 0;
                            }
                            undefine = false;
                            break;
                        }
                    }
                    if (undefine)
                        errNotify('L');
                    num -= reg[regA];
                }
                else
                    num = stoi(temp);
                to16Bits(num);
                dec += num;
            }
            else if (opcode == 4)
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
                        errNotify('L');
                    num -= (PC + 1);
                }
                else
                    num = stoi(temp);
                to16Bits(num);
                dec += num;
            }
        }

        nextLine(inFile);
        outFile << dec << endl;
        PC++;
    }
    inFile.close();
    for (int i = 1; i < maxStack; i++)
        outFile << 0 << endl;
    outFile.close();
    return 0;
}

// ------------------------------ function ------------------------------

bool haveLabel(ifstream &inFile)
{
    return (inFile.peek() != '\t' && inFile.peek() != ' ');
}

int getReg(ifstream &inFile)
{
    string temp;
    inFile >> temp;
    int reg = temp[0] - '0';
    return reg;
}

bool isLabel(string data)
{
    bool flag;
    if (data[0] == '-')
        flag = !isdigit(data[1]);
    else
        flag = !isdigit(data[0]);
    return flag;
}

void rewindFile(ifstream &inFile)
{
    inFile.clear();
    inFile.seekg(ios::beg);
}

void nextLine(ifstream &inFile)
{
    inFile.ignore(numeric_limits<streamsize>::max(), '\n');
}

void to16Bits(int &data)
{
    while (data < 0 || data > 65535)
    {
        if (data < 0)
        {
            data += 65536;
        }
        else
        {
            data -= 65536;
        }
    }
}

void errNotify(char data)
{
    switch (data)
    {
    case 'O':
        cerr << "error in opening\n";
        break;
    case 'F':
        cerr << "error: first char is digit\n";
        break;
    case 'D':
        cerr << "error: duplicate label\n";
        break;
    case 'L':
        cerr << "error: undefined label\n";
        break;
    case 'C':
        cerr << "error: unrecognized opcode\n";
        break;
    default:
        break;
    }
    exit(1);
}

int transOC(string data)
{
    int opcode;
    if (data.compare("add") == 0)
        opcode = 0;
    else if (data.compare("nand") == 0)
        opcode = 1;
    else if (data.compare("lw") == 0)
        opcode = 2;
    else if (data.compare("sw") == 0)
        opcode = 3;
    else if (data.compare("beq") == 0)
        opcode = 4;
    else if (data.compare("jalr") == 0)
        opcode = 5;
    else if (data.compare("halt") == 0)
        opcode = 6;
    else if (data.compare("noop") == 0)
        opcode = 7;
    else if (data.compare(".fill") == 0)
        opcode = 0;
    else
        errNotify('C');
    return opcode;
}