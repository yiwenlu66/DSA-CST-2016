---
header-includes:
    - \usepackage{ctex}
---

# 1-5 Graphic 解题报告

## 原理

对输入的 x, y 坐标分别进行排序, 则 `x[i]`, `y[i]` 表示从左到右第 `i` 条线段. 通过二分查找确定输入的点在哪两条线段之间即可.

## 复杂度分析

- 时间复杂度: 排序 $\mathcal{O}(n\log(n))$, 每次查找 $\mathcal{O}(\log(n))$, 共 $m$ 次查找, 总体时间复杂度 $\mathcal{O}((m+n)\log(n))$.

- 空间复杂度: 除去输入的坐标外只需要常数空间, $\mathcal{O}(1)$.