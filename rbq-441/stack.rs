class Stack {
	var list
	var top
	function push(v) {
		this.list[this.top] = v
		this.top = this.top + 1
	}
	function pop() {
		local v = this.list[this.top - 1]
		this.top = this.top - 1
		return v
	}
	function get_top() {
		return this.list[this.top - 1]
	}
	function clear() {
		this.top = 0
	}
	function empty() {
		return this.top == 0
	}
}


s = Stack({}, 0)
foreach(i, (1984 : 2021)) s.push("AK NOI-" + i)
while(!s.empty()) print(s.pop())

