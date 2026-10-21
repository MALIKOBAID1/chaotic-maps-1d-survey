"""Cycle-length check of pilot maps in IEEE-754 double precision (Brent's algorithm).

Equations are as read from the papers; evaluation order follows the formulas as printed.
Exact values depend on language/compiler and operation order; the order of magnitude is the result.
Usage: python3 cycle_check.py [max_steps]   (default 6,000,000)
"""
import math, random, statistics as st, sys

fl = math.floor

def brent(f, x0, limit):
    """Return (transient, cycle) or None if no cycle is detected within `limit` steps."""
    power = lam = 1
    t = x0; h = f(x0); steps = 0
    while t != h:
        if power == lam:
            t = h; power *= 2; lam = 0
        h = f(h); lam += 1; steps += 1
        if steps > limit:
            return None
    t = h = x0
    for _ in range(lam):
        h = f(h)
    mu = 0
    while t != h:
        t = f(t); h = f(h); mu += 1
    return mu, lam

def MTM(m, s, a):            # multimodal tent map (Zhang et al., TIE 2025)
    def f(x):
        u = m * x; u -= fl(u)
        y = s * a * u if u < 0.5 else s * a * (1 - u)
        return y - fl(y)
    return f

def MLM(m, s, a):            # multimodal logistic map (same paper)
    def f(x):
        u = m * x; u -= fl(u)
        y = s * a * u * (1 - u)
        return y - fl(y)
    return f

def BLM(p, b):               # buffeting logistic map (Zhang et al., TIE 2023), R = 1
    def f(x):
        y = p * x * (1 - x) + b / x
        return y - fl(y)
    return f

def ELOG(r, k=10):           # enhanced logistic map (Alawida et al., 2022)
    def f(x):
        y = 2.0 ** (k - r * x * (1 - x))
        return y - fl(y)
    return f

def SCM_logistic(a):         # sine chaotification of the logistic map (Hua et al., TIE 2019)
    return lambda x: math.sin(math.pi * a * x * (1 - x))

def SCM_sine(mu):
    return lambda x: math.sin(math.pi * mu * math.sin(math.pi * x))

def STBCS_LS(r):             # logistic-sine map (Hua et al., TIE 2018) = LSC map (Inf. Sci. 2019)
    return lambda x: math.sin(math.pi * (4 * r * x * (1 - x) + (1 - r) * math.sin(math.pi * x)))

def logistic(r):
    return lambda x: r * x * (1 - x)

CASES = [  # name, map, range of random initial values
    ("MTM  m=132 s=156.7 a=687.5", MTM(132, 156.7, 687.5), (0, 1)),
    ("MLM  m=132 s=156.7 a=687.5", MLM(132, 156.7, 687.5), (0, 1)),
    ("SCM logistic a=2",           SCM_logistic(2.0),      (-1, 1)),
    ("SCM sine mu=2",              SCM_sine(2.0),          (-1, 1)),
    ("STBCS LS r=0.5",             STBCS_LS(0.5),          (0, 1)),
    ("BLM  p=200 b=200",           BLM(200.0, 200.0),      (0, 1)),
    ("ELOG r=3.999",               ELOG(3.999),            (0, 1)),
    ("logistic r=3.99",            logistic(3.99),         (0, 1)),
]

if __name__ == "__main__":
    limit = int(sys.argv[1]) if len(sys.argv) > 1 else 6_000_000
    runs = 8
    random.seed(1)
    for name, f, (lo, hi) in CASES:
        found = [r for r in (brent(f, random.uniform(lo, hi), limit) for _ in range(runs)) if r]
        if found:
            mus = [r[0] for r in found]; lams = [r[1] for r in found]
            print(f"{name:28s} found {len(found)}/{runs}: cycle min/median/max = "
                  f"{min(lams)}/{int(st.median(lams))}/{max(lams)}; median transient = {int(st.median(mus))}")
        else:
            print(f"{name:28s} no cycle within {limit} steps in {runs} runs")
