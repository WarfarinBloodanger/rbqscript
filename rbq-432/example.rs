struct Stack(list, top)
function stack() {
	return Stack({}, 0)
}
function stack_push(stk, v) {
	stk.list[stk.top] = v
	stk.top = stk.top + 1
}
function stack_pop(stk) {
	local ret = stk.list[stk.top - 1]
	stk.top = stk.top - 1
	return ret
}
function stack_gettop(stk) {
	return stk.list[stk.top - 1]
}
function stack_empty(stk) {
	return stk.top == 0
}
s = stack()
foreach(i, (1 : 1000)) stack_push(s, "You accepted P" + (1000 + i))
while(!stack_empty(s)) {
	print(stack_pop(s))
}

struct Class(a, b)
struct Class2 extends Class(c, d)
obj = Class("hello", 5 + 9)
obj2 = Class2("world", 3 + 1, 5.433, {1, 2, 3})
print("a is", obj.a, "b is", obj.b)
print("a is", obj2.a, "b is", obj2.b, "c is", obj2.c, "d is", obj2.d[1])

struct Person(name)
struct OIer extends Person(age)
oier = OIer("ff0", 343)
print(oier.name, oier.age, oier.class_name, oier.super_name)
