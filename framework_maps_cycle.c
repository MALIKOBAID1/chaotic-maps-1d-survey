// Two framework maps read from the papers (equations taken from the page images), IEEE-754 binary64.
//  P25 (Fibonacci chaotification model), logistic seed:  x' = mod(6765*(a*x*(1-x)) + 21, 1)
//  P16 (hybrid chaotic system), logistic seed:           G = r*x;  x' = mod( r*G*(1-G) + r/G , 1 )
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
static double PA; static int MODE;
static double f(double x){ double y;
  if(MODE==0) y=6765.0*(PA*x*(1.0-x))+21.0;
  else { double G=PA*x; y=PA*G*(1.0-G)+PA/G; }
  return y-floor(y); }
static int brent(double x0,uint64_t limit,uint64_t*mu,uint64_t*lam){
  uint64_t power=1,l=0,steps=0; double t=x0,h=x0;
  for(;;){ h=f(h); l++; steps++; if(h==t)break; if(l==power){t=h;power*=2;l=0;} if(steps>limit)return 0; }
  t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h); uint64_t m=0; while(t!=h){t=f(t);h=f(h);m++;} *mu=m;*lam=l; return 1; }
static uint64_t rs=88172645463325252ULL;
static double urand(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return (rs>>11)*(1.0/9007199254740992.0); }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
static void run(const char*name,int mode,double pa,double log2G){ MODE=mode; PA=pa; uint64_t R[32],L[32],mu,lam; int n=0;
  for(int r=0;r<32;r++){ if(brent(urand(),1500000000ULL,&mu,&lam)){R[n]=mu+lam;L[n]=lam;n++;} }
  if(!n){ printf("%s: no cycle within 1.5e9 steps in 32 runs\n",name); return; }
  qsort(R,n,8,cmp);qsort(L,n,8,cmp); double rho=(double)R[n/2];
  printf("%s: found %d/32; cycle min/med/max=%llu/%llu/%llu; median rho=%llu; eff.bits=%.1f",name,n,(unsigned long long)L[0],(unsigned long long)L[n/2],(unsigned long long)L[n-1],(unsigned long long)R[n/2],log2(2*rho*rho/M_PI));
  if(log2G>0) printf("; 53-log2G=%.1f",53-log2G); printf("\n"); fflush(stdout); }
int main(){
  run("P25 enhanced logistic, a=4  ",0,4.0,log2(6765.0*4/4));
  run("P25 enhanced logistic, a=100",0,100.0,log2(6765.0*100/4));
  run("P16 logistic-G, r=3.99      ",1,3.99,0);
  run("P16 logistic-G, r=10        ",1,10.0,0);
  return 0; }
