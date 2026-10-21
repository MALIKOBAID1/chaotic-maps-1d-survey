// Class D countermeasure of P19 (bit reversal of all fractional bits after each iteration),
// applied to the fixed-point logistic (r = 3.99) and tent (mu = 1.99) maps of Section 9.
// n-bit unsigned fraction, truncation, same arithmetic as fixedpoint_cycle_script.m.
// n = 16, 24: all 2^n initial values followed.  n = 32: Brent from 32 random initial values.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
static int N; static uint64_t S,H,RQ,MQ;
static inline uint64_t logi(uint64_t X){ uint64_t P=(X*(S-X))>>N; return (RQ*P)>>(N-2); }
static inline uint64_t tent(uint64_t X){ uint64_t T=X<H?X:S-X; return (MQ*T)>>(N-1); }
static inline uint64_t rev(uint64_t x){ uint64_t r=0; for(int i=0;i<N;i++){ r=(r<<1)|(x&1); x>>=1; } return r; }
static int MODE;   // 0 logistic, 1 tent, 2 logistic+rev, 3 tent+rev
static inline uint64_t f(uint64_t X){ switch(MODE){case 0:return logi(X);case 1:return tent(X);case 2:return rev(logi(X));default:return rev(tent(X));} }
static int cmp(const void*a,const void*b){ uint64_t x=*(uint64_t*)a,y=*(uint64_t*)b; return (x>y)-(x<y); }
static const char*NAME[4]={"logistic","tent","logistic + bit reversal","tent + bit reversal"};
int main(){
  int ns[3]={16,24,32};
  for(int q=0;q<3;q++){ N=ns[q]; S=1ULL<<N; H=S>>1; RQ=(uint64_t)floor(3.99*pow(2,N-2)); MQ=(uint64_t)floor(1.99*pow(2,N-1));
    for(MODE=0;MODE<4;MODE++){
      if(N<=24){ uint32_t n=(uint32_t)S; uint32_t*F=malloc(4ULL*n),*G=malloc(4ULL*n),*T=malloc(4ULL*n),*len=calloc(n,4); uint8_t*seen=calloc(n,1),*img=calloc(n,1);
        for(uint32_t i=0;i<n;i++){ F[i]=(uint32_t)f(i); img[F[i]]=1; G[i]=F[i]; }
        for(int j=0;j<N;j++){ for(uint32_t i=0;i<n;i++)T[i]=G[G[i]]; uint32_t*t=G;G=T;T=t; }
        uint64_t ncyc=0,longest=0; for(uint32_t i=0;i<n;i++){ uint32_t c=G[i]; if(seen[c])continue; uint32_t k=0,j=c; while(!seen[j]){seen[j]=1;j=F[j];k++;} j=c; for(uint32_t u=0;u<k;u++){len[j]=k;j=F[j];} ncyc++; if(k>longest)longest=k; }
        for(uint32_t i=0;i<n;i++)T[i]=len[G[i]]; uint64_t im=0; for(uint32_t i=0;i<n;i++)im+=img[i];
        uint64_t*v=malloc(8ULL*n); for(uint32_t i=0;i<n;i++)v[i]=T[i]; qsort(v,n,8,cmp);
        printf("n=%2d %-24s cycles=%6llu longest=%9llu median-over-starts=%9llu image=%5.1f%%\n",N,NAME[MODE],(unsigned long long)ncyc,(unsigned long long)longest,(unsigned long long)v[n/2],100.0*im/n);
        free(F);free(G);free(T);free(len);free(seen);free(img);free(v);
      } else { uint64_t L[32],R[32]; uint64_t rs=88172645463325252ULL; int n=0;
        for(int r=0;r<32;r++){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; uint64_t x0=(rs>>20)&(S-1); if(!x0)x0=1;
          uint64_t power=1,l=0,t=x0,h=x0; for(;;){ h=f(h); l++; if(h==t)break; if(l==power){t=h;power*=2;l=0;} }
          t=h=x0; for(uint64_t i=0;i<l;i++)h=f(h); uint64_t mu=0; while(t!=h){t=f(t);h=f(h);mu++;} L[n]=l;R[n]=mu+l;n++; }
        qsort(L,n,8,cmp);qsort(R,n,8,cmp);
        printf("n=%2d %-24s 32 starts: cycle min/med/max=%llu/%llu/%llu ; steps before repetition, median=%llu\n",N,NAME[MODE],(unsigned long long)L[0],(unsigned long long)L[n/2],(unsigned long long)L[n-1],(unsigned long long)R[n/2]); }
      fflush(stdout); } }
  return 0; }
