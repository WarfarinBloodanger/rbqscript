#include <bits/stdc++.h>

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;

#ifdef WIN32
struct ColourController {
	#define SV(x, r) \
	static void x() {\
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_##r);\
	}
	SV(red,		RED)
	SV(green,	GREEN)
	SV(blue,	BLUE)
	#undef SV
	
	#define SV(x, a, b)\
	static void x() {\
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_##a | FOREGROUND_##b);\
	}
	SV(yellow,	RED,	GREEN)
	SV(purple,	RED,	BLUE)
	SV(cyan,	GREEN,	BLUE)
	#undef SV
	#define SV(x, a)\
	static void x() {\
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);\
	}
	SV(black, 0)
	SV(white, 7)
	static void set(string s) {
		if(s == "red") red();
		if(s == "blue") blue();
		if(s == "green") green();
		if(s == "yellow") yellow();
		if(s == "purple") purple();
		if(s == "cyan") cyan();
		if(s == "black") black();
		if(s == "white") white();
	}
};
#else
struct ColourController {
	static void set(string s) {}
};
#endif

#define OPCODE const unsigned char

const int MAX_CPOOL = 1024 * 64;
const int MAX_FUNC_CNT = 1024 * 4;

double cpool[MAX_CPOOL];
int cpool_pos;

void add_const(double x) {
	cpool[cpool_pos++] = x;
}

map<string, string> res;
	
string LDS	= "cyan";
string OPR	= "yellow";
string LN	= "green";
string MAT	= "red";
string CCAL	= "red";
string BKF	= "white";

void check_colour(unsigned char byte) {
	#define RN(a, b, c) if(byte >= a && byte <= b) {\
		ColourController::set(c);\
		return;\
	}
	
	#define E(a, c) if(byte == a) {\
		ColourController::set(c);\
		return;\
	}
	
	E(0xca, LDS)
	E(0xcf, LDS)
	E(0xcd, LDS)
	E(0xcb, LDS)
	E(0xce, LDS)
	E(0xd2, LDS)
	E(0xd5, LDS)
	E(0xb6, OPR)
	E(0xc2, OPR)
	E(0xc9, OPR)
	E(0xd1, OPR)
	E(0xd9, OPR)
	E(0xb4, LN)
	E(0xc5, LN)
	E(0xcc, LN)
	E(0xda, LN)
	E(0xc0, MAT)
	E(0xc3, MAT)
	E(0xc4, MAT)
	E(0xc6, MAT)
	E(0xc8, CCAL)
	E(0xd0, MAT)
	E(0xd3, MAT)
	E(0xd4, MAT)
	E(0xdb, LN)
	E(0xdc, LDS)
	E(0xdd, LDS)
	E(0xde, LDS)
	
	RN(0x80, 0x87, LN)
	RN(0x90, 0x9f, LN)
	
	RN(0xa0, 0xb6, OPR)
	RN(0xc0, 0xd9, "white")

} 

OPCODE LDC0 = 0x80;
OPCODE LDC1 = 0x81;
OPCODE LDC2 = 0x82;
OPCODE LDC3 = 0x83;
OPCODE LDC4 = 0x84;
OPCODE LDC5 = 0x85;
OPCODE LDC6 = 0x86;
OPCODE LDC7 = 0x87;
OPCODE LD0 = 0x90;
OPCODE LD1 = 0x91;
OPCODE LD2 = 0x92;
OPCODE LD3 = 0x93;
OPCODE LD4 = 0x94;
OPCODE LD5 = 0x95;
OPCODE LD6 = 0x96;
OPCODE LD7 = 0x97;
OPCODE LD8 = 0x98;
OPCODE LD9 = 0x99;
OPCODE LD10 = 0x9a;
OPCODE LD11 = 0x9b;
OPCODE LD12 = 0x9c;
OPCODE LD13 = 0x9d;
OPCODE LD14 = 0x9e;
OPCODE LD15 = 0x9f;
OPCODE ADD = 0xa0;
OPCODE SUB = 0xa1;
OPCODE MUL = 0xa2;
OPCODE DIV = 0xa3;
OPCODE NOT = 0xa4;
OPCODE MOD = 0xa5;
OPCODE LAND = 0xa6;
OPCODE LOR = 0xa7;
OPCODE BAND = 0xa8;
OPCODE BOR = 0xa9;
OPCODE XOR = 0xaa;
OPCODE BIG = 0xab;
OPCODE BIGE = 0xac;
OPCODE SML = 0xad;
OPCODE SMLE = 0xae;
OPCODE EQL = 0xaf;
OPCODE NEQL = 0xb0;
OPCODE LSHF = 0xb1;
OPCODE RSHF = 0xb2;
OPCODE NEG = 0xb3;
OPCODE LDNUL = 0xb4;
OPCODE BNOT = 0xb5;
OPCODE MKRG = 0xb6;
OPCODE LOOP = 0xc0;
OPCODE POP = 0xc1;
OPCODE MKARR = 0xc2;
OPCODE JMP = 0xc3;
OPCODE JMPN = 0xc4;
OPCODE LDC = 0xc5;
OPCODE RET = 0xc6;
OPCODE DUP = 0xc7;
OPCODE CALL = 0xc8;
OPCODE STR = 0xc9;
OPCODE LOAD = 0xca;
OPCODE STORE = 0xcb; 
OPCODE LDN = 0xcc;
OPCODE MGET = 0xcd;
OPCODE MSET = 0xce;
OPCODE GETSUB = 0xcf;
OPCODE LOOPIF = 0xd0;
OPCODE FUNC = 0xd1;
OPCODE LSTORE = 0xd2;
OPCODE BREAK = 0xd3;
OPCODE CNTN = 0xd4;
OPCODE RESET = 0xd5;
OPCODE LRMV = 0xd6;
OPCODE NOP = 0xd7;
OPCODE LOCK = 0xd8;
OPCODE MKBIG = 0xd9;
OPCODE LDNC = 0xda;
OPCODE LDCC = 0xdb;
OPCODE LOADC = 0xdc;
OPCODE STOREC = 0xdd;
OPCODE LSTOREC = 0xde;
OPCODE END = 0xff;

string opname[256];

void init_ch() {
res["func"] = "函数";
res["pcnt"] = "参数数量";
res["bltn"] = "内建函数";
opname[LDC0] = "加载常量表项 0";
opname[LDC1] = "加载常量表项 1";
opname[LDC2] = "加载常量表项 2";
opname[LDC3] = "加载常量表项 3";
opname[LDC4] = "加载常量表项 4";
opname[LDC5] = "加载常量表项 5";
opname[LDC6] = "加载常量表项 6";
opname[LDC7] = "加载常量表项 7";
opname[LD0] = "加载数字 0";
opname[LD1] = "加载数字 1";
opname[LD2] = "加载数字 2";
opname[LD3] = "加载数字 3";
opname[LD4] = "加载数字 4";
opname[LD5] = "加载数字 5";
opname[LD6] = "加载数字 6";
opname[LD7] = "加载数字 7";
opname[LD8] = "加载数字 8";
opname[LD9] = "加载数字 9";
opname[LD10] = "加载数字 10";
opname[LD11] = "加载数字 11";
opname[LD12] = "加载数字 12";
opname[LD13] = "加载数字 13";
opname[LD14] = "加载数字 14";
opname[LD15] = "加载数字 15";
opname[ADD] = "相加";
opname[SUB] = "相减";
opname[MUL] = "相乘";
opname[DIV] = "相除";
opname[NOT] = "非";
opname[MOD] = "取模";
opname[LAND] = "逻辑与";
opname[LOR] = "逻辑或";
opname[BAND] = "按位与";
opname[BOR] = "按位或";
opname[XOR] = "异或";
opname[BIG] = "大于";
opname[BIGE] = "大于等于";
opname[SML] = "小于";
opname[SMLE] = "小于等于";
opname[EQL] = "等于";
opname[NEQL] = "不等于";
opname[LSHF] = "左移";
opname[RSHF] = "右移";
opname[NEG] = "取反";
opname[BNOT] = "按位取反";
opname[MKRG] = "构造 Range 对象";
opname[LDNUL] = "加载 Null";
opname[LOOP] = "往前跳转";
opname[POP] = "弹出栈顶";
opname[MKARR] = "构造 Array 对象";
opname[JMP] = "往后跳转";
opname[JMPN] = "条件为假时往后跳转";
opname[LDC] = "加载常量表项";
opname[RET] = "返回";
opname[DUP] = "复制栈顶";
opname[CALL] = "调用";
opname[STR] = "构造 String 对象";
opname[LOAD] = "加载变量";
opname[STORE] = "保存到变量";
opname[LDN] = "加载 2 字节整数";
opname[MGET] = "加载地址";
opname[MSET] = "保存到地址";
opname[GETSUB] = "获取对应下标";
opname[LOOPIF] = "条件为真时往前跳转";
opname[FUNC] = "构造函数对象";
opname[LSTORE] = "保存到局部变量";
opname[BREAK] = "跳出循环";
opname[CNTN] = "跳转到循环头部";
opname[RESET] = "重置栈顶";
opname[LRMV] = "移除局部变量";
opname[NOP] = "空指令";
opname[LOCK] = "设为只读";
opname[MKBIG] = "构造 BigInteger 对象";
opname[LDNC] = "加载 1 字节整数";
opname[LDCC] = "加载常量表项";
opname[LOADC] = "加载变量";
opname[STOREC] = "保存到变量";
opname[LSTOREC] = "保存到局部变量";
opname[END] = "结束";
}

void init_en() {
res["func"] = "function";
res["pcnt"] = "number of argument";
res["bltn"] = "built in function";
opname[LDC0] = "load_constant_0";
opname[LDC1] = "load_constant_1";
opname[LDC2] = "load_constant_2";
opname[LDC3] = "load_constant_3";
opname[LDC4] = "load_constant_4";
opname[LDC5] = "load_constant_5";
opname[LDC6] = "load_constant_6";
opname[LDC7] = "load_constant_7";
opname[LD0] = "load_number_0";
opname[LD1] = "load_number_1";
opname[LD2] = "load_number_2";
opname[LD3] = "load_number_3";
opname[LD4] = "load_number_4";
opname[LD5] = "load_number_5";
opname[LD6] = "load_number_6";
opname[LD7] = "load_number_7";
opname[LD8] = "load_number_8";
opname[LD9] = "load_number_9";
opname[LD10] = "load_number_10";
opname[LD11] = "load_number_11";
opname[LD12] = "load_number_12";
opname[LD13] = "load_number_13";
opname[LD14] = "load_number_14";
opname[LD15] = "load_number_015";
opname[ADD] = "add";
opname[SUB] = "substract";
opname[MUL] = "multiply";
opname[DIV] = "divide";
opname[NOT] = "logic_not";
opname[MOD] = "modulo";
opname[LAND] = "logic_and";
opname[LOR] = "logic_or";
opname[BAND] = "bitwise_and";
opname[BOR] = "bitwise_or";
opname[XOR] = "bitwise_xor";
opname[BIG] = "greater";
opname[BIGE] = "greater_equal";
opname[SML] = "smaller";
opname[SMLE] = "smaller_equal";
opname[EQL] = "equal";
opname[NEQL] = "not_equal";
opname[LSHF] = "left_shift";
opname[RSHF] = "right_shift";
opname[NEG] = "negative";
opname[BNOT] = "bitwise_negative";
opname[MKRG] = "construct_range";
opname[LDNUL] = "load_null";
opname[LOOP] = "loop";
opname[POP] = "pop";
opname[MKARR] = "construct_array";
opname[JMP] = "jump";
opname[JMPN] = "jump_if_not";
opname[LDC] = "load_constant";
opname[RET] = "return";
opname[DUP] = "duplicate_top";
opname[CALL] = "call";
opname[STR] = "construct_string";
opname[LOAD] = "load_variable";
opname[STORE] = "store_variable";
opname[LDN] = "load_number";
opname[MGET] = "get_address";
opname[MSET] = "store_address";
opname[GETSUB] = "get_subscript";
opname[LOOPIF] = "loop_if_true";
opname[FUNC] = "construct_function";
opname[LSTORE] = "store_local_variable";
opname[BREAK] = "break";
opname[CNTN] = "continue";
opname[RESET] = "reset_stack";
opname[LRMV] = "remove_local_variable";
opname[NOP] = "nop";
opname[LOCK] = "readonly";
opname[MKBIG] = "construct_biginteger";
opname[LDNC] = "load_number";
opname[LDCC] = "load_constant";
opname[LOADC] = "load_variable";
opname[STOREC] = "store_variable";
opname[LSTOREC] = "store_local_variable";
opname[END] = "end";
}

void init_op() {
res["func"] = "FUNC";
res["pcnt"] = "ARGC";
res["bltn"] = "BUILTIN";
opname[LDC0] = "LDC0";
opname[LDC1] = "LDC1";
opname[LDC2] = "LDC2";
opname[LDC3] = "LDC3";
opname[LDC4] = "LDC4";
opname[LDC5] = "LDC5";
opname[LDC6] = "LDC6";
opname[LDC7] = "LDC7";
opname[LD0] = "LD0";
opname[LD1] = "LD1";
opname[LD2] = "LD2";
opname[LD3] = "LD3";
opname[LD4] = "LD4";
opname[LD5] = "LD5";
opname[LD6] = "LD6";
opname[LD7] = "LD7";
opname[LD8] = "LD8";
opname[LD9] = "LD9";
opname[LD10] = "LD10";
opname[LD11] = "LD11";
opname[LD12] = "LD12";
opname[LD13] = "LD13";
opname[LD14] = "LD14";
opname[LD15] = "LD15";
opname[ADD] = "ADD";
opname[SUB] = "SUB";
opname[MUL] = "MUL";
opname[DIV] = "DIV";
opname[NOT] = "NOT";
opname[MOD] = "MOD";
opname[LAND] = "LAND";
opname[LOR] = "LOR";
opname[BAND] = "BAND";
opname[BOR] = "BOR";
opname[XOR] = "XOR";
opname[BIG] = "BIG";
opname[BIGE] = "BIGE";
opname[SML] = "SML";
opname[SMLE] = "SMLE";
opname[EQL] = "EQL";
opname[NEQL] = "NEQL";
opname[LSHF] = "LSHF";
opname[RSHF] = "RSHF";
opname[NEG] = "NEG";
opname[BNOT] = "BNOT";
opname[MKRG] = "MKRG";
opname[LDNUL] = "LDNUL";
opname[LOOP] = "LOOP";
opname[POP] = "POP";
opname[MKARR] = "MKARR";
opname[JMP] = "JMP";
opname[JMPN] = "JMPN";
opname[LDC] = "LDC";
opname[RET] = "RET";
opname[DUP] = "DUP";
opname[CALL] = "CALL";
opname[STR] = "STR";
opname[LOAD] = "LOAD";
opname[STORE] = "STORE";
opname[LDN] = "LDN";
opname[MGET] = "MGET";
opname[MSET] = "MSET";
opname[GETSUB] = "GETSUB";
opname[LOOPIF] = "LOOPIF";
opname[FUNC] = "FUNC";
opname[LSTORE] = "LSTORE";
opname[BREAK] = "BREAK";
opname[CNTN] = "CNTN";
opname[RESET] = "RESET";
opname[LRMV] = "LRMV";
opname[NOP] = "NOP";
opname[LOCK] = "LOCK";
opname[MKBIG] = "MKBIG";
opname[LDNC] = "LDNC";
opname[LDCC] = "LDCC";
opname[LOADC] = "LOADC";
opname[STOREC] = "STOREC";
opname[LSTOREC] = "LSTOREC";
opname[END] = "END";
}
int get_id(string);

int byte_block_len[MAX_FUNC_CNT];
unsigned char byte_map[MAX_FUNC_CNT][10000 + 5];
int func_cnt;
map<string, int> func_map;
map<int, string> id_map; 
map<int, vector<int> > reg_map;
map<int, int> pc_map;
set<int> builtin;

vector<string> all_func;
char buffer[1024];

string stt(int len, string which) {
	for(int i = which.length(); i < len; i++) which += " ";
	return which;
}

string tostr(int x) {
	stringstream ss("");
	ss << x;
	return ss.str();
}

string check_func(int id) {
	if(builtin.find(id) != builtin.end()) return "(" + res["bltn"] + " " + id_map[id] + ")";
	if(id_map.find(id) != id_map.end()) return "(" + res["func"] + " " + id_map[id] + ", " + res["pcnt"] + " " + tostr(pc_map[id]) + ")";
	return "";
}
const string HEX_CHAR = "0123456789ABCDEF";

string to_hex(unsigned char x) {
	string str = "";
	while(x) str += HEX_CHAR[x % 16], x /= 16;
	reverse(str.begin(), str.end());
	if(str.length() == 0) str = "00";
	if(str.length() == 1) str = "0" + str;
	return str;
}

void init() {
	int a = 0;
	#define BUILTIN(x) id_map[a] = x, builtin.insert(a++)
	BUILTIN("print");
    BUILTIN("len");
    BUILTIN("sub");
    BUILTIN("clock");
    BUILTIN("time");
    BUILTIN("memset");
    BUILTIN("system");
    BUILTIN("rand");
    BUILTIN("srand");
    
    BUILTIN("math_sin");
    BUILTIN("math_cos");
    BUILTIN("math_tan");
    BUILTIN("math_asin");
    BUILTIN("math_acos");
    BUILTIN("math_atan");
    BUILTIN("math_floor");
    BUILTIN("math_ceil");
    BUILTIN("math_sqrt");
    BUILTIN("math_exp");
    BUILTIN("math_log");
    BUILTIN("math_log10");
    BUILTIN("math_abs");
    
    BUILTIN("file_open");
    BUILTIN("file_close");
    BUILTIN("file_read_number");
    BUILTIN("file_read_string");
    BUILTIN("file_read_line");
    BUILTIN("file_write_string");
    BUILTIN("file_write_number");
    BUILTIN("file_eof");
    
    BUILTIN("read_number");
    BUILTIN("read_string");
    BUILTIN("read_line");
    
    BUILTIN("str2ascii");
    BUILTIN("ascii2str");
    BUILTIN("exit");
    
    BUILTIN("big");
    BUILTIN("num");
}

#define RS ColourController::set(BKF);

void run_byte(string func, bool of = true) {
	cout << endl;
	int index = get_id(func);
	RS; 
	if(of) cout << res["func"] << " " << func << "\nID " << stt(8, tostr(index)) << endl;
	for(int ip = 0; ip < byte_block_len[index]; ip++) {
		unsigned char byte = byte_map[index][ip];
		cout << "\t";
		cout << stt(8, tostr(ip));
		switch(byte) {
			case LOOP: case LOOPIF:{
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int loop = l_l + l_h * 256;
				int newip = ip - loop;
				cout << stt(16, to_hex(byte) + " " + to_hex(l_h) + " " + to_hex(l_l));
				check_colour(byte);
				cout << stt(24, opname[byte]);
				cout << stt(8, tostr(loop));
				RS
				cout << stt(16, "(to " + tostr(newip + 1) + ")");
			    break;
			}
			case JMP: case BREAK: case CNTN: case JMPN: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int loop = l_l + l_h * 256;
				int newip = ip + loop;
				cout << stt(16, to_hex(byte) + " " + to_hex(l_h) + " " + to_hex(l_l));
				check_colour(byte);
				cout << stt(24, opname[byte]);
				cout << stt(8, tostr(loop));
				RS
				cout << stt(16, "(to " + tostr(newip + 1) + ")");
			    break;
			}
			case LDC: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int ldc = l_l + l_h * 256;
				cout << stt(16, to_hex(byte) + " " + to_hex(l_h) + " " + to_hex(l_l));
				check_colour(byte);
				cout << stt(24, opname[byte]);
				cout << stt(8, tostr(ldc));
				RS
				cout << stt(16, "(" + tostr(cpool[ldc]) + ")");
			    break;
			}
			case LDNC: {
				int l_h = byte_map[index][++ip];
				cout << stt(16, to_hex(byte) + " " + to_hex(l_h));
				check_colour(byte);
				cout << stt(24, opname[byte]);
				cout << stt(8, tostr(l_h));
				RS
				break;
			}
			case STOREC: case LOADC: case LSTOREC: {
				int l_h = byte_map[index][++ip];
				cout << stt(16, to_hex(byte) + " " + to_hex(l_h));
				check_colour(byte);
				cout << stt(24, opname[byte]);
				cout << stt(8, tostr(l_h));
				RS
				cout << check_func(l_h);
				break;
			}
			case LDCC: {
				int l_h = byte_map[index][++ip];
				cout << stt(16, to_hex(byte) + " " + to_hex(l_h));
				check_colour(byte);
				cout << stt(24, opname[byte]);
				cout << stt(8, tostr(l_h));
				RS
				cout << stt(16, "(" + tostr(cpool[l_h]) + ")");
				break;
			}
			case LOAD: case STORE: case LSTORE: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int ldc = l_l + l_h * 256;
				cout << stt(16, to_hex(byte) + " " + to_hex(l_h) + " " + to_hex(l_l));
				check_colour(byte);
				cout << stt(24, opname[byte]);
				cout << stt(8, tostr(ldc));
				RS
				cout << check_func(ldc);
				break;
			}
			case STR: case MKBIG: case FUNC:
			case LRMV: case LDN: case MKARR: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int ldc = l_l + l_h * 256;
				cout << stt(16, to_hex(byte) + " " + to_hex(l_h) + " " + to_hex(l_l));
				check_colour(byte);
				cout << stt(24, opname[byte]);
				cout << stt(8, tostr(ldc));
				RS
			    break;
			}
			default: {
				cout << stt(16, to_hex(byte));
				check_colour(byte);
				cout << stt(24, opname[byte]);
				RS
				break;
			}
		}
		printf("\n");
	}
}

map<string, int> var_id;
int varcnt;
int get_id(string var) {
	if(var_id.find(var) == var_id.end()) var_id[var] = varcnt, varcnt++;
	return var_id[var];
}

int main_id;
void read_bytecode(string file) {
	memset(byte_map, END, sizeof(byte_map));
	ifstream fcin(file.c_str(), ios::binary);
	if(!fcin) {
		cout << ("bytefile " + file + " not found.");
		exit(1);
	}
	int all;
	fcin >> all >> main_id;
	int ccnt;
	fcin >> ccnt;
	for(int i = 0; i < ccnt; i++) {
		double d;
		fcin >> d;
		add_const(d);
	}
	for(int i = 0; i < all; i++) {
		string func;
		int id;
		int pcnt;
		int len;
		fcin >> func >> id >> pcnt;
		var_id[func] = id;
		id_map[id] = func;
		pc_map[id] = pcnt;
		for(int i = 0; i < pcnt; i++) {
			int pname;
			fcin >> pname;
			reg_map[id].push_back(pname);
		}
		fcin >> len;
		byte_block_len[id] = len;
		fcin.get();
		for(int j = 0; j < len; j++) {
			char a;
			fcin.get(a);
			byte_map[id][j] = a;
		}
		all_func.push_back(func);
	}
}


void launch() {
	for(int f = 0; f < all_func.size(); f++) {
		run_byte(all_func[f]);
	}
}
const string EXTENSION = "rbq";

string full_filename(string file) {
	for(int i = file.length() - 1; i >= 0; i--) {
		if(file[i] == '.') {
			return file;
		}
	}
	return file + "." + EXTENSION;
}

string tol(string s) {
	for(int i = 0; i < s.length(); i++) s[i] = tolower(s[i]);
	return s; 
}
string input_file = "";
string output_file = "";

int main(int argc, char ** argv) {
	init();
	if(argc >= 2) {
		read_bytecode(full_filename(argv[1]));
		if(argc >= 3) {
			string l = (char *)tol(argv[2]).c_str();
			if(l == "en") init_en();
			if(l == "ch") init_ch();
			if(l == "op") init_op();
		}
		else init_en();
		launch();
	}
	else {
		cout << "Usage: rbqup <rbq-file-name> [en|ch|op]" << endl;
	}
	ColourController::set("white");
	return 0;
}
