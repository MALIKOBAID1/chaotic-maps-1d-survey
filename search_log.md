# Search log — survey of 1D chaotic maps for lightweight cryptography

Record of the literature search, for the methodology section and the selection flow diagram.

## Source

- Database: Lens.org, Scholarly Works
- Date of both searches: 4 October 2026
- Filters applied to both: Year Published 2018–2026; Publication Type = journal article
- Queries pasted unchanged into the query editor (confirmed by the author who ran them)

## Query 1 — 363 records exported

```
(title:("chaotic map" OR "chaotic maps" OR chaotification OR "chaotic system")
 OR abstract:("chaotic map" OR "chaotic maps" OR chaotification))
AND (title:("one-dimensional" OR "1D" OR "1-D")
 OR abstract:("one-dimensional" OR "1D" OR "1-D"))
AND (title:(encryption OR cryptography OR cipher OR "random number generator" OR "S-box" OR hash)
 OR abstract:(encryption OR cryptography OR cipher OR "random number generator" OR "S-box" OR hash))
```

## Query 2 — 723 records exported

```
title:(chaotification OR "chaotic map" OR "chaotic system" OR "chaotic model")
AND title:(new OR novel OR improved OR enhanced)
AND (title:(encryption OR cryptography OR cipher OR "random number" OR "S-box" OR hardware OR FPGA)
     OR abstract:(encryption OR cryptography OR cipher OR "random number" OR "S-box"))
```

## Flow counts

| Stage | Records |
|---|---|
| Identified, Query 1 | 363 |
| Identified, Query 2 | 723 |
| Total identified | 1,086 |
| Duplicates removed (by DOI, then by normalised title) | 137 |
| Screened on title and abstract | 949 |
| Excluded at screening | 607 |
| — higher-dimensional or continuous system | 366 |
| — uses existing maps, no new map | 213 |
| — cryptanalysis only (retained for discussion) | 16 |
| — no cryptographic use | 10 |
| — survey or comparison (retained for related work) | 2 |
| Included at screening | 116 |
| Uncertain, full text needed | 226 |

## Status and caveats

- Screening so far is rule-based (keyword rules on title, abstract and keywords). It has not yet been
  checked by two human screeners, which the protocol requires. Counts will change after that check.
- An earlier trial export (1,000 records, truncated and mostly off topic) was discarded and is not part
  of the search.
- Sensitivity check against papers known beforehand: 7 of the 13 pilot papers were retrieved by the two
  queries. The 6 not retrieved do not contain the search terms in their titles or abstracts. They enter
  the corpus as "records identified from other sources", together with papers found by checking
  reference lists (snowballing, not yet done).
- Only one database was searched. Scopus, Web of Science and IEEE Xplore were not searched; the paper
  must state this.

## Files

- `screening_stage1.csv` — all 949 records with decision and reason
- `pdf_download_list.csv` — 42 priority papers for the detailed-assessment tier

## Addition, 6 October 2026 — IEEE Xplore

- Source: IEEE Xplore, Advanced Search; export file `export2026_10_06-10_40_38.csv`
- Query, fields and filters: **TO BE RECORDED** (the author who ran it must supply the exact text)
- Records exported: 64 (60 IEEE journals, 4 partner-publisher journals; 2018–2026)
- Already in the Lens set: 19. New records: 45.
- Screening of the 64 (rule-based, then checked by reading all 64 titles):
  - in scope and already on the list: 6 (one of them, pilot paper P1, was previously "from other sources")
  - in scope, new: 2 — "One-Dimensional Nonlinear Model for Producing Chaos" (2018) and
    "Exponential Chaotic Model for Generating Robust Chaos" (2021)
  - candidates that need the full text: 4
  - out of scope or excluded: the rest (see `ieee_screening.csv`)
- Sensitivity check: the IEEE search retrieved pilot papers P1, P2 and P5; it did not retrieve P6, P7 or P9,
  which are also IEEE publications.

## Addition, 6 October 2026 — IEEE Xplore, second search

- Export file `export2026_10_06-10_53_41.csv`
- Search text as reported by the author who ran it: `chaotification OR "chaotic map" OR "chaotic model" OR "chaotic system"`
- Field searched, filters and the result count shown by IEEE Xplore: **TO BE RECORDED**
- Records exported: 1,000 (970 IEEE journals, 30 partner-publisher journals; 2018–2026).
  Exactly 1,000 records suggests the export was cut off at a limit; the true result count is needed.
  Only 323 of the 1,000 have a search term in the title, so the search ran on all metadata, not on titles.
- Overlap: 45 records were in the first IEEE export, 66 in the Lens set.
- Screening: keyword rules gave 55 candidates, of which 39 were not already known; all 39 titles were read.
  - in scope, new: 10
  - candidates that need the full text: 11
  - the remaining 18: higher-dimensional systems, protocols or applications of existing maps, or already cited
- Sensitivity check: retrieved pilot papers P1, P2, P5, P6 and P7 and both frameworks found in the first IEEE search.
  Did not retrieve the first author's IEEE Internet of Things Journal paper (STCAMI-NCM).

## Full-text reading of IEEE Xplore records, 20 October 2026

- 11 of the 27 records on `ieee_download_list.csv` received and read.
- In scope: 6 (P38–P43 in the manuscript).
- Excluded: 5 — two-dimensional maps (3), cipher built on an existing generator without a new map (1),
  no cryptographic application (1).
- Not yet read: 16 (8 in scope at screening, 8 candidates).
- The manuscript now counts IEEE Xplore as a second source; the exact queries, fields, filters and result
  counts are still **TO BE RECORDED**.

## Full-text reading of IEEE Xplore records, 20 October 2026 (second part)

- The remaining 16 records on `ieee_download_list.csv` received and read; all 27 are now read.
- In scope: 6 (P44–P49 in the manuscript).
- Excluded: 10 — maps or models of dimension two or higher (5); a 1D framework proposed only as a step to a
  higher-dimensional system that alone is implemented and applied (2); analogue transistor circuits without
  cryptographic application (2); framework applied to chaos-shift-keying communication only (1).
- IEEE Xplore totals: 27 read, 12 in scope, 15 excluded.
- In-scope corpus now 51 (39 Lens, 12 IEEE Xplore); 45 assessed from the full text; 6 PDFs from the first list
  still pending.
- The exact IEEE Xplore queries, fields, filters and result counts are still **TO BE RECORDED**.

## Update, 20 October 2026

- Second IEEE Xplore search, as shown by the database (screenshot supplied by the author who ran it):
  `("All Metadata":chaotification OR "All Metadata":"chaotic map" OR "All Metadata":"chaotic model" OR "All Metadata":"chaotic system")`,
  filters Journals and 2018–2026, **2,017 results**. The export of 1,000 records is the first 1,000 in order of
  relevance; the remaining 1,017 were not exported or screened.
- First IEEE Xplore search: exact query text still **TO BE RECORDED**; 64 records, all exported.
- Decision by the authors: the six PDFs still pending from `pdf_download_list.csv` are not pursued. They are
  reported in the flow as "full text not obtained, not assessed" (6). In-scope corpus 51; assessed 45.
- The keyword-based taxonomy table of the Lens papers was removed; classification now rests on the 45 full texts.

## Second screening, 20 October 2026

- The 342 records proposing a new map (title and abstract) and the 91 rule-excluded records that mention hardware
  or degradation were screened a second time, independently of the keyword rules (`second_screening.csv`).
- Agreement with the rules on selection for full text (342 records): 90% (Cohen's kappa 0.61); 92% (kappa 0.63)
  when uncertain records are counted as not selected.
- The second screening rejected the six records that full-text reading later excluded.
- Missed by the rules: 37 records (20 apparently in scope, 17 uncertain) — `missed_candidates.csv`.
- The 22 open-access records were sought (`open_access_download_list.csv`); all 22 received and read:
  11 in scope (P50–P60), 11 excluded (EC1 3, EC2/EC3 3, IC3 2, IC5 2, EC4 1).
- Not obtained: 15 (paywalled); 8 of them appear to be in scope. Reported in the
  flow table and threats to validity; not assessed.
- The disagreements between the two screenings still have to be checked by a human reviewer.

## Resolution of screening disagreements, 20 October 2026

- 17 disputed records not settled by full text (15 not obtained, 2 selected by the rules only) were screened again by a
  co-author on title and abstract, without access to the earlier decisions (`screening_disagreements.csv`).
- Six of his decisions conflicted with the criteria or with included proposals and were resolved by discussion.
- Final: 14 of the 15 not obtained are in scope on title and abstract, 1 excluded; the 2 rule-selected records are
  excluded (EC2, IC5).
- In-scope corpus: 74 (Lens 37, IEEE Xplore 12, second screening 25); assessed 56; full text not obtained 18.
