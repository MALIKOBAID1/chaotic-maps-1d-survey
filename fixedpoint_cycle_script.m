% FIXEDPOINT_CYCLE_SCRIPT  Cycle structure of 1D chaotic maps in n-bit fixed point.
%
% Plain script: no function files and no local functions. Press Run.
%
% Number format : unsigned fraction with n bits (state x = X / 2^n, X integer).
% Rounding      : truncation (floor) after every multiplication and division.
% Arithmetic    : exact 64-bit integer operations (uint64), so the result is
%                 bit-exact and must be identical on every platform.
%
% n = 16 and 24 : EXHAUSTIVE. Every one of the 2^n starting values is followed,
%                 so the complete cycle structure is reported.
% n = 32        : too many states to enumerate; Brent's algorithm from random
%                 starting values.
%
% Memory: the 24-bit pass needs roughly 1 GB of RAM.

clear; clc;

wordLengths = [16 24 32];   % word lengths to test
runs32      = 8;            % random starting values for n = 32
limit32     = 5e6;          % step limit per run for n = 32
rng(1);

names = {'logistic r=3.99', 'tent mu=1.99', 'PWLCM L=0.445', ...
         'MTM (P6)', 'MLM (P6)', 'BLM p=b=200'};

for n = wordLengths

    % ------------------------------------------------ constants for this n
    S   = bitshift(uint64(1), n);          % 2^n   (represents 1.0)
    H   = bitshift(uint64(1), n-1);        % 2^(n-1) (represents 0.5)
    one = uint64(1);
    Rq  = uint64(floor(3.99 * 2^(n-2)));   % logistic parameter, n-2 fractional bits
    Mq  = uint64(floor(1.99 * 2^(n-1)));   % tent parameter, n-1 fractional bits
    L   = uint64(floor(0.445 * 2^n));      % PWLCM parameter, n fractional bits
    G   = uint64(7060275200);              % s*a = 107731.25 with 16 fractional bits
    fg  = 16;
    m   = uint64(132);
    b   = uint64(200);

    % ---------------------------------------------- building blocks
    % TT(U): U if U < 0.5, else 1-U          P(X): x(1-x)          Um(X): frac(m*x)
    TT = @(U) U.*uint64(U < H) + (S - U).*uint64(U >= H);
    P  = @(X) bitshift(X.*(S - X), -n);
    Um = @(X) mod(m.*X, S);
    Xf = @(X) min(TT(X), H - one);         % folded state for the PWLCM
    Xs = @(X) max(X, one);                 % guards the division at X = 0

    % ---------------------------------------------- the maps (work on vectors)
    f_log  = @(X) bitshift(Rq.*P(X), -(n-2));
    f_tent = @(X) bitshift(Mq.*TT(X), -(n-1));
    f_pwl  = @(X) idivide(Xf(X).*S, L).*uint64(Xf(X) < L) + ...
                  idivide((Xf(X) - L).*S, H - L).*uint64(Xf(X) >= L);
    f_mtm  = @(X) mod(bitshift(G.*TT(Um(X)), -fg), S);
    f_mlm  = @(X) mod(bitshift(G.*P(Um(X)),  -fg), S);
    f_blm  = @(X) mod(mod(b.*P(X), S) + idivide(mod(b*S, Xs(X)).*S, Xs(X)), S) ...
                  .* uint64(X > 0);
    maps = {f_log, f_tent, f_pwl, f_mtm, f_mlm, f_blm};

    for c = 1:numel(maps)
        f = maps{c};

        if n <= 24
            % ======================= EXHAUSTIVE ANALYSIS =======================
            N  = double(S);
            F1 = zeros(N, 1, 'uint32');            % next state, as a 1-based index
            chunk = 2^20;
            for a0 = 0:chunk:N-1
                a1 = min(a0 + chunk, N) - 1;
                X  = uint64((a0:a1)');
                F1(a0+1:a1+1) = uint32(f(X)) + 1;
            end
            imagePct = 100 * numel(unique(F1)) / N;

            % F^(2^n): every state is sent to a state on its cycle
            Gm = F1;
            for j = 1:n
                Gm = Gm(Gm);
            end
            cyc = unique(Gm);                      % one or more states of every cycle

            % measure each cycle once
            seen = false(N, 1);
            clen = zeros(N, 1, 'uint32');
            lens = [];
            for i = 1:numel(cyc)
                j = cyc(i);
                if seen(j), continue; end
                k = 0; q = j;
                while ~seen(q)
                    seen(q) = true; q = F1(q); k = k + 1;
                end
                q = j;
                for t = 1:k
                    clen(q) = k; q = F1(q);
                end
                lens(end+1) = k;                   %#ok<SAGROW>
            end

            per = clen(Gm);                        % cycle length reached from each start
            fprintf('n=%2d %-16s cycles=%6d longest=%8d median-over-starts=%8d fixed-point starts=%6.2f%%  image=%5.1f%%\n', ...
                n, names{c}, numel(lens), max(lens), round(median(double(per))), ...
                100*mean(per == 1), imagePct);

        else
            % ===================== BRENT, RANDOM STARTS (n = 32) =====================
            mus = []; lams = [];
            for r = 1:runs32
                x0 = uint64(floor(rand * (double(S) - 1))) + 1;

                % phase 1: cycle length
                power = 1; lam = 0; steps = 0; found = true;
                t = x0; h = x0;
                while true
                    h = f(h); lam = lam + 1; steps = steps + 1;
                    if h == t, break; end
                    if lam == power
                        t = h; power = power*2; lam = 0;
                    end
                    if steps > limit32
                        found = false; break;
                    end
                end
                if ~found, continue; end

                % phase 2: transient length
                t = x0; h = x0;
                for i = 1:lam
                    h = f(h);
                end
                mu = 0;
                while t ~= h
                    t = f(t); h = f(h); mu = mu + 1;
                end
                mus(end+1) = mu; lams(end+1) = lam;    %#ok<SAGROW>
            end

            if isempty(lams)
                fprintf('n=%2d %-16s no cycle within %d steps in %d runs\n', n, names{c}, limit32, runs32);
            else
                fprintf('n=%2d %-16s found %d/%d: cycle min/med/max=%d/%d/%d median transient=%d\n', ...
                    n, names{c}, numel(lams), runs32, min(lams), round(median(lams)), max(lams), round(median(mus)));
            end
        end
    end
    fprintf('\n');
end
