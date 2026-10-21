# Clock constraint for timing analysis of chaos_map (out-of-context synthesis).
# 10 ns = 100 MHz target; change the period to explore the maximum frequency.
create_clock -name clk -period 10.000 [get_ports clk]
