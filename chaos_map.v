// chaos_map.v
// One-dimensional chaotic maps in N-bit unsigned fixed point (state x = X / 2^N).
// Truncation after every multiplication. One iteration per clock cycle.
// Bit-exact with fixedpoint_cycle_script.m (same number format and rounding).
//
//   MAP = 0 : logistic map,            x' = r x (1 - x),            r  = 3.99
//   MAP = 1 : tent map,                x' = mu min(x, 1 - x),       mu = 1.99
//   MAP = 2 : multimodal tent map,     x' = frac(G tent(frac(132 x)))
//   MAP = 3 : multimodal logistic map, x' = frac(G u (1 - u)),      u = frac(132 x)
//             G = 156.7 * 687.5 = 107731.25, held with 16 fractional bits
//
// N must be 16, 24 or 32 (the parameter constants are tabulated for these).

module chaos_map #(
    parameter integer N   = 32,
    parameter integer MAP = 0
)(
    input  wire         clk,
    input  wire         rst,    // synchronous reset, clears the state
    input  wire         load,   // loads x0 as the initial state
    input  wire [N-1:0] x0,
    output reg  [N-1:0] x       // current state
);

    // ---- parameter constants: floor(3.99 * 2^(N-2)) and floor(1.99 * 2^(N-1))
    localparam [31:0] RQ_C = (N == 16) ? 32'd65372 :
                             (N == 24) ? 32'd16735272 : 32'd4284229877;
    localparam [31:0] MQ_C = (N == 16) ? 32'd65208 :
                             (N == 24) ? 32'd16693329 : 32'd4273492459;
    localparam [N-1:0] RQ  = RQ_C[N-1:0];
    localparam [N-1:0] MQ  = MQ_C[N-1:0];
    localparam [32:0]  G   = 33'd7060275200;      // 107731.25 * 2^16
    localparam [N:0]   S   = {1'b1, {N{1'b0}}};   // 1.0

    reg [N-1:0] x_next;   // driven by exactly one generate branch

    generate
        if (MAP == 0) begin : g_logistic
            wire [N:0]     omx = S - {1'b0, x};            // 1 - x
            wire [2*N:0]   xp  = x * omx;                   // x (1 - x), 2N+1 bits
            wire [N-1:0]   p   = xp[2*N-1:N];               // truncated to N fractional bits
            wire [2*N-1:0] y   = RQ * p;
            always @* x_next = y[2*N-3:N-2];                // >> (N-2)
        end
        else if (MAP == 1) begin : g_tent
            wire [N:0]     omx = S - {1'b0, x};
            wire [N-1:0]   t   = x[N-1] ? omx[N-1:0] : x;   // min(x, 1 - x)
            wire [2*N-1:0] y   = MQ * t;
            always @* x_next = y[2*N-2:N-1];                // >> (N-1)
        end
        else if (MAP == 2) begin : g_mtm
            wire [N+7:0]   mx  = x * 8'd132;
            wire [N-1:0]   u   = mx[N-1:0];                 // frac(132 x)
            wire [N:0]     omu = S - {1'b0, u};
            wire [N-1:0]   t   = u[N-1] ? omu[N-1:0] : u;   // tent(u)
            wire [N+32:0]  y   = G * t;
            always @* x_next = y[N+15:16];                  // >> 16, then mod 1
        end
        else begin : g_mlm
            wire [N+7:0]   mx  = x * 8'd132;
            wire [N-1:0]   u   = mx[N-1:0];
            wire [N:0]     omu = S - {1'b0, u};
            wire [2*N:0]   up  = u * omu;                   // u (1 - u)
            wire [N-1:0]   p   = up[2*N-1:N];
            wire [N+32:0]  y   = G * p;
            always @* x_next = y[N+15:16];
        end
    endgenerate

    always @(posedge clk) begin
        if (rst)       x <= {N{1'b0}};
        else if (load) x <= x0;
        else           x <= x_next;
    end

endmodule
