#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8		/* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct
{
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
} stateType;

void printState(stateType *);
//
int getOpcode(int inst);
int getRegB(int inst);
int getRegA(int inst);
int getMemData(int addr, stateType *statePtr);
void Initialize(stateType *statePtr);
int readMem(int addr, stateType *statePtr);
int readReg(int addr, stateType *statePtr);
void writeMem(int addr, int data, stateType *statePtr);
void writeReg(int addr, int data, stateType *statePtr);
short execute(stateType *statePtr);
void haltError(stateType *statePtr);

int main(int argc, char *argv[])
{
	char line[MAXLINELENGTH];
	stateType state;
	FILE *filePtr;

	if (argc != 2)
	{
		printf("error: usage: %s <machine-code file>\n", argv[0]);
		exit(1);
	}

	//filePtr = fopen("assm01.txt", "r");
	filePtr = fopen(argv[1], "r");
	if (filePtr == NULL)
	{
		printf("error: can't open file %s", argv[1]);
		perror("fopen");
		exit(1);
	}
	//
	Initialize(&state);
	//
	/* read in the entire machine-code file into memory */
	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
		 state.numMemory++)
	{
		if (sscanf(line, "%d", state.mem + state.numMemory) != 1)
		{
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		}
		printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
	}

	int regA[state.numMemory];
	int regB[state.numMemory];
	short isBeq = 0;
	short isChange = 0;
	int instCount = 0;
	short isHalt = 0;
	int inst = 0;
	int opcode = 0;
	while (!isHalt)
	{
		instCount++;
		printState(&state);
		isHalt = execute(&state);
	}
	printf("machine halted\n");
	printf("total of %d instructions executed\n", instCount);
	printf("final state of machine:\n");
	printState(&state);
	return (0);
}

void printState(stateType *statePtr)
{
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", statePtr->pc);
	printf("\tmemory:\n");
	for (i = 0; i < statePtr->numMemory; i++)
	{
		printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
	printf("\tregisters:\n");
	for (i = 0; i < NUMREGS; i++)
	{
		printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
	printf("end state\n");
}

short execute(stateType *statePtr)
{
	int inst = statePtr->mem[statePtr->pc];
	int opcode = getOpcode(inst);
	statePtr->pc++;
	int regA = getRegA(inst);
	int regB = getRegB(inst);
	int destReg = inst & 0x00000007;
	int OSF = getOSF(inst);
	int result = 0;
	//	printf(	"opcode: %d\n",opcode);
	//	printf(	"regA: %d\n",regA);
	//	printf(	"regB: %d\n",regB);
	switch (opcode)
	{
	case 0:
		result = readReg(regA, statePtr) + readReg(regB, statePtr);
		writeReg(destReg, result, statePtr);
		break;
	case 1:
		result = (~readReg(regA, statePtr)) & readReg(regB, statePtr);
		writeReg(destReg, result, statePtr);
		break;
	case 2:
		result = readMem(readReg(regA, statePtr) + OSF, statePtr);
		writeReg(regB, result, statePtr);
		break;
	case 3:
		result = readReg(regA, statePtr) + OSF;
		writeMem(result, regB, statePtr);
		break;
	case 4:
		if (readReg(regA, statePtr) == readReg(regB, statePtr))
		{
			statePtr->pc += OSF;
		}
		break;
	case 5:
		writeReg(regB, statePtr->pc, statePtr);
		statePtr->pc = (readReg(regA, statePtr));
		break;
	case 6:
		return 1;
	case 7:
		break;
	}
	return 0;
}

int getOpcode(int inst)
{
	//printf(	"%d\n",(opcode<<7)>>29&0x00000007);
	return (inst >> 22) & 0x00000007;
}

int getRegB(int inst)
{

	return (inst >> 16) & 0x00000007;
}
int getRegA(int inst)
{
	return (inst >> 19) & 0x00000007;
}

int getOSF(int inst)
{
	return (inst << 16) >> 16;
}

void Initialize(stateType *statePtr)
{
	int i = 0;
	for (i = 0; i < NUMREGS; i++)
	{
		statePtr->reg[i] = 0;
	}
	statePtr->pc = 0;
}

int readMem(int addr, stateType *statePtr)
{
	if (addr < statePtr->numMemory && addr >= 0)
	{
		return statePtr->mem[addr];
	}
	else
	{
		haltError(statePtr);
	}
}
int readReg(int addr, stateType *statePtr)
{
	if (addr < NUMREGS & addr >= 0)
	{
		return statePtr->reg[addr];
	}
	else
	{
		haltError(statePtr);
	}
}
void writeMem(int addr, int data, stateType *statePtr)
{
	if (addr < statePtr->numMemory)
	{
		statePtr->mem[addr] = statePtr->reg[data];
	}
}

void writeReg(int addr, int data, stateType *statePtr)
{
	if (addr < NUMREGS)
	{
		statePtr->reg[addr] = data;
	}
}

void haltError(stateType *statePtr)
{
	printf("machine halted\n");
	printf("worng memory address reference\n");
	printf("final state of machine:\n");
	printState(statePtr);
	exit(1);
}
