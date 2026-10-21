# run_hw_all.tcl
# Synthesises and implements chaos_map for all 12 cases (N = 16, 24, 32; MAP = 0..3)
# and writes the reports plus one summary table.
#
# How to run (no typing needed):
#   1. Open the project and CLOSE any open simulation.
#   2. Menu: Tools -> Run Tcl Script... -> select this file.
#   3. Wait. Results appear in the folder  <project folder>/hw_results .
#
# Flow per case: out-of-context synthesis -> opt -> place -> route, clock target 10 ns.
# All figures are tool estimates (post-synthesis and post-implementation).
# Nothing here is an on-board measurement.

close_sim -quiet
set proj_dir [get_property DIRECTORY [current_project]]
set part     [get_property PART [current_project]]
set out_dir  [file join $proj_dir hw_results]
file mkdir $out_dir

# counts primitive cells whose reference name matches a pattern
proc cnt {pat} {
    return [llength [get_cells -quiet -hierarchical -filter "IS_PRIMITIVE && REF_NAME =~ $pat"]]
}

set period 10.000
set csv [open [file join $out_dir summary.csv] w]
puts $csv "N,MAP,stage,LUT,FF,DSP48,CARRY4,WNS_ns,Fmax_MHz,status"
flush $csv

foreach n {16 24 32} {
    foreach m {0 1 2 3} {
        set tag "n${n}_map${m}"
        puts "=================== case $tag ==================="
        catch {close_design -quiet}

        if {[catch {
            # ---------------- synthesis ----------------
            synth_design -top chaos_map -part $part -mode out_of_context \
                         -generic N=$n -generic MAP=$m
            if {[llength [get_clocks -quiet]] == 0} {
                create_clock -name clk -period $period [get_ports clk]
            }
            report_utilization -file [file join $out_dir "${tag}_synth_utilization.rpt"]
            puts $csv "$n,$m,post-synthesis,[cnt LUT*],[cnt FD*],[cnt DSP48*],[cnt CARRY4],,,ok"
            flush $csv

            # ---------------- implementation ----------------
            opt_design
            place_design
            route_design
            report_utilization    -file [file join $out_dir "${tag}_impl_utilization.rpt"]
            report_timing_summary -file [file join $out_dir "${tag}_impl_timing.rpt"]
            report_power          -file [file join $out_dir "${tag}_impl_power.rpt"]

            set wns  [get_property SLACK [get_timing_paths -setup -max_paths 1]]
            set fmax [format %.2f [expr {1000.0 / ($period - $wns)}]]
            puts $csv "$n,$m,post-implementation,[cnt LUT*],[cnt FD*],[cnt DSP48*],[cnt CARRY4],$wns,$fmax,ok"
            flush $csv
        } err]} {
            puts "ERROR in case $tag: $err"
            puts $csv "$n,$m,error,,,,,,,\"$err\""
            flush $csv
        }
    }
}

catch {close_design -quiet}
close $csv
puts "ALL CASES DONE. Results are in: $out_dir"
