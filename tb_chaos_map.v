// tb_chaos_map.v
// Self-checking testbench: compares the hardware state with the fixed-point
// software model, state by state.
// Golden file: one hex value per line; line 0 is x0, lines 1..K are the next states.
// Override N, MAP and the file name from the simulator, e.g. (Vivado Tcl):
//   set_property generic {N=24 MAP=2 GOLDEN="golden_map2_n24.hex"} [get_filesets sim_1]

`timescale 1ns / 1ps

module tb_chaos_map;
    parameter integer N   = 32;
    parameter integer MAP = 0;
    parameter integer K   = 1000;
    parameter GOLDEN      = "golden_map0_n32.hex";

    reg          clk = 1'b0;
    reg          rst = 1'b1;
    reg          load = 1'b0;
    reg  [N-1:0] x0;
    wire [N-1:0] x;

    reg  [N-1:0] golden [0:K];
    integer i, errors;

    chaos_map #(.N(N), .MAP(MAP)) dut (.clk(clk), .rst(rst), .load(load), .x0(x0), .x(x));

    always #5 clk = ~clk;

    initial begin
        $readmemh(GOLDEN, golden);
        errors = 0;
        x0 = golden[0];
        @(posedge clk); #1 rst = 1'b0; load = 1'b1;
        @(posedge clk); #1 load = 1'b0;          // x now holds x0
        if (x !== golden[0]) errors = errors + 1;
        for (i = 1; i <= K; i = i + 1) begin
            @(posedge clk); #1;
            if (x !== golden[i]) begin
                errors = errors + 1;
                if (errors <= 5)
                    $display("MISMATCH at iteration %0d: hardware %h, model %h", i, x, golden[i]);
            end
        end
        if (errors == 0)
            $display("PASS  N=%0d MAP=%0d: %0d iterations match the fixed-point model", N, MAP, K);
        else
            $display("FAIL  N=%0d MAP=%0d: %0d mismatches in %0d iterations", N, MAP, errors, K);
        $finish;
    end
endmodule
