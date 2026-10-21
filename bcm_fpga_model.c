// Seed values used in the paper: IMV = 0.47892 (bifurcation, Lyapunov exponent) and 14.795 (chi-square test). Change the constant to switch.
// Fixed-point model of the BCM FPGA core, built from the description in the paper (Sec. II-D, Fig. 4):
//   state f: Q1.31 (31 fractional bits);  IMV: Q1.31 key register (constant)
//   Umod = (IMV - f) mod 1               (32-bit subtraction, fractional-field selection)
//   sine: 1024-entry ROM, sin(a/1024) in Q2.16, addressed by the top 10 fractional bits of f
//   f^2 : Q2.62 product shortened to Q2.30
//   MUL1 = 84*beta (Q15.16) * sin (Q2.16);  MUL2 = 80*beta^2 (Q32.16) * f^2 (Q2.30)
//   ACC  = MUL1 + Umod + 168 - MUL2, 31 fractional bits;  f' = low 31 bits of ACC
// Assumptions not stated in the paper: ROM entries and coefficients rounded to nearest; products truncated when aligned.
// Mode "acc": the modulator is accumulated as in the authors' MATLAB code (U = (U - f) mod 1), state = (f, U).
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
typedef __int128 i128;
static int64_t C1,C2; static uint32_t ROM[1024]; static uint64_t IMVq; static const uint64_t M=(1ULL<<31)-1;
static inline uint64_t core(uint64_t f,uint64_t U){
  int64_t s=ROM[f>>21]; i128 mul1=((i128)C1*s)>>1;                 // 32 -> 31 fractional bits
  uint64_t f2=(f*f)>>32;                                           // Q2.62 -> Q2.30
  i128 mul2=((i128)C2*(i128)f2)>>15;                               // 46 -> 31 fractional bits
  i128 acc=mul1+(i128)U+((i128)168<<31)-mul2; return (uint64_t)(acc&(i128)M); }
static inline uint64_t f1(uint64_t f){ return core(f,(IMVq-f)&M); }
static uint64_t rs=88172645463325252ULL; static uint64_t ur(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
int main(int argc,char**argv){
  for(int a=0;a<1024;a++) ROM[a]=(uint32_t)llround(sin(a/1024.0)*65536.0);
  IMVq=(uint64_t)llround((14.795-floor(14.795))*2147483648.0)&M;
  double betas[4]={154.235,11.547,-2.354,-14.354};
  for(int k=0;k<4;k++){ double b=betas[k]; C1=llround(84.0*b*65536.0); C2=llround(80.0*b*b*65536.0);
    uint64_t R[64],L[64]; int n=0;
    for(int r=0;r<64;r++){ uint64_t x0=(r==0)?(uint64_t)llround(0.71649952*2147483648.0):(ur()&M);
      uint64_t power=1,l=0,t=x0,h=x0; for(;;){ h=f1(h); l++; if(h==t)break; if(l==power){t=h;power*=2;l=0;} }
      t=h=x0; for(uint64_t i=0;i<l;i++)h=f1(h); uint64_t mu=0; while(t!=h){t=f1(t);h=f1(h);mu++;}
      if(r==0) printf("beta=%9.3f  one-state core, f0=0.71649952: first repeated state after %llu iterations (transient %llu + cycle %llu)\n",b,(unsigned long long)(mu+l),(unsigned long long)mu,(unsigned long long)l);
      R[n]=mu+l; L[n]=l; n++; }
    qsort(R,n,8,cmp); qsort(L,n,8,cmp);
    printf("            64 starts: cycle min/med/max = %llu/%llu/%llu ; iterations before repetition min/med/max = %llu/%llu/%llu\n",(unsigned long long)L[0],(unsigned long long)L[n/2],(unsigned long long)L[n-1],(unsigned long long)R[0],(unsigned long long)R[n/2],(unsigned long long)R[n-1]); fflush(stdout); }
  if(argc>1&&!strcmp(argv[1],"acc")){ double b=154.235; C1=llround(84.0*b*65536.0); C2=llround(80.0*b*b*65536.0);
    uint64_t limit=(uint64_t)atof(argv[2]); uint64_t f0=(uint64_t)llround(0.71649952*2147483648.0),U0=IMVq;
    uint64_t tf=f0,tU=U0,hf=f0,hU=U0,power=1,l=0,steps=0; int ok=1;
    for(;;){ hU=(hU-hf)&M; hf=core(hf,hU); l++; steps++; if(hf==tf&&hU==tU)break; if(l==power){tf=hf;tU=hU;power*=2;l=0;} if(steps>limit){ok=0;break;} }
    if(ok) printf("beta=154.235  two-state core (accumulated modulator): cycle %llu found after %llu steps\n",(unsigned long long)l,(unsigned long long)steps);
    else   printf("beta=154.235  two-state core (accumulated modulator): no repetition within %.1e steps\n",(double)limit); }
  return 0; }
