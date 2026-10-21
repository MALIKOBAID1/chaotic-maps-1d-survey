// Test of the cycle-length claim of the exponential chaotic model (Hua and Zhou, 2021):
// "the cycle length of the digitized ECM E(x) = f(x)^ln(g(x)+c) is the lowest common multiple of p and q".
// Digitisation as in P35/P42: x -> floor(2^m x)/2^m after each iteration (computed in binary64).
// Map tested: LEL, x' = (4 a x(1-x))^ln(4 b x(1-x) + c), base = exponent = logistic map, a = b = 1, c = 2.5.
// All 2^m initial values are followed (functional-graph cycle decomposition).
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
static int M; static double Q;
static uint32_t qf(double v){ double t=floor(v*Q); if(t<0)t=0; if(t>Q-1)t=Q-1; return (uint32_t)t; }
static uint32_t Lg(uint32_t i){ double x=i/Q; return qf(4.0*x*(1.0-x)); }
static uint32_t E(uint32_t i){ double x=i/Q; double b=4.0*x*(1.0-x); double e=log(4.0*x*(1.0-x)+2.5); return qf(pow(b,e)); }
static void cycles(uint32_t (*f)(uint32_t),const char*name){
  uint32_t N=1u<<M; uint32_t*F=malloc(4ULL*N); uint8_t*col=calloc(N,1); uint32_t*stk=malloc(4ULL*N);
  for(uint32_t i=0;i<N;i++)F[i]=f(i);
  uint32_t ncyc=0; uint64_t lens[64]; int nl=0;
  for(uint32_t s=0;s<N;s++){ if(col[s])continue; uint32_t sp=0,u=s; while(!col[u]){col[u]=1;stk[sp++]=u;u=F[u];}
    if(col[u]==1){ uint32_t len=0; for(int j=sp-1;j>=0;j--){len++; if(stk[j]==u)break;} if(nl<64) lens[nl++]=len; ncyc++; }
    for(uint32_t j=0;j<sp;j++)col[stk[j]]=2; }
  printf("  %-8s m=%2d: %u cycles; lengths:",name,M,ncyc); for(int k=0;k<nl&&k<12;k++) printf(" %llu",(unsigned long long)lens[k]); printf("%s\n",nl>12?" ...":"");
  free(F);free(col);free(stk); }
int main(){ int ms[2]={16,24}; for(int k=0;k<2;k++){ M=ms[k]; Q=(double)(1u<<M); cycles(Lg,"logistic"); cycles(E,"LEL"); } return 0; }
