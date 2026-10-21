// LS map of the cosine-transform model (P24), as printed in the paper:
// x' = 50*pi*|cos(pi*(4*a*x*(1-x) + (1-a)*sin(pi*x) + 0.4))|,  a in [0,1].  IEEE-754 binary64.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
static double A;
static double f(double x){ return 50.0*M_PI*fabs(cos(M_PI*(4.0*A*x*(1.0-x)+(1.0-A)*sin(M_PI*x)+0.4))); }
static int brent(double x0,uint64_t limit,uint64_t*mu,uint64_t*lam){
  uint64_t power=1,l=0,steps=0; double t=x0,h=x0;
  for(;;){ h=f(h); l++; steps++; if(h==t)break; if(l==power){t=h;power*=2;l=0;} if(steps>limit)return 0; }
  t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h); uint64_t m=0; while(t!=h){t=f(t);h=f(h);m++;} *mu=m;*lam=l; return 1; }
static uint64_t rs=88172645463325252ULL;
static double urand(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return (rs>>11)*(1.0/9007199254740992.0); }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
int main(){ double as[3]={0.2,0.5,0.9};
  for(int k=0;k<3;k++){ A=as[k]; uint64_t R[16],L[16],mu,lam; int n=0;
    for(int r=0;r<16;r++){ if(brent(urand(),300000000ULL,&mu,&lam)){R[n]=mu+lam;L[n]=lam;n++;} }
    if(n==0){ printf("alpha=%.1f: no cycle within 3e8 steps in 16 runs\n",A); continue; }
    qsort(R,n,8,cmp);qsort(L,n,8,cmp); double rho=(double)R[n/2];
    printf("alpha=%.1f: found %d/16; cycle min/med/max=%llu/%llu/%llu; median rho=%llu; eff.bits=%.1f\n",A,n,(unsigned long long)L[0],(unsigned long long)L[n/2],(unsigned long long)L[n-1],(unsigned long long)R[n/2],log2(2*rho*rho/M_PI)); fflush(stdout);}
  return 0; }
