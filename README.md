# RBQScript

### update 15th Apr 2022

- 加快了虚拟机的启动时间。
- 完善了防非法访问内存的保护机制。
- 增加了动态申请内存，运行所耗内存现在和运行时间和操作数量成正比。
- 修复部分 0 day bug。

### update 25th Mar 2022

- 修复了单引号带空格字符串无法正确编译的问题。详情可见：https://github.com/WarfarinBloodanger/rbqscript/issues/3#
- 修复了在特定情况下的带括号字符串无法正确编译的问题。
- 添加新内置函数 `read_eof`，用来检测键盘读入是否达到输入末尾（`Ctrl+Z`）。如下示例程序：可以不断读入两个高精度整数（即 `big_number` 类型）直到用户输入 Ctrl+Z 终止为止：

```
while(!read_eof()) print((lambda {x, y} {x * y})(big(read_string()), big(read_string())))  
```
- 修复部分 0 day bug；

### update 24th Mar 2022

- 修复了 `rbq xxx.rbq` 的命令行启动方式无法运行文件的错误；
- 修复了带空格符的字符串编译后，存在的读入问题。详情可见：https://github.com/WarfarinBloodanger/rbqscript/issues/2
- 修复部分 0 day bug；
- 在 rbqup 中添加部分被漏掉的字节码；


### update 26th Feb 2022

- 修复了对象不会被垃圾回收的 bug；

- 修复了对象垃圾回收错误的 bug；
- 修复了垃圾回收后内存未释放的 bug；
- 加入了函数重载检查机制（见下），可以通过选项 `-xnochkoverload` 关闭；
- 加入了 `overload` 关键字，用法如下：

```

function test() {
	print("Test 1")
}
test()
function test() {
	print("Test 2")
}
test()
function overload test() {
	print("Test 3")
}
test()
function another() {
	print("Test 4")
}
test = another
test()
```

其中，第二个函数重载会触发 `OverloadWarning` 触发警告，因为它用 `function` 关键字显式声明了一个重名的函数，但是没有用 `overload` 标识。但是第四个不是显式声明，而是赋值，并不会触发警告。

请注意：**显式重载的函数会以最后一个函数为准**，也就是说，如上示例代码应当输出三次 `Test 3`。

- 加入了 `Sequence`、`Math` 和 `Algorithm` 库，位于代码仓库根目录下的 `lib` 目录中。

### update 22nd Feb 2022


- 加入了异常追溯；
- 加入了 `_md5` 内置函数；
- 加入了变量名检查机制，可以通过在命令行选项中开启 `-nochkname` 来禁用检查；-
- 加入了有效变量名的查验机制，现在不再会对 `;` 之类的意外符号作 `NameError` 报错；

- 将 function 类型细化，分为 built-in function、`native-function` 和 `function`；
- 将命令行变为交互式命令行；

- 删除了诸如 (1:3) 的字面量，加入了 range(1, 3) 作为替代品；

***

弱类型脚本语言。

请注意：**与 4.0 之前的版本不兼容**。

## Part 1. RBQScript 启动方式：

1. 首先找到要下载的版本，如 rbq-472 文件夹内下载 4.72 版本；
2. 下载 fvm.cpp（或 rbq.cpp），使用 C++ 编译器（作者编译器版本：C++ 11 TDM GCC 4.8.1 64-bit Release）编译生成 fvm.exe。

编译器兼解释器：`fvm.exe`。

总命令格式：
```
fvm [<byte-file>] | [[-i <input-file>] [-o <output-file>] [<options>] [<switches>]] 
```

格式一：`fvm [<byte-file>]`，将会执行 `<byte-file>` 中的字节码。如果检测到 `<bytecode-file>` 没有拓展名，将会自动加上一个 `.rbq` 作为文件名，否则直接使用它作为文件名。

格式二：`fvm -i <input-file> -o <output-file> [<options>] {<switches>}`。

将会从 `<input-file>` 里面读入源代码文件，以 `<options>` 和 `<switches>` 为附加条件进行编译后输出字节码文件到 `<output-file>`。

如果有输入文件没有输出文件，将会默认将输入文件名的后缀改为 `.rbq`。

options 格式：`-{<op-symbol>}`。op-symbol 表示执行不同操作，包含如下几个选项：

- `c` 编译；
- `r` 运行，注意：必须要包含 `c` 选项才能运行；
- `d` 调试，会在编译结束之后输出所有函数的字节码和符号表。

如：`-crd` 表示编译、运行加调试；`-cr` 表示编译运行；而 `-c` 表示只编译不运行。

switches 包含如下几个选项：

- `-xnobig`：禁止程序自动将过大的实数转化为高精度；
- `-xfull`：禁止程序使用简单版字节码；
- `-xnocheck`：禁用访问检查器，允许访问内部变量。

另有一个调试用工具 `rbqup.exe` 调用格式如下：

`rbqup <bytecode-file> [en|ch|op]`

这将会把 `<bytecode-file>` 里面的字节码进行标记后输出到控制台并着色。第三个参数，en 表示使用英文标注，ch 表示使用中文标注，op 表示使用助记符标注，如果没有第三个参数则默认为 en 模式。

如果检测到 `<bytecode-file>` 没有拓展名，将会自动加上一个 `.rbq` 作为文件名，否则直接使用它作为文件名。

## Part 2. RBQScript 语法

RBQScript 的语法非常简单，有 JavaScript 开发经验的程序员将会更容易的上手 RBQScript。

- 1. 变量、赋值、表达式；

**普通**变量无需声明，可以直接赋值：`a = 1`。注意，所有语句**一行一条**。

RBQScript 内置以下几种值类型：
- 实数，为 IEEE 规范下 C/C++/Java 中的 double 类型；
- 高精度整数，没有小数部分；
- 字符串；
- 函数（闭包）；
- 数组；
- null，表示空；
- 范围，表示一个下标的范围。

变量可以定义成局部变量。格式是在变量前面加上 `local`。
```
a = 0
function f() {
    local a = 1
    print(a)
}
f()
print(a)
```
如果定义了一个局部变量但不赋初值，那么这个变量将被自动初始化成 null。另外：全局变量不会自动初始化，均为 undefined 未定义类型。

注意：这里的局部变量是以**函数**为作用域的，不存在块作用域。也就是说如下代码能够正常运行：
```
if(true) {
	a = 10
}
print(a)
```

实数可以有两种表示方式：十进制表示和十六进制表示，如：

```
num = 1241
x = 0xff0
```

注意：当计算过程中，实数的绝对值超过了 0x7FFFFFFF 时，将会被自动转换为高精度（损失小数部分精度）进行运算，**可以使用** `-xnobig` **禁止该行为**。

字符串字面量为双引号或单引号括起来的字符序列，如：
```
s = "hello"
name = 'rbq'
```

可以用反斜杠 `\\` 进行转义。另外有特殊的一个转义字符：`\\u0x????`，将会把十六进制的 `???` 转化为 Unicode 字符。例如：`"\\'\\uC4E3"` 是字符串 `'你`。

数组字面量用大括号包围的逗号分隔的一个或多个值组成，如 `a = {1, 2, "hello"}` 。

- 2. 控制流程语句；

- if 语句：
```
if (<condition>) {
    ...
}
else {
    ...
}
```

允许 if-else-if 级联语句。

- while 语句：
```
while(<condition>) {
    ...
}
```

- for 语句：
```
for(<name>, <first-value>, <condition>, <step-value>) {
    ...
}
```

这等价于 JavaScript 中的：`for(var <name> = <first-value>; <condition>; <name> = <name> + <step-value>)`。 

例如：`for(i, 1, i <= 10, 1)` 等价于 JavaScript 中的 `for(var i = 1; i <= 10; i = i + 1)` 。

- foreach 语句：
```
foreach(<name>, <iterable-value>) {

}
```

这将会迭代 `<iterable-value>` 中所有的值，**关联数组中的字符串下标不会被迭代**。可迭代的值类型有：字符串、数组、范围。

例如：
```
foreach(i, {1, 2, 4, 4, 1}) print(i)
```

特殊：你可以使用 `<name>__iterator` 来访问到迭代器的下标，如：

```
foreach(i, "abcdef") print(i, i__iterator)
```

将会在输出给定字符串的每个字符的同时在后面跟上字符所在的下标（从 $0$ 到 $6$）。

- break / continue 语句：

将会跳出循环 / 跳回到循环的头部重新开始。

- 3. 函数定义；

基本格式为：
```
function <name>([<arg1> [, <arg2>, <arg3>, ...]]) {

}
```

这将定义一个名为 `<name>` 的函数。请注意：函数都是**当前作用域的局部变量**（可以是全局变量）。

RBQScript 还支持第二种函数定义：Lambda 匿名函数。格式为：`(lambda {<arg-list>} {<expression>})`。

比如：`(lambda {x, y} {x * x + y})` 将会得到一个二元函数，计算 `x * x + y`。

用 `return` 从函数中返回值。

- 4. 其他；

使用 include 语句可以从其他文件中导入**源代码**，必须包含**完整拓展名**。

- 5. 内置函数：

- `print` 输出函数，参数个数不固定，将会把所有参数转化为字符串后**用空格分隔** 输出在同一行；

- `len` 函数接受一个参数，取范围、字符串、数组的长度，如 `len("hello")` 得到 $5$；
- `sub` 函数接受两个参数，取范围中给定下标的值。如 `sub((0:4), 1)` 得到 $2$；
- `time` 和 `clock` 函数不接受参数，分别获得当前时间和程序运行时间；
- `system` 函数接受一个字符串参数，返回执行系统命令的结果；
- `memset` 函数接受参数范围在 $1$ 到 $4$ 之间。第一个参数必须是数组类型；
1. 当只有一个参数时，将会把这个数组全部填充为 $0$（包含关联数组内的所有内容）；
2. 当有两个参数时，将会把这个数组全部填充为第二个参数的值；
3. 当有三个参数时，将会把这个数组从 $0$ 到第三个参数之间的下标全部填充为第二个参数。如果第三个参数是字符串，那么所有数字下标都会被填充成第二个参数；
4. 当有四个参数时，将会把这个数组从第三个参数到第四个参数之间的下标全部填充为第二个参数。
- 数学函数：使用 `math_xxx` 调用，都只接受一个实数参数。其中 `xxx` 可以替换为对应功能：这些功能包含：`sin/cos/tan/asin/acos/atan/sqrt/floor/ceil/log/log10/abs`。如 `math_sin` 计算正弦值。

- read 系列函数：不接受参数，`read_number`、`read_string`、`read_line` 分别读入一个实数、字符串（空白符分割）和一整行（换行符分割）并返回；

- file 系列函数：
1. `file_open` 的第一个参数是文件名，如果有第二个参数，则将其作为打开文件的方式，返回一个整数表示文件的句柄；
2. `file_close` 接受一个整数参数，将会关闭该句柄指向的文件；
3. `file_eof` 接受一个整数参数，如果到达文件尾部则返回 $1$，否则返回 $0$；
4. `file_read_xxx` 系列函数，和 read 系列参数类似的功能，接受一个参数作为文件句柄，从文件中按要求读取给定数据并返回；
5. `file_write_xxx` 系列参数，功能和 read 相反，写入到给定的文件句柄当中；

- `str2ascii` 接受一个字符串参数，如果该字符串长度为 $1$，则返回该字符串的字符的 ASCII 码；否则返回一个数组，依次存储字符串各个字符的 ASCII；

- `ascii2str` 可接受多个参数。
1. 如果第一个参数是数组，那么将该数组中所有的实数值转换为 ASCII 字符后连在一起，作为字符串返回；
2. 否则，将所有参数依次转换为 ASCII 字符后连在一起，作为字符串返回；
3. 上面两条中对于无法转换的非数字值，将会以 NaN 代替。

- `num` 接受一个字符串参数，将其转换为一个实数后返回；
- `map` 不接受参数，生成一个 map 对象；
- `big` 接受一个字符串参数，将其转换为一个高精度整数后返回；
- `serialize` 接受一个任意类型参数，返回一个数组，为参数序列化后的字节流；
- `unserialize` 接受一个数组类型参数，返回一个值，为反序列化后的值。如果不存在该值对应的类，将会创建一个**不包含成员方法**的纯数据类；
- `load_native` 接受两个字符串参数，第一个字符串参数指定本地方法的文件，第二个字符串参数指定函数名，返回一个函数；

### update 6th Dec 2021:

加入了对象系统，语法如下：

```
struct ClassName [extends SuperName] (field1, field2, field3...)
```
这将会定义一个叫做 ClassName 的结构体。目前 RBQScript **struct 不支持成员函数**，因此称作是 struct，不是 class。

构造对象的方法：使用类名作为函数名调用，**从父类开始，按声明的字段顺序写入参数**。如：

```
struct Class(a, b)
struct Class2 extends Class(c, d)
obj = Class("hello", 5 + 9)
obj2 = Class2("world", 3 + 1, 5.433, {1, 2, 3})
print("obj: a is", obj.a, ", b is", obj.b)
print("obj2: a is", obj2.a, ", b is", obj2.b, ", c is", obj2.c, ", d[1] is", obj2.d[1])
```

输出：
```
obj: a is hello , b is 14
obj2: a is world , b is 4 , c is 5.433 , d[1] is 2
```

**推荐自己定义构造函数，如下：**

```
struct Class(a, b)
function make_Class(a) {
    return Class(a, a * a)
}
```

Object 是所有类的基类，在定义的时候不需要显示地写上 `extends Object`，会自动继承。

特殊字段：`obj.class_name` 返回该对象的类的名称，`obj.super_name` 返回该对象的基类的名称。

```
struct Person(name)
struct OIer extends Person(age)
oier = OIer("ff0", 343)
print(oier.name, oier.age, oier.class_name, oier.super_name)
```

输出：

```
ff0 343 OIer Person
```


### update 9th Dec 2021:

正式加入 `class` 关键字，语法：

```
class <classname> extends <supername> {
	var field
	var f1, f2, f3...
	function method1(v) {
		print(this.f1)
	}
	function method2() {
		this.f2 = this.f1 + this.f3
	}
}
```

1. 字段和方法全部都是 public 且非静态类型。

2. **在成员方法内部访问字段必须显式的加上 this 关键字**。

3. 为保持兼容，struct 保留，一切行为不变。

4. **若在成员方法内部声明局部变量，建议显式加上 local 关键字**。

5. **struct 的效率高于 class，若声明纯数据类型的数据复合体，建议使用 struct 而非 class**。

### update 31th Dec 2021

新增一种内置类型：`map` 类，即映射表。

创建方式：`test = map()`。

可以使用任何类型作为 map 的键值，如果该键在 map 中不存在，则会返回 `undefined` 值。

语法：`a[key] = value`。如果 `key` 是字符串类型，可以使用 `a.key = value`。

配套提供了 3 个新内置函数：

- clear_map，接受一个 map 类型的参数，清除 map 中的所有内容；
- keys，接受一个 map 类型的参数，返回一个列表，表示 map 中的所有键名；
- values，接受一个 map 类型的参数，返回一个列表，表示 map 中的所有键值。

示例如下：

```
a = map()
a["test"] = 5
a[4] = "num"
print(a.test)
list = keys(a)
foreach(i, list) print(i, "->", list[i])
```

**Reference**，即列表类型**不再支持字符串下标**。以下代码会报错：

```
a = {}
a["test"] = 5
```

报错：`RuntimeError: "should use 'number' as subscript of 'reference', not 'string'"`。

加入了类型检查机制，语法与 Kotlin / TypeScript 类似。

```
class Foo {
	var x [: param_type], y [: param_type]
}
function add(a [: param_type], b [: param_type]...) [: param_type] {
	return a + b
}
```

类型检查机制是可选的。`any` 可以匹配所有类型。

如果没有在参数 / 函数声明之后加上类型声明，则会默认使用 `any`。**请注意：Object 只会匹配对象类型，内置类型无法匹配。**

将错误提示修改得更加可读，减少了 VerifyError 的数量。

### update 7th Feb 2022

加入了对象序列化 / 反序列化函数（serialize 和 unserialize）。

改写部分代码以适配 C++ 11 标准。

加入了 Windows 和 Linux 系统下的加载本地方法函数（load_native）。

修复部分 0day Bug。


