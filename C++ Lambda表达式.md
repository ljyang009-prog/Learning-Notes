# C++ Lambda表达式 

## 一、Lambda 是什么

**Lambda 表达式 = 匿名内联函数**
不需要单独定义函数、仿函数，直接写在代码中，**最常用于 STL 算法（sort、find\_if、remove\_if 等）充当谓词**。
C\+\+11 及以上标准支持。

### 基础语法格式

```cpp
[捕获列表](参数列表) -> 返回类型 {函数体};
```

拆解各部分：

1. `[]` **捕获列表**：捕获外部作用域变量（最重要）

2. `()` 参数列表：和普通函数形参一致

3. `-> 返回类型`：尾置返回类型，可以省略（自动推导）

4. `{}` 函数体：逻辑代码

最简形式：

```cpp
[](){ std::cout << "hello lambda"; }(); 
// 末尾()代表立即调用这个lambda
```

## 二、捕获列表规则【核心考点】

|捕获写法|含义|
|---|---|
|`[]`|不捕获任何外部变量|
|`[=]`|**值捕获**：所有外部变量拷贝一份，只读|
|`[&]`|**引用捕获**：所有外部变量引用，可以修改原值|
|`[x]`|仅值捕获变量 x|
|`[&x]`|仅引用捕获变量 x|
|`[=, &x]`|默认值捕获，唯独 x 采用引用捕获|
|`[&, x]`|默认引用捕获，唯独 x 采用值捕获|
|`[this]`|捕获当前类 this 指针（成员函数内使用）|

> ⚠重点：**值捕获的变量默认无法修改**，想要修改需要加关键字 `mutable`
> 
> 

### 示例 1：值捕获

```cpp
#include <iostream>
using namespace std;

int main()
{
    int a = 10;
    auto func = [=](){
        // a是拷贝值，只读，不能 a++
        cout << a << endl;
    };
    func();
    return 0;
}
```

### 示例 2：引用捕获（可修改外部变量）

```cpp
int main()
{
    int a = 10;
    auto func = [&](){
        a++; 
        cout << a << endl;
    };
    func(); // 输出11
    cout << a << endl; // 原值被修改
    return 0;
}
```

### 示例 3：mutable 解除值捕获只读限制

```cpp
int main()
{
    int a = 10;
    auto func = [=]() mutable {
        a++; // 修改的是拷贝副本，不影响外部原始a
        cout << a << endl;
    };
    func();
    cout << a << endl; // 仍然是10
    return 0;
}
```

## 三、返回类型

1. **自动推导（常用，省略 \-\> type）**
函数体内只有一条 return 语句，编译器自动识别返回类型

```cpp
auto add = [](int x, int y){
    return x + y;
};
```

2. **显式指定返回类型（多条 return、类型不一致时使用）**

```cpp
auto func = [](int x) -> double {
    if(x > 0)
        return 1.5;
    else
        return 2.5;
};
```

## 四、Lambda 与 STL 算法结合【重中之重】

STL 算法大量需要**一元谓词 / 二元谓词**，lambda 是最优方案。

### 案例 1：sort 自定义排序（二元谓词）

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    vector<int> vec = {3,1,9,4,2};
    // 降序排序
    sort(vec.begin(), vec.end(), [](int a, int b){
        return a > b;
    });

    for(auto v : vec) cout << v << " ";
    return 0;
}
```

### 案例 2：find\_if 条件查找（一元谓词）

```cpp
vector<int> vec = {10,20,30,40};
// 找到第一个大于25的数字
auto it = find_if(vec.begin(), vec.end(), [](int x){
    return x > 25;
});
if(it != vec.end())
    cout << *it;
```

### 案例 3：remove\_if 删除符合条件元素

```cpp
vector<int> vec{1,2,3,4,5,6};
// 删除所有偶数
auto newEnd = remove_if(vec.begin(), vec.end(), [](int x){
    return x % 2 == 0;
});
vec.erase(newEnd, vec.end());
```

### 案例 4：for\_each 遍历并使用外部变量（捕获）

```cpp
vector<int> vec{1,2,3,4};
int sum = 0;
// 引用捕获sum，累加所有元素
for_each(vec.begin(), vec.end(), [&](int x){
    sum += x;
});
cout << sum;
```

## 五、lambda 类型特性

1. 每个 lambda**独一无二类型**，无法用函数指针直接接收（auto 接收最方便）

2. 无捕获的 lambda `[]()`\{\} 可以隐式转为普通函数指针；

3. 有捕获的 lambda **不能转函数指针**。

```cpp
// 无捕获，可以转函数指针
using FuncPtr = int(*)(int);
FuncPtr fp = [](int x){return x*2;};

// [&](int x){return x;} 有捕获，不能赋值给FuncPtr
```

## 六、常见坑点

1. **悬垂引用陷阱**
引用捕获局部变量，lambda 在变量销毁后调用，产生野指针！

```cpp
auto bad()
{
    int x = 10;
    return [&](){cout << x;}; // 危险！函数结束x销毁
}
```

2. 区分 `=` 和 `&`：
值捕获复制数据，修改副本；引用捕获直接操作外部变量。

3. mutable 只对**值捕获副本**生效，不影响外部原始变量。

4. lambda 不能使用递归（C\+\+11\~C\+\+14 原生不支持，需要 std::function 辅助）

## 七、递归 lambda 拓展（选学）

```cpp
#include <functional>
function<int(int)> fib = [&](int n){
    if(n <= 2) return 1;
    return fib(n-1) + fib(n-2);
};
cout << fib(6);
```



