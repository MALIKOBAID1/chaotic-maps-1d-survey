// Maps of the seventh batch (open-access records found by the second screening), Brent's method, 16 random initial values.
//  0 e-CM logistic (Nazish & Banday 2025, Table 3): x' = ((1792 e^7 + 29) r x(1-x)) mod 1, r = 100 (setting of the paper's figures), binary64
//  1 same, r = 4, binary64
//  2 same, r = 100, binary32 (single precision, as on an FPU that supports float only)
//  3 same, r = 4,   binary32
//  4 LE-cubic (Ablay 2022, Eq. 6 with Eq. 15): x' = (1e5 (s x - x^3)) mod 1, s = 2.82, binary64
//  5 LE-sinh  (Ablay 2022, Eq. 6 with Eq. 17): x' = (1e2 (s x - sinh x)) mod 1, s = 3.4, binary64
//  6 Generator-logistic (Htiti et al. 2025, Table 1): x' = |cos(4 a pi b x(1-x)) (1 - sin(4 a pi b x(1-x)))|, a = 560, b = 0.9
//  7 SPM-logistic (Paul et al. 2023, Table 2): x' = (0.04 (x + C) + 3.9) x (1-x), C = 0.5
// mod 1 is v - floor(v) (MATLAB mod for a positive modulus).
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
static int M;
static const double G=1792.0*7.38905609893065*7.38905609893065*7.38905609893065*2.718281828459045+29.0; // 1792 e^7 + 29
static double f(double x){
  double v;
  switch(M){
  case 0: v=G*(100.0*x*(1-x)); return v-floor(v);
  case 1: v=G*(4.0*x*(1-x)); return v-floor(v);
  case 2: { float xf=(float)x, Gf=(float)G; float vf=Gf*(100.0f*xf*(1.0f-xf)); return (double)(vf-floorf(vf)); }
  case 3: { float xf=(float)x, Gf=(float)G; float vf=Gf*(4.0f*xf*(1.0f-xf)); return (double)(vf-floorf(vf)); }
  case 4: v=1e5*(2.82*x-x*x*x); return v-floor(v);
  case 5: v=1e2*(3.4*x-sinh(x)); return v-floor(v);
  case 6: { double t=4.0*560.0*M_PI*0.9*x*(1-x); return fabs(cos(t)*(1-sin(t))); }
  default: return (0.04*(x+0.5)+3.9)*x*(1-x);
  }}
static int brent(double x0,uint64_t limit,uint64_t*mu,uint64_t*lam){
  uint64_t power=1,l=0,steps=0; double t=x0,h=x0;
  for(;;){ h=f(h); l++; steps++; if(h==t)break; if(l==power){t=h;power*=2;l=0;} if(steps>limit)return 0; }
  t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h); uint64_t m=0; while(t!=h){t=f(t);h=f(h);m++;} *mu=m;*lam=l; return 1; }
static uint64_t rs=88172645463325252ULL;
static double urand(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return (rs>>11)*(1.0/9007199254740992.0); }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
int main(int argc,char**argv){ const char*name[8]={"e-CM logistic r=100 binary64","e-CM logistic r=4 binary64","e-CM logistic r=100 binary32","e-CM logistic r=4 binary32","LE-cubic a=1e5 s=2.82","LE-sinh a=1e2 s=3.4","Generator-logistic a=560 b=0.9","SPM-logistic C=0.5"};
  M=atoi(argv[1]); uint64_t limit=(uint64_t)atof(argv[2]);
  uint64_t R[16],L[16],mu,lam; int n=0;
  for(int r=0;r<16;r++){ double x0=urand(); if(M==2||M==3) x0=(double)(float)x0; if(brent(x0,limit,&mu,&lam)){R[n]=mu+lam;L[n]=lam;n++;} }
  if(!n){ printf("%s: no repetition within %.1e steps in 16 runs\n",name[M],(double)limit); return 0; }
  qsort(R,n,8,cmp);qsort(L,n,8,cmp); double rho=(double)R[n/2];
  printf("%s: cycle found in %d/16; cycle min/med/max=%llu/%llu/%llu; median rho=%llu; eff.bits=%.1f\n",name[M],n,(unsigned long long)L[0],(unsigned long long)L[n/2],(unsigned long long)L[n-1],(unsigned long long)R[n/2],log2(2*rho*rho/M_PI));
  return 0; }
