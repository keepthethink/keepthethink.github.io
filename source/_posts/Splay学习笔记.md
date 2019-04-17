---
title: Splay学习笔记
date: 2019-03-28 20:59:43
tags: [数据结构, 平衡树]
---
# 前言

伸展树（英语：Splay Tree）是一种能够**自我平衡**的二叉查找树，它能在均摊O(log n)的时间内完成基于伸展（Splay）操作的插入、查找、修改和删除操作。

众所周知，普通BST在进行各种操作时很容易因为左子树与右子树高度差的问题导致时间复杂度退化为O(n)，Splay为了避免这个问题，使用了一种叫做“伸展”的操作来使左右子树平衡，在后面会介绍。

# 定义

## 节点

`node.v`：节点node的值

`node.father`：节点node的父节点

`node.ch[0]`与`node.ch[1]`：节点node的左子节点与右子节点

`node.size`：以节点node为根节点的子树的节点总数

`node.cnt`：数值与节点node相同的节点的数量（都储存在节点node中）

代码：
```cpp
class Node {
public:
    int v, father, ch[2], size, cnt;

    Node(int v = 0, int father = 0, int size = 0, int cnt = 0):v(v), father(father), size(size), cnt(cnt) {
        ch[0] = ch[1] = 0;
    }
};
```

## 树

`root`：根节点

`cnt`：总结点数

```cpp
Node node[MAXN];
int root = 0, cnt = 0;
```

# 操作

## 基本操作

### pushup

`pushup()`函数：**更新**节点p的`size`值

```cpp
void pushup(int p) {
    node[p].size = node[node[p].ch[0]].size + node[p].cnt + node[node[p].ch[1]].size; //节点数 = 左子树节点数 + 本身的节点数 + 右子树节点数
}
```
---

### check

`check()`函数：询问节点p是其父节点的左子节点还是右子节点

```cpp
int check(int p) {
    return node[node[p].father].ch[0] == p ? 0 : 1; //0代表左子节点，1代表右子节点
}
```
---

### connect

`connect()`函数：将节点x连接为节点f的子节点，方向为d (d = 0, 1，同上)

```cpp
void connect(int x, int f, int d) {
    node[f].ch[d] = x; //将节点f的子节点设置为x
    node[x].father = f; //将节点x的父节点设置为f
}
```

## 旋转

### rotate

旋转是平衡树最主要的操作，其本质在于，每次进行旋转时，左右子树当中之一高度 -1，另外一棵高度 +1，以达到平衡的目的。

左旋：

第一次连边，节点x的子节点成为x的父节点的右子节点

第二次连边，节点x成为节点x的父节点的父节点的子节点，方向与x的父节点相同

第三次连边，节点x的父节点成为节点x的左子节点

![](https://keepthethink.github.io/images/left_rotate.jpg)

右旋：

第一次连边，节点x的子节点成为x的父节点的左子节点

第二次连边，节点x成为节点x的父节点的父节点的子节点，方向与x的父节点相同

第三次连边，节点x的父节点成为节点x的右子节点

![](https://keepthethink.github.io/images/right_rotate.jpg)

旋转操作只与标为红，蓝，绿的三个部分有关。

```cpp
void rotate(int x) {
    int y = node[x].father, z = node[y].father, d = check(x), w = node[x].ch[d ^ 1]; //w判断应该左旋还是右旋
    connect(w, y, d); //第一次连边，节点x的子节点连接到x的父节点，方向与节点x相同
    connect(x, z, check(y)); //第二次连边，节点x连接到节点x的父节点的父节点，方向与x的父节点相同
    connect(y, x, d ^ 1); //第三次连边，节点x的父节点连接到节点x，方向与节点x原先的方向相反
    pushup(y); //更新子树
    pushup(x); //更新子树
}
```

## 伸展

### splay

Splay操作：将节点x旋转到节点dist的子节点。通常是将该节点旋转到根节点，在这种情况下，应当将`root`置为x

最朴素的想法：只要父节点不是dist就一直旋转该节点，但这样很容易被某些机（wu）智（liang）出题人卡。

```cpp
void splay(int x, int dist = 0) {
    while(node[x].father != dist) {
        rotate(x);
    }
    if(dist == 0) {
        root = x;
    }
}
```

所以，在实际操作中，通常会预判节点x的父节点的方向，若方向一致则旋转其父节点，减少被卡的可能性。

```cpp
void splay(int x, int dist = 0) {
    for(int f = node[x].father; f = node[x].father, f != dist; rotate(x)) {
        if(node[f].father != dist) {
            if(check(x) == check(f)) {
                rotate(f); //方向一致则旋转x的父节点
            } else {
                rotate(x); //方向不一致则旋转x
            }
        }
    }
    if(dist == 0) {
        root = x;
    }
}
```

## 查找

### find

查找值为x的节点，找到后将其置为`root`以便操作。

`find`操作的意义在于将值为x的节点伸展（splay）到根，在不存在值为x的节点的情况下，应将小于x的节点中最大的节点伸展（splay）到根。

```cpp
void find(int x) {
    int cur = root;
    while(node[cur].ch[x > node[cur].v] != 0 && x != node[cur].v) {
        cur = node[cur].ch[x > node[cur].v]; //查找值为x的节点
    }
    splay(cur);
}
```

## 公共操作

如果将本文讲的Splay打包成一个`class`，则前文所述的操作应包含在`private`中，本节所述的操作应包含在`public`中。

### insert

Splay中的`insert`其实与朴素BST中的`insert`没有什么区别，但若直接插入可能导致树退化为链，所以要在末尾处调用一次splay()函数，使Splay树保持平衡。

```cpp
void insert(int x) {
    int cur = root, p = 0;
    while(cur != 0 && node[cur].v != x) {
        p = cur;
        cur = node[cur].ch[x < node[cur].v ? 0 : 1];
    }
    if(cur != 0) {
        node[cur].cnt++;
    } else {
        cur = ++cnt;
        if(p != 0) {
            node[p].ch[x <= node[p].v ? 0 : 1] = cur;
        }
        node[cur] = Node(x, p, 1, 1);
    }
    splay(cur);
}
```

### serial

`serial`操作：查询值为x的节点，在`find`操作的基础上，`serial`只需要在`find`过后输出左子树节点数量即可。

```cpp
int serial(int x) {
    find(x);
    return node[node[root].ch[0]].size
}
```

### pre

找出值为x的节点的前驱，将节点splay到root后在左子树查找最大值即可。

```cpp
int pre(int x) {
    find(x);
    if(node[root].v < x) {
        return root;
    }
    int cur = node[root].ch[0];
    while(node[cur].ch[1] != 0) {
        cur = node[cur].ch[1];
    }
    return cur;
}
```

### suc

找出值为x的点的后继，与前驱同理。

```cpp
int suc(int x) {
    find(x);
    if(node[root].v > x) {
        return root;
    }
    int cur = node[root].ch[1];
    while(node[cur].ch[0] != 0) {
        cur = node[cur].ch[0];
    }
    return cur;
}
```

### remove

删除一个节点。

删除较为复杂，分四步来完成：

1. 定义`last`为节点的前驱，`next`为节点的后继。
2. 将`last`节点splay到`root`，这时`last`的左子树皆小于x
3. 将`next`节点splay到`last`的子节右点，此时`next`的右子树皆大于x
4. `next`的左节点`rm`必然满足 `last` < rm < `next`，删除`rm`即可

```cpp
void remove(int x) {
    int last = pre(x), next = suc(x);
    splay(last);
    splay(next, last);
    int rm = node[next].ch[0];
    if(node[rm].cnt > 1) {
        node[rm].cnt--;
        splay(rm);
    } else {
        node[next].ch[0] = 0;
        pushup(next);
        pushup(root);
    }
}
```

### rank

查找排名为k的节点

用一个指针cur从root开始查找，每次根据左子树大小于k的关系修改cur以及k。

```cpp
int rank(int k) {
    int cur = root;
    while(1) {
        if(node[cur].ch[0] != 0 && k <= node[node[cur].ch[0]].size) {
            cur = node[cur].ch[0];
        } else if(k > node[node[cur].ch[0]].size + node[cur].cnt) {
            k -= node[node[cur].ch[0]].size + node[cur].cnt;
            cur = node[cur].ch[1];
        } else {
            return cur;
        }
    }
}
```

# 参考资料

[伸展树- 维基百科，自由的百科全书](https://zh.wikipedia.org/zh-hans/伸展树)

[Splay Tree Introduction](https://www.youtube.com/watch?v=IBY4NtxmGg8)