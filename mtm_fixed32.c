#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
static const uint64_t S=1ULL<<32, H=1ULL<<31, G=7060275200ULL;
static inline uint64_t f(uint64_t X){ uint64_t U=(132*X)&(S-1); uint64_t T=U<H?U:S-U; return ((G*T)>>16)&(S-1); }
int main(){
  srand(1);
  for(int r=0;r<8;r++){
    uint64_t x0=(((uint64_t)rand()<<16)^rand())&(S-1); if(!x0)x0=1;
    uint64_t power=1,lam=0,t=x0,h=x0,steps=0; int ok=1;
    for(;;){ h=f(h); lam++; steps++; if(h==t)break; if(lam==power){t=h;power*=2;lam=0;} if(steps>(1ULL<<34)){ok=0;break;} }
    if(!ok){printf("x0=%llu none\n",(unsigned long long)x0);continue;}
    t=h=x0; for(uint64_t i=0;i<lam;i++)h=f(h);
    uint64_t mu=0; while(t!=h){t=f(t);h=f(h);mu++;}
    /* low-bit structure on the cycle */
    uint64_t c=h, orv=0, andv=S-1, z=c; for(uint64_t i=0;i<lam;i++){orv|=z;andv&=z;z=f(z);}
    printf("x0=%10llu transient=%llu cycle=%llu  bits constant on cycle: 0x%08llx\n",(unsigned long long)x0,(unsigned long long)mu,(unsigned long long)lam,(unsigned long long)((~(orv^andv))&(S-1)));
    fflush(stdout);
  }
  return 0;
}
