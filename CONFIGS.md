# Configuration Register

Every physical article that goes on the stand gets an ID and a row here.
Record the ID on every data run. Two identical-looking printed nozzles are
not identical, and in six months you will not remember which was which.

## Naming

```
TS-<rev>       thrust stand         e.g. TS-A
BM-<rev>       bellmouth inlet      e.g. BM-A
N-<area>-<rev> nozzle, % of FSA     e.g. N-085-A
TT-<rev>       thrust tube          e.g. TT-A
I-<type>-<rev> inlet duct           e.g. I-SD1-A
RK-<rev>       pressure rake        e.g. RK-A
```

## Register

| ID | Description | Material | Built | Measured dims | Status | Notes |
|---|---|---|---|---|---|---|
| TS-A | Thrust stand, plywood base, 10 kg cell | - | | | planned | |
| BM-A | Bellmouth, lip r = 0.08 D | PETG | | | planned | Reference inlet for all phases |
| N-100-A | Constant area extension (no contraction) | PETG | | | planned | Free 5th data point |
| N-090-A | Nozzle, 90% FSA | PETG | | | planned | |
| N-085-A | Nozzle, 85% FSA | PETG | | | planned | |
| N-080-A | Nozzle, 80% FSA | PETG | | | planned | |
| N-078-A | Nozzle, 78% FSA | PETG | | | planned | |
| I-STR-A | Straight duct control | PETG | | | planned | |
| I-SD1-A | Chin S-duct baseline | PETG | | | planned | |
| I-SD2-A | Chin S-duct + BL diverter | PETG | | | planned | |

All nozzles: same contraction length (1.0 D), same material, same layer
height, same print orientation. Only exit diameter varies.
