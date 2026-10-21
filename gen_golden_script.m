% GEN_GOLDEN_SCRIPT  Golden vectors for the Verilog testbench (tb_chaos_map.v).
%
% Plain script, no functions. Writes 12 files, golden_map<M>_n<N>.hex, each with
% x0 on the first line followed by K = 1000 states of the fixed-point model.
% The arithmetic is the same as in fixedpoint_cycle_script.m.
%   M = 0 logistic, 1 tent, 2 multimodal tent (MTM), 3 multimodal logistic (MLM)

clear; clc;
K = 1000;

for n = [16 24 32]
    S  = bitshift(uint64(1), n);
    H  = bitshift(uint64(1), n-1);
    Rq = uint64(floor(3.99 * 2^(n-2)));
    Mq = uint64(floor(1.99 * 2^(n-1)));
    G  = uint64(7060275200);
    m  = uint64(132);

    for M = 0:3
        X   = uint64(floor(0.123456789 * 2^n));          % initial state x0
        fid = fopen(sprintf('golden_map%d_n%d.hex', M, n), 'w');
        fprintf(fid, '%s\n', dec2hex(double(X), n/4));

        for k = 1:K
            if M == 0                                     % logistic
                P = bitshift(X*(S - X), -n);
                X = bitshift(Rq*P, -(n-2));
            elseif M == 1                                 % tent
                if X < H, T = X; else, T = S - X; end
                X = bitshift(Mq*T, -(n-1));
            else
                U = mod(m*X, S);                          % frac(132 x)
                if M == 2                                 % multimodal tent
                    if U < H, T = U; else, T = S - U; end
                    X = mod(bitshift(G*T, -16), S);
                else                                      % multimodal logistic
                    P = bitshift(U*(S - U), -n);
                    X = mod(bitshift(G*P, -16), S);
                end
            end
            fprintf(fid, '%s\n', dec2hex(double(X), n/4));
        end
        fclose(fid);
    end
end
disp('12 golden files written.');
