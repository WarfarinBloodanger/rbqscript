#include <iostream>

#include <vector>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <string>

#include <sstream>
#include <fstream>

#include <algorithm>

#include <cmath>
#include <cstring>
#include <ctime>

#define FAST_O3
#ifdef FAST_O3
#pragma GCC optimize(3)
#pragma GCC optimize("Ofast")
#pragma GCC optimize("inline")
#pragma GCC optimize("-fgcse")
#pragma GCC optimize("-fgcse-lm")
#pragma GCC optimize("-fipa-sra")
#pragma GCC optimize("-ftree-pre")
#pragma GCC optimize("-ftree-vrp")
#pragma GCC optimize("-fpeephole2")
#pragma GCC optimize("-ffast-math")
#pragma GCC optimize("-fsched-spec")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("-falign-jumps")
#pragma GCC optimize("-falign-loops")
#pragma GCC optimize("-falign-labels")
#pragma GCC optimize("-fdevirtualize")
#pragma GCC optimize("-fcaller-saves")
#pragma GCC optimize("-fcrossjumping")
#pragma GCC optimize("-fthread-jumps")
#pragma GCC optimize("-funroll-loops")
#pragma GCC optimize("-freorder-blocks")
#pragma GCC optimize("-fschedule-insns")
#pragma GCC optimize("inline-functions")
#pragma GCC optimize("-ftree-tail-merge")
#pragma GCC optimize("-fschedule-insns2")
#pragma GCC optimize("-fstrict-aliasing")
#pragma GCC optimize("-falign-functions")
#pragma GCC optimize("-fcse-follow-jumps")
#pragma GCC optimize("-fsched-interblock")
#pragma GCC optimize("-fpartial-inlining")
#pragma GCC optimize("no-stack-protector")
#pragma GCC optimize("-freorder-functions")
#pragma GCC optimize("-findirect-inlining")
#pragma GCC optimize("-fhoist-adjacent-loads")
#pragma GCC optimize("-frerun-cse-after-loop")
#pragma GCC optimize("inline-small-functions")
#pragma GCC optimize("-finline-small-functions")
#pragma GCC optimize("-ftree-switch-conversion")
#pragma GCC optimize("-foptimize-sibling-calls")
#pragma GCC optimize("-fexpensive-optimizations")
#pragma GCC optimize("inline-functions-called-once")
#pragma GCC optimize("-fdelete-null-pointer-checks")
#endif
// -Wl,--stack=1073741824

using namespace std;

const int LIMIT = 0x7fffffff;

const string VM_NAME = "RBQScript";
const string VM_VERSION = "4.91";
bool debug = false;
bool enable_big = false;
bool step_pause = false;
bool show_step = false;
bool show_stack = false;
bool call_pause = false;
bool show_param = false;
bool short_op = true; 

#define STEP (void(printf("Step: %d\n", __LINE__))) 
#define OPCODE const unsigned char

const int MAX_LENGTH = 1024 * 1024;
const int MAX_CPOOL = 1024 * 64;
const int MAX_FUNC_CNT = 1024 * 4;
const int MAX_REG_CNT = 1024 * 128 * 16;
const int MAX_HEAP_SIZE = 1024 * 1024;
const int MAX_OBJ_CNT = 1024 * 16;
const int MAX_FIELD_CNT = 256;

const char NUM_TYPE = '0';
const char STR_TYPE = 'a';
const char FUNC_TYPE = 'F';
const char REF_TYPE = '@';
const char NUL_TYPE = 'x';
const char RNG_TYPE = '[';
const char BIG_TYPE = '9';
const char OBJ_TYPE = 'o';
const char UNDEFINED_TYPE = '?'; 

string type_name[128];

typedef vector<unsigned char> ByteList;
map<string, int> bind_power;

void concat(vector<unsigned char> & a, vector<unsigned char> & b) {
	for(int i = 0; i < b.size(); i++) a.push_back(b[i]);
}

char buffer[35];

string tostr(double n) {
	sprintf(buffer, "%.14g", n);
	return buffer;
}
string tostr_d(double n) {
	sprintf(buffer, "%.14g", n);
	return buffer;
}

const int END_RANGE = -1;

struct Range {
	int from, to;
	Range() {
		from = 0, to = END_RANGE;
	}
	Range(int f, int t) {
		from = f, to = t;
		if(to < from) swap(to, from);
	}
};

Range make_range(int a, int b) {
	return Range(a, b);
}

Range make_front_range(int x) {
	return Range(0, x);
}

Range make_back_range(int x) {
	return Range(0, END_RANGE);
}

Range make_range() {
	return Range();
}

const string TIP = "RBQScript Virtual Machine has caught an error.\n";

void error(string msg, string type = "VerifyError") {
	cout << TIP << type << ": \"" << msg << "\"" << endl;
	exit(0);
}

class BigInteger
{
    void mknum(const char *s, int len = -1)
    {
        sign = 0;
        if(*s == '-')
        {
            mknum(s+1);
            sign = 1;
            return;
        }
        int l;
        if(len == -1)
            l = strlen(s);
        else
            l = len;
        l = strlen(s);
        bits.clear();
        bits.resize(l);
        for(int i = l-1; i >= 0; i--)
            bits[l-i-1] = s[i] - '0';
        maintain();
    }
    void mknum(string &s)
    {
        mknum(s.c_str(), s.length());
    }
    void us_addto(BigInteger &b)
    {
        int mlen = max(b.bits.size(), bits.size());
        int slen = bits.size();
        int olen = b.bits.size();
        bits.resize(mlen);
        for(int i = 0; i < mlen; i++)
        {
            int s = 0;
            if(i < slen)s += bits[i];
            if(i < olen)s += b.bits[i];
            bits[i] = s;
        }
        maintain();
    }
    class FFTer
    {
        class Complex
        {
        public:
            double real, image;
            Complex(double a = 0, double b = 0)
            {
                real = a;
                image = b;
            }
            Complex operator + (const Complex &o){return Complex(real+o.real, image+o.image);}
            Complex operator - (const Complex &o){return Complex(real-o.real, image-o.image);}
            Complex operator * (const Complex &o){return Complex(real*o.real-image*o.image, real*o.image+o.real*image);}
            Complex operator * (double k){return Complex(real*k, image*k);}
            Complex operator / (double k){return Complex(real/k, image/k);}
        };
        public:
        vector<Complex> a;
        int n;
        FFTer(vector<int> &vec)
        {
            a.resize(vec.size());
            for(int i = 0; i < vec.size(); i++)
                a[i].real = vec[i];
            n = vec.size();
        }
        void transform()
        {
            int j = 0;
            int k;
            for(int i = 0; i < n; i++)
            {
                if(j > i)swap(a[i], a[j]);
                k = n;
                while(j & (k >>= 1))j &= ~k;
                j |= k;
            }
        }
        void FFT(bool IDFT = false)
        {
            const double Pi = IDFT ? -acos(-1.0) : acos(-1.0);
            transform(); 
            for(int s = 1; s < n; s <<= 1)
            {
                for(int t = 0; t < n; t += s<<1)
                {
                    double x = Pi/s;
                    Complex omgn(cos(x), sin(x));
                    Complex omg(1.0, 0.0);
                    for(int m = 0; m < s; m++)
                    {      
                        int a1 = m + t;
                        int a2 = m + t + s;
                        Complex comm = omg * a[a2];
                        a[a2] = a[a1] - comm;
                        a[a1] = a[a1] + comm; 
                        omg = omg * omgn;
                    }
                }
            }
            if(IDFT)
                for(int i = 0; i < n; i++)
                    a[i] = a[i] / n;
        }
        void mul(FFTer &o)
        {
            int s = 1;
            while(s < n + o.n)s <<= 1;
            n = o.n = s;
            a.resize(s);
            o.a.resize(s);

            FFT(false);
            o.FFT(false);
            for(int i = 0; i < n; i++)
                a[i] = a[i] * o.a[i];
            FFT(true);
        }
    };
    void us_multo(BigInteger &b)
    {
        FFTer x(bits);
        FFTer y(b.bits);
        x.mul(y);
        bits.clear();
        bits.resize(x.a.size());
        for(int i = 0; i < x.n; i++)
            bits[i] = (int)(x.a[i].real+0.5);
        maintain();
    }
    void us_multo_simu(BigInteger &b)
    {
        vector<int> r;
        r.resize(max(length(),b.length())<<1);
        for(int i = 0; i < length(); i++)
            for(int j = 0; j < b.length(); j++)
                r[i+j] += bits[i] * b.bits[j];
        *(&(this -> bits)) = r;
        maintain();
    }
    void us_subto(BigInteger &b)
    {
        int mlen = length();
        int olen = b.length();
        for(int i = 0; i < mlen; i++)
        {
            int s = bits[i];
            if(i < olen)s -= b.bits[i];
            bits[i] = s;
            if(bits[i] < 0)
            {
                bits[i] += 10;
                bits[i+1] -= 1;
            }
        }
        for(int i = bits.size() - 1; !bits[i] && i >= 1; i--)bits.pop_back();
        if(bits.size() == 1 && bits[0] == 0)sign = 0;
    }
    void us_divto(BigInteger &b)
    {
        if(length() == 1 && bits[0] == 0)return;
        BigInteger L("0");
        L.sign = 0;
        BigInteger R(*this);
        R.sign = 0;
        BigInteger two("2");
        R *= two;
        BigInteger one("1");
        one.sign = 0;
        while(L + one != R)
        {
            BigInteger M = L+R;
            M.divto2();
            BigInteger t = M*b;
            if(t > *this)
            {
                R = M;
            }else if(t < *this)
            {
                L = M;
            }else
            {
                *this = M;
                L = M;
                break;
            }
        }
        *this = L;
    }
public:
    int sign;
    vector<int> bits;
    int length()
    {
        return bits.size();
    }
    void maintain()
    {
        for(int i = 0; i < bits.size(); i++)
        {
            if(i + 1 < bits.size())
                bits[i+1] += bits[i]/10;
            else if(bits[i] > 9)
                bits.push_back(bits[i]/10);
            bits[i] %= 10;
        }
        if(bits.size() == 0)
        {
            bits.push_back(0);
            sign = 0;
        }
        for(int i = bits.size() - 1; !bits[i] && i >= 1; i--)bits.pop_back();
    }

    BigInteger(string &s)
    {
        BigInteger();
        mknum(s);
    }
    BigInteger(const char *s)
    {
        BigInteger();
        mknum(s);
    }
    BigInteger(int n)
    {
        BigInteger();
        char buf[15];
        sprintf(buf, "%d", n);
        mknum(buf);
    }
    BigInteger()
    {
        sign = 0;
        bits.push_back(0);
    }
    BigInteger(const BigInteger& b) 
    {
        copy(b);
    }
    void copy(const BigInteger& b)
    {
        sign = b.sign;
        bits = b.bits;
    }
    bool us_cmp(BigInteger &b)  
    {
        if(length() != b.length())return false;
        int l = length();
        for(int i = 0; i < l; i++)
            if(bits[i] != b.bits[i])
                return false;
        return true;
    }
    bool us_larger(BigInteger &b)
    {
        if(length() > b.length())return true;
        else if(length() < b.length())return false;
        int l = length();
        for(int i = l-1; i >= 0; i--)
            if(bits[i] > b.bits[i])
                return true;
            else if(bits[i] < b.bits[i])
                return false;
        return false;
    }
    bool operator== (BigInteger &o)
    {
        if(sign != o.sign)
            return false;
        return us_cmp(o);
    }
    bool operator!= (BigInteger &o)
    {
        return !(*this == o);
    }
    bool operator> (BigInteger &o)
    {
        if(sign == 0 && o.sign == 1)return true;
        if(sign == 1 && o.sign == 0)return false;
        if(sign == o.sign && sign)return !us_larger(o);
        return us_larger(o);
    }
    bool operator< (BigInteger &o)
    {
        return !(*this == o || *this > o);
    }
    bool operator<= (BigInteger &o)
    {
        return *this < o || *this == o;
    }
    bool operator>= (BigInteger &o)
    {
        return *this > o || *this == o;
    }
    BigInteger& operator+= (BigInteger &o)
    {
        if(!sign && !o.sign)
        {
            us_addto(o);
            sign = 0;
        }
        else if(sign && o.sign)
        {
            us_addto(o);
            sign = 1;
        }
        else if(sign && !o.sign)
        {
            if(o.us_larger(*this))
            {
                BigInteger t(o);
                t.us_subto(*this);
                *this = t;
                sign = 0;
            }else
            {
                us_subto(o);
                sign = 1;
                if(bits.size() == 1 && bits[0] == 0)sign = 0;
            }
        }else if(!sign && o.sign)
        {
            if(us_larger(o))
            {
                us_subto(o);
                sign = 0;
            }else
            {
                BigInteger t(o);
                t.us_subto(*this);
                *this = t;
                sign = 1;
                if(bits.size() == 1 && bits[0] == 0)sign = 0;
            }
        }
        return *this;
    }
    BigInteger operator+ (BigInteger &o)
    {
        BigInteger t(*this);
        t += o;
        return t;
    }
    BigInteger& operator*= (BigInteger &o)
    {
        if(length() + o.length() > 800)
            us_multo(o);                
        else
            us_multo_simu(o);             
        if(sign == o.sign)sign = 0;
        else sign = 1;
        return *this;
    }
    BigInteger operator* (BigInteger &o)
    {
        BigInteger t(*this);
        t *= o;
        return t;
    }
    BigInteger& operator-= (BigInteger &o)
    {
        if(!sign && !o.sign) 
        {
            if(us_larger(o))
            {
                us_subto(o);
                sign = 0;
            }
            else
            {
                BigInteger t(o);
                t.us_subto(*this);
                *this = t;
                sign = 1;
                if(bits.size() == 1 && bits[0] == 0)sign = 0;
            }
        }else if(sign && o.sign)
        {
            if(us_larger(o))
            {
                us_subto(o);
                sign = 1;
                if(bits.size() == 1 && bits[0] == 0)sign = 0;
            }else
            {
                BigInteger t(o);
                t.us_subto(*this);
                *this = t;
                sign = 0;
            }
        }else if(!sign && o.sign)
        {
            us_addto(o);
            sign = 0;
        }else if(sign && !o.sign)
        {
            us_addto(o);
            sign = 1;
        }
        return *this;
    }
    BigInteger operator- (BigInteger &o)
    {
        BigInteger t(*this);
        t -= o;
        return t;
    }
    BigInteger& divto2()
    {
        if(!bits.size())return *this;
        bits[0] >>= 1;
        int i;
        for(i = 1; i < bits.size(); i++)
        {
            if(bits[i] & 1)bits[i-1] += 5;
            bits[i] >>= 1;
        }
        if(bits[i-1] == 0)bits.pop_back();
        return *this;
    }
    BigInteger& operator/= (BigInteger &o)
    {
    	BigInteger zero = "0";
    	if(o == zero) error("Can not devide by zero.");
        us_divto(o);
        if(sign == o.sign)sign = 0;
        else sign = 1;
        return *this;
    }
    BigInteger operator/ (BigInteger &o)
    {
        BigInteger t(*this);
        t /= o;
        return t;
    }
    BigInteger abs()
    {
        BigInteger t(*this);
        t.sign = 0;
        return t;
    }


    BigInteger sqrt()
    {
        BigInteger L("0"), R(*this);
        BigInteger one("1");
        BigInteger m, t;
        while(L + one != R)
        {
            m = L+R;
            m.divto2();
            BigInteger t = m*m;
            if(t == *this)return m;
            else if(t > *this)R = m;
            else L = m;
        }
        return L;
    }

    BigInteger pow(BigInteger &e)
    {
        if(e.sign)return 1;
        BigInteger ans("1");
        BigInteger base(*this);
        BigInteger zero("0");
        BigInteger exp(e);
        while(exp > zero)
        {
            if(exp.bits[0] & 1)
            {
                ans *= base;
            }
            base *= base;
            exp.divto2();
        }
        if(sign && e.bits[0] & 1)ans.sign = 1;
        return ans;
    }

    BigInteger log(BigInteger &base)
    {
        if(sign)return 0;
        if(length() == 1 && bits[0] == 1)return 0;
        if(*this <= base)return 1;
        BigInteger one("1");

        BigInteger r("1");
        BigInteger c("0");
        while(r < *this)
        {
            r *= base;
            c += one;
        }
        if(r != *this)c -= one; 
        return c;
    }
    BigInteger lg()
    {
        BigInteger ten("10");
        return log(ten);
    }

    BigInteger factorial()
    {
        BigInteger r("1");
        BigInteger zero("0");
        BigInteger one("1");
        BigInteger t(*this);
        while(t > zero)
        {
            r *= t;
            t -= one;
        }
        return r;
    }
  
    friend istream& operator>>(istream &is, BigInteger &b)
    {
        string s;
        is >> s;
        b.mknum(s);
        return is;
    }
    friend BigInteger operator<<(BigInteger &a, BigInteger &b)
    {
    	BigInteger two = BigInteger(2);
        BigInteger c = two.pow(b);
        a *= c;
        return a;
    }
    friend ostream& operator<<(ostream &os, BigInteger b)
    {
        if(b.sign)os << '-';
        for(int i = b.bits.size()-1; i >= 0; i--)os << b.bits[i];
        return os;
    }
    

    string to_string()
    {
        int sz = length();
        string s;
        if(sign)
            s.resize(sz+1);
        else
            s.resize(sz);
        int i = 0;
        if(sign)s[i++] = '-';
        for(int j = sz-1; i < sz+sign; i++, j--)
            s[i] = bits[j] + '0';
        return s;
    }    

    BigInteger operator% (BigInteger &o) {
    	BigInteger t(*this);
    	BigInteger one = t / o;
    	one = one * o;
    	return t - one;
	}
	
	string to_str() {
		stringstream ss("");
		ss << (*this);
		return ss.str();
	}
};

string obj_to_str(int ref);

struct Val {
	int ref;
	int obj_ref;
	double num;
	string str;
	string func;
	BigInteger big;
	Range rng;
	int extra;
	char type;
	string to_str() {
		if(type == NUM_TYPE) return tostr(num);
		if(type == STR_TYPE) return str;
		if(type == FUNC_TYPE) return '<' + func + '>';
		if(type == REF_TYPE) return '$' + tostr(ref);
		if(type == BIG_TYPE) return big.to_str();
		if(type == RNG_TYPE) return "[" + tostr(rng.from) + ", " + tostr(rng.to) + "]";
		if(type == OBJ_TYPE) return "<" + obj_to_str(obj_ref / MAX_FIELD_CNT) + ">";
		if(type == UNDEFINED_TYPE) return "undefined";
		return "null"; 
	}
	
	string inner_str() {
		return type_name[type] + to_str();
	}
	
	Val(double a) {
		extra = -1;
		type = NUM_TYPE, num = a;
	}
	Val(string a) {
		extra = -1;
		type = STR_TYPE, str = a;
	}
	Val() {
		extra = -1;
		type = UNDEFINED_TYPE;
	}
	Val(Range r) {
		extra = -1;
		type = RNG_TYPE;
		rng = r;
	}
	
	Val(BigInteger b) {
		extra = -1;
		type = BIG_TYPE;
		big = b;
	}
	
	Val to_big() {
		return BigInteger(num);
	}
	void check() {
		if(!enable_big) return;
		if(num > LIMIT || num < -LIMIT) {
			type = BIG_TYPE;
			string s = tostr_d(num);
			big = BigInteger(s);
		}
	}
};

Val func_val(string f, int obj_id = -1) {
	Val v;
	v.func = f;
	v.obj_ref = obj_id;
	v.type = FUNC_TYPE;;
	return v;
}

Val null_val() {
	Val v;
	v.type = NUL_TYPE;
	return v;
}

Val ref_val(int f) {
	Val v;
	v.type = REF_TYPE;
	v.ref = f;
	return v;
}

Val obj_val(int f) {
	Val v;
	v.type = OBJ_TYPE;
	v.obj_ref = f;
	return v;
}

Val make_num(double n) {
	Val v;
	v.type = NUM_TYPE, v.num = n;
	return v;
}

Val make_big(BigInteger b) {
	Val v;
	v.type = BIG_TYPE, v.big = b;
	return v;
}

Val make_str(string n) {
	Val v;
	v.type = STR_TYPE, v.str = n;
	return v;
}

bool include_char(string, char);

struct Object {
	map<Val, Val> hashmap;
	Val field[MAX_FIELD_CNT];
	char type;
	string class_name;
	Object() { type = 0; }
	~Object() {} 
};

Object obj_heap[MAX_OBJ_CNT];
bool obj_used[MAX_OBJ_CNT];
bool obj_visible[MAX_OBJ_CNT];
priority_queue<int> obj_pool;

int get_next_obj() {
	int x = obj_pool.top(); obj_pool.pop();
	obj_used[x] = true;
	return x * MAX_FIELD_CNT;
}

void free_obj(int ref) {
	obj_used[ref / MAX_FIELD_CNT] = false;
	obj_pool.push(ref / MAX_FIELD_CNT);
}

enum {
	CLS_FIELD, CLS_METHOD,
	CLS_STATIC_FIELD, CLS_STATIC_METHOD
};

bool check_type(Val, string);
string get_type_name(Val);

struct ObjectManager {
	map<string, map<string, int> > field_id;
	map<string, vector<string> > fields;
	map<string, vector<string> > field_type;
	map<string, vector<char> > types;
	map<string, string> superclass;
	
	bool instanceof(string cls, string cls2) {
		string cur = cls;
		while(cur != cls2 && cur != "Object") {
			cur = superclass[cur];
		}
		return cur == cls2;
	}
	
	int get_id(string cls, string field) {
		if(!field_id[cls].count(field)) {
			error("Class '" + cls + "' has no field called '" + field + "'.", "RuntimeError");
			return -1;
		}
		else return field_id[cls][field];
	}
	
	Val getfield(Object object, string field) {
		cout << "getting " << object.class_name << " of " << field << endl;
		return object.field[get_id(object.class_name, field)];
	}
	
	Object setfield(Object & object, string field, Val v) {
		object.field[get_id(object.class_name, field)] = v;
		return object;
	}
	
	void define_class(string cls, string super, vector<string> field_name, vector<char> ftype, vector<string> fval_type, bool permission = false) {
		if(!permission && cls == "Object") {
			error("Shouldn't overwrite Object class.", "RuntimeError");
		}
		int supersize = fields[super].size();
		for(int i = 0; i < fields[super].size(); i++) {
			field_name.push_back(fields[super][i]);
			ftype.push_back(types[super][i]);
			fval_type.push_back(field_type[super][i]);
		}
		for(int i = 0; i < field_name.size(); i++) {
			field_id[cls][field_name[i]] = i;
		}
		types[cls] = ftype;
		field_type[cls] = fval_type;
		fields[cls] = field_name;
		superclass[cls] = super;
	}
	
	void set_objheap(int loc, Val v) {
		int obj_id = loc / MAX_FIELD_CNT, offset = loc % MAX_FIELD_CNT;
		string cls = obj_heap[obj_id].class_name;
		if(!check_type(v, field_type[cls][offset])) {
			error("should use '" + field_type[cls][offset] + "' as field '" + fields[cls][offset] + "' in class '" + cls + "', not '" + get_type_name(v) + "'.", "RuntimeError");
		}
		obj_heap[obj_id].field[offset] = v;
	}
	
	int get_field_cnt(string cls) {
		return fields[cls].size();
	} 
	
	string super(string cls) {
		return superclass[cls];
	}
	
	bool has_class(string cls) {
		return fields.count(cls);
	}
	
	Object construct(string cls, Val * args, int argc, int id) {
		Object obj;
		obj.class_name = cls;
		if(argc > fields[cls].size()) return obj;
		
		int pos = 0;
		for(int i = 0; i < types[cls].size(); i++) {
			if(types[cls][i] == CLS_FIELD) {
				if(pos == argc) continue;
				if(!check_type(args[pos], field_type[cls][i])) {
					error("should use '" + field_type[cls][i] + "' as field '" + fields[cls][i] + "' in class '" + cls + "', not '" + get_type_name(args[pos]) + "'.", "RuntimeError");
				}
				obj.field[field_id[cls][fields[cls][i]]] = args[pos];
				pos++;
			}
			else {
				obj.field[field_id[cls][fields[cls][i]]] = func_val(cls + "." + fields[cls][i], id);
			}
		}
		if(cls == "map") obj.type = 'm';
		setfield(obj, "class_name", cls);
		setfield(obj, "super_name", superclass[cls]);
		return obj;
	}
};

ObjectManager manager;

string obj_to_str(int ref) {
	string cls = obj_heap[ref].class_name;
	string s = cls;
	string f;
	return s + " @ " + tostr(ref);
} 

const int MAX_FILE_CNT = 512;
struct FileManager {
	FILE * file_ptrs[MAX_FILE_CNT];
	int size;
	FileManager() {
		size = 0;
	}
	int file_open(string file, string mode = "r") {
		bool write = false;
		if(include_char(mode, 'w')) write = true;
		FILE * ptr = fopen(file.c_str(), mode.c_str());
		if(ptr == NULL) {
			if(write) {
				ofstream fcout(file.c_str());
				fcout.close();
			}
			else error("file doesn't exist: " + file);
		}
		file_ptrs[size++] = ptr;
		return size - 1;
	}
	
	void check_handle(int handle) {
		if(handle < 0 || handle >= size) error("invalid handle: " + tostr(handle), "RuntimeError");
	}
	
	void file_close(int handle) {
		check_handle(handle);
		fclose(file_ptrs[handle]);
	}
	
	bool file_eof(int handle) {
		check_handle(handle);
		return feof(file_ptrs[handle]);
	}
	
	char fbuffer[2048];
	string fread_string(int handle) {
		check_handle(handle);
		fscanf(file_ptrs[handle], "%s", fbuffer);
		return fbuffer;
	}
	string fread_line(int handle) {
		check_handle(handle);
		string result = "";
		char c;
		while(isspace(c) && !feof(file_ptrs[handle])) c = fgetc(file_ptrs[handle]);
		c = fgetc(file_ptrs[handle]);
		while(c != '\n' && !feof(file_ptrs[handle])) {
			result += c;
			c = fgetc(file_ptrs[handle]);
		}
		return result;
	}
	void fwrite_string(int handle, string text) {
		check_handle(handle);
		fprintf(file_ptrs[handle], "%s", text.c_str());
		fflush(file_ptrs[handle]);
	}
	double fread_number(int handle) {
		check_handle(handle);
		double x;
		return atof(fread_string(handle).c_str());
	}
	void fwrite_number(int handle, double x) {
		check_handle(handle);
		fprintf(file_ptrs[handle], "%.14g", x);
		fflush(file_ptrs[handle]);
	}
};

FileManager file_manager;

#define DEFINE_OP(x) \
inline Val operator x (Val a, Val b) {\
	Val c;\
	if(a.type == NUM_TYPE && b.type == NUM_TYPE) {\
		c = make_num(a.num x b.num);\
	}\
	else if(a.type == NUM_TYPE && b.type == BIG_TYPE) {\
		BigInteger tmp = a.to_big().big;\
		c = make_big(b.big x tmp);\
	}\
	else if(b.type == NUM_TYPE && a.type == BIG_TYPE) {\
		BigInteger tmp = b.to_big().big;\
		c = make_big(a.big x tmp);\
	}\
	else if(b.type == BIG_TYPE && a.type == BIG_TYPE) {\
		c = a.big x b.big;\
	}\
	else {\
		error("cannot operate calculation '" + get_type_name(a) + "'" + #x + "'" + get_type_name(b) + "'");\
	}\
	c.check();\
	return c;\
}

#define DEFINE_IOP(x) \
inline Val operator x (Val a, Val b) {\
	Val c;\
	if(a.type == NUM_TYPE && b.type == NUM_TYPE) {\
		c = make_num((int)a.num x (int)b.num);\
	}\
	else {\
		error("cannot operate calculation '" + get_type_name(a) + "'" + #x + "'" + get_type_name(b) + "'");\
	}\
	return c;\
}

inline Val operator + (Val a, Val b) {
	Val c;
	if(a.type == STR_TYPE || b.type == STR_TYPE) {
		c = make_str(a.to_str() + b.to_str());
	}
	else if(a.type == NUM_TYPE && b.type == NUM_TYPE) {
		c = make_num(a.num + b.num);
	}
	else if(a.type == NUM_TYPE && b.type == BIG_TYPE) {
		BigInteger tmp = a.to_big().big;
		c = make_big(b.big + tmp);
	}
	else if(b.type == NUM_TYPE && a.type == BIG_TYPE) {
		BigInteger tmp = b.to_big().big;
		c = make_big(a.big + tmp);
	}
	else if(b.type == BIG_TYPE && a.type == BIG_TYPE) {
		c = a.big + b.big;
	}
	else {
		error("cannot operate calculation '" + get_type_name(a) + "' + '" + get_type_name(b) + "'");
	}
	return c;
}

inline Val operator == (Val a, Val b) {
	return a.to_str() == b.to_str();
}

inline Val operator != (Val a, Val b) {
	return a.to_str() != b.to_str();
}

BigInteger ZERO("0");

inline Val operator ! (Val a) {
	if(a.type == STR_TYPE) return a.str == "";
	if(a.type == NUM_TYPE) return a.num == 0;
	if(a.type == BIG_TYPE) return a.big == ZERO;
	return false;
}

inline Val operator && (Val a, Val b) {
	return !((!a).num || (!b).num);
}

inline Val operator || (Val a, Val b) {
	return !((!a) && (!b));
}

inline Val operator % (Val a, Val b) {
	Val c;
	if(a.type == NUM_TYPE && b.type == NUM_TYPE) {
		c = fmod(a.num, b.num);
	}
	else {
		error("cannot operate calculation '" + get_type_name(a) + "' % '" + get_type_name(b) + "'");
	}
	return c;
}

inline Val operator ~ (Val a) {
	if(a.type == NUM_TYPE) {
		return ~(int)a.num;
	}
	error("cannot operate calculation ~ '" + get_type_name(a) + "'");
}

DEFINE_OP(-) 
DEFINE_OP(*)
DEFINE_OP(/) 

DEFINE_OP(>=) 
DEFINE_OP(<=) 
DEFINE_OP(>) 
inline bool compare(Val a, Val b) {
	if(a.type == NUM_TYPE && b.type == NUM_TYPE) {
		return a.num < b.num;
	}
	else if(a.type == NUM_TYPE && b.type == BIG_TYPE) {
		BigInteger tmp = a.to_big().big;
		return b.big < tmp;
	}
	else if(b.type == NUM_TYPE && a.type == BIG_TYPE) {
		BigInteger tmp = b.to_big().big;
		return a.big < tmp;
	}
	else if(b.type == BIG_TYPE && a.type == BIG_TYPE) {
		return a.big < b.big;
	}
	else {
		error("cannot operate calculation '" + get_type_name(a) + "' < '" + get_type_name(b) + "'");
	}
	return false;
}

inline bool operator < (Val a, Val b) {
	return a.inner_str() < b.inner_str();
}

DEFINE_IOP(>>) 
DEFINE_IOP(<<) 
DEFINE_IOP(&) 
DEFINE_IOP(|)
DEFINE_IOP(^) 

Val cpool[MAX_CPOOL];
int cpool_pos;

int pos;
int start[MAX_FUNC_CNT];
int callcnt;

map<Val, int> has_consted;

int addconst(const Val& x) {
	if(has_consted.count(x)) return has_consted[x];
	cpool[cpool_pos++] = x;
	has_consted[x] = cpool_pos - 1;
	return cpool_pos - 1;
}

Val runtime_stack[100000 + 5];
struct RunStack {
	int t;
	Val pop() {
		if(t == 0) error("Stack underflow.");
		Val r = runtime_stack[t - 1];
		t--;
		return r;
	}
	void push(Val val) {
		runtime_stack[t++] = val;
	}
	Val top() {
		return runtime_stack[t - 1];
	}
	bool empty() {
		return t == 0;
	}
	RunStack() {
		t = 0;
	}
	RunStack(int offset) {
		t = offset;
	}
};

struct Register {
	Val r[MAX_REG_CNT];
};

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
OPCODE MKCLS = 0xdf;
OPCODE THIS = 0xe0;
OPCODE PTYPE = 0xe1;
OPCODE RTYPE = 0xe2;
OPCODE END = 0xff;

#define OBJCODE const unsigned char

OBJCODE NEW_CLASS = 'c';
OBJCODE END_CLASS = 'C';
OBJCODE USED_CLASS = 'u';
OBJCODE NO_SUPER_CLASS = 'n';

OBJCODE NEW_OBJ = 'o';
OBJCODE END_OBJ = 'O';
OBJCODE USED_OBJ = 'U';
OBJCODE REF_BEGIN = 'r';
OBJCODE REF_END = 'R';
OBJCODE UNDEF_SYMBOL = '_';

const int ARR_GROW = 1024 * 4;
Val get_heap(int);
Val make_array(vector<Val>);


struct Serializer {
	set<string> class_sr;
	map<int, int> obj_sr;
	map<int, int> obj_served;
	vector<unsigned char> out;
	int ip;
	int obj_cnt;
	
	Serializer() {
		init();
	}
	
	void init() {
		obj_cnt = 0, out.clear();
		obj_sr.clear(), class_sr.clear();
		ip = 0;
	}
	
	void add(unsigned char c) {
		printf("%2d(%c)\t", (int)c, isprint(c) ? c : ' ');
		out.push_back(c);
	}
	void add_int(int x) {
		int h = x / 256;
		int l = x % 256;
		add(h);
		add(l);
	}
	void add_long_int(int x) {
		int h = x / (256 * 256);
		int l = x % (256 * 256);
		add_int(h);
		add_int(l);
	}
	void add_str(string s) {
		add_int(s.length());
		for(int i = 0; i < s.length(); i++) add(s[i]);
	}
	
	int current() {
		return out[ip];
	}
	int read() {
		if(ip > out.size() - 1) throw "Uncomplete serialization";
		return out[ip++];
	}
	int read_int() {
		return read() * 256 + read();
	}
	int read_long_int() {
		return read_int() * 256 + 256 + read_int();
	}
	string read_string() {
		int siz = read_int();
		string s = "";
		for(int i = 0; i < siz; i++) s += read();
		return s;
	} 
	
	Val unserialize() {
		unsigned char uc;
		switch(read()) {
			case 'A': {
				string s = read_string();
				return s;
				break;
			}
			case 'o': {
				return unserialize_obj();
				break;
			}
			case REF_BEGIN: {
				vector<Val> vals;
				Val v;
				vals.resize(ARR_GROW);
				while(current() != REF_END) {
					int loc = read_int();
					v = unserialize();
					vals[loc] = v;
				}
				read(); // REF_END
				return make_array(vals);
				break;
			}
			case '0': {
				return atof(read_string().c_str());
				break;
			}
			case '9': {
				string s = read_string();
				return BigInteger(s);
				break;
			}
			case UNDEF_SYMBOL: {
				return Val();
				break;
			}
			default: {
				throw "Invalid byte " + tostr(uc);
				break;
			}
		}
	}
	
	void serialize(Val v) {
		switch(v.type) {
			case STR_TYPE: {
				add('A');
				add_str(v.str);
				break;
			}
			case OBJ_TYPE: {
				add('o');
				serialize_obj(v.obj_ref / MAX_FIELD_CNT);
				break;
			}
			case REF_TYPE: {
				add(REF_BEGIN);
				for(int i = 0; i < ARR_GROW; i++) {
					if(get_heap(i + v.ref).type != UNDEFINED_TYPE) {
						add_int(i);
						serialize(get_heap(i + v.ref));
					}
				}
				add(REF_END);
				break;
			}
			case NUM_TYPE: {
				add('0');
				add_str(v.to_str());
				break;
			}
			case BIG_TYPE: {
				add('9');
				add_str(v.to_str());
				break;
			}
			case UNDEFINED_TYPE: {
				add(UNDEF_SYMBOL);
				break;
			}
		}
	}
	Val unserialize_obj() {
		unsigned char uc;
		switch(read()) {
			case USED_OBJ: {
				return obj_served[read_long_int()];
				break;
			}
			case 'm': {
				int loc = read_long_int();
				map<Val, Val> hashmap;
				while(read() != 'M') {
					hashmap[unserialize()] = unserialize();
					read(); // '>'
				}
				int id = get_next_obj();
				obj_heap[id / MAX_FIELD_CNT] = manager.construct("map", NULL, 0, id);
				obj_served[loc] = id;
				obj_heap[id / MAX_FIELD_CNT].hashmap = hashmap;
				return obj_val(id);
				break;
			}
			case NEW_OBJ: {
				int loc = read_long_int();
				string cls_name = unserialize_class();
				int siz = read_int();
				Val * vals = new Val[siz];
				for(int i = 0; i < siz; i++) {
					vals[i] = unserialize(); 
				}
				read(); // END_OBJ
				int id = get_next_obj();
				obj_heap[id / MAX_FIELD_CNT] = manager.construct(cls_name, vals, siz, id);
				obj_served[loc] = id;
				return obj_val(id);
				break;
			}
			default: {
				throw "Invalid byte " + tostr(uc);
				break;
			}
		}
	}
	void serialize_obj(int id) {
		if(obj_sr.count(id)) {
			add(USED_OBJ);
			add_long_int(obj_sr[id]);
			return;
		}
		if(obj_heap[id].type == 'm') {
			add('m');
			obj_sr[id] = obj_cnt, obj_cnt++;
			add_long_int(obj_cnt);
			for(map<Val, Val>::iterator it = obj_heap[id].hashmap.begin(); it != obj_heap[id].hashmap.end(); it++) {
				add('<');
				serialize(it -> first);
				serialize(it -> second);
				add('>');
			}
			add('M');
			return; 
		}
		add(NEW_OBJ);
		obj_sr[id] = obj_cnt, obj_cnt++;
		add_long_int(obj_cnt);
		serialize_class(obj_heap[id].class_name);
		add_int(manager.fields[obj_heap[id].class_name].size());
		for(int i = 0; i < manager.fields[obj_heap[id].class_name].size(); i++) {
			serialize(obj_heap[id].field[i]);
		}
		add(END_OBJ);
	}
	string unserialize_class() {
		unsigned char uc;
		switch(read()) {
			case USED_CLASS: {
				return read_string();
				break;
			}
			case 'A' :{
				return "string";
				break;
			}
			case '?': {
				return "any";
				break;
			}
			case '0': {
				return "number";
				break;
			}
			case '[': {
				return "reference";
				break;
			}
			case NEW_CLASS: {
				stringstream ret("");
				string super_class = "Object";
				string cls = read_string();
				int siz = read_int();
				vector<string> field_names, field_type;
				for(int i = 0; i < siz; i++) {
					string r = unserialize_class();
					field_type.push_back(r);
					string field_name = read_string();
					field_names.push_back(field_name);
				}
				read(); // END_CLASS
				if(current() == NO_SUPER_CLASS) {
					read();
				}
				else {
					super_class = unserialize_class();
				}
				manager.define_class(cls, super_class, field_names, vector<char>(siz), field_type, true);
				return cls;
				break;
			}
			default: {
				throw "Invalid byte " + tostr(uc);
				break;
			}
		}
	}
	void serialize_class(string cls) {
		if(class_sr.count(cls)) {
			add(USED_CLASS);
			add_str(cls);
		}
		else {
			if(cls == "string") {
				add('A');
				return;
			}
			else if(cls == "any") {
				add('?');
				return;
			}
			else if(cls == "number") {
				add('0');
				return;
			}
			else if(cls == "reference") {
				add('[');
				return;
			}
			string super = manager.super(cls);
			add(NEW_CLASS);
			add_str(cls);
			add_int(manager.fields[cls].size());
			for(int i = 0; i < manager.fields[cls].size(); i++) {
				serialize_class(manager.field_type[cls][i]);
				add_str(manager.fields[cls][i]);
			}
			add(END_CLASS);
			if(super != "Object") {
				serialize_class(super);
			} 
			else add(NO_SUPER_CLASS);
			class_sr.insert(cls);
		}
	} 
};

Serializer serializer;

string opname[256];

void init() {
type_name[NUM_TYPE] = "number";
type_name[STR_TYPE] = "string";
type_name[NUL_TYPE] = "null";
type_name[RNG_TYPE] = "range";
type_name[BIG_TYPE] = "big_number";
type_name[REF_TYPE] = "reference";
type_name[OBJ_TYPE] = "Object";
type_name[FUNC_TYPE] = "function";
type_name[UNDEFINED_TYPE] = "undefined";
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
opname[MKCLS] = "MKCLS";
opname[THIS] = "THIS";
opname[PTYPE] = "PTYPE";
opname[RTYPE] = "RTYPE";
opname[END] = "END";
}

int itop;
Register reg;

const int MAX_ARR_CNT = 256;

Val heap[ARR_GROW * MAX_ARR_CNT];
bool used_loc[MAX_ARR_CNT];

void gc();

const double GC_START = 0.8;
const int MAX_LOCAL_CNT = 256;
inline void free_used(int x) { used_loc[x] = false; }
inline void mark_used(int x) { used_loc[x] = true; }
inline int get_free() {
	int ret = -1;
	int sum = 0;
	for(int i = 1; i < MAX_ARR_CNT; i++) {
		if(!used_loc[i]) {
			if(ret == -1) ret = i;
		}
		else sum++;
	}
	if((double) sum / MAX_ARR_CNT >= GC_START) gc();
	if(ret == -1) error("out of memory");
	return ret;
}

Val get_sub(Val, Val);

Val get_heap(int loc) {
	if(loc < 0 || loc >= MAX_HEAP_SIZE) error("heap index '" + tostr(loc) + "' out of bound", "RuntimeError");
	if(loc < ARR_GROW) return Val((string)"" + (char)loc);
	return heap[loc];
}

void set_heap(int loc, Val v) {
	if(loc < ARR_GROW || loc >= MAX_HEAP_SIZE) error("heap index '" + tostr(loc) + "'out of bound", "RuntimeError");
	heap[loc] = v;
}

string get_name(int);
void local_store(int pos, Val v) {
	reg.r[itop * MAX_LOCAL_CNT + pos] = v;
}

set<int> protect;

inline bool is_protected(int p) {
	return protect.find(p) != protect.end();
}
void store(int pos, Val v) {
	if(is_protected(pos)) error("assigning '" + get_name(pos) + "' which is a constant", "RuntimeError");
	Val v2;
	for(int i = itop; i >= 1; i--) {
		v2 = reg.r[i * MAX_LOCAL_CNT + pos];
		if(v2.type != UNDEFINED_TYPE) {
			reg.r[i * MAX_LOCAL_CNT + pos] = v;
			return;
		}
	}
	reg.r[itop * MAX_LOCAL_CNT + pos] = v;
}

bool not_check = false;
string get_name(int);
Val load(int pos) {
	if(!not_check && pos > MAX_LOCAL_CNT) error("Loading " + get_name(pos) + " that doesn't initialized.", "NameError");
	for(int i = itop; i >= 1; i--) {
		Val v = reg.r[i * MAX_LOCAL_CNT + pos];
		if(v.type != UNDEFINED_TYPE) return v;
	}
	error("Loading " + get_name(pos) + " that doesn't initialized", "NameError");
}

inline void release() {
	int tmp = (itop) * MAX_LOCAL_CNT;
	for(int i = 0; i < MAX_LOCAL_CNT; i++) {
		reg.r[tmp + i].type = UNDEFINED_TYPE;
	}
	itop--;
}

inline void new_frame() {
	itop++;
}

bool visible[MAX_ARR_CNT];

void mark_visible(Val v) {
	if(v.type != REF_TYPE && v.type != OBJ_TYPE) return;
	else {
		Val v2;
		if(v.type == REF_TYPE) {
			if(!visible[v.ref]) {
				visible[v.ref / ARR_GROW] = true;
				for(int i = 0; i < ARR_GROW; i++) {
					v2 = get_heap(get_sub(v, i).ref);
					if(v2.type != UNDEFINED_TYPE) mark_visible(v2);
				}
			}
		}
		else {
			if(!obj_visible[v.obj_ref / MAX_FIELD_CNT]) {
				obj_visible[v.obj_ref / MAX_FIELD_CNT] = true;
				int field_size = manager.get_field_cnt(obj_heap[v.obj_ref / MAX_FIELD_CNT].class_name);
				for(int i = 0; i < field_size; i++) {
					v2 = obj_heap[v.obj_ref / MAX_FIELD_CNT].field[i];
					if(v2.type != UNDEFINED_TYPE) mark_visible(v2);
				}
			}
		}
	}
}

void check_visible() {
	memset(visible, false, sizeof(visible));
	memset(obj_visible, false, sizeof(obj_visible));
	for(int i = itop; i >= 0; i--) {
		for(int pos = 0; pos < MAX_LOCAL_CNT; pos++) {
			Val v = reg.r[i * MAX_LOCAL_CNT + pos]; 
			if(v.type == REF_TYPE || v.type == OBJ_TYPE) {
				mark_visible(v);
			}
		}
	}
}

void gc() {
	check_visible();
	for(int i = 0; i < MAX_ARR_CNT; i++) {
		if(!visible[i] && used_loc[i]) {
			free_used(i);
		}
	}
	for(int i = 0; i < MAX_OBJ_CNT; i++) {
		if(!obj_visible[i] && obj_used[i]) {
			free_obj(i);
		}
	}
}

int hash_cnt;
map<string, int> hash_value;
int get_hash(string s) {
    if(hash_value.find(s) == hash_value.end()) hash_cnt++, hash_value[s] = hash_cnt;
    return hash_value[s] + ARR_GROW * 3 / 4;
}

Val clone(Val);
int get_len(Val);
Val make_array(vector<Val>);
Val range_sub(Val, Val);
Val get_sub(Val a, Val b) {
	string str;
	if(a.type == STR_TYPE) {
		if(b.type == RNG_TYPE) {
			return a.str.substr(b.rng.from, b.rng.to - b.rng.from);
		}
		else if(b.type == NUM_TYPE) return (string)"" + a.str[(int)b.num];
		else error("should use 'range' or 'number' as subscript of 'string', not '" + get_type_name(b) + "'", "RuntimeError");
	}
	else if(a.type == REF_TYPE) {
		if(b.type != NUM_TYPE) error("should use 'number' as subscript of 'reference', not '" + get_type_name(b) + "'", "RuntimeError");
		if(b.type == RNG_TYPE) return range_sub(a, b);
		return ref_val(a.ref + b.num);
	} 
	else if(a.type == OBJ_TYPE) {
		if(obj_heap[a.obj_ref / MAX_FIELD_CNT].type == 'm') { // Map
			b.extra = a.obj_ref;
			return b;
		}
		if(b.type != STR_TYPE) error("'" + get_type_name(b) + "' is not a valid member of '" + get_type_name(a) + "'", "RuntimeError");
		return obj_val(a.obj_ref + manager.get_id(obj_heap[a.obj_ref / MAX_FIELD_CNT].class_name, b.str));
	}
	else if(a.type == RNG_TYPE) {
		if(b.type != NUM_TYPE) error("should use 'number' as subscript of 'range', not '" + get_type_name(b) + "'", "RuntimeError");
		if(b.type == RNG_TYPE) return range_sub(a, b);
		return a.rng.from + b.num;
	}
	else error("operation '" + get_type_name(a) + "[" + get_type_name(b) + "]' is invalid", "RuntimeError");
}

Val make_array(vector<Val> arr) {
	int index = get_free();
	mark_used(index);
	int offset = index * ARR_GROW;
	for(int i = 0; i < arr.size(); i++) {
		set_heap(i + offset, arr[i]);
	}
	return ref_val(offset);
}

string get_type_name(Val v) {
	switch(v.type) {
		case OBJ_TYPE: {
			return obj_heap[v.obj_ref / MAX_FIELD_CNT].class_name;
			break;
		}
		default: return type_name[v.type];
	}
}

bool check_type(Val v, string type) {
	if(type == "any") return true;
	switch(v.type) {
		case OBJ_TYPE: {
			return manager.instanceof(obj_heap[v.obj_ref / MAX_FIELD_CNT].class_name, type);
			break;
		}
		default: return type_name[v.type] == type;
	}
}

int get_id(string);

int byte_block_len[MAX_FUNC_CNT];
unsigned char byte_map[MAX_FUNC_CNT][10000 + 5];
int func_cnt;
map<string, int> func_map;
int get_func_id(string func) {
	return get_id(func);
}
map<int, vector<int> > reg_map;
set<string> builtin;
inline bool is_builtin(string func) {
	return(builtin.find(func) != builtin.end()) || manager.has_class(func);
}

int make_obj(Object);

Val clone(Val a) {
	if(a.type == REF_TYPE) {
		vector<Val> vals;
		for(int i = 0; i < ARR_GROW; i++) {
			vals.push_back(clone(get_heap(get_sub(a, i).ref)));
		}
		return make_array(vals);
	}
	else if(a.type == OBJ_TYPE) {
		return obj_val(make_obj(obj_heap[a.obj_ref / MAX_FIELD_CNT]));
	}
	return a;
}

Val range_sub(Val a, Val b) {
	if(b.type == RNG_TYPE) {
		int from = b.rng.from;
		int to;
		if(b.rng.to == -1) to = get_len(a);
		else to = b.rng.to;
		if(a.type == STR_TYPE) {
			if(to > a.str.length() || from < 0) error("string index '" + b.to_str() + "' out of bound", "RuntimeError");
			return a.str.substr(from, to - from); 
		}
		else if(a.type == REF_TYPE) {
			if(to >= ARR_GROW || from < 0) error("reference index '" + b.to_str() + "' out of bound", "RuntimeError");
			vector<Val> vals;
			for(int i = from; i < to; i++) vals.push_back(clone(get_heap(get_sub(a, i).ref)));
			Val ret = make_array(vals);
			cout << ret.to_str() << endl;
			return ret;
		}
		else if(a.type == RNG_TYPE) {
			vector<Val> vals;
			for(int i = from; i < to; i++) vals.push_back(a.rng.from + i);
			Val ret = make_array(vals);
			return ret;
		}
		else error("operation '" + get_type_name(a) + "[" + get_type_name(b) + "]' is invalid", "RuntimeError");
	}
	return null_val();
}

int get_len(Val p) {
	int len = 0;
	if(p.type == STR_TYPE) {
		len = p.str.length();
		return len;
	}
	else if(p.type == REF_TYPE) {
		for(int i = 0; i < ARR_GROW; i++) {
			if(heap[(int)p.ref + i].type == UNDEFINED_TYPE) break;
			len++;
		}
		return len;
	}
	else if(p.type == RNG_TYPE) {
		return p.rng.to - p.rng.from;
	}
	else error("type '" + get_type_name(p) + "' has no 'len' property", "RuntimeError");
}

int make_obj(Object obj) {
	int id = get_next_obj();
	obj_heap[id / MAX_FIELD_CNT] = obj;
	return id;
}

void map_keys(map<Val, Val> & hashmap, vector<Val> & vals) {
	vals.clear();
	for(map<Val, Val>::iterator it = hashmap.begin(); it != hashmap.end(); it++) vals.push_back((*it).first);
}

void map_values(map<Val, Val> & hashmap, vector<Val> & vals) {
	vals.clear();
	for(map<Val, Val>::iterator it = hashmap.begin(); it != hashmap.end(); it++) vals.push_back((*it).second);
}
string to_hex(double);
double to_hex(string);

Val call_builtin(string func, Val * params, int pcnt, int obj_id) {
	#define CNT(a) if(pcnt != a) error("number of arguments of builtin function '" + func + "' should be " + tostr(a) + ", given " + tostr(pcnt), "RuntimeError")
	#define TYPE(a, t) if(pcnt <= a || params[a].type != t##_TYPE) error("arguments of builtin function '" + func + "' should be '" + type_name[t##_TYPE] + "', not '" + type_name[params[a].type] + "'", "RuntimeError")
	#define CHECK_MATH(x, m) \
	else if(func == (string)"math_" + x) {\
		TYPE(0, NUM);\
		return m(params[0].num);\
	}
	#define CHECK_FILE(x, m, t) \
	else if(func == (string)"file_" + x) {\
		TYPE(0, t);\
		return file_manager.m(params[0].num);\
	}
	
	if(manager.has_class(func)) {
		int id = get_next_obj();
		obj_heap[id / MAX_FIELD_CNT] = manager.construct(func, params, pcnt, id);
		return obj_val(id);
	}
	else if(func == "instanceof") {
		CNT(2);
		TYPE(1, STR);
		return check_type(params[0], params[1].str);
	}
	else if(func == "exit") {
		CNT(1);
		TYPE(0, NUM);
		exit(params[0].num);
	}
	else if(func == "print") {
		for(int i = 0; i < pcnt; i++) cout << params[i].to_str() << ' ';
		cout << endl;
		return pcnt;
	}
	else if(func == "len") {
		CNT(1);
		return get_len(params[0]);
	}
	else if(func == "sub") {
		CNT(2);
		return range_sub(params[0], params[1]);
	}
	else if(func == "time") {
		return time(NULL);
	}
	else if(func == "clock") {
		return clock();
	}
	else if(func == "system") {
		CNT(1);
		TYPE(0, STR);
		return system(params[0].str.c_str());
	}
	else if(func == "rand") {
		CNT(0);
		return rand();
	}
	else if(func == "srand") {
		CNT(1);
		TYPE(0, NUM);
		srand(params[0].num);
		return null_val();
	}
	else  if(func == "clone") {
		CNT(1);
		return clone(params[0]);
	}
	else if(func == "memset") {
		TYPE(0, REF);
		int ref;
		Val nulval = null_val();
		if(pcnt == 1) {
			for(int i = 0; i < ARR_GROW; i++) {
				ref = get_sub(params[0], i).ref;
				set_heap(ref, nulval);
			}
			return ARR_GROW;
		}
		else if(pcnt == 2) {
			for(int i = 0; i < ARR_GROW; i++) {
				ref = get_sub(params[0], i).ref;
				set_heap(ref, params[1]);
			}
			return ARR_GROW;
		}
		else if(pcnt == 3) {
			TYPE(2, NUM);
			int lim = params[2].num;
			for(int i = 0; i < lim; i++) {
				ref = get_sub(params[0], i).ref;
				set_heap(ref, params[1]);
			}
			return lim;
		}
		else if(pcnt == 4) {
			TYPE(2, NUM);
			TYPE(3, NUM);
			int lim = params[2].num;
			int len = params[3].num;
			for(int i = len; i < lim + len; i++) {
				ref = get_sub(params[0], i).ref;
				set_heap(ref, params[1]);
			}
			return len;
		}
		error("number of arguments of builtin function '" + func + "' should between 1 and 4, given " + tostr(pcnt), "RuntimeError");
	}
	CHECK_MATH("sin", sin)
	CHECK_MATH("cos", cos)
	CHECK_MATH("tan", tan)
	CHECK_MATH("asin", asin)
	CHECK_MATH("acos", acos)
	CHECK_MATH("atan", atan)
	CHECK_MATH("sqrt", sqrt)
	CHECK_MATH("floor", floor)
	CHECK_MATH("ceil", ceil)
	CHECK_MATH("log", log)
	CHECK_MATH("log10", log10)
	CHECK_MATH("exp", exp)
	CHECK_MATH("abs", fabs)
	else if(func == "file_open") {
		TYPE(0, STR);
		if(pcnt == 1) return file_manager.file_open(params[0].str);
		else if(pcnt == 2) {
			TYPE(1, STR);
			return file_manager.file_open(params[0].str, params[1].str);
		}
		error("number of arguments of builtin function '" + func + "' should between 1 and 2, given " + tostr(pcnt), "RuntimeError");
	}
	else if(func == "file_close") {
		CNT(1);
		TYPE(0, NUM);
		file_manager.file_close(params[0].num);
		return null_val();
	}
	else if(func == "file_read_string") {
		CNT(1);
		TYPE(0, NUM);
		return file_manager.fread_string(params[0].num);
	}
	else if(func == "file_read_number") {
		CNT(1);
		TYPE(0, NUM);
		return file_manager.fread_number(params[0].num);
	}
	else if(func == "file_read_line") {
		CNT(1);
		TYPE(0, NUM);
		return file_manager.fread_line(params[0].num);
	}
	else if(func == "read_string") {
		if(pcnt == 1) cout << params[0].to_str();
		else if(pcnt) error("number of arguments of builtin function '" + func + "' should between 0 and 1, given " + tostr(pcnt), "RuntimeError");
		string s;
		cin >> s;
		return s;
	}
	else if(func == "read_number") {
		if(pcnt == 1) cout << params[0].to_str();
		else if(pcnt) error("number of arguments of builtin function '" + func + "' should between 0 and 1, given " + tostr(pcnt), "RuntimeError");
		double x;
		cin >> x;
		return x;
	}
	else if(func == "read_line") {
		if(pcnt == 1) cout << params[0].to_str();
		else if(pcnt) error("number of arguments of builtin function '" + func + "' should between 0 and 1, given " + tostr(pcnt), "RuntimeError");
		string s;
		getline(cin, s);
		return s;
	}
	else if(func == "file_write_string") {
		CNT(2);
		TYPE(0, NUM);
		TYPE(1, STR);
		file_manager.fwrite_string(params[0].num, params[1].str);
		return null_val();
	}
	else if(func == "file_write_number") {
		CNT(2);
		TYPE(0, NUM);
		TYPE(1, NUM);
		file_manager.fwrite_number(params[0].num, params[1].num);
		return null_val();
	}
	else if(func == "file_eof") {
		CNT(1);
		TYPE(0, NUM);
		return file_manager.file_eof(params[0].num);
	}
	else if(func == "str2ascii") {
		CNT(1);
		TYPE(0, STR);
		if(params[0].str.length() == 1) return params[0].str[0];
		vector<Val> vals;
		for(int i = 0; i < params[0].str.length(); i++) vals.push_back(params[0].str[i]);
		return make_array(vals);
	}
	else if(func == "ascii2str") {
		if(pcnt <= 0) error("number of arguments of builtin function '" + func + "' should bigger than 0, given " + tostr(pcnt), "RuntimeError");
		if(params[0].type == REF_TYPE) {
			CNT(1);
			string str = "";
			int len = get_len(params[0]);
			for(int i = 0; i < len; i++) {
				Val v = get_heap(get_sub(params[0], i).ref);
				if(v.type == NUM_TYPE) str += (char)v.num;
				else str += "NaN";
			}
			return str;
		}
		else {
			string str = "";
			for(int i = 0; i < pcnt; i++) {
				Val v = params[i];
				if(v.type == NUM_TYPE) str += (char)v.num;
				else str += "NaN";
			}
			return str;
		}
	}
	else if(func == "big") {
		CNT(1);
		TYPE(0, STR);
		return BigInteger(params[0].str);
	}
	else if(func == "num") {
		CNT(1);
		TYPE(0, STR);
		return atof(params[0].str.c_str());
	}
	else if(func == "clear_map") {
		CNT(1);
		TYPE(0, OBJ);
		obj_heap[params[0].obj_ref / MAX_FIELD_CNT].hashmap.clear();
		return null_val();
	}
	else if(func == "keys") {
		CNT(1);
		TYPE(0, OBJ);
		vector<Val> vals;
		map_keys(obj_heap[params[0].obj_ref / MAX_FIELD_CNT].hashmap, vals);
		return make_array(vals);
	}
	else if(func == "values") {
		CNT(1);
		TYPE(0, OBJ);
		vector<Val> vals;
		map_values(obj_heap[params[0].obj_ref / MAX_FIELD_CNT].hashmap, vals);
		return make_array(vals);
	}
	else if(func == "serialize") {
		CNT(1);
		serializer.init();
		serializer.serialize(params[0]);
		vector<Val> ret;
		for(int i = 0; i < serializer.out.size(); i++) ret.push_back(serializer.out[i]);
		return make_array(ret);
	}
	else if(func == "unserialize") {
		CNT(1);
		TYPE(0, REF);
		vector<unsigned char> vs;
		Val v;
		for(int i = 0; i < get_len(params[0]); i++) {
			v = get_heap(params[0].ref + i);
			if(v.type == NUM_TYPE && (v.num >= 0x00 && v.num <= 0xff)) {
				vs.push_back(v.num);
			}
			else return (string)"Invalid code " + v.to_str(); 
		}
		serializer.init();
		serializer.out = vs;
		try {
			return serializer.unserialize();
		} 
		catch(string s) {
			return s;
		}
	}
	else if(func == "hex") {
		CNT(1);
		if(params[0].type == STR_TYPE) return to_hex(params[0].str);
		else if(params[0].type == NUM_TYPE) return to_hex(params[0].num);
		else error("arguments of builtin function '" + func + "' should be 'string' or 'number', not '" + type_name[params[0].type] + "'", "RuntimeError");
	}
}
int x;
string random_name() {
	return "fvm_" + tostr(x++);
}

vector<string> all_func;
bool has_func(string func) {
	for(int i = 0; i < all_func.size(); i++) if(all_func[i] == func) return true;
	return false;
}
Val run_byte(Val fval, int stk_offset, Val * params, int pcnt) {
	string ret_type = "any";
	reverse(params, params + pcnt);
	int stkoff = stk_offset;
	if(fval.type != FUNC_TYPE) {
		error("Should call a function. Not a " + type_name[fval.type]);
	}
	string func = fval.func;
	if(is_builtin(func)) return call_builtin(func, params, pcnt, fval.obj_ref);
	if(call_pause) {
		cout << "Function Call: " << func << "\tStack Offset: " << stk_offset << "\tParameter Count: " << pcnt << endl;
		system("pause > nul");
	}
	if(show_param) {
		for(int i = 0; i < pcnt; i++) cout << "Param#" << i << "\t" << params[i].to_str() << endl;
	}
	if(!has_func(func)) error("function " + func + " not defined.");
	int index = get_id(func);
	RunStack rstack(stk_offset);
	vector<char> vctmp;
	Val op1, op2;
	int itop_back = itop;
	new_frame();
	#define MATH(x, a) \
		case x: {\
			op1 = rstack.pop(), op2 = rstack.pop();\
			rstack.push(op2 a op1);\
			break;\
		}
	#define IMATH(x, a) \
		case x: {\
			op1 = rstack.pop(), op2 = rstack.pop();\
			rstack.push(op2 a op1);\
			break;\
		}
	if(pcnt != reg_map[index].size()) error("wrong argument count.");
	for(int i = 0; i < pcnt; i++) {
		local_store(reg_map[index][i], params[i]);
	}
	unsigned char byte;
	vector<string> vstmp, vstmp2;
	for(int ip = 0; ; ip++) {
		byte = byte_map[index][ip];
		if(debug) {
			if(show_step) cout << ip << " " << (int)byte << " " << opname[byte] << endl;
			if(show_stack) {
				for(int i = stkoff; i < rstack.t; i++) cout << "#" << i << "\t" << runtime_stack[i].to_str() << endl;
				cout << endl;
			}
			if(step_pause) system("pause > nul");
		}
		switch(byte) {
			MATH(ADD, +)
			MATH(SUB, -)
			MATH(MUL, *)
			MATH(DIV, /)
			MATH(LAND, &&)
			MATH(LOR, ||)
			MATH(BIG, >)
			MATH(BIGE, >=)
			case SML: {
				op1 = rstack.pop(), op2 = rstack.pop();
				rstack.push(compare(op2, op1));
				break;
			}
			MATH(SMLE, <=)
			MATH(EQL, ==)
			MATH(NEQL, !=)
			case NOT: {
				op1 = rstack.pop();
				rstack.push(!op1);
			    break;
			}
			case BNOT: {
				op1 = rstack.pop();
				rstack.push(~op1);
			    break;
			}
			case NEG: {
				op1 = rstack.pop();
				rstack.push(0 - op1);
			    break;
			}
			IMATH(BAND, &)
			IMATH(BOR, |)
			IMATH(XOR, ^)
			IMATH(LSHF, <<)
			IMATH(RSHF, >>)
			IMATH(MOD, %)
			case LOOP: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int loop = l_l + l_h * 256;
				int newip = ip - loop;
				ip = newip;
			    break;
			}
			case LOOPIF: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int loop = l_l + l_h * 256;
				int newip = ip - loop;
				if((!rstack.pop()).num) {
					ip = newip;
				}
			    break;
			}
			case POP: {
				rstack.pop();
			    break;
			}
			case GETSUB: {
				rstack.push(get_sub(rstack.pop(), rstack.pop()));
			    break;
			}
			case MKARR: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int len = l_l + l_h * 256;
				vector<Val> arr;
				for(int i = 0; i < len; i++) arr.push_back(rstack.pop());
				reverse(arr.begin(), arr.end());
				rstack.push(make_array(arr));
				break;
			    break;
			}
			case JMP: case BREAK: case CNTN: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int loop = l_l + l_h * 256;
				int newip = ip + loop;
				ip = newip;
			    break;
			}
			case JMPN: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int loop = l_l + l_h * 256;
				int newip = ip + loop;
				if((!rstack.pop()).num) {
					ip = newip;
				}
			    break;
			}
			case LDC: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int ldc = l_l + l_h * 256;
				rstack.push(cpool[ldc]);
			    break;
			}
			case LDN: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int ldc = l_l + l_h * 256;
				rstack.push(ldc);
			    break;
			}
			case RET: {
				release();
				itop = itop_back;
				if(!check_type(rstack.top(), ret_type)) {
					error("function '" + func + "' should return '" + ret_type + "', not '" + get_type_name(rstack.top()) + "'.", "RuntimeError");
				}
				return rstack.top();
			    break;
			}
			case DUP: {
				rstack.push(rstack.top());
			    break;
			}
			case LDNC: {
				int l_h = byte_map[index][++ip];
				rstack.push(l_h);
				break;
			}
			case LDCC: {
				int l_h = byte_map[index][++ip];
				rstack.push(cpool[l_h]);
				break;
			}
			case LOADC: {
				int l_h = byte_map[index][++ip];
				rstack.push(load(l_h));
				break;
			}
			case STOREC: {
				int l_h = byte_map[index][++ip];
				store(l_h, rstack.pop());
				break;
			}
			case LSTOREC: {
				int l_h = byte_map[index][++ip];
				local_store(l_h, rstack.pop());
				break;
			}
			case CALL: {
				int cnt = rstack.pop().num;
				Val * par = new Val[cnt];
				for(int i = 0; i < cnt; i++) par[i] = rstack.pop();
				if(rstack.top().type != FUNC_TYPE) {
					error("'" + get_type_name(rstack.top()) + "' is not callable.");
				}
				Val addr = rstack.pop();
				Val ret = run_byte(addr, rstack.t, par, cnt);
				rstack.push(ret);
			    break;
			}
			case MKCLS: {
				int cnt = rstack.pop().num;
				vstmp.clear(), vctmp.clear(), vstmp2.clear();
				for(int i = 0; i < cnt; i++) vctmp.push_back(rstack.pop().num), vstmp.push_back(rstack.pop().str), vstmp2.push_back(rstack.pop().str) ;
				manager.define_class(rstack.pop().str, rstack.pop().str, vstmp, vctmp, vstmp2);
			    break;
			}
			case LDC0: case LDC1: case LDC2: case LDC3: 
			case LDC4: case LDC5: case LDC6: case LDC7: {
				rstack.push(cpool[byte - LDC0]);
				break;
			}
			case END: {
				return Val((string)"End");
			    break;
			}
			case LOAD: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int ldc = l_l + l_h * 256;
				rstack.push(load(ldc));
			    break;
			}
			case STORE: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int ldc = l_l + l_h * 256;
				store(ldc, rstack.pop());
			    break;
			}
			case LSTORE: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int ldc = l_l + l_h * 256;
				local_store(ldc, rstack.pop());
			    break;
			}
			case MGET: {
				Val v = rstack.pop();
				if(v.extra != -1) {
					rstack.push(obj_heap[v.extra / MAX_FIELD_CNT].hashmap[v]);
				}
				else if(v.type == STR_TYPE) rstack.push(v.str);
				else if(v.type == NUM_TYPE) rstack.push(v.num);
				else if(v.type == REF_TYPE) rstack.push(get_heap(v.ref));
				else if(v.type == OBJ_TYPE) rstack.push(obj_heap[v.obj_ref / MAX_FIELD_CNT].field[v.obj_ref % MAX_FIELD_CNT]);
				else rstack.push(v);
			    break;
			}
			case MSET: {
				Val v2 = rstack.pop();
				int addr = v2.ref;
				Val v = rstack.pop();
				if(v2.extra != -1) {
					obj_heap[v2.extra / MAX_FIELD_CNT].hashmap[v2] = v;
				}
				else if(v2.type == REF_TYPE) set_heap(addr, v);
				else if(v2.type == OBJ_TYPE) manager.set_objheap(v2.obj_ref, v);
				else error("wrong type of subscript setting");
			    break;
			}
			case STR: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int len = l_l + l_h * 256;
				string s = "";
				for(int i = 0; i < len; i++) s += (char)rstack.pop().num;
				reverse(s.begin(), s.end());
				rstack.push(s);
				break;
			}
			case MKBIG: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int len = l_l + l_h * 256;
				string s = "";
				for(int i = 0; i < len; i++) s += (char)(rstack.pop().num + '0');
				reverse(s.begin(), s.end());
				rstack.push(BigInteger(s));
				break;
			}
			case FUNC: {
				rstack.push(func_val(rstack.top().str));
				break;
			}
			case LRMV: {
				int l_h = byte_map[index][++ip];
				int l_l = byte_map[index][++ip];
				int loc = l_l + l_h * 256;
				store(loc, Val());
				break;
			}
			case THIS: {
				if(fval.obj_ref == -1) {
					error("Token 'this' shouldn't be used in a non-method function.", "RunError");
				}
				rstack.push(obj_val(fval.obj_ref));
				break;
			}
			case NOP: {
				;
				break;
			}
			case RESET: {
				rstack.t = stkoff;
				break;
			}
			case LD0: case LD1: case LD2: case LD3:
			case LD4: case LD5: case LD6: case LD7: 
			case LD8: case LD9: case LD10: case LD11:
			case LD12: case LD13: case LD14: case LD15: {
				rstack.push(byte - LD0);
				break;
			}
			case LDNUL: {
				rstack.push(null_val());
				break;
			}
			case MKRG: {
				Val l = rstack.pop();
				Val r = rstack.pop();
				if(l.type == NUM_TYPE && r.type == NUM_TYPE) {
					rstack.push(make_range(l.num, r.num));
				}
				else error("range left/right value must be number");
				break;
			}
			case RTYPE: {
				ret_type = rstack.pop().str;
				break;
			}
			case PTYPE: {
				string type = rstack.pop().str;
				int which = rstack.pop().num;
				if(!check_type(params[which], type)) {
					error("should use '" + type + "' as argument " + tostr(which) + " of function '" + func + "', not '" + get_type_name(params[which]) + "'", "RuntimeError");
				}
				break;
			}
			default: {
				error("uncompatible byte " + tostr(byte));
				break;
			}
		}
	}
}

const string opchar = "+-*/!=><%&|@^~:";
const string spliter = "+-*/=><;!&|%@^,~#:";
string trim(string s) {
	bool all_space = true;
	for(int i = 0; i < s.length(); i++) if(!isspace(s[i])) {
		all_space = false;
		break;
	}
	if(all_space) return "";
    int x = 0, y = s.length() - 1;
    while(isspace(s[x]) || s[x] == 0) x++;
    while(isspace(s[y]) || s[y] == 0) y--;
    return s.substr(x, y - x + 1);
}
#define MIDDLE(str) (str.length() >= 2 ? str.substr(1, str.length() - 2) : str)

bool include_char(string s, char c) {
    for(int i = 0; i < s.length(); i++) if(s[i] == c) return true;
    return false;
}
string join_string(vector<string> vs, string spaces = "") {
    string r = "";
    for(int i = 0; i < vs.size(); i++) r += vs[i] + spaces;
    return r;
}
vector<string> remove_all(vector<string> vs, string remv) {
    vector<string> result;
    for(int i = 0; i < vs.size(); i++) {
        if(remv != vs[i]) result.push_back(vs[i]);
    }
    return result;
}

bool check_char(char c) {
	return isalpha(c) || isdigit(c) || c == '_';
}

vector<string> tokenize(string text) {
	vector<string> tmp;
	if(text == "") return tmp;
	string cur = "";
	for(int i = 0; i < text.length(); i++) {
		if(text[i] == '(') {
			int top = 1;
			bool inq = false;
			if(cur != "") {
				tmp.push_back(cur);
			}
			cur = "(";
			while(top && i < text.length()) {
				i++;
				if((!isspace(text[i]) && text[i] != '\\') || inq) cur = cur + text[i];
				if(text[i] == '(') top++;
				else if(text[i] == ')') top--;
				if(text[i] == '\\') {
					if(i < text.length() - 1) {
						i++;
						cur = cur + text[i];
					}
				}
				else if(text[i] == '"' || text[i] == '\'') {
					inq = !inq;
				}
			}
			tmp.push_back(cur);
			cur = "";
		}
		else if(text[i] == '[') {
			int top = 1;
			bool inq = false;
			if(cur != "") {
				tmp.push_back(cur);
			}
			cur = "[";
			while(top && i < text.length()) {
				i++;
				if((!isspace(text[i]) && text[i] != '\\') || inq) cur = cur + text[i];
				if(text[i] == '[') top++;
				else if(text[i] == ']') top--;
				if(text[i] == '\\') {
					if(i < text.length() - 1) {
						i++;
						cur = cur + text[i];
					}
				}
				else if(text[i] == '"' || text[i] == '\'') {
					inq = !inq;
				}
			}
			tmp.push_back(cur);
			cur = "";
		}
		else if(text[i] == '{') {
			int top = 1;
			bool inq = false;
			if(cur != "") {
				tmp.push_back(cur);
			}
			cur = "{";
			while(top && i < text.length()) {
				i++;
				if((!isspace(text[i]) && text[i] != '\\') || inq) cur = cur + text[i];
				if(text[i] == '{') top++;
				else if(text[i] == '}') top--;
				if(text[i] == '\\') {
					if(i < text.length() - 1) {
						i++;
						cur = cur + text[i];
					}
				}
				else if(text[i] == '"' || text[i] == '\'') {
					inq = !inq;
				}
			}
			tmp.push_back(cur);
			cur = "";
		}
		else if(text[i] == '"') {
			int top = 1;
			if(cur != "") {
				tmp.push_back(cur);
			}
			cur = "\"";
			while(i < text.length()) {
				i++;
				if(text[i] == '\\') {
					cur = cur + text[i];
					if(i < text.length() - 1) {
						i++;
						cur = cur + text[i];
					}
				}
				else if(text[i] == '"') {
					cur = cur + text[i];
					break;
				}
				else cur = cur + text[i];
			}
			tmp.push_back(cur);
			cur = "";
		}
		else if(text[i] == '\'') {
			int top = 1;
			if(cur != "") {
				tmp.push_back(cur);
			}
			cur = "\'";
			while(i < text.length()) {
				i++;
				if(text[i] == '\\') {
					cur = cur + text[i];
					if(i < text.length() - 1) {
						i++;
						cur = cur + text[i];
					}
				}
				else if(text[i] == '\'') {
					cur = cur + text[i];
					break;
					
				}
				else cur = cur + text[i];
			}
			tmp.push_back(cur);
			cur = "";
		}
		else if(!check_char(text[i])) {
			if(cur != "") {
				tmp.push_back(cur);
			} 
			if(!isspace(text[i])) {
				cur = "";
				cur += text[i];
				tmp.push_back(cur);
			}
			cur = "";
		}
		else if(check_char(text[i])) {
			cur += text[i];
		}
	}
	if(cur != "") {
		tmp.push_back(cur);
	}
	vector<string> ret;
	vector<string> final;
	for(int i = 0; i < tmp.size(); i++) {
		if(tmp[i] != "") {
			tmp[i] = trim(tmp[i]);
			ret.push_back(tmp[i]);
		}
	}
    for(int i = 0; i < ret.size(); i++) {
        if(i > 0) {
            if(ret[i] == "=") {
                if(ret[i - 1] == ">" || ret[i - 1] == "<" || ret[i - 1] == "!" || ret[i - 1] == "=" ||
				ret[i - 1] == "+" || ret[i - 1] == "-" || ret[i - 1] == "*" || ret[i - 1] == "/" ||
				ret[i - 1] == "%" || ret[i - 1] == ">>" || ret[i - 1] == "<<" || ret[i - 1] == "&" ||
				ret[i - 1] == "|") {
					final[final.size() - 1] += "=";
				}
                else final.push_back(ret[i]);
            } else if(ret[i] == ".") {
            	if(i >= ret.size() - 1) error("need number or an identifier after '.' .", "SyntaxError");
            	if(isdigit(ret[i - 1][0]) || ret[i - 1][0] == '-') {
            		final[final.size() - 1] += ret[i] + ret[i + 1];
            		i++;
				}
				else final.push_back(ret[i]);
			} else if(ret[i] == "&") {
                if(ret[i - 1] == "&") final[final.size() - 1] += "&";
                else final.push_back(ret[i]);
            } else if(ret[i] == "|") {
                if(ret[i - 1] == "|") final[final.size() - 1] += "|";
                else final.push_back(ret[i]);
            } else if(ret[i] == ">") {
                if(ret[i - 1] == ">") final[final.size() - 1] += ">";
                else final.push_back(ret[i]);
            } else if(ret[i] == "<") {
                if(ret[i - 1] == "<") final[final.size() - 1] += "<";
                else final.push_back(ret[i]);
            } else if(ret[i] == "+") {
                if(ret[i - 1] == "+") final[final.size() - 1] += "+";
                else final.push_back(ret[i]);
            } else if(ret[i] == "-") {
                if(ret[i - 1] == "-") final[final.size() - 1] += "-";
                else final.push_back(ret[i]);
            }else final.push_back(ret[i]);
        } else final.push_back(ret[i]);
    }
	return final;
}

vector<string> split(string text, char c, bool space = true) {
    string cur = "";
    bool includeSpace = false;
    vector<string> tmp;
    vector<string> ret;
    int top[4] = {
        0, 0, 0, 0
    };
    for(int i = 0; i < text.length(); i++) {
        string cur2 = cur + text[i];
        if(text[i] == '(') {
            top[0]++;
        } else if(text[i] == ')') {
            top[0]--;
        } else if(text[i] == '[') {
            top[1]++;
        } else if(text[i] == ']') {
            top[1]--;
        } else if(text[i] == '{') {
            top[2]++;
        } else if(text[i] == '}') {
            top[2]--;
        } else if(text[i] == '"' && (i == 0 || text[i - 1] != '\\')) {
            top[3] = !top[3];
        }
        if((((c == text[i]) || (isspace(text[i])))) && !top[0] && !top[1] && !top[2] && !top[3]) {
            tmp.push_back(cur);
            cur = "";
            if(!isspace(text[i]) || includeSpace) {
                cur += text[i];
                tmp.push_back(cur);
                cur = "";
            }
        } else {
            if(top[3]) cur += text[i];
            else if(!isspace(text[i])) cur += text[i];
        }
    }
    tmp.push_back(cur);
    for(int i = 0; i < tmp.size(); i++) {
        if(tmp[i].length() && !isspace(tmp[i][0])) {
            ret.push_back(tmp[i]);
        }
    }
    return ret;
}

string standard_call(string str) {
    vector<string> vs = tokenize(str);
    vector<string> result;
    string ret = "";
    for(int i = 0; i < vs.size(); i++) {
        if(i > 0 && !(include_char(opchar + "[{}]", vs[i - 1][0])) && vs[i][0] == '(') {
            string allParam = MIDDLE(vs[i]);
            result.push_back("^");
            result.push_back("[" + allParam + "]");
        } else result.push_back(vs[i]);
    }

    ret = join_string(result);
    return ret;
}
string standard_subscript(string str) {
    vector<string> vs = tokenize(str);
    vector<string> result;
    string ret = "";
    for(int i = 0; i < vs.size(); i++) {
        if(vs[i][0] == '[') {
            string subExpr = MIDDLE(vs[i]);
            result.push_back("@");
            result.push_back("(" + subExpr + ")");
        } else result.push_back(vs[i]);
    }

    ret = join_string(result);
    return ret;
}
double to_hex(string);
string string_expression(string str) {
    string result = "";
    for(int i = 0; i < str.length(); i++) {
        if(str[i] == '\\') {
            i++;
            if(i > str.length()) error("need an escape character after '\\'.", "SyntaxError");
            switch(str[i]) {
            	case 'u': {
            		string s = "";
            		unsigned int hex = 0;
            		if(i + 4 >= str.length()) error("invalid unicode sequence " + str, "SyntaxError");
            		s += str[++i];
            		s += str[++i];
            		hex = to_hex("0x" + s);
            		result += (unsigned char)(hex);
            		s = "";
            		s += str[++i];
            		s += str[++i];
            		hex = to_hex("0x" + s);
            		result += (unsigned char)(hex);
					break;
				}
                case 'a':
                    result += '\a';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '\'':
                    result += '\'';
                    break;
                case '\"':
                    result += '\"';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 's':
                    result += ' ';
                    break;
                default: {
                    string msg = " unsupported escape character: '\\";
                    msg += str[i];
                    msg += "'.";
                    error(msg, "SyntaxError");
                    break;
                }
            }
        } else result += str[i];
    }
    return result;
}

vector<unsigned char> load_int(double x) {
	vector<unsigned char> rets;
	if(short_op && ceil(x) == x && x >= 0 && x <= 15) {
		rets.push_back(LD0 + x);
	}
	else if(short_op && ceil(x) == x && x >= 0 && x <= 0xff) {
		rets.push_back(LDNC);
		rets.push_back(x);
	}
	else if(ceil(x) != x || x < 0 || x >= 0xffff) {
		int siz = addconst(x);
		if(short_op && siz >= 0 && siz <= 7) rets.push_back(LDC0 + siz);
		else if(short_op && siz >= 0 && siz <= 0xff) {
			rets.push_back(LDCC);
			rets.push_back(siz);
		}
		else {
			rets.push_back(LDC);
			rets.push_back(siz / 256);
			rets.push_back(siz % 256);
		}
	}
	else {
		int ax = (int)x;
		rets.push_back(LDN);
		rets.push_back(ax / 256);
		rets.push_back(ax % 256);
	}
	return rets;
}
vector<unsigned char> load_str(string x) {
	vector<unsigned char> rets;
	int siz = addconst(x);
	if(short_op && siz >= 0 && siz <= 7) rets.push_back(LDC0 + siz);
	else if(short_op && siz >= 0 && siz <= 0xff) {
		rets.push_back(LDCC);
		rets.push_back(siz);
	}
	else {
		rets.push_back(LDC);
		rets.push_back(siz / 256);
		rets.push_back(siz % 256);
	}
	return rets;
}
double to_hex(string s) {
	int result = 0;
	char parsing[256];
	memset(parsing, 0, sizeof(parsing));
	for(int i = 0; i <= 9; i++) parsing['0' + i] = i;
	for(int i = 'a'; i <= 'f'; i++) parsing[i] = i - 'a' + 10;
	for(int i = 'A'; i <= 'F'; i++) parsing[i] = i - 'F' + 10;
	for(int i = 2; i < s.length(); i++) result = result * 16 + parsing[tolower(s[i])];
	return result;
}
map<string, int> var_id;
string get_name(int id) {
	for(map<string, int>::iterator it = var_id.begin(); it != var_id.end(); it++) {
		string name = (*it).first;
		int idx = (*it).second;
		if(idx == id) return name;
	}
	error("invalid register " + tostr(id), "RuntimeError");
}
int varcnt;
int get_id(string var) {
	if(var_id.find(var) == var_id.end()) var_id[var] = varcnt, varcnt++;
	return var_id[var];
}
vector<unsigned char> get_var_val(string name) {
	vector<unsigned char> rets;
	if(name == "this") {
		rets.push_back(THIS);
		return rets;
	}
	int id = get_id(name);
	if(short_op && id >= 0 && id <= 0xff) {
		rets.push_back(LOADC);
		rets.push_back(id);
	}
	else {
		rets.push_back(LOAD);
		rets.push_back(id / 256);
		rets.push_back(id % 256);
	}
	return rets;
}
vector<unsigned char> load_func(string s) {
	vector<unsigned char> rets, tmp;
	tmp = load_str(s);
	for(int i = 0; i < tmp.size(); i++) rets.push_back(tmp[i]);
	rets.push_back(FUNC);
	return rets;
}

vector<unsigned char> compile_big_int(string b) {
	vector<unsigned char> rets;
	for(int i = 0; i < b.length(); i++) {
    	if(!isdigit(b[i])) error("invalid big integer string " + b, "RuntimeError");
    	rets.push_back(LD0 + b[i] - '0');
	}
	int siz = b.length();
	rets.push_back(MKBIG);
	rets.push_back(siz / 256);
	rets.push_back(siz % 256);
	return rets;
}

vector<unsigned char> compile_literal(string s) {
	vector<unsigned char> rets;
    if(s[0] == '\"'|| s[0] == '\'') {
    	string str = string_expression(MIDDLE(s));
    	vector<unsigned char> rets2 = load_str(str);
    	for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
	}
    else if(isdigit(s[0])) {
    	if(s[0] == '0' && s[1] == 'x') {
    		vector<unsigned char> rets2 = load_int(to_hex(s));
    		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
		}
    	else {
    		if(s.length() >= 13 && enable_big) {
    			return compile_big_int(s);
			}
	    	double d = atof(s.c_str());
	    	
	    	if(d > LIMIT || d < -LIMIT) {
	    		if(enable_big) return compile_big_int(tostr_d((int)d));
			}
    		vector<unsigned char> rets2 = load_int(d);
    		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
		}
	}
    else return get_var_val(s);
    return rets;
}

bool has_range(vector<string> vs) {
	if(vs.size() >= 4) return false;
	if(vs.size() == 3) return vs[1] == ":";
	if(vs.size() == 2) return vs[0] == ":" || vs[1] == ":";
	if(vs.size() == 1) return vs[0] == ":";
	return false;
}

void get_range_param(vector<string> vs, string & left, string & right) {
	if(vs.size() == 3) left = vs[0], right = vs[2];
	if(vs.size() == 2) {
		if(vs[0] == ":") left = "0", right = vs[1];
		else left = vs[0], right = "-1";
	}
	if(vs.size() == 1) left = "0", right = "-1";
}

string parse_subscript(string);
vector<unsigned char> execute(string);
bool is_self(string);
vector<unsigned char> make_lambda(string, string);
vector<unsigned char> compile_expression(string expr) {
  	vector<string> vs;
    stack<string> stk;
    vector<string> str;
	string backup = expr;
	vector<unsigned char> bytecodes;
	
    expr = standard_call(standard_subscript(parse_subscript(expr)));

    for(int i = 0; i < expr.length(); i++) {
        if(expr[i] == '-') {
            if(i == 0) {
                expr.insert(0, 1, '0');
            } else if(expr[i - 1] == '(') {
                expr.insert(i, 1, '0');
            }
        }
    }
    str = tokenize(expr);
    if(str[0] == "lambda") {
    	if(str.size() != 3) error("lambda syntax must be '(lambda {<params>} {<expr})'", "SyntaxError");
    	return make_lambda(str[2], str[1]);
	}
	else if(str.size() == 2 && (str[1] == "++" || str[1] == "--")) {
		vector<unsigned char> ret = compile_expression(str[0]);
		vector<unsigned char> exret = execute(backup);
		for(int i = 0; i < exret.size(); i++) ret.push_back(exret[i]);
		return ret;
	}
	else if(has_range(str)) {
		string left, right;
		get_range_param(str, left, right);
		vector<unsigned char> ret = compile_expression(left);
		vector<unsigned char> ret2 = compile_expression(right);
		for(int i = 0; i < ret2.size(); i++) ret.push_back(ret2[i]);
		ret.push_back(MKRG);
		return ret;
	}
	else if(str.size() == 2 && (str[0] == "++" || str[0] == "--")) {
		return execute(str[1] + str[0]);
	}
	else  {
		for(int i = 0; i < str.size(); i++) {
	    	if((str[i].length() == 1 && str[i] == "=") || (str[i].length() <= 3 && (str[i][str[i].length() - 1] == '=' && is_self(str[i].substr(0, str[i].length() - 1))))) {
				return execute(backup);
	    		break;
			}
		}
	}
	
    for(int i = 0; i < str.size(); i++) {
        string tmp = "";
        string a = "";
        if(include_char(opchar, str[i][0])) {
            string c = str[i];
            if(stk.empty() || stk.top() == "(") {
                stk.push(c);
            } else {
                while(!stk.empty() && bind_power[stk.top()] >= bind_power[c] ) {
                    tmp += stk.top();
                    vs.push_back(tmp);
                    stk.pop();
                    tmp = "";
                }
                stk.push(c);
            }
        } else vs.push_back(str[i]);
    }
    while(!stk.empty()) {
        string tmp = "";
        tmp += stk.top();
        vs.push_back(tmp);
        stk.pop();
    }
	
	#define PUSH(x) bytecodes.push_back(x)
	#define CHECK_PUSH(op, push) \
	else if(vs[i] == op) {\
		PUSH(push);\
	}
    for(int i = 0; i < vs.size(); i++) {
        string tmp = vs[i];
        if(vs[i] == "+") {
        	PUSH(ADD);
        }
		CHECK_PUSH("-", SUB)
		CHECK_PUSH("*", MUL)
		CHECK_PUSH("/", DIV)
		CHECK_PUSH("%", MOD)
		CHECK_PUSH(">", BIG)
		CHECK_PUSH("<", SML)
		CHECK_PUSH(">=", BIGE)
		CHECK_PUSH("<=", SMLE)
		CHECK_PUSH("==", EQL)
		CHECK_PUSH("!=", NEQL)
		CHECK_PUSH("!", NOT)
		CHECK_PUSH("&&", LAND)
		CHECK_PUSH("||", LOR)
		CHECK_PUSH(">>", RSHF)
		CHECK_PUSH("<<", LSHF)
		CHECK_PUSH("&", BAND)
		CHECK_PUSH("|", BOR)
		CHECK_PUSH("~", BNOT)
		else if(vs[i] == "@") {
            PUSH(GETSUB);
            PUSH(MGET);
        } else if(vs[i] == "^") {
            PUSH(CALL);
        } else {
            string simpleExpr = vs[i];
            if(vs[i][0] == '(') {
                simpleExpr = MIDDLE(vs[i]);
            	vector<unsigned char> rets = compile_expression(simpleExpr);
            	for(int i = 0; i < rets.size(); i++) PUSH(rets[i]);
            } else if(vs[i][0] == '[') {
                simpleExpr = MIDDLE(vs[i]);
                string parameter = simpleExpr;
                vector<string> args = remove_all(split(parameter, ','), ",");
	            for(int i = 0; i < args.size(); i++) {
	            	vector<unsigned char> rets = compile_expression(args[i]);
	            	for(int j = 0; j < rets.size(); j++) PUSH(rets[j]);
				}
				int siz = args.size();
				vector<unsigned char> rets2 = load_int(siz);
				for(int i = 0; i < rets2.size(); i ++) PUSH(rets2[i]);
            } else if(vs[i][0] == '{') {
                simpleExpr = MIDDLE(vs[i]);
                vector<string> args = remove_all(split(simpleExpr, ','), ",");
                for(int i = 0; i < args.size(); i++) {
                	vector<unsigned char> rets = compile_expression(args[i]);
	            	for(int j = 0; j < rets.size(); j++) PUSH(rets[j]);
				}
				int siz = args.size();
                PUSH(MKARR);
                PUSH(siz / 256);
                PUSH(siz % 256);
            } else {
            	vector<unsigned char> rets = compile_literal(vs[i]);
            	for(int i = 0; i < rets.size(); i++) PUSH(rets[i]);
			}
        }
    }
    return bytecodes;
}
enum { EXPR, ASSIGN };
string parse_subscript(string expr) {
	vector<string> vs = tokenize(expr);
	string result = "";
	for(int i = 0; i < vs.size(); i++) {
		if(i < vs.size() - 1 && vs[i] == ".") {
			i++;
			result += "[\"" + vs[i] + "\"]";
		}
		else result += vs[i];
	}
	return result;
}
vector<unsigned char> set_var_val(string name, string exp) {
	vector<unsigned char> vc;
	vector<unsigned char> rets = compile_expression(exp);
	for(int i = 0; i < rets.size(); i++) vc.push_back(rets[i]); 
	int id = get_id(name);
	if(short_op && id >= 0 && id <= 0xff) {
		vc.push_back(STOREC);
		vc.push_back(id);
	}
	else {
		vc.push_back(STORE);
		vc.push_back(id / 256);
		vc.push_back(id % 256); 
	}
	return vc;
}
enum { SUB_SET, VAR_SET };
vector<unsigned char> set_var_ref(string var, string exp) {
	vector<unsigned char> bytecodes;
	vector<unsigned char> rets2;
	/*
    for(int i = 0; i < protectVar.size(); i++) {
    	if(var == protectVar[i]) RUN_ERR(" access denied: " + var + " can not be modified.", 16);
	}
	*/
	if(include_char(var, '.')) var = parse_subscript(var);
    vector<string> vs;
    stack<string> stk;
    vector<string> str;
    string expr = var;
    int type = VAR_SET;

    for(int i = 0; i < var.length(); i++) {
        if(var[i] == '[') type = SUB_SET;
    }

    if(type == VAR_SET) {
        return set_var_val(var, exp);
    }
	
	rets2 = compile_expression(exp);
	for(int i = 0; i < rets2.size(); i++) PUSH(rets2[i]);
	
    expr = standard_call(standard_subscript(expr));

    for(int i = 0; i < expr.length(); i++) {
        if(expr[i] == '-') {
            if(i == 0) {
                expr.insert(0, 1, '0');
            } else if(expr[i - 1] == '(') {
                expr.insert(i, 1, '0');
            }
        }
    }

    str = tokenize(expr);
	
	string tmp;
	string a;
	string c;
    for(int i = 0; i < str.size(); i++) {
        tmp = "";
        a = "";
        if(include_char(opchar, str[i][0])) do {
                c = str[i];
                if(stk.empty() || stk.top() == "(") {
                    stk.push(c);
                } else {
                    while(!stk.empty() && bind_power[stk.top()] >= bind_power[c] ) {
                        tmp += stk.top();
                        vs.push_back(tmp);
                        stk.pop();
                        tmp = "";
                    }
                    stk.push(c);
                }
            } while(0);
        else vs.push_back(str[i]);
    }

    while(!stk.empty()) {
        string tmp = "";
        tmp += stk.top();
        vs.push_back(tmp);
        stk.pop();
    }
	vector<unsigned char> rets;
    for(int i = 0; i < vs.size() - 1; i++) {
        if(vs[i] == "@") {
            PUSH(GETSUB);
            PUSH(MGET);
        } else {
            string simpleExpr = vs[i];
            if(vs[i][0] == '(') {
                simpleExpr = MIDDLE(vs[i]);
            	vector<unsigned char> rets = compile_expression(simpleExpr);
            	for(int i = 0; i < rets.size(); i++) PUSH(rets[i]);
            } else if(vs[i][0] == '[') {
                simpleExpr = MIDDLE(vs[i]);
                string parameter = simpleExpr;
                vector<string> args = remove_all(split(parameter, ','), ",");
                vector<unsigned char> rets;
	            for(int i = 0; i < args.size(); i++) {
	            	rets = compile_expression(args[i]);
	            	for(int j = 0; j < rets.size(); j++) PUSH(rets[j]);
				}
				int siz = args.size();
				PUSH(LDN);
				PUSH(siz / 256);
				PUSH(siz % 256);
            } else if(vs[i][0] == '{') {
                simpleExpr = MIDDLE(vs[i]);
                vector<string> args = remove_all(split(simpleExpr, ','), ",");
                vector<unsigned char> rets; 
                for(int i = 0; i < args.size(); i++) {
                	rets = compile_expression(args[i]);
	            	for(int j = 0; j < rets.size(); j++) PUSH(rets[j]);
				}
				int siz = args.size();
                PUSH(MKARR);
                PUSH(siz / 256);
                PUSH(siz % 256);
            } else {
            	rets = compile_literal(vs[i]);
            	for(int i = 0; i < rets.size(); i++) PUSH(rets[i]);
			}
        }
    }

    PUSH(GETSUB);
    PUSH(MSET);
    
    return bytecodes;
}
const int SELF_CNT = 9;
const string SELF_OPS[SELF_CNT] = {
	"+", "-", "*", "/", "%", ">>", "<<", "&", "|"
};

bool is_self(string op) {
	for(int i = 0; i < SELF_CNT; i++) if(SELF_OPS[i] == op) return true;
	return false; 
} 
vector<unsigned char> execute(string stat) {
    vector<string> vs = tokenize(stat);
    int i = 0;
    int type = EXPR;
    string flag = vs[0];

    bool inq = false;
	int x;
	if(vs.size() == 2 && (vs[1] == "++" || vs[1] == "--")) {
		string newstat = vs[0] + " = (" + vs[0] + ") ";
		if(vs[1] == "++") {
			newstat += "+";
		}
		else if(vs[1] == "--") {
			newstat += "-";
		}
		newstat += "1";
		return execute(newstat);
	}
	else {
		for(int i = 0; i < vs.size(); i++) {
	    	if((vs[i].length() == 1 && vs[i] == "=") || (vs[i].length() <= 3 && (vs[i][vs[i].length() - 1] == '=' && is_self(vs[i].substr(0, vs[i].length() - 1))))) {
				type = ASSIGN, x = i;
	    		break;
			}
		}
	}
    string left;
    string right;
    vector<unsigned char> ret;
    if(type == ASSIGN) {
    	for(int i = 0; i < x; i++) left += vs[i];
   		for(int j = x + 1; j < vs.size(); j++) right += vs[j];
        
        string op = "X";
		if(vs[x] != "=") op = vs[x].substr(0, vs[x].length() - 1);
		
		if(op != "X") right = "(" + left + ")" + op + "(" + right + ")";
		ret = set_var_ref(left, right);
    } else if(flag == "stop") {
    } else {
        ret = compile_expression(stat);
    }
    return ret;
}
void init2() {
	new_frame();
#define BIND(c, rbp, code) bind_power[(c)] = (rbp)
    BIND("+", 60, ADD);
    BIND("-", 60, DEC);
    BIND("*", 70, MUL);
    BIND("/", 70, DIV);
    BIND("%", 70, MOD);
    BIND("+=", 10, ADD_E);
    BIND("-=", 10, DEC_E);
    BIND("*=", 10, MUL_E);
    BIND("/=", 10, DIV_E);
    BIND("%=", 10, MOD_E);
    BIND(">", 50, BIG);
    BIND(">=", 50, BIGEQL);
    BIND("<", 50, SML);
    BIND("<=", 50, SMLEQL);
    BIND("==", 40, EQL);
    BIND("!=", 40, NEQL);
    BIND("!", 30, NOT);
    BIND("&&", 30, AND);
    BIND("||", 29, OR);
    BIND("&", 65, BITAND);
    BIND("|", 64, BITOR);
    BIND(">>", 65, RIGHTSHIFT);
    BIND("<<", 65, LEFTSHIFT);
    BIND("&=", 10, BITAND_E);
    BIND("|=", 10, BITOR_E);
    BIND(">>=", 10, RIGHTSHIFT_E);
    BIND("<<=", 10, LEFTSHIFT_E);
    BIND("@", 90, SUB);
    BIND("^", 90, CALL);
    BIND("~", 90, BITNOT);
    BIND(":", 15, MKRANGE);
    
    #define BUILTIN(a) store(get_id(a), func_val(a)), builtin.insert(a)
    #define CONST(a, x) store(get_id(a), x), protect.insert(get_id(a))
    
    BUILTIN("print");
    BUILTIN("len");
    BUILTIN("sub");
    BUILTIN("clock");
    BUILTIN("time");
    BUILTIN("memset");
    BUILTIN("system");
    BUILTIN("rand");
    BUILTIN("srand");
    BUILTIN("clone");
    
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
    BUILTIN("instanceof");
    BUILTIN("serialize");
    BUILTIN("unserialize");
    BUILTIN("hex");
	
	CONST("true", Val(1));
	CONST("false", Val(0));
	CONST("null", null_val());
	CONST("math_pi", Val(3.1415926535897932384));
	
	for(int i = 0; i < MAX_OBJ_CNT; i++) obj_pool.push(i);
	
	manager.define_class("Object", "Object", tokenize("class_name super_name"), vector<char>(2), tokenize("any any"), true);
	manager.define_class("map", "Object", vector<string>(0), vector<char>(0), vector<string>(0), true);
	BUILTIN("map");
	BUILTIN("clear_map");
	BUILTIN("keys");
	BUILTIN("values");
}

struct Token {
	string str;
	int line;
	Token() {}
	Token(string s) {
		str = s;
	}
};
inline bool operator == (Token a, Token b) {
	return a.str == b.str;
}
inline bool operator == (Token a, const char * b) {
	return a == Token(b);
}
inline bool operator != (Token a, Token b) {
	return !(a == b);
}
vector<Token> tokenstream;
int tok;
Token next_token() {
	if(tok == tokenstream.size()) return (string)"#END_TOKEN#";
	return tokenstream[tok++];
}

bool has_next() {
	return tok < tokenstream.size();
}

void read_token(string expect) {
	if(tok == tokenstream.size()) error("expected " + expect + " at the end", "SyntaxError");
	if(tokenstream[tok++] != expect) error("expected " + expect + " at line " + tostr(tokenstream[tok - 1].line), "SyntaxError");
}

inline bool next_token_is(string next) {
	if(!has_next()) return false;
	return tokenstream[tok] == next;
}

void add_token(string token, int line) {
	Token t(token);
	t.line = line;
	tokenstream.push_back(t);
}
const string BEGIN_LABEL = "{";
const string END_LABEL = "}";
const string EOL_LABEL = "<EOL>"; 
vector<unsigned char> compile_block();

int loop_stack[10000 + 5];
int top;

int pop_loop() {
	return loop_stack[top--];
}

void add_loop(int addr) {
	loop_stack[++top] = addr;
}

void replace_holder(int loop_break, int loop_cntn, int loop_head, vector<unsigned char> & rets) {
	for(int i = loop_break - 1; i >= loop_head; i--) {
		if(rets[i] == BREAK) {
			if(rets[i + 1] == 0x00 && rets[i + 2] == 0x00) { // Not Replaced Break
				int jmp = loop_break - (i + 3);
				rets[i + 1] = jmp / 256; // Replace Break
				rets[i + 2] = jmp % 256;
			}
		}
		if(rets[i] == CNTN) {
			if(rets[i + 1] == 0x00 && rets[i + 2] == 0x00) { // Not Replaced Continue
				int lop = loop_cntn - (i + 3);
				rets[i + 1] = lop / 256; // Replace Continue
				rets[i + 2] = lop % 256;
			}
		}
	}
}
int tabcnt = 0;
string gettab() {
	string s = "";
	for(int i = 0; i < tabcnt; i++) s += "    ";
	return s;
}

vector<unsigned char> make_struct(string cls, string super, vector<string> fields, vector<char> type, vector<string> vtype) {
	vector<unsigned char> tmp;
	vector<unsigned char> ret;
	reverse(fields.begin(), fields.end());
	reverse(type.begin(), type.end());
	reverse(vtype.begin(), vtype.end());
	if(type.size() != fields.size()) {
		type.clear();
		for(int i = 0; i < fields.size(); i++) type.push_back(CLS_FIELD);
	}
	tmp = compile_literal("\'" + cls + "\'");
	for(int i = 0; i < tmp.size(); i++) {
		ret.push_back(tmp[i]);
	}
	tmp = compile_literal("\'" + super + "\'");
	for(int i = 0; i < tmp.size(); i++) {
		ret.push_back(tmp[i]);
	}
	for(int i = 0; i < fields.size(); i++) {
		tmp = compile_literal("\'" + vtype[i] + "\'");
		for(int j = 0; j < tmp.size(); j++) {
			ret.push_back(tmp[j]);
		}
		tmp = compile_literal("\'" + fields[i] + "\'");
		for(int j = 0; j < tmp.size(); j++) {
			ret.push_back(tmp[j]);
		}
		tmp = load_int(type[i]);
		for(int j = 0; j < tmp.size(); j++) {
			ret.push_back(tmp[j]);
		}
	}
	tmp = load_int(fields.size());
	for(int i = 0; i < tmp.size(); i++) ret.push_back(tmp[i]);
	ret.push_back(MKCLS);
	return ret;
} 
vector<unsigned char> make_function(string &, string);
vector<unsigned char> make_class() {
	vector<unsigned char> tmp;
	vector<unsigned char> rets;
	vector<char> types;
	vector<string> fields;
	
	Token class_name = next_token();
	Token super_name = Token("Object");
	Token field_type, field_name;
	string str_name;
	
	if(next_token_is("extends")) {
		read_token("extends");
		super_name = next_token();
	}
	read_token(BEGIN_LABEL);
	
	vector<string> vtype;
	string t;
	while(!next_token_is(END_LABEL)) {
		if(next_token_is("var")) {
			read_token("var");
			fields.push_back(next_token().str);
			if(next_token_is(":")) {
				read_token(":");
				t = next_token().str;
				vtype.push_back(t);
			}
			else vtype.push_back("any");
			types.push_back(CLS_FIELD);
			while(next_token_is(",")) {
				read_token(",");
				fields.push_back(next_token().str);
				if(next_token_is(":")) {
					read_token(":");
					t = next_token().str;
					vtype.push_back(t);
				}
				else vtype.push_back("any");
				types.push_back(CLS_FIELD);
			}
			read_token(EOL_LABEL); 
		}
		else if(next_token_is("function")) {
			read_token("function");
			tmp = make_function(str_name, class_name.str + ".");
			for(int i = 0; i < tmp.size(); i++) rets.push_back(tmp[i]);
			
			fields.push_back(str_name);
			types.push_back(CLS_METHOD);
			vtype.push_back("function");
		}
		else error("Unsupported field type \'" + next_token().str + "\' in class declaration.", "SyntaxError");
	}
	read_token(END_LABEL);
	
	tmp = make_struct(class_name.str, super_name.str, fields, types, vtype); // Class
	for(int i = 0; i < tmp.size(); i++) rets.push_back(tmp[i]);
	
	vector<unsigned char> lint = load_func(class_name.str);
	int id = get_id(class_name.str);
	for(int i = 0; i < lint.size(); i++) rets.push_back(lint[i]);
	if(short_op && id >= 0 && id <= 0xff) {
		rets.push_back(STOREC);
		rets.push_back(id);
	}
	else {
		rets.push_back(STORE);
		rets.push_back(id / 256);
		rets.push_back(id % 256);
	}
	return rets;
}
vector<unsigned char> compile_statement();

void get_arg_name(vector<string> vs, vector<string> & arg_name, vector<string> & arg_type) {
	arg_name.clear(), arg_type.clear();
	vector<string> tmp;
	for(int i = 0; i < vs.size(); i++) {
		tmp = remove_all(split(vs[i], ':'), ":");
		arg_name.push_back(tmp[0]);
		if(tmp.size() <= 1) {
			arg_type.push_back("any");
		}
		else {
			arg_type.push_back(tmp[1]);
		}
	}
}

vector<unsigned char> make_function(string & fname, string prefix = "") {
	vector<unsigned char> rets;
	Token func_name = next_token();
	Token param_list = next_token();
	param_list = MIDDLE(param_list.str);
	vector<string> args;
	vector<string> arg_type;
	get_arg_name(remove_all(split(param_list.str, ','), ","), args, arg_type);
	
	vector<unsigned char> func, tmp, tmp2;
	for(int i = 0; i < arg_type.size(); i++) {
		if(arg_type[i] == "any") continue;
		string type = arg_type[i];
		tmp2 = load_int(i);
		for(int i = 0; i < tmp2.size(); i++) tmp.push_back(tmp2[i]);
		tmp2 = load_str(type);
		for(int i = 0; i < tmp2.size(); i++) tmp.push_back(tmp2[i]);
		tmp.push_back(PTYPE);
		for(int i = 0; i < tmp.size(); i++) func.push_back(tmp[i]);
	}
	if(next_token_is(":")) {
		read_token(":");
		string ret_type = next_token().str;
		tmp = load_str(ret_type);
		tmp.push_back(RTYPE);
		for(int i = 0; i < tmp.size(); i++) func.push_back(tmp[i]);
	}
	
	fname = func_name.str;
	
	string fname2 = prefix + fname;
	all_func.push_back(fname2);
	int func_id = get_id(fname2);
	int id = func_id;
	
	tmp = compile_statement();
	for(int i = 0; i < tmp.size(); i++) func.push_back(tmp[i]);
	vector<int> regs;
	func.push_back(LD0);
	func.push_back(RET); 
	for(int i = 0; i < func.size(); i++) {
		byte_map[func_id][i] = func[i];
	}
	byte_block_len[func_id] = func.size();
	byte_map[func_id][func.size()] = END;
	for(int i = 0; i < args.size(); i++) regs.push_back(get_id(args[i]));
	reg_map[func_id] = regs;
	
	// Set func_name = func_id
	vector<unsigned char> lint = load_func(fname2);
	for(int i = 0; i < lint.size(); i++) rets.push_back(lint[i]);
	if(short_op && id >= 0 && id <= 0xff) {
		rets.push_back(STOREC);
		rets.push_back(id);
	}
	else {
		rets.push_back(STORE);
		rets.push_back(id / 256);
		rets.push_back(id % 256);
	}
	return rets;
}

vector<unsigned char> compile_statement() {
	vector<unsigned char> rets;
	Token flag = next_token();
	tabcnt++;
	if(flag == BEGIN_LABEL) {
		vector<unsigned char> rets2 = compile_block();
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
		read_token(END_LABEL);
	}
	else if(flag == "if") {
		Token expr = next_token();
		vector<unsigned char> rets2 = compile_expression(expr.str);
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
		vector<unsigned char> if_branch;
		vector<unsigned char> rets3 = compile_statement();
		for(int i = 0; i < rets3.size(); i++) if_branch.push_back(rets3[i]);
		if_branch.push_back(JMP);
		if_branch.push_back(0x00);
		if_branch.push_back(0x00);
		int jump_loc = if_branch.size();
		rets.push_back(JMPN);
		rets.push_back(jump_loc / 256);
		rets.push_back(jump_loc % 256);
		vector<unsigned char> else_branch;
		if(next_token_is("else")) {
			read_token("else");
			vector<unsigned char> rets4 = compile_statement();
			for(int i = 0; i < rets4.size(); i++) else_branch.push_back(rets4[i]);
			int jumploc2 = else_branch.size();
			if_branch[if_branch.size() - 2] = jumploc2 / 256;
			if_branch[if_branch.size() - 1] = jumploc2 % 256;			
		}
		for(int i = 0; i < if_branch.size(); i++) rets.push_back(if_branch[i]);
		for(int i = 0; i < else_branch.size(); i++) rets.push_back(else_branch[i]);
	}
	else if(flag == "function") {
		string t;
		vector<unsigned char> rets2 = make_function(t);
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
	}
	else if(flag == "return") {
		string str = "";
		string cur = next_token().str;
		while(cur != EOL_LABEL) str += cur, cur = next_token().str;
		vector<unsigned char> rets2 = compile_expression(str);
		rets2.push_back(RET);
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
	}
	else if(flag == "while") {
		string expr = next_token().str;
		int loop_head = rets.size();
		vector<unsigned char> rets2 = compile_expression(expr);
		rets2.push_back(JMPN);
		rets2.push_back(0x00); // Place Hol
		rets2.push_back(0x00);
		vector<unsigned char> loop_body;
		vector<unsigned char> rets3 = compile_statement();
		for(int i = 0; i < rets3.size(); i++) loop_body.push_back(rets3[i]);
		
		vector<unsigned char> suf_work; 
		suf_work.push_back(RESET);
		suf_work.push_back(LOOP);
		suf_work.push_back(0x00);
		suf_work.push_back(0x00); // Place Holder
		
		int jump_loc = loop_body.size() + suf_work.size();
		int loop_loc = loop_body.size() + suf_work.size() + rets2.size();
		suf_work[suf_work.size() - 2] = loop_loc / 256; // Replace Loop
		suf_work[suf_work.size() - 1] = loop_loc % 256;
		rets2[rets2.size() - 2] = jump_loc / 256; // Replace Checker
		rets2[rets2.size() - 1] = jump_loc % 256;
		
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
		for(int i = 0; i < loop_body.size(); i++) rets.push_back(loop_body[i]);
		int loop_cntn = rets.size();
		for(int i = 0; i < suf_work.size(); i++) rets.push_back(suf_work[i]);
		int loop_break = rets.size();
		
		replace_holder(loop_break, loop_cntn, loop_head, rets);
	}
	else if(flag == "local") {
		string t = "";
		while((t = next_token().str) != EOL_LABEL) {
			int id = get_id(t);
			if(next_token_is("=")) {
				next_token();
				string str = "";
				while(!next_token_is(",") && !next_token_is(EOL_LABEL)) str += next_token().str;
				vector<unsigned char> rets2 = compile_expression(str);
				for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
			}
			else {
				rets.push_back(LDNUL);
			}
			if(short_op && id >= 0 && id <= 0xff) {
				rets.push_back(LSTOREC);
				rets.push_back(id);
			}
			else {
				rets.push_back(LSTORE);
				rets.push_back(id / 256);
				rets.push_back(id % 256);
			}
			if(!next_token_is(EOL_LABEL)) read_token(",");
		}
	}
	else if(flag == "for") {
		string cond = next_token().str;
		cond = MIDDLE(cond);
		vector<string> vars = remove_all(split(cond, ','), ",");
		string var = vars[0];
		string step = vars[3];
		string first = "(" + vars[1] + ")";
		string expr = vars[2];
		int var_id = get_id(var);
		vector<unsigned char> fret = compile_expression(first);
		for(int i = 0; i < fret.size(); i++) rets.push_back(fret[i]);
		if(short_op && var_id >= 0 && var_id <= 0xff) {
			rets.push_back(LSTOREC);
			rets.push_back(var_id);
		}
		else {
			rets.push_back(LSTORE);
			rets.push_back(var_id / 256);
			rets.push_back(var_id % 256);
		}
		
		int loop_head = rets.size();
		vector<unsigned char> rets2 = compile_expression(expr); // Condition
		rets2.push_back(JMPN);
		rets2.push_back(0x00); // Place Holder
		rets2.push_back(0x00);
		
		vector<unsigned char> loop_body;
		vector<unsigned char> part = compile_statement();
		
		for(int i = 0; i < part.size(); i++) loop_body.push_back(part[i]); // Loop body
		
		vector<unsigned char> loop_body2;
		
		vector<unsigned char> stepr = compile_expression(step);
		for(int i = 0; i < stepr.size(); i++) loop_body2.push_back(stepr[i]); // Step Modifying
		if(short_op && var_id >= 0 && var_id <= 0xff) {
			loop_body2.push_back(LOADC);
			loop_body2.push_back(var_id);
		}
		else {
			loop_body2.push_back(LOAD);
			loop_body2.push_back(var_id / 256);
			loop_body2.push_back(var_id % 256);
		}
		loop_body2.push_back(ADD); 
		if(short_op && var_id >= 0 && var_id <= 0xff) {
			loop_body2.push_back(LSTOREC);
			loop_body2.push_back(var_id);
		}
		else {
			loop_body2.push_back(LSTORE);
			loop_body2.push_back(var_id / 256);
			loop_body2.push_back(var_id % 256);
		}
		
		loop_body2.push_back(RESET); // End of loop
		loop_body2.push_back(LOOP);
		loop_body2.push_back(0x00);
		loop_body2.push_back(0x00); // Place Holder
		
		int jump_loc = loop_body.size() + loop_body2.size();
		int loop_loc = loop_body.size() + loop_body2.size() + rets2.size();
		
		loop_body2[loop_body2.size() - 2] = loop_loc / 256; // Replace Loop
		loop_body2[loop_body2.size() - 1] = loop_loc % 256;
		
		rets2[rets2.size() - 2] = jump_loc / 256; // Replace Checker
		rets2[rets2.size() - 1] = jump_loc % 256;
		
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]); // Initializing
		for(int i = 0; i < loop_body.size(); i++) rets.push_back(loop_body[i]); // Loop Body
		int loop_cntn = rets.size();
		for(int i = 0; i < loop_body2.size(); i++) rets.push_back(loop_body2[i]); // Step Modifying
		int loop_break = rets.size();
		
		replace_holder(loop_break, loop_cntn, loop_head, rets);
	}
	else if(flag == "foreach") {
		string cond = next_token().str;
		cond = MIDDLE(cond);
		vector<string> vars = remove_all(split(cond, ','), ",");
		string real_index = vars[0];
		string var = vars[0] + "__iterator";
		string first = "0";
		string expr = var + " < len(" + vars[1] + ")";
		string step = "1";
		string append = real_index + " = " + vars[1] + "[" + var + "]";
		int var_id = get_id(var);
		int read_id = get_id(real_index);
		vector<unsigned char> fret = compile_expression(first);
		for(int i = 0; i < fret.size(); i++) rets.push_back(fret[i]);
		if(short_op && var_id >= 0 && var_id <= 0xff) {
			rets.push_back(LSTOREC);
			rets.push_back(var_id);
		}
		else {
			rets.push_back(LSTORE);
			rets.push_back(var_id / 256);
			rets.push_back(var_id % 256);
		}
		
		int loop_head = rets.size();
		
		vector<unsigned char> rets2 = compile_expression(expr);
		rets2.push_back(JMPN);
		rets2.push_back(0x00); // Place Holder
		rets2.push_back(0x00);
		vector<unsigned char> loop_body;
		
		vector<unsigned char> part = compile_expression(append); // Index
		
		vector<unsigned char> rets3 = compile_statement();
		for(int i = 0; i < rets3.size(); i++) part.push_back(rets3[i]);
		for(int i = 0; i < part.size(); i++) loop_body.push_back(part[i]); // Loop body
		
		vector<unsigned char> loop_body2;
		vector<unsigned char> stepr = compile_expression(step); // Step Modifying
		for(int i = 0; i < stepr.size(); i++) loop_body2.push_back(stepr[i]);
		if(short_op && var_id >= 0 && var_id <= 0xff) {
			loop_body2.push_back(LOADC);
			loop_body2.push_back(var_id);
		}
		else {
			loop_body2.push_back(LOAD);
			loop_body2.push_back(var_id / 256);
			loop_body2.push_back(var_id % 256);
		}
		loop_body2.push_back(ADD); 
		if(short_op && var_id >= 0 && var_id <= 0xff) {
			loop_body2.push_back(LSTOREC);
			loop_body2.push_back(var_id);
		}
		else {
			loop_body2.push_back(LSTORE);
			loop_body2.push_back(var_id / 256);
			loop_body2.push_back(var_id % 256);
		}
		
		loop_body2.push_back(RESET);
		loop_body2.push_back(LOOP);
		loop_body2.push_back(0x00);
		loop_body2.push_back(0x00); // Place Holder
		
		int jump_loc = loop_body.size() + loop_body2.size();
		int loop_loc = loop_body.size() + loop_body2.size() + rets2.size();
		loop_body2[loop_body2.size() - 2] = loop_loc / 256; // Replace Loop
		loop_body2[loop_body2.size() - 1] = loop_loc % 256;
		rets2[rets2.size() - 2] = jump_loc / 256; // Replace Checker
		rets2[rets2.size() - 1] = jump_loc % 256;
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
		for(int i = 0; i < loop_body.size(); i++) rets.push_back(loop_body[i]);
		int loop_cntn = rets.size();
		for(int i = 0; i < loop_body2.size(); i++) rets.push_back(loop_body2[i]);
		int loop_break = rets.size();
		
		replace_holder(loop_break, loop_cntn, loop_head, rets);
	}
	else if(flag == "break") {
		rets.push_back(BREAK);
		rets.push_back(0x00);
		rets.push_back(0x00);
		read_token(EOL_LABEL); // Check EOL
	}
	else if(flag == "continue") {
		rets.push_back(CNTN);
		rets.push_back(0x00);
		rets.push_back(0x00);
		read_token(EOL_LABEL); // Check EOL
	}
	else if(flag == "#END_TOKEN#") {
		;
	}
	else if(flag == "struct") {
		Token class_name = next_token();
		Token super_name = Token("Object");
		if(next_token_is("extends")) {
			read_token("extends");
			super_name = next_token();
		} 
		Token param_list = next_token();
		read_token(EOL_LABEL);
		param_list = MIDDLE(param_list.str);
		vector<string> vtype;
		vector<string> fields = remove_all(split(param_list.str, ','), ",");
		for(int i = 0; i < fields.size(); i++) vtype.push_back("any");
		int func_id = get_id(class_name.str);
		int id = func_id;
		vector<unsigned char> lint = make_struct(class_name.str, super_name.str, fields, vector<char>(), vtype);
		for(int i = 0; i < lint.size(); i++) rets.push_back(lint[i]);
		lint = load_func(class_name.str);
		for(int i = 0; i < lint.size(); i++) rets.push_back(lint[i]);
		if(short_op && id >= 0 && id <= 0xff) {
			rets.push_back(STOREC);
			rets.push_back(id);
		}
		else {
			rets.push_back(STORE);
			rets.push_back(id / 256);
			rets.push_back(id % 256);
		}
	}
	else if(flag == "class") {
		vector<unsigned char> rets2 = make_class();
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
	}
	else {
		string str = flag.str;
		string cur = next_token().str;
		while(cur != EOL_LABEL) str += cur, cur = next_token().str;
		vector<unsigned char> rets2 = execute(str);
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
	}
	tabcnt--;
	return rets;
}
vector<unsigned char> compile_block() {
	vector<unsigned char> rets;
	vector<unsigned char> rets2;
	while(has_next()) {
		rets2 = compile_statement();
		for(int i = 0; i < rets2.size(); i++) rets.push_back(rets2[i]);
		if(next_token_is(END_LABEL)) break;
	}
	return rets;
}

int main_id;
vector<unsigned char> make_lambda(string expr, string param_list) {
	vector<unsigned char> rets;
	expr = MIDDLE(expr);
	string func_name = random_name();
	all_func.push_back(func_name);
	param_list = MIDDLE(param_list);
	vector<string> args = remove_all(split(param_list, ','), ",");
	int func_id = get_id(func_name);
	vector<unsigned char> func = compile_expression(expr);
	vector<int> regs;
	func.push_back(RET);
	func.push_back(LD0);
	func.push_back(RET); 
	for(int i = 0; i < func.size(); i++) {
		byte_map[func_id][i] = func[i];
	}
	byte_block_len[func_id] = func.size();
	byte_map[func_id][func.size()] = END;
	for(int i = 0; i < args.size(); i++) regs.push_back(get_id(args[i]));
	reg_map[func_id] = regs;
	int id = func_id;
	// Set func_name = func_id
	vector<unsigned char> lint = load_func(func_name);
	for(int i = 0; i < lint.size(); i++) rets.push_back(lint[i]);
	if(short_op && id >= 0 && id <= 0xff) {
		rets.push_back(STOREC);
		rets.push_back(id); 
	}
	else {
		rets.push_back(STORE);
		rets.push_back(id / 256);
		rets.push_back(id % 256);
	}
	if(short_op && id >= 0 && id <= 0xff) {
		rets.push_back(LOADC);
		rets.push_back(id);
	}
	else {
		rets.push_back(LOAD);
		rets.push_back(id / 256);
		rets.push_back(id % 256);
	}
	return rets;
}
void save_bytecode(string file) {
	ofstream fcout(file.c_str(), ios::binary);
	fcout << all_func.size() << ' ' << main_id << ' ';
	fcout << cpool_pos << ' ';
	for(int i = 0; i < cpool_pos; i++) {
		if(cpool[i].type == STR_TYPE) fcout << '"';
		fcout << cpool[i].to_str() << ' ';
	}
	for(int i = 0; i < all_func.size(); i++) {
		string func = all_func[i];
		int id = get_id(func);
		fcout << func << " ";
		fcout << id << " ";
		fcout << reg_map[id].size() << " ";
		for(int j = 0; j < reg_map[id].size(); j++) fcout << reg_map[id][j] << " ";
		stringstream sfcout("");
		int len = byte_block_len[id];
		for(int i = 0; i < len; i++) sfcout << byte_map[id][i];
		fcout << len << " " << sfcout.str() << " ";
	}
}
void read_bytecode(string file) {
	memset(byte_map, END, sizeof(byte_map));
	ifstream fcin(file.c_str(), ios::binary);
	if(!fcin) {
		error("bytefile " + file + " not found.", "FileError");
	}
	int all, ccnt;
	fcin >> all >> main_id;
	fcin >> ccnt;
	string d;
	for(int i = 0; i < ccnt; i++) {
		fcin >> d;
		if(d[0] == '"') addconst(d.substr(1, d.length()));
		else addconst(atof(d.c_str()));
	}
	for(int i = 0; i < all; i++) {
		string func;
		int id;
		int pcnt;
		int len;
		fcin >> func >> id >> pcnt;
		var_id[func] = id;
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
stringstream all_source("");
void read_file_source(string file) {
	ifstream fcin(file.c_str());
	if(!fcin) {
		error("file " + file + " not found.", "FileError");
	}
	string str;
	while(getline(fcin, str)) {
		if(str == "") continue;
		str = trim(str);
		vector<string> tokens = tokenize(str);
		if(tokens[0] == "include") {
			string f = "";
			for(int i = 1; i < tokens.size(); i++) {
				f += tokens[i];
			}
			read_file_source(f);
		}
		else all_source << str << endl;
	}
}
void read_cli_source() {
	string str;
	cout << ">>> ";
	while(getline(cin, str)) {
		if(str == "") {
			printf(">>> ");
			continue;
		}
		str = trim(str);
		if(str == "exit") return;
		vector<string> tokens = tokenize(str);
		if(tokens[0] == "include") {
			string f = "";
			for(int i = 1; i < tokens.size(); i++) {
				f += tokens[i];
			}
			read_file_source(f);
		}
		else all_source << str << endl;
		cout << ">>> ";
	}	
}

void cli_read() {
	read_cli_source();
}

const string LAUNCH_BLOCK = "main";
void start_compile() {
	memset(byte_map, END, sizeof(byte_map));
	all_func.push_back(LAUNCH_BLOCK);
	string str;
	main_id = get_id(LAUNCH_BLOCK);
	int line = 0;
	while(getline(all_source, str)) {
		line++;
		str = trim(str);
		vector<string> tokens = tokenize(str);
		for(int i = 0; i < tokens.size(); i++) tokens[i] = trim(tokens[i]);
		for(int i = 0; i < tokens.size(); i++) {
			add_token(tokens[i], line);
		}
		if(tokens[tokens.size() - 1] != BEGIN_LABEL && tokens[tokens.size() - 1] != END_LABEL) {
			add_token(EOL_LABEL, line);
		}
	}
	vector<unsigned char> r = compile_block();
	for(int i = 0; i < r.size(); i++) {
		byte_map[main_id][i] = r[i];
	}
	byte_block_len[main_id] = r.size();
	byte_map[main_id][r.size()] = END;
}

void start_vm() {
	run_byte(func_val(LAUNCH_BLOCK), 0, NULL, 0).to_str();
} 

const string HEX_CHAR = "0123456789ABCDEF";

string to_hex(double _x) {
	string str = "";
	int x = (int)_x;
	while(x) str += HEX_CHAR[x % 16], x /= 16;
	reverse(str.begin(), str.end());
	if(str.length() == 0) str = "00";
	if(str.length() == 1) str = "0" + str;
	return str;
}

void debug_output() {
	for(int f = 0; f < all_func.size(); f++) {
		cout << "function " << all_func[f] << endl;
		int id = get_id(all_func[f]);
		cout << "\tid " << id << endl;
		for(int i = 0; i < byte_block_len[id]; i++) {
			unsigned char b = byte_map[id][i];
			cout << "\t#" << i << "\t" << to_hex((int)b) << "\t" << opname[b];
			if(b == JMP || b == JMPN|| b == BREAK || b == CNTN) {
				cout << "\tJUMP TO " << i + 2 + byte_map[id][i + 1] * 256 + byte_map[id][i + 2];
			}
			if(b == LOOP || b == LOOPIF) {
				cout << "\tLOOP TO " << i + 2 - byte_map[id][i + 1] * 256 - byte_map[id][i + 2];
			}
			cout << endl;
		}
	}
	
	cout << "ID\tVar" << endl;
	for(map<string, int>::iterator it = var_id.begin(); it != var_id.end(); it++) {
		string name = (*it).first;
		int id = (*it).second;
		cout << id << "\t" << name << endl;
	}
}

const string EXTENSION = "rbq";
string replace_ext(string file) {
	for(int i = file.length() - 1; i >= 0; i--) {
		if(file[i] == '.') {
			string prefix = file.substr(0, i);
			return prefix + "." + EXTENSION;
		}
	}
	return file + "." + EXTENSION;
}

string full_filename(string file) {
	for(int i = file.length() - 1; i >= 0; i--) {
		if(file[i] == '.') {
			string prefix = file.substr(0, i);
			return file;
		}
	}
	return file + "." + EXTENSION;
}

bool compile = false;
bool run = false;
bool cli = true;
bool save = false;
string input_file = "";
string output_file = "";

int main(int argc, char ** argv) {
	init();
	init2();
	if(argc == 1) {
		cout << VM_NAME << ' ' << VM_VERSION << endl;
		cli_read();
		start_compile();
		if(debug) debug_output();
		start_vm();
		return 0;
	}
	if(argc == 2) {
		read_bytecode(full_filename(argv[1]));
		start_vm();
		return 0;
	}
	for(int i = 1; i < argc; i++) {
		string p = argv[i];
		for(int j = 0; j < p.length(); j++) p[i] = tolower(p[i]);
		if(p == "-i") {
			i++;
			cli = false;
			save = true;
			if(i >= argc) error("need a file name after option '-i'", "SyntaxError");
			input_file = argv[i];
			output_file = replace_ext(input_file);
		}
		else if(p == "-o") {
			i++;
			save = true;
			if(i >= argc) error("need a file name after option '-o'", "SyntaxError");
			output_file = argv[i];
		}
		else if(p[0] == '-' && p[1] != 'x') {
			for(int j = 1; j < p.length(); j++) {
				if(p[j] == 'c') compile = true;
				if(p[j] == 'r') run = true;
				if(p[j] == 'd') debug = true;
			}
		}
		else if(p == "-xnobig") {
			enable_big = false;
		}
		else if(p == "-xfull") {
			short_op = false;
		}
		else if(p == "-xnocheck") {
			not_check = true;
		}
	}
	
	if(cli) {
		cli_read();
	}
	else {
		read_file_source(input_file);
 	}
	if(compile) {
		start_compile();
	}
	if(debug) {
		debug_output();
	}
	if(save) {
		save_bytecode(output_file);
	}
	if(run) start_vm();
	return 0;
}
