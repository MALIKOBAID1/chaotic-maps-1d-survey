% CYCLE_CHECK_SCRIPT  Cycle-length check of pilot maps in IEEE-754 double precision.
%
% Plain script: no function files and no local functions. Press Run.
% Each map is iterated from several random initial values; the transient and
% cycle lengths are found with Brent's cycle-detection algorithm.
% Equations are as read from the papers; the evaluation order follows the
% formulas as printed. Exact values depend on the language, the maths
% library and the operation order; the order of magnitude is the result.

clear; clc;

limit = 6e6;      % maximum steps per run (use 1e6 for a quick trial)
runs  = 8;        % random initial values per map
rng(1);           % fixed seed, for repeatable runs

% ---------------------------------------------------------------- maps
frac = @(y) y - floor(y);                         % y mod 1
tent = @(u) u*(u < 0.5) + (1 - u)*(u >= 0.5);     % u or 1-u

m = 132; s = 156.7; a = 687.5;                    % parameters of the FPGA demo
mtm  = @(x) frac(s*a*tent(frac(m*x)));            % multimodal tent map (TIE 2025)
mlm  = @(x) frac(s*a*frac(m*x)*(1 - frac(m*x)));  % multimodal logistic map (TIE 2025)
scml = @(x) sin(pi*2*x*(1 - x));                  % sine-chaotified logistic map, a = 2 (TIE 2019)
scms = @(x) sin(pi*2*sin(pi*x));                  % sine-chaotified sine map, mu = 2 (TIE 2019)
ls   = @(x) sin(pi*(4*0.5*x*(1 - x) + (1 - 0.5)*sin(pi*x)));  % logistic-sine map, r = 0.5 (TIE 2018)
blm  = @(x) frac(200*x*(1 - x) + 200/x);          % buffeting logistic map, p = b = 200 (TIE 2023)
elog = @(x) frac(2^(10 - 3.999*x*(1 - x)));       % enhanced logistic map, r = 3.999 (JKSUCIS 2022)
lgst = @(x) 3.99*x*(1 - x);                       % classical logistic map, r = 3.99

names  = {'MTM  m=132 s=156.7 a=687.5', 'MLM  m=132 s=156.7 a=687.5', ...
          'SCM logistic a=2', 'SCM sine mu=2', 'STBCS LS r=0.5', ...
          'BLM  p=200 b=200', 'ELOG r=3.999', 'logistic r=3.99'};
maps   = {mtm, mlm, scml, scms, ls, blm, elog, lgst};
x0low  = [0 0 -1 -1 0 0 0 0];                     % range of the random initial value
x0high = [1 1  1  1 1 1 1 1];

% ------------------------------------------------------------ main loop
for c = 1:numel(maps)
    f = maps{c};
    mus = []; lams = [];

    for k = 1:runs
        x0 = x0low(c) + (x0high(c) - x0low(c))*rand;

        % ---- Brent's algorithm, phase 1: find the cycle length lam
        power = 1; lam = 1; steps = 0; found = true;
        t = x0; h = f(x0);
        while t ~= h
            if power == lam
                t = h; power = power*2; lam = 0;
            end
            h = f(h); lam = lam + 1; steps = steps + 1;
            if steps > limit
                found = false; break;
            end
        end
        if ~found
            continue;                             % no cycle within the limit
        end

        % ---- phase 2: find the transient length mu
        t = x0; h = x0;
        for i = 1:lam
            h = f(h);
        end
        mu = 0;
        while t ~= h
            t = f(t); h = f(h); mu = mu + 1;
        end

        mus(end+1) = mu; lams(end+1) = lam;       %#ok<SAGROW>
    end

    % ---- report
    if isempty(lams)
        fprintf('%-28s no cycle within %d steps in %d runs\n', names{c}, limit, runs);
    else
        fprintf('%-28s found %d/%d: cycle min/median/max = %d/%d/%d; median transient = %d\n', ...
            names{c}, numel(lams), runs, min(lams), round(median(lams)), max(lams), round(median(mus)));
    end
end
