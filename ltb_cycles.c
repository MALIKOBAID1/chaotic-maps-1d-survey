// LTB map of P34 (Yamni et al. 2024) in the 32-bit format of its Table 4: sign, 8 integer bits, 23 fractional bits.
// Three independent maps; output q = x XOR y XOR |z|. Sign-magnitude multiply with truncation (qmult style).
// Parameters of the paper: lambda = (4, 1.99, 1.99), x0 = y0 = z0 = 0.4. Also 64 random initial triples.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define F 23
static const int64_t ONE=1LL<<F;
static int64_t L1,L2,L3;
static inline int64_t mul(int64_t a,int64_t b){ int s=(a<0)^(b<0); int64_t m=((a<0?-a:a)*(b<0?-b:b))>>F; return s?-m:m; }
static inline int64_t fx(int64_t x){ return mul(mul(L1,x),ONE-x); }
static inline int64_t fy(int64_t y){ return y<ONE/2?mul(L2,y):mul(L2,ONE-y); }
static inline int64_t fz(int64_t z){ return z<0?mul(L3,z)+ONE:mul(L3,z)-ONE; }
typedef int64_t (*fn)(int64_t);
static void brent(fn f,int64_t x0,uint64_t*mu,uint64_t*lam){ uint64_t p=1,l=0; int64_t t=x0,h=x0;
  for(;;){ h=f(h); l++; if(h==t)break; if(l==p){t=h;p*=2;l=0;} }
  t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h); uint64_t m=0; while(t!=h){t=f(t);h=f(h);m++;} *mu=m;*lam=l; }
static uint64_t gcd(uint64_t a,uint64_t b){ while(b){uint64_t t=a%b;a=b;b=t;} return a; }
static uint64_t rs=88172645463325252ULL; static uint64_t ur(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
int main(){ L1=4LL<<F; L2=(int64_t)(1.99*ONE); L3=(int64_t)(1.99*ONE);
  uint64_t m1,c1,m2,c2,m3,c3; int64_t x0=(int64_t)(0.4*ONE);
  brent(fx,x0,&m1,&c1); brent(fy,x0,&m2,&c2); brent(fz,x0,&m3,&c3);
  uint64_t lcm=c1/gcd(c1,c2)*c2; lcm=lcm/gcd(lcm,c3)*c3; uint64_t tr=m1>m2?m1:m2; tr=tr>m3?tr:m3;
  printf("paper's initial values: logistic transient %llu cycle %llu | tent %llu cycle %llu | Bernoulli %llu cycle %llu\n",(unsigned long long)m1,(unsigned long long)c1,(unsigned long long)m2,(unsigned long long)c2,(unsigned long long)m3,(unsigned long long)c3);
  printf("   LTB state cycle = lcm = %llu, entered after at most %llu steps\n",(unsigned long long)lcm,(unsigned long long)tr);
  uint64_t C[64],P[64],Q[64],R[64];
  for(int r=0;r<64;r++){ int64_t a=(int64_t)(ur()%ONE), b=(int64_t)(ur()%ONE), c=(int64_t)(ur()%(2*ONE))-ONE;
    brent(fx,a,&m1,&c1); brent(fy,b,&m2,&c2); brent(fz,c,&m3,&c3);
    uint64_t L=c1/gcd(c1,c2)*c2; L=L/gcd(L,c3)*c3; C[r]=L; P[r]=c1; Q[r]=c2; R[r]=c3; }
  qsort(C,64,8,cmp); qsort(P,64,8,cmp); qsort(Q,64,8,cmp); qsort(R,64,8,cmp);
  printf("64 random initial triples, median cycle: logistic %llu, tent %llu, Bernoulli %llu; LTB min/median/max %llu/%llu/%llu\n",(unsigned long long)P[32],(unsigned long long)Q[32],(unsigned long long)R[32],(unsigned long long)C[0],(unsigned long long)C[32],(unsigned long long)C[63]);
  int ones=0; for(int r=0;r<64;r++) if(P[r]==1) ones++; printf("   logistic component ends in a fixed point in %d of 64 runs\n",ones);
  return 0; }
