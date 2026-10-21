// Bernologistic chaotic map, one-dimensional form: U = mod(IMV - f, 1) with a constant seed IMV.
// This is the form used in the analysis (Eq. 12) and in the FPGA core of the paper. The software form, with a running accumulator, is in bcm_cycle.c.
// f' = mod( a*b*sin(f) + U + 2a - (a-4)*b^2*f^2, 1 ),  U = mod(IMV - f, 1),  a = 84 (terms added in the printed order)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
static double B, IMV; static const double A=84.0;
static double f(double x){ double u=IMV-x; u-=floor(u); double y=A*B*sin(x)+u+2.0*A-(A-4.0)*B*B*x*x; return y-floor(y); }
static int brent(double x0,uint64_t limit,uint64_t*mu,uint64_t*lam){
  uint64_t power=1,l=0,steps=0; double t=x0,h=x0;
  for(;;){ h=f(h); l++; steps++; if(h==t)break; if(l==power){t=h;power*=2;l=0;} if(steps>limit)return 0; }
  t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h); uint64_t m=0; while(t!=h){t=f(t);h=f(h);m++;} *mu=m;*lam=l; return 1; }
static uint64_t rs=88172645463325252ULL;
static double urand(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return (rs>>11)*(1.0/9007199254740992.0); }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
int main(){
  double betas[4]={154.235,11.547,-2.354,-14.354}; IMV=14.795;
  for(int b=0;b<4;b++){ B=betas[b]; uint64_t R[32],L[32],mu,lam; int n=0;
    if(brent(0.71649952,1ULL<<31,&mu,&lam)) printf("beta=%9.3f  f0=0.71649952: transient=%llu cycle=%llu\n",B,(unsigned long long)mu,(unsigned long long)lam);
    for(int r=0;r<32;r++){ if(brent(urand(),1ULL<<31,&mu,&lam)){R[n]=mu+lam;L[n]=lam;n++;} }
    qsort(R,n,8,cmp);qsort(L,n,8,cmp); double rho=(double)R[n/2]; double G=80.0*B*B;
    printf("   32 random starts: found %d; cycle min/med/max=%llu/%llu/%llu; median rho=%llu; eff.bits=%.1f; bound 53-log2(80 b^2)=%.1f\n",n,(unsigned long long)L[0],(unsigned long long)L[n/2],(unsigned long long)L[n-1],(unsigned long long)R[n/2],log2(2*rho*rho/M_PI),53-log2(G)); fflush(stdout);}
  return 0; }
