---
header-includes:
    - \usepackage{ctex}
---

# 2-5 庐州月\ 解题报告

## 原理

问题即为每个任务（桥）匹配一种资源（原料），满足 $\begin{cases} resource.cost \geq task.cost \\ resource.value \geq task.value \end{cases}$，使 $\sum{task.cost}$ 最小。将任务和资源分别以 $value$ 作第一关键字、$cost$ 作第二关键字降序排序。对每个任务，将所有 $value$ 足够大的资源加入一个集合（由于已按 $value$ 降序，每次只需在前一个任务的集合的基础上追加），然后贪心地选取其中 $cost$ 足够大的元素中 $cost$ 最小者作为这个匹配给这个任务的资源、取出集合。集合用 AVL 树实现。

## 遇到的问题

无。

## 复杂度分析

$0.5m\leq n \leq 0.7\Rightarrow m,n$ 在渐进意义上同阶，下都用 $n$ 表示。

- 时间复杂度：排序 $\mathcal{O}(n\log{n})$；AVL 树的每次插入、查找、删除 $\mathcal{O}(\log{n})$；累计 $\mathcal{O}(n\log{n})$。
    
- 空间复杂度：除输入外，额外空间即 AVL 树所需空间，$\mathcal{O}(n)$。