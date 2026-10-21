# P35-style check of the exponential delayed feedback logistic map (Liu et al. 2025), Eqs. (5), (7), (8):
#   FL(v) = floor(2^p v)/2^p ;  u = 4, x1 = 0.247, x2 = 0.3698, k = 1, b = 4.  State = (x_{i-1}, x_i).
import math
def FL(v,p): return math.floor(v*2**p)/2**p
def step(eq,a,b,p,u=4.0,B=4.0):
    if eq==5: return FL(u*b*(1-b),p)
    base=b*(1-b)
    e=a if eq==7 else B*a*(1-a)
    return FL(math.fmod(u*(base**e),1.0),p)
def analyse(eq,p,limit=200000):
    a,b=FL(0.247,p),FL(0.3698,p)
    seen={}
    for i in range(limit):
        key=(a,b) if eq!=5 else b
        if key in seen: return i, i-seen[key], (a,b)
        seen[key]=i
        a,b=b,step(eq,a,b,p)
    return None
for eq in (5,7,8):
    row=[]
    for p in range(8,25):
        r=analyse(eq,p)
        if r is None: row.append('N')
        else:
            i,per,st=r; row.append(f'{per}{"(0)" if st[1]==0 else ""}@{i-per}')
    print(f'Eq.({eq}):',' '.join(f'p{p}:{v}' for p,v in zip(range(8,25),row)))
