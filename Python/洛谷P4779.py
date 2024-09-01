import queue as pq

N = int(1e5 + 5)
M = int(2e5 + 5)
INF = 0x3f3f3f3f

class qxx: # 向前星
    def __init__(self):
        self.next = 0
        self.t = 0
        self.v = 0

e = [qxx() for i in range(M)] #链表
h = [0 for i in range(N)]
cnt = 0

dist = [INF for i in range(N)]
q = pq.PriorityQueue() # 定义优先队列,默认小根堆

def add_path(f, t, v):
    # 修改全局变量,需要使用global
    global cnt, e, h
    print("add_path(%d,%d,%d)" % (f,t,v))
    cnt += 1
    e[cnt].next = h[f]
    e[cnt].t = t
    e[cnt].v= v
    h[f] = cnt

def nextedgeid(u):  # 生成器，可以用在 for 循环里
    i = h[u]
    while i:
        yield i
        i = e[i].nex


def dijkstra(s):
    dist[s] = 0
    q.put((0, s))
    while not q.empty():
        u = q.get()
        if dist[u[1]] < u[0]:
            continue
        for i in nextedgeid(u[1]):
            v = e[i].t
            w = e[i].v
            if dist[v] <= dist[u[1]] + w:
                continue
            dist[v] = dist[u[1]] + w
            q.put((dist[v], v))


# 如果你直接运行这个python代码（不是模块调用什么的）就执行命令
if __name__ == "__main__":
    # 一行读入多个整数。注意它会把整行都读进来
    n, m, s = map(int, input().split())
    for i in range(m):
        u, v, w = map(int, input().split())
        add_path(u, v, w)

    dijkstra(s)

    for i in range(1, n + 1):
        # 两种输出语法都是可以用的
        print("{}".format(dist[i]), end=" ")
        # print("%d" % dist[i],end=' ')

    print()  # 结尾换行