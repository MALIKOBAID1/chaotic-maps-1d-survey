function cycle_check(limit)
% CYCLE_CHECK  Cycle-length check of pilot maps in IEEE-754 double precision.
%   cycle_check          uses a limit of 6,000,000 steps per run
%   cycle_check(limit)   uses the given step limit
%
%   Each map is iterated from 8 random initial values and the transient and
%   cycle lengths are found with Brent's cycle-detection algorithm.
%   Equations are as read from the papers; the evaluation order follows the
%   formulas as printed. Exact values depend on the language, the maths
%   library and the operation order; the order of magnitude is the result.

if nargin < 1, limit = 6e6; end
runs = 8;
rng(1);                                   % fixed seed, for repeatable runs

%        name                            map                              range of x0
cases = { ...
  'MTM  m=132 s=156.7 a=687.5',  @(x) mtm(x,132,156.7,687.5),            [0 1];
  'MLM  m=132 s=156.7 a=687.5',  @(x) mlm(x,132,156.7,687.5),            [0 1];
  'SCM logistic a=2',            @(x) sin(pi*2*x*(1-x)),                 [-1 1];
  'SCM sine mu=2',               @(x) sin(pi*2*sin(pi*x)),               [-1 1];
  'STBCS LS r=0.5',              @(x) sin(pi*(4*0.5*x*(1-x) + (1-0.5)*sin(pi*x))), [0 1];
  'BLM  p=200 b=200',            @(x) blm(x,200,200),                    [0 1];
  'ELOG r=3.999',                @(x) elog(x,3.999,10),                  [0 1];
  'logistic r=3.99',             @(x) 3.99*x*(1-x),                      [0 1]};

for c = 1:size(cases,1)
    name = cases{c,1}; f = cases{c,2}; rg = cases{c,3};
    mus = []; lams = [];
    for k = 1:runs
        x0 = rg(1) + (rg(2)-rg(1))*rand;
        [mu, lam] = brent(f, x0, limit);
        if ~isnan(lam), mus(end+1) = mu; lams(end+1) = lam; end %#ok<AGROW>
    end
    if isempty(lams)
        fprintf('%-28s no cycle within %d steps in %d runs\n', name, limit, runs);
    else
        fprintf('%-28s found %d/%d: cycle min/median/max = %d/%d/%d; median transient = %d\n', ...
            name, numel(lams), runs, min(lams), round(median(lams)), max(lams), round(median(mus)));
    end
end
end

% ---------------------------------------------------------------- maps
function y = mtm(x, m, s, a)      % multimodal tent map (Zhang et al., TIE 2025)
u = m*x; u = u - floor(u);
if u < 0.5, y = s*a*u; else, y = s*a*(1-u); end
y = y - floor(y);
end

function y = mlm(x, m, s, a)      % multimodal logistic map (same paper)
u = m*x; u = u - floor(u);
y = s*a*u*(1-u);
y = y - floor(y);
end

function y = blm(x, p, b)         % buffeting logistic map (Zhang et al., TIE 2023), R = 1
y = p*x*(1-x) + b/x;
y = y - floor(y);
end

function y = elog(x, r, k)        % enhanced logistic map (Alawida et al., 2022)
y = 2^(k - r*x*(1-x));
y = y - floor(y);
end

% ------------------------------------------------- Brent's algorithm
function [mu, lam] = brent(f, x0, limit)
% Returns the transient length mu and cycle length lam,
% or NaN for both if no cycle is detected within `limit` steps.
power = 1; lam = 1; steps = 0;
t = x0; h = f(x0);
while t ~= h
    if power == lam
        t = h; power = power*2; lam = 0;
    end
    h = f(h); lam = lam + 1; steps = steps + 1;
    if steps > limit, mu = NaN; lam = NaN; return; end
end
t = x0; h = x0;
for i = 1:lam, h = f(h); end
mu = 0;
while t ~= h
    t = f(t); h = f(h); mu = mu + 1;
end
end
