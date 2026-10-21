// Gain sweep and many-start cycle statistics, IEEE-754 binary64.
// Build: gcc -O2 -ffp-contract=off -o sweep sweep.c -lm
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef double (*map_t)(double);
static double P1,P2,P3;                 // parameters of the current map

static double f_mtm(double x){ double u=P1*x; u-=floor(u); double sa=P2; double y=(u<0.5)? sa*u : sa*(1.0-u); return y-floor(y); }
static double f_mlm(double x){ double u=P1*x; u-=floor(u); double y=P2*u*(1.0-u); return y-floor(y); }
static double f_blm(double x){ double y=P1*x*(1.0-x)+P2/x; return y-floor(y); }
static double f_elm(double x){ double y=pow(2.0,P2-P1*x*(1.0-x)); return y-floor(y); }
static double f_scl(double x){ return sin(M_PI*P1*x*(1.0-x)); }
static double f_scs(double x){ return sin(M_PI*P1*sin(M_PI*x)); }
static double f_ls (double x){ return sin(M_PI*(4.0*P1*x*(1.0-x)+(1.0-P1)*sin(M_PI*x))); }
static double f_log(double x){ return P1*x*(1.0-x); }

static int brent(map_t f,double x0,uint64_t limit,uint64_t*mu,uint64_t*lam){
  uint64_t power=1,l=0,steps=0; double t=x0,h=x0;
  for(;;){ h=f(h); l++; steps++; if(h==t)break; if(l==power){t=h;power*=2;l=0;} if(steps>limit)return 0; }
  t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h);
  uint64_t m=0; while(t!=h){t=f(t);h=f(h);m++;}
  *mu=m;*lam=l; return 1;
}
static uint64_t rs=88172645463325252ULL;
static double urand(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return (rs>>11)*(1.0/9007199254740992.0); }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
static uint64_t q(uint64_t*v,int n,double p){ int i=(int)(p*(n-1)+0.5); return v[i]; }

static void stats(const char*name,map_t f,double lo,double hi,int runs,uint64_t limit,double log2G){
  uint64_t *L=malloc(runs*8),*M=malloc(runs*8),*R=malloc(runs*8); int n=0;
  for(int r=0;r<runs;r++){ double x0=lo+(hi-lo)*urand(); uint64_t mu,lam; if(brent(f,x0,limit,&mu,&lam)){L[n]=lam;M[n]=mu;R[n]=mu+lam;n++;} }
  if(n==0){ printf("%s|found 0/%d|limit %llu\n",name,runs,(unsigned long long)limit); fflush(stdout); return; }
  qsort(L,n,8,cmp);qsort(M,n,8,cmp);qsort(R,n,8,cmp);
  double rho=(double)q(R,n,0.5); double bits=log2(2.0*rho*rho/M_PI);
  printf("%s|found %d/%d|lam min/q1/med/q3/max %llu/%llu/%llu/%llu/%llu|mu med %llu|rho q1/med/q3 %llu/%llu/%llu|bits %.2f",
    name,n,runs,(unsigned long long)L[0],(unsigned long long)q(L,n,.25),(unsigned long long)q(L,n,.5),(unsigned long long)q(L,n,.75),(unsigned long long)L[n-1],
    (unsigned long long)q(M,n,.5),(unsigned long long)q(R,n,.25),(unsigned long long)q(R,n,.5),(unsigned long long)q(R,n,.75),bits);
  if(log2G>0) printf("|log2G %.2f|rule %.2f",log2G,53.0-log2G);
  printf("\n"); fflush(stdout); free(L);free(M);free(R);
}

int main(int argc,char**argv){
  const char*mode=argv[1];
  if(!strcmp(mode,"check")){ P1=132;P2=156.7*687.5; uint64_t mu,lam; brent(f_mtm,0.123,1ULL<<32,&mu,&lam); printf("MTM x0=0.123: transient=%llu cycle=%llu (Python: 64301, 16205)\n",(unsigned long long)mu,(unsigned long long)lam);
    P1=132;P2=156.7*687.5; brent(f_mlm,0.123,1ULL<<32,&mu,&lam); printf("MLM x0=0.123: transient=%llu cycle=%llu (Python: 198407, 272368)\n",(unsigned long long)mu,(unsigned long long)lam);
    P1=200;P2=200; brent(f_blm,0.456,1ULL<<32,&mu,&lam); printf("BLM x0=0.456: transient=%llu cycle=%llu (Python: 436114, 5527488)\n",(unsigned long long)mu,(unsigned long long)lam); return 0; }
  int runs=atoi(argv[2]);
  if(!strcmp(mode,"sweep_mtm")){ int k=atoi(argv[3]); char nm[64]; double g=ldexp(1.3147,k); P1=132;P2=g; snprintf(nm,64,"MTM g=1.3147*2^%d",k); rs+=k*7919ULL; stats(nm,f_mtm,0,1,runs,1ULL<<33,log2(g/2.0)); }
  else if(!strcmp(mode,"sweep_blm")){ int k=atoi(argv[3]); char nm[64]; double b=ldexp(1.3147,k); P1=3.99;P2=b; snprintf(nm,64,"BLM p=3.99 b=1.3147*2^%d",k); rs+=k*104729ULL; stats(nm,f_blm,0,1,runs,1ULL<<33,log2(b)); }
  else if(!strcmp(mode,"MTM")){ P1=132;P2=156.7*687.5; stats("MTM",f_mtm,0,1,runs,1ULL<<33,log2(P2/2)); }
  else if(!strcmp(mode,"MLM")){ P1=132;P2=156.7*687.5; stats("MLM",f_mlm,0,1,runs,1ULL<<33,log2(P2/4)); }
  else if(!strcmp(mode,"BLM")){ P1=200;P2=200; stats("BLM",f_blm,0,1,runs,1ULL<<33,log2(200.0)); }
  else if(!strcmp(mode,"ELM")){ P1=3.999;P2=10; stats("ELM",f_elm,0,1,runs,1ULL<<31,9.5); }
  else if(!strcmp(mode,"SCL")){ P1=2; stats("SC-logistic",f_scl,-1,1,runs,1ULL<<31,0); }
  else if(!strcmp(mode,"SCS")){ P1=2; stats("SC-sine",f_scs,-1,1,runs,atoll(argv[3]),0); }
  else if(!strcmp(mode,"LS")){ P1=0.5; stats("Logistic-sine",f_ls,0,1,runs,atoll(argv[3]),0); }
  else if(!strcmp(mode,"LOG")){ P1=3.99; stats("Logistic",f_log,0,1,runs,1ULL<<33,0); }
  return 0;
}
