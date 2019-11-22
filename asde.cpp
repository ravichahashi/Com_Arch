#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <limits>
using namespace std;

#define LEVEL_OF_STACK 10

class myObj
{
public:
    vector<string> label;
    vector<int> addr;
    vector<int> fill;
    int reg[8] = {0};
    stack<int> stack;
};

bool haveLabel(ifstream &);
int getReg(ifstream &);
bool isLabel(string);
void rewindFile(ifstream &);
void nextLine(ifstream &);
void errNotify(char, string = "", int = 0);
int transOC(string);
int getValue(ifstream &, myObj *, int = 0, int = 0, int = 0, int = 0);

int main(int argc, char **argv)
{
    ifstream inFile;
    ofstream outFile;
    myObj obj;
    string temp;

    // if (argc != 3)
    // {
    //     printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
    //            argv[0]);
    //     exit(1);
    // }

    inFile.open("2.txt");
    outFile.open("out.txt", ofstream::out);
    if (!inFile.is_open())
        errNotify('O', argv[1]);

    //------------------------- recognize label ------------------------
    int PC = 0;
    while (!inFile.eof())
    {
        if (haveLabel(inFile))
        {
            inFile >> temp;
            if (isdigit(temp[0]))
                errNotify('F', temp);
            // --- check duplicate
            for (string s : obj.label)
                if (temp.compare(s) == 0)
                    errNotify('D', temp);
            obj.label.push_back(temp);
            obj.addr.push_back(PC);
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
                    for (int i = 0; i < obj.label.size(); i++)
                    {
                        if (temp.compare(obj.label[i]) == 0)
                        {
                            obj.fill.push_back(obj.addr[i]);
                            undefine = false;
                            break;
                        }
                    }
                    if (undefine)
                        errNotify('L', temp);
                }
                else
                    obj.fill.push_back(stoi(temp));
            }
            else
                obj.fill.push_back(0);
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
            int num = getValue(inFile, &obj);
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
                    obj.reg[dest] = obj.reg[regA] + obj.reg[regB];
            }
            else if (opcode == 2)
            {
                int num = getValue(inFile, &obj, opcode, regA, regB, PC);
                dec += num;
            }
            else if (opcode == 3)
            {
                int num = getValue(inFile, &obj, opcode, regA, regB, PC);
                dec += num;
            }
            else if (opcode == 4)
            {
                int num = getValue(inFile, &obj, opcode, regA, regB, PC);
                dec += num;
            }
        }

        nextLine(inFile);
        outFile << dec << endl;
        PC++;
    }
    inFile.close();
    for (int i = 1; i < LEVEL_OF_STACK; i++)
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

void errNotify(char data, string miss, int num)
{
    switch (data)
    {
    case 'O':
        cerr << "error in opening: " << miss;
        break;
    case 'F':
        cerr << "error: first char is digit: " << miss;
        break;
    case 'D':
        cerr << "error: duplicate label: " << miss;
        break;
    case 'L':
        cerr << "error: undefined label: " << miss;
        break;
    case 'C':
        cerr << "error: unrecognized opcode: " << miss;
        break;
    case 'N':
        cerr << "error: too large number: " << num;
        break;
    default:
        break;
    }
    cerr << endl;
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
        errNotify('C', data);
    return opcode;
}

int getValue(ifstream &inFile, myObj *obj, int opcode, int regA, int regB, int PC)
{
    string temp;
    inFile >> temp;
    int num;
    if (isLabel(temp))
    {
        bool undefine = true;
        for (int i = 0; i < obj->label.size(); i++)
        {
            if (temp.compare(obj->label[i]) == 0)
            {
                switch (opcode)
                {
                case 2:
                    if (temp.compare("stack") == 0)
                    {
                        obj->addr[i]--;
                        obj->reg[regB] = obj->stack.top();
                        obj->stack.pop();
                    }
                    else
                        obj->reg[regB] = obj->fill[i];
                    num = obj->addr[i] - obj->reg[regA];
                    break;
                case 3:
                    num = obj->addr[i] - obj->reg[regA];
                    if (temp.compare("stack") == 0)
                    {
                        obj->addr[i]++;
                        obj->stack.push(obj->reg[regB]);
                    }
                    break;
                case 4:
                    num = obj->addr[i] - (PC + 1);
                    break;
                default:
                    num = obj->addr[i];
                    break;
                }
                undefine = false;
                break;
            }
        }
        if (undefine)
            errNotify('L', temp);
    }
    else
        num = stoi(temp);
    if (opcode != 0)
        if (num < -32768 || num > 32767)
            errNotify('N',"",num);
    return num;
}