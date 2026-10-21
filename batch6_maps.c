// Maps of the sixth batch (IEEE Xplore), IEEE-754 binary64, Brent's method from 16 random initial values.
//  0 SML  (Sheng et al. 2025, Table I): x' = mod(N/(sin(4 pi r x(1-x)) + alpha), N), N=1, alpha=1e-14, r=200 (key of the paper)
//  1 CLM  (Alawida et al. 2019, Eq. 9): x' = cos(2^(k + r x(1-x))), k=22.5, r=3.99 (PRNG setting of the paper), x in [-1,1]
//  2 CLM  with k=20.123456, r=4 (setting of the paper's Fig. 7)
//  3 CLM  with k=10, r=4 (smallest k of the stated range)
//  4 LT   (Hasan et al. 2023, Eq. 6): normalised logistic+tent, CL=0.9, CT=0.25 (setting of the paper's Fig. 19)
//  5 E1   (Hua & Zhou 2018, Eq. 11): x' = sin(pi sin(pi p sin(pi x))), p=0.9
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
static int M;
static double f(double x){
  switch(M){
  case 0: { double v=1.0/(sin(4*M_PI*200.0*x*(1-x))+1e-14); return v-floor(v); }
  case 1: return cos(pow(2.0,22.5+3.99*x*(1-x)));
  case 2: return cos(pow(2.0,20.123456+4.0*x*(1-x)));
  case 3: return cos(pow(2.0,10.0+4.0*x*(1-x)));
  case 4: { double CL=0.9,CT=0.25; return x<0.5? x*(4*CL*(1-x)+2*CT)/(CL+CT) : (1-x)*(4*CL*x+2*CT)/(CL+CT); }
  default: return sin(M_PI*sin(M_PI*0.9*sin(M_PI*x)));
  }}
static int brent(double x0,uint64_t limit,uint64_t*mu,uint64_t*lam){
  uint64_t power=1,l=0,steps=0; double t=x0,h=x0;
  for(;;){ h=f(h); l++; steps++; if(h==t)break; if(l==power){t=h;power*=2;l=0;} if(steps>limit)return 0; }
  t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h); uint64_t m=0; while(t!=h){t=f(t);h=f(h);m++;} *mu=m;*lam=l; return 1; }
static uint64_t rs=88172645463325252ULL;
static double urand(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return (rs>>11)*(1.0/9007199254740992.0); }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
int main(int argc,char**argv){ const char*name[6]={"SML (r=200)","CLM (k=22.5,r=3.99)","CLM (k=20.123456,r=4)","CLM (k=10,r=4)","LT (CL=0.9,CT=0.25)","E1 (p=0.9)"};
  M=atoi(argv[1]); uint64_t limit=(uint64_t)atof(argv[2]);
  uint64_t R[16],L[16],mu,lam; int n=0;
  for(int r=0;r<16;r++){ double x0=urand(); if(M>=1&&M<=3) x0=2*x0-1; if(brent(x0,limit,&mu,&lam)){R[n]=mu+lam;L[n]=lam;n++;} }
  if(!n){ printf("%s: no repetition within %.1e steps in 16 runs\n",name[M],(double)limit); return 0; }
  qsort(R,n,8,cmp);qsort(L,n,8,cmp); double rho=(double)R[n/2];
  printf("%s: cycle found in %d/16; cycle min/med/max=%llu/%llu/%llu; median rho=%llu; eff.bits=%.1f\n",name[M],n,(unsigned long long)L[0],(unsigned long long)L[n/2],(unsigned long long)L[n-1],(unsigned long long)R[n/2],log2(2*rho*rho/M_PI));
  return 0; }
