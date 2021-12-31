class Complex {
	var x : number
	var y : number
	function add(b : Complex) : Complex {
		return Complex(this.x + b.x, this.y + b.y)
	}
	function sub(b : Complex) : Complex {
		return Complex(this.x - b.x, this.y - b.y)
	}
	function mul(b : Complex) : Complex {
		return Complex(this.x * b.x - this.y * b.y, this.x * b.y + this.y * b.x)
	}
}

function conv(f : reference, flag : number) {
	foreach(i, (0 : n)) {
		if(i < tr[i]) {
			local tmp = f[i]
			f[i] = f[tr[i]]
			f[tr[i]] = tmp
		}
	}
	local p = 2
	while(p <= n) {
		local length = p >> 1
		local tg = Complex(math_cos(2 * pi / p), math_sin(2 * pi / p))
		if(flag == 0) tg.y = tg.y * (-1)
		local k = 0
		while(k < n) {
			local buf = Complex(1, 0)
			for(l, k, l < k + length, 1) {
				local tt = buf.mul(f[length + l])
				f[length + l] = f[l].sub(tt)
				f[l] = f[l].add(tt)
				buf = buf.mul(tg)
			} 
			k = k + p
		}
		p = p << 1
	}
}

p = {}
f = {}
memset(p, 0)
memset(f, 0)
pi = math_acos(-1)
tr = {}

n = read_number()
m = read_number()
foreach(i, (0 : (n + 1))) {
	p[i] = 0
	f[i] = 0
}
foreach(i, (0 : (n + 1))) {
	f[i] = read_number()
}
foreach(i, (0 : (m + 1))) {
	p[i] = read_number()
}

m = m + n
n = 1
while(n <= m) {
	n = n << 1
}
foreach(i, (0 : (n + 1))) {
	p[i] = Complex(p[i], 0)
	f[i] = Complex(f[i], 0)
}
foreach(i, (0 : (n + 1))) tr[i] = 0
foreach(i, (0 : n)) {
	tr[i] = (tr[i >> 1] >> 1)
	if(i & 1) tr[i] = tr[i] | (n >> 1) 	
}
conv(f, 1)
conv(p, 1)
foreach(i, (0 : n)) f[i] = f[i].mul(p[i])
conv(f, 0)
foreach(i, (0 : (m + 1))) print(f[i].x / n)
