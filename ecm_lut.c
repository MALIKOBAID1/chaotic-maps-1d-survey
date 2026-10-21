// Look-up-table realisation of the exposine map, Steps 1-6 of its paper:
//   X_i = (i-1)*step on [0,1], step = 1/(N-1);  f(X_i) = |sin(r^2 * i * X_i - exp(r*sin X_i))|;  LUT[i] = (f(X_i)+1)/2
//   X_{n+1} = LUT[ round(X_n/step) + 1 ]
// The table size N is not stated in the paper; three sizes are tested. r = 38.451, X0 = 0.987 (NIST setting of the paper).
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int main(){ double r=38.451; int sizes[3]={1<<10,1<<16,1<<20};
  for(int k=0;k<3;k++){ int N=sizes[k]; double step=1.0/(N-1); double*L=malloc(sizeof(double)*(N+1));
    for(int i=1;i<=N;i++){ double X=(i-1)*step; double f=fabs(sin(r*r*i*X-exp(r*sin(X)))); L[i]=(f+1.0)/2.0; }
    int*seen=malloc(sizeof(int)*(N+1)); for(int i=0;i<=N;i++)seen[i]=-1;
    int idx=(int)lround(0.987/step)+1; int n=0; while(seen[idx]<0){ seen[idx]=n++; idx=(int)lround(L[idx]/step)+1; }
    int cyc=n-seen[idx], tr=seen[idx];
    // all starting indices: distribution of cycle lengths (functional graph)
    int*color=calloc(N+1,sizeof(int)); int*clen=calloc(N+1,sizeof(int)); long ncyc=0,maxc=0; int*stack=malloc(sizeof(int)*(N+2));
    for(int s=1;s<=N;s++){ if(color[s])continue; int sp=0,u=s; while(!color[u]){ color[u]=1; stack[sp++]=u; u=(int)lround(L[u]/step)+1; }
      if(color[u]==1){ int len=0; for(int j=sp-1;j>=0;j--){ len++; if(stack[j]==u)break; } int v=u; for(int j=0;j<len;j++){ clen[v]=len; v=(int)lround(L[v]/step)+1; } ncyc++; if(len>maxc)maxc=len; }
      for(int j=0;j<sp;j++) color[stack[j]]=2; }
    int reach=0; for(int i=1;i<=N;i++){ int t=(int)lround(L[i]/step)+1; if(t>=1&&t<=N) {} } 
    printf("N_LUT=%8d: from X0=0.987 transient %d, cycle %d; %ld cycles in the whole table, longest %ld\n",N,tr,cyc,ncyc,maxc);
    free(L);free(seen);free(color);free(clen);free(stack); }
  return 0; }
