# One-dimensional chaotic maps for lightweight cryptography — data and code

Supplementary material for the survey

> M. O. U. Islam, S. A. Khanday, J. Ali, "One-dimensional chaotic maps for lightweight cryptography:
> A taxonomy, critical review, and FPGA-based performance analysis", submitted to *Computer Science Review*.

The repository contains the record of the literature search and selection, the programs used for the
cycle-length measurements, the Verilog designs of the hardware evaluation, and the raw outputs.
Identifiers P1–P60 refer to the proposals assessed in the paper (Tables 6–12).

## Contents

```
screening/        search and selection records (Section 3)
measurements/     cycle-length measurements (Sections 8 and 9)
  floating_point/ IEEE-754 binary64 (and binary32) measurements, Brent's algorithm
  fixed_point/    n-bit fixed-point models, complete cycle structure, map-specific realisations
  claims/         test of a published cycle-length claim
hardware/         Verilog designs, testbenches, Vivado script, golden vectors (Section 9)
results/          raw program outputs used in the paper
```

### screening/

| File | Content |
|---|---|
| `search_log.md` | Databases, dates, exact queries, filters, result counts, and every screening step |
| `screening_stage1.csv` | All 949 Lens records with the first (rule-based) decision and reason |
| `second_screening.csv` | Independent second screening of the 342 new-map records and of 91 flagged excluded records |
| `screening_disagreements.csv` | The 46 records on which the two screenings disagree, their resolution, and the co-author decisions |
| `missed_candidates.csv` | The 37 records missed by the rules and found by the second screening |
| `open_access_download_list.csv` | The 22 open-access candidates and the outcome of full-text reading |
| `pdf_download_list.csv`, `ieee_download_list.csv` | Full-text reading lists of the Lens and IEEE Xplore searches |
| `ieee_screening.csv`, `ieee_screening_search2.csv` | Screening of the two IEEE Xplore exports |

### measurements/floating_point/

| File | Measures |
|---|---|
| `cycle_check.py`, `cycle_check.m`, `cycle_check_script.m` | Pilot maps (P1, P2, P5, P6, P10) and logistic reference, Python and MATLAB (Table 13) |
| `cycle_sweep.c` | 100-start statistics and the gain sweep of the MTM (P6) and BLM (P5) (Table 14, Fig. 4) |
| `ls_map_cycle.c` | LS map of P24 |
| `framework_maps_cycle.c` | P16 and P25 framework maps |
| `batch3_maps.c` | P32, P33, P37 |
| `batch6_maps.c` | P44, P46, P48, P49 |
| `batch7_maps.c` | P51 (binary64 and binary32), P53, P54, P55 |

### measurements/fixed_point/

| File | Measures |
|---|---|
| `fixedpoint_cycle_script.m` | Complete cycle structure of the maps at 16 and 24 bits (Table 15) |
| `mtm_fixed32.c` | 32-bit sampled cycles (Table 16) |
| `bitreversal_fixedpoint.c` | Bit-reversal countermeasure of P19 (Table 17) |
| `ltb_cycles.c` | LTB map of P34 in its 32-bit format |
| `plpwlcm.c` | 32-bit PL_PWLCM&LM of P41 |
| `edf.py` | Delayed-feedback logistic map of P35 at 17 precisions |
| `ecm_lut.c` | Look-up-table realisation of the exposine map (P36) |
| `bcm_cycle.c`, `bcm_cycle_constant_modulator.c`, `bcm_fpga_model.c` | BCM of P31: accumulating and constant modulator, and its 32-bit core with sine ROM |

### measurements/claims/

`ecm_lcm.c` — tests the statement that the cycle length of a digitised exponential chaotic model equals
the least common multiple of the cycles of its base and exponent maps; follows all 2^16 and 2^24 states.

### hardware/

`chaos_map.v` (four maps, N = 16, 24, 32), `tb_chaos_map.v` and `tb_all_chaos_maps.v` (bit-exact
self-checking testbenches), `gen_golden_script.m` and `golden_vectors.zip` (reference vectors),
`chaos_map.xdc` (10 ns clock constraint), `run_hw_all.tcl` (synthesis and implementation of all 12 cases).
All hardware figures in the paper are post-synthesis and post-implementation tool estimates
(Vivado, out-of-context, one Artix-7 device). No design was run on a board.

## Building and running

C programs (GCC; contraction of floating-point expressions disabled so that results are bit-reproducible):

```bash
gcc -O2 -ffp-contract=off -o prog file.c -lm
./batch7_maps 0 4e8        # map index, step limit (see the header comment of each file)
./batch6_maps 1 4e8
./batch3_maps 0 2e8
./cycle_sweep MTM 100      # modes: check, MTM, MLM, sweep_mtm <runs> <k>, sweep_blm <runs> <k>, ...
./bcm_cycle 1e9            # step limit
./bcm_fpga_model acc 1e9
./ecm_lcm                  # no arguments; the other programs without arguments likewise
```

Python 3: `python3 cycle_check.py [step_limit]`, `python3 edf.py`.
MATLAB: open the `*_script.m` files and press Run.
Vivado: `vivado -mode batch -source run_hw_all.tcl`; simulation with any Verilog simulator
(`iverilog -g2012 tb_all_chaos_maps.v chaos_map.v && vvp a.out`), after unzipping `golden_vectors.zip`
into the same folder.

Each program states in its header the map, the equation as read from the original paper, the
parameters and the number format. Long runs (step limits of 10^8–10^9) take minutes to hours.

## Notes

* The full texts of the reviewed papers are not included (copyright); DOIs are given in the CSV files.
* Random initial values come from a fixed-seed xorshift generator, so the C results are reproducible.

## Licence

Code: MIT licence (see `LICENSE`). Data (CSV, Markdown, text outputs): CC BY 4.0.
