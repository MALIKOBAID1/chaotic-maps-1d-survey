// Seed values used in the paper: IMV = 0.47892 (bifurcation, Lyapunov exponent) and 14.795 (chi-square test). Change the constant to switch.
// BCM: equation as printed in the paper, modulator as in the authors' MATLAB code (running accumulator).
//   s  = mod(s - x, 1);            s starts at IMV
//   x' = mod( (84*r*sin(x)) + s + (84*2) - 80*r^2*x^2 , 1 );
// State is the pair (x, s). IEEE-754 binary64, MATLAB order of operations.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
static double R;
typedef struct { double x, s; } st;
static inline st f(st a){ st b; double s=a.s-a.x; s-=floor(s); double y=((84.0*R*sin(a.x))+s+(84.0*2.0))-(80.0*(R*R))*(a.x*a.x); b.x=y-floor(y); b.s=s; return b; }
int main(int argc,char**argv){
  double betas[4]={154.235,11.547,-2.354,-14.354}; uint64_t limit=(uint64_t)atof(argv[1]);
  for(int k=0;k<4;k++){ R=betas[k];
    st x0={0.71649952,14.795}, t=x0, h=x0; uint64_t power=1,l=0,steps=0; int ok=1;
    for(;;){ h=f(h); l++; steps++; if(h.x==t.x&&h.s==t.s)break; if(l==power){t=h;power*=2;l=0;} if(steps>limit){ok=0;break;} }
    if(!ok) printf("beta=%9.3f: no repetition of the pair (x, s) within %.1e steps\n",R,(double)limit);
    else    printf("beta=%9.3f: cycle of length %llu found after %llu steps\n",R,(unsigned long long)l,(unsigned long long)steps);
    fflush(stdout); }
  return 0; }
