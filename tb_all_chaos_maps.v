// tb_all_chaos_maps.v
// Runs all 12 cases (4 maps x 3 word lengths) in ONE simulation and prints
// one PASS/FAIL line per case, followed by a summary.
// Needs chaos_map.v and the 12 golden_map<M>_n<N>.hex files.
// In Vivado: add as a simulation source, right-click it, "Set as Top", run.

`timescale 1ns / 1ps

// ---------------------------------------------------------------- one case
module check_case #(
    parameter integer N   = 32,
    parameter integer MAP = 0,
    parameter integer K   = 1000,
    parameter GOLDEN      = "golden_map0_n32.hex"
)(
    input  wire clk,
    output reg  done,
    output reg  pass
);
    reg          rst  = 1'b1;
    reg          load = 1'b0;
    reg  [N-1:0] x0;
    wire [N-1:0] x;
    reg  [N-1:0] golden [0:K];
    integer i, errors;

    chaos_map #(.N(N), .MAP(MAP)) dut (.clk(clk), .rst(rst), .load(load), .x0(x0), .x(x));

    initial begin
        done = 1'b0; pass = 1'b0;
        $readmemh(GOLDEN, golden);
        errors = 0;
        x0 = golden[0];
        @(posedge clk); #1 rst = 1'b0; load = 1'b1;
        @(posedge clk); #1 load = 1'b0;
        if (x !== golden[0]) errors = errors + 1;
        for (i = 1; i <= K; i = i + 1) begin
            @(posedge clk); #1;
            if (x !== golden[i]) begin
                errors = errors + 1;
                if (errors <= 3)
                    $display("  N=%0d MAP=%0d MISMATCH at iteration %0d: hardware %h, model %h",
                             N, MAP, i, x, golden[i]);
            end
        end
        if (errors == 0)
            $display("PASS  N=%0d MAP=%0d: %0d iterations match the fixed-point model", N, MAP, K);
        else
            $display("FAIL  N=%0d MAP=%0d: %0d mismatches in %0d iterations", N, MAP, errors, K);
        pass = (errors == 0);
        done = 1'b1;
    end
endmodule

// ---------------------------------------------------------------- all cases
module tb_all_chaos_maps;
    reg clk = 1'b0;
    always #5 clk = ~clk;

    wire [11:0] done, pass;

    check_case #(.N(16), .MAP(0), .GOLDEN("golden_map0_n16.hex")) c00 (.clk(clk), .done(done[0]),  .pass(pass[0]));
    check_case #(.N(16), .MAP(1), .GOLDEN("golden_map1_n16.hex")) c01 (.clk(clk), .done(done[1]),  .pass(pass[1]));
    check_case #(.N(16), .MAP(2), .GOLDEN("golden_map2_n16.hex")) c02 (.clk(clk), .done(done[2]),  .pass(pass[2]));
    check_case #(.N(16), .MAP(3), .GOLDEN("golden_map3_n16.hex")) c03 (.clk(clk), .done(done[3]),  .pass(pass[3]));
    check_case #(.N(24), .MAP(0), .GOLDEN("golden_map0_n24.hex")) c04 (.clk(clk), .done(done[4]),  .pass(pass[4]));
    check_case #(.N(24), .MAP(1), .GOLDEN("golden_map1_n24.hex")) c05 (.clk(clk), .done(done[5]),  .pass(pass[5]));
    check_case #(.N(24), .MAP(2), .GOLDEN("golden_map2_n24.hex")) c06 (.clk(clk), .done(done[6]),  .pass(pass[6]));
    check_case #(.N(24), .MAP(3), .GOLDEN("golden_map3_n24.hex")) c07 (.clk(clk), .done(done[7]),  .pass(pass[7]));
    check_case #(.N(32), .MAP(0), .GOLDEN("golden_map0_n32.hex")) c08 (.clk(clk), .done(done[8]),  .pass(pass[8]));
    check_case #(.N(32), .MAP(1), .GOLDEN("golden_map1_n32.hex")) c09 (.clk(clk), .done(done[9]),  .pass(pass[9]));
    check_case #(.N(32), .MAP(2), .GOLDEN("golden_map2_n32.hex")) c10 (.clk(clk), .done(done[10]), .pass(pass[10]));
    check_case #(.N(32), .MAP(3), .GOLDEN("golden_map3_n32.hex")) c11 (.clk(clk), .done(done[11]), .pass(pass[11]));

    integer k, npass;
    initial begin
        wait (&done);
        #1;
        npass = 0;
        for (k = 0; k < 12; k = k + 1) if (pass[k]) npass = npass + 1;
        $display("SUMMARY: %0d of 12 cases passed", npass);
        $finish;
    end
endmodule
