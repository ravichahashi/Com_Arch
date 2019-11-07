//need function reg(string s), imm(string s)
string type;
string answer = "";
string tempLabel[]
if(str[1] == "add" || str[1] == "nand"){
	type = "R";
}
else if(str[1] == "lw" || str[1] == "sw" || str[1] == "beq"){
	type = "I";
}
else if(str[1] == "jalr"){
	type = "J";
}
else if(str[1] == "halt" || str[1] == "noop"){
	type = "O";
}
else{
	cerr << "Your instruction is wrong!!!";
}

if(type == "R"){
	if(str[1]=="add"){
		answer = "000";
	}else{
		answer = "001"; //bug opcode
	}
	
	if(atoi(str[2]) >=0 && atoi(str[2]) <= 8){ // <8
		answer += reg(str[2]);
	}else{
		cerr << "Your register not define!!";
	}
	
	if(atoi(str[3]) >=0 && atoi(str[3]) <= 8){
		answer += reg(str[3]);
	}else{
		cerr << "Your register not define!!";
	}
	answer += "0000000000000";
	if(atoi(str[4]) >=0 && atoi(str[4) <= 8){
		answer += reg(str[4);
	}else{
		cerr << "Your register not define!!";
	}
}

else if(type == "I"){
	if(str[1]=="lw"){
		answer = "010";
	}else if(str[1]=="sw"){ 
		answer = "011";
	}else{
		answer = "100";
	}
	
	if(atoi(str[2]) >=0 && atoi(str[2]) <= 8){
		answer += reg(str[2]);
	}else{
		cerr << "Your register not define!!";
	}
	
	if(atoi(str[3]) >=0 && atoi(str[3]) <= 8){
		answer += reg(str[3]);
	}else{
		cerr << "Your register not define!!";
	}
	
	if(atoi(str[4]) >= -32768 && atoi(str[4]) <= 32767){
		answer += imm(str[4]);
	}else{
		cerr << "Your offsetfied not define!!";
	}
}

else if(type == "J"){
	answer = "101";
	if(atoi(str[2]) >=0 && atoi(str[2]) <= 8){
		answer += reg(str[2]);
	}else{
		cerr << "Your register not define!!";
	}
	
	if(atoi(str[3]) >=0 && atoi(str[3]) <= 8){
		answer += reg(str[3]);
	}else{
		cerr << "Your register not define!!";
	}
	answer += "0000000000000000";
}

else{
	if(str[1]=="halt"){
		answer = "110";
	}else{
		answer = "111";
	}
	answer += "0000000000000000000000";
}

answer = "0000000" + answer;

