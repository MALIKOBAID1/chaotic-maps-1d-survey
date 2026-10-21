// Three maps of the third batch, IEEE-754 binary64, Brent's method from 32 random initial values.
//  REM1 (Moysis et al. 2022, Eq. 25):  x' = 10 sin(pi x) + rem(100 x, 1)                      (k = 10, a = 100, N = 1)
//  REM3 (Moysis et al. 2023, Eq. 19):  x' = 2 sin(pi x) + rem(10x,1) + rem(100x,1) + rem(1000x,1)   (k = 2, N = 1)
//  STLEM (Jiang et al. 2026, Eqs. 4-5): a=7.5, b=0.3, c=0.2, d=-0.5, with the boundary rules of Eq. (5)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
static int M;
static double f(double x){
  if(M==0) return 10.0*sin(M_PI*x)+fmod(100.0*x,1.0);
  if(M==1) return 2.0*sin(M_PI*x)+fmod(10.0*x,1.0)+fmod(100.0*x,1.0)+fmod(1000.0*x,1.0);
  double a=7.5,b=0.3,c=0.2,d=-0.5,y;
  if(isnan(x)||isinf(x)) x=0.5;
  if(x<=0.5) y=a*x+(8-a)*sin(M_PI*x)+b*x*x*x+c*exp(d*x);
  else { double u=1-x; y=a*u+(8-a)*sin(M_PI*u)+b*pow(1-2*x,3)+c*exp(d*u); }
  y=y-floor(y); if(y<1e-12) y=0.48; else if(y>1-1e-12) y=0.52; return y; }
static int brent(double x0,uint64_t limit,uint64_t*mu,uint64_t*lam){
  uint64_t power=1,l=0,steps=0; double t=x0,h=x0;
  for(;;){ h=f(h); l++; steps++; if(h==t)break; if(l==power){t=h;power*=2;l=0;} if(steps>limit)return 0; }
  t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h); uint64_t m=0; while(t!=h){t=f(t);h=f(h);m++;} *mu=m;*lam=l; return 1; }
static uint64_t rs=88172645463325252ULL;
static double urand(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return (rs>>11)*(1.0/9007199254740992.0); }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
int main(int argc,char**argv){ const char*name[3]={"REM1 (k=10,a=100)","REM3 (k=2)","STLEM (7.5,0.3,0.2,-0.5)"};
  int which=atoi(argv[1]); uint64_t limit=(uint64_t)atof(argv[2]); M=which;
  uint64_t R[32],L[32],mu,lam; int n=0;
  for(int r=0;r<16;r++){ double x0=(M==2)?urand():urand(); if(brent(x0,limit,&mu,&lam)){R[n]=mu+lam;L[n]=lam;n++;} }
  if(!n){ printf("%s: no repetition within %.1e steps in 16 runs\n",name[M],(double)limit); return 0; }
  qsort(R,n,8,cmp);qsort(L,n,8,cmp); double rho=(double)R[n/2];
  printf("%s: cycle found in %d/16; cycle min/med/max=%llu/%llu/%llu; median rho=%llu; eff.bits=%.1f\n",name[M],n,(unsigned long long)L[0],(unsigned long long)L[n/2],(unsigned long long)L[n-1],(unsigned long long)R[n/2],log2(2*rho*rho/M_PI));
  return 0; }
