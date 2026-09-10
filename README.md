# Chin-Inlet EDF UAV Testbed

Quantifying the installed thrust penalty of a chin-mounted S-duct inlet on a
70 mm electric ducted fan, reducing it through design iteration, and validating
a CFD model against self-generated test data on a flying airframe.

**Baseline hardware:** Schubeler DS-30-AXI HDS 70 mm fan, 6S, with a deferred
12S upgrade path.

## Figures of merit

| Metric | Definition | Threshold | Objective |
|---|---|---|---|
| Duct loss coefficient | K = dp_loss / q_duct | <= 0.35 | <= 0.15 |
| Fraction of fan rise lost | dp_loss / dp_fan | <= 15% | <= 7% |
| Installed thrust ratio | T_installed / T_bare | >= 88% | >= 93% |
| Inlet pressure recovery | PR = Pt2_bar / Pt0 | >= 0.995 | >= 0.998 |
| Inlet distortion | DC60 | < 0.30 | < 0.15 |

Turbine-engine pressure recovery figures (0.95-0.98) do NOT apply here. The
fan's total pressure rise is only about 2600 Pa, so a 0.95 recovery would mean
losing roughly double everything the fan produces.

## Phases

| Phase | Content | Gate |
|---|---|---|
| 0 | Pilot proficiency on a foam trainer | 10 consecutive undamaged landings |
| 1 | Thrust stand, bare fan baseline | 3 sweeps / 3 days within +/-2% |
| 2 | Nozzle exit area trade study | 4 areas ranked with error bars |
| 3 | Inlet characterization (core) | K reduced by >= 0.15 |
| 4 | CFD correlation | Grid independent, within +/-8% |
| 5 | Airframe integration and flight test | Vmax within 15% of prediction |
| 6 | Tail configuration study | Dutch roll freq + damping, both configs |

## Layout

```
cad/        SolidWorks / NX models
firmware/   Arduino code for the thrust stand
data/raw/   Written by the logger. NEVER edited after the fact.
data/       processed/  outputs of analysis scripts
analysis/   Python
reports/    Formal test reports (.docx) - one per phase
photos/     Test setups, resized
LOG.md      Engineering log, append only
CONFIGS.md  Configuration register - every tested article
```

## Rules

1. **Raw data is evidence.** Write once, never edit. All processing writes to
   `data/processed/`.
2. **Every tested article gets an ID** in `CONFIGS.md`, recorded on every run.
3. **Record ambient conditions every session** and density-correct before
   comparing anything across days.
4. **Measure what you built, not what you drew.** Caliper every exit diameter.
5. **Log every session**, including the ones where nothing worked.
6. **Commit at the end of every session.**

## Getting started

```bash
pip install numpy pandas matplotlib
```

Firmware: open `firmware/thrust_stand/thrust_stand.ino` in the Arduino IDE.
It defaults to `SIMULATE 1`, which runs the whole program with synthetic
sensor readings so the serial protocol and calibration math can be debugged
with no hardware attached. Set to 0 once the load cell is wired.

Calibration workflow:

```bash
# 1. send 'c' over serial, hang known masses, record steady raw counts
# 2. build a CSV: mass_g,raw,direction
python analysis/calibrate.py data/raw/2026-09-15_cal.csv
# 3. paste countsPerGram into the firmware

# after a run:
python analysis/density_correct.py data/raw/2026-09-20_sweep.csv \
    --temp 22.4 --press 1008 --rh 44
```

Example files are prefixed `EXAMPLE_` and can be deleted once real data exists.
