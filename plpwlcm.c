// 32-bit PL_PWLCM&LM of Li, Deng and Gu (IEEE Access 2020), Eqs. (2), (4), (7):
//   logistic  H(x) = floor(4 x (2^32 - x) / 2^32), stored in 32 bits
//   PWLCM     F(x,p) = floor(2^32 x / p)                       0 < x < p
//                    = floor(2^32 (x-p) / (2^31 - p))          p <= x <= 2^31   (x = 2^31 not covered by Eq. 4; put here)
//                    = F(2^32 - x, p)                          2^31 < x < 2^32
//   results reduced mod 2^32 (32-bit storage); output r = ((x XOR y) + z) mod 2^32. The three maps run independently.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
typedef unsigned __int128 u128;
static uint64_t P;
static inline uint64_t H(uint64_t x){ return (uint64_t)(((u128)4*x*((1ULL<<32)-x))>>32)&0xFFFFFFFFULL; }
static inline uint64_t Fp(uint64_t x){ const uint64_t T=1ULL<<32, h=1ULL<<31;
  if(x>h) x=T-x;
  if(x==0) return 0;
  if(x<P) return (uint64_t)(((u128)x<<32)/P)&0xFFFFFFFFULL;
  return (uint64_t)(((u128)(x-P)<<32)/(h-P))&0xFFFFFFFFULL; }
typedef uint64_t (*fn)(uint64_t);
static void brent(fn f,uint64_t x0,uint64_t*mu,uint64_t*lam){ uint64_t p=1,l=0,t=x0,h=x0;
  for(;;){ h=f(h); l++; if(h==t)break; if(l==p){t=h;p*=2;l=0;} }
  t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h); uint64_t m=0; while(t!=h){t=f(t);h=f(h);m++;} *mu=m;*lam=l; }
static uint64_t gcd(uint64_t a,uint64_t b){ while(b){uint64_t t=a%b;a=b;b=t;} return a; }
static uint64_t rs=88172645463325252ULL; static uint64_t ur(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
int main(){ int R=48; uint64_t CL[48],CP[96],TOT[48],TR[48]; int zeroL=0,zeroP=0;
  for(int r=0;r<R;r++){ uint64_t mu,lam,muy,lamy,muz,lamz;
    uint64_t x0=(ur()&0xFFFFFFFFULL)|1; brent(H,x0,&mu,&lam); CL[r]=lam; if(lam==1) zeroL++;
    P=(ur()%((1ULL<<31)-2))+1; uint64_t y0=(ur()&0x7FFFFFFFULL)|1; brent(Fp,y0,&muy,&lamy); CP[2*r]=lamy; if(lamy==1) zeroP++;
    uint64_t p1=P; P=(ur()%((1ULL<<31)-2))+1; uint64_t z0=(ur()&0x7FFFFFFFULL)|1; brent(Fp,z0,&muz,&lamz); CP[2*r+1]=lamz; if(lamz==1) zeroP++;
    uint64_t L=lam/gcd(lam,lamy)*lamy; L=L/gcd(L,lamz)*lamz; TOT[r]=L; uint64_t t=mu>muy?mu:muy; TR[r]=t>muz?t:muz; (void)p1; }
  qsort(CL,R,8,cmp); qsort(CP,2*R,8,cmp); qsort(TOT,R,8,cmp); qsort(TR,R,8,cmp);
  printf("logistic component (48 runs): cycle min/median/max = %llu/%llu/%llu; fixed point in %d runs\n",(unsigned long long)CL[0],(unsigned long long)CL[R/2],(unsigned long long)CL[R-1],zeroL);
  printf("PWLCM components (96 runs):   cycle min/median/max = %llu/%llu/%llu; fixed point in %d runs\n",(unsigned long long)CP[0],(unsigned long long)CP[R],(unsigned long long)CP[2*R-1],zeroP);
  printf("combined generator (48 runs): period min/median/max = %llu/%llu/%llu; transient median %llu\n",(unsigned long long)TOT[0],(unsigned long long)TOT[R/2],(unsigned long long)TOT[R-1],(unsigned long long)TR[R/2]);
  return 0; }
