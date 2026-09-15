# Decisions and Technical Reference

Running record of what has been decided and why. Append as you go; never silently
edit a past decision — supersede it with a new dated entry so the reasoning trail
stays intact.

Last updated: 2026-09-14

---

## 1. Governing physics

**Thrust equation.** `F = ṁ(Ve − V0)`

Thrust falls to zero as airspeed approaches exhaust velocity. Ve is therefore a hard
ceiling on top speed, and raising it (higher voltage, tighter nozzle) is the only way
to raise that ceiling. A ducted fan cannot approach Mach 1 — Ve tops out near 140 m/s
against 343 m/s for sonic.

**Top speed is found where thrust meets drag:**

`ṁ(Ve − V) = ½ρV²·CD0·S`

Weight does not appear. At high speed, induced drag is negligible and parasite drag
dominates, so **top speed is governed by drag area (CD0·S), not mass.** Weight matters
for acceleration, climb, and turn — not Vmax. Optimising "speed per weight" would send
you chasing grams when you should be cleaning up the duct.

**Fan pressure rise.** `Δp_fan ≈ ½ρVe² ≈ 2,600 Pa` at Ve = 65 m/s.

Against 101,325 Pa ambient that is a pressure ratio of about **1.026**. A turbofan runs
30–40. Everything about ducting an EDF follows from this number.

**Duct losses.** `Δp_loss = K · q_duct`. At 45 m/s duct velocity, q_duct ≈ 1,240 Pa,
so a single bend at K = 0.3 costs ~370 Pa — **14% of everything the fan produces.**

---

## 2. Figures of merit

| Metric | Definition | Threshold | Objective |
|---|---|---|---|
| Duct loss coefficient | K = Δp_loss / q_duct | ≤ 0.35 | ≤ 0.15 |
| Fraction of fan rise lost | Δp_loss / Δp_fan | ≤ 15% | ≤ 7% |
| Installed thrust ratio | T_installed / T_bare | ≥ 88% | ≥ 93% |
| Pressure recovery (reference only) | PR = P̄t2 / Pt0 | ≥ 0.995 | ≥ 0.998 |
| Inlet distortion | DC60 | < 0.30 | < 0.15 |

Lead with **installed thrust ratio** in conversation — easiest to measure, hardest to
argue with, easiest to explain in an interview.

Specific excess power `Ps = (T − D)V / W` is the professionally correct performance
metric (energy-manoeuvrability theory, John Boyd). Use it rather than invented terms
like "speed efficiency," which are dimensionally undefined and will be attacked in a
design review.

---

## 3. Duct geometry rules

**Length budget**, in fan diameters D (70 mm):

| Station | Length | Note |
|---|---|---|
| Inlet lip radius | ≥ 0.08 D | Sharp lips separate at static/low speed |
| S-bend | R/D ≥ 3 | Below R/D = 2 the inner wall separates |
| Settling before fan | 1.0–1.5 D | Hard floor 1.0 D; below 0.5 D expect real distortion |
| Mixing after stator | 0.5–1.0 D | Before contraction begins |
| Nozzle contraction | ≥ 1.0 D | Hold constant across all nozzle variants |
| **Total** | **4–5 D** | ~300 mm of fuselage |

**Loss sources, ranked:**

| Source | Rough cost | Fix |
|---|---|---|
| Tip clearance | 3–10% | < 0.3 mm on a 70 mm fan. Biggest item most people ignore. |
| Inlet lip separation | 5–15% static | Lip radius ≥ 8% of inlet diameter |
| Diffusion (area growth) | 5–20% | Equivalent cone half-angle ≤ 3°, never above 5° |
| Bends | 5–15% each | R/D ≥ 3 |
| Wires/struts in duct | 2–6% | Route ESC wires outside or in a fairing |
| Surface roughness | 2–5% | Sand and epoxy-coat the interior |
| Hub blockage mismatch | 2–5% | Grow duct area as hub grows, hold annulus area constant |
| Base drag behind hub | 1–3% | Tailcone, not a blunt cutoff |

Contracting flow is stable and forgiving. **Diffusing flow separates the moment you
push it**, and once separated the duct is effectively blocked.

**Rectangular-to-round transition:**

1. Hold area constant through the transition. Never change shape and area at once.
2. Length ≥ 1.5–2× hydraulic diameter (D_h = 4A/P).
3. Corner radius ≥ 20% of the short side.
4. Build as a superellipse loft: `|x/a|ⁿ + |y/b|ⁿ = 1`, n ≈ 5 at inlet → n = 2 (circle)
   at the fan, adjusting a and b to hold area constant.
5. Inlet aspect ratio under 2:1.

**Sizing:** exit nozzle 78–90% FSA, inlet area ≥ 100% FSA (110% for static).
`FSA = (π/4)(D_fan² − D_hub²)`.

---

## 4. Measurement rules

**Density correction is mandatory.** `T_corr = T_meas × (1.225 / ρ_meas)`.
Air density runs 1.291 at 0 °C to 1.138 at 30 °C — a 13% swing. Uncorrected data taken
on different days is not comparable. This is the most common way amateur thrust testing
produces numbers that look like real effects but are not.

**Station pressure, not sea-level-adjusted.** Most weather apps report the adjusted
figure. Champaign is ~220 m elevation, so station pressure runs ~25 hPa lower. Getting
this wrong is a ~2.5% density error — your entire repeatability budget. Record which
convention you are using and stay consistent.

**Check standard every session.** Hang the 1 kg mass at the start of every session and
confirm it reads 1000 g. Log the reading. If it drifted, you found it before wasting a
session rather than after. The drift record across many sessions is itself a good figure
for a test report.

**Wind kills static testing.** A 5 m/s breeze into the inlet costs ṁ×5 ≈ 1 N of ~12.7 N
— an 8% error, four times the repeatability target, and it gusts so it is not correctable.
Set a hard cutoff (~1 m/s) and document the rule.

**Recirculation diagnosis.** Run a 90-second steady-throttle hold and log it. A clean
space gives a flat trace; recirculation shows as slow monotonic thrust decay as the fan
ingests its own wake. Converts "is this space big enough" into a number.

**Battery sag.** A 6S pack sags under 100 A and drops further as it discharges, so
ascending and descending throttle sweeps run at different voltages and produce apparent
hysteresis that is really state of charge. Fix: log voltage at every point and plot
against **electrical power**, not throttle percentage.

**Equal-area probe placement.** Divide the annulus into 8 equal-area rings, probe at each
ring's area centroid. Equal radial spacing over-weights the hub and biases the average.

---

## 5. Hardware decisions

| Item | Decision | Reasoning |
|---|---|---|
| Fan | Schübeler DS-30-AXI HDS 70 mm | CFD-optimised blading, machined alloy stator, FEA-stiffened rotor. A rotor that flexes under load changes tip clearance with throttle, which corrupts the measurement being made. |
| Load cell | SparkFun TAL220, 10 kg | Published datasheet: hysteresis ±0.05% FS, repeatability ±0.03% FS, creep ±0.05% FS/3 min, temp coeff ±0.05% FS/10 °C, 1000 Ω bridge, M4 + M5 mounting. Those specs populate the uncertainty budget and are citable. |
| Connector | **EC5**, not XT90 | EC5 ~120 A vs XT90 ~90 A. Continuous draw is ~100 A, so XT90 was marginally *under* the requirement. |
| Pressure sensor | Sensirion SDP810-500Pa | ±500 Pa, ~0.2% of reading. MPXV7002DP is ±2 kPa at 2.5% FS = ±50 Pa error against a 150–400 Pa signal. |
| Current sensing | INA226 + **external 100 A / 75 mV shunt** (0.75 mΩ) | Stock modules max at 20 A. 75 mV at 100 A sits inside the ±81.92 mV range. **12S caution:** INA226 bus input is 36 V max — mount the shunt low-side for 12S. Alternative: ACS758LCB-100B Hall sensor, isolated, ~2% accuracy. |
| Trainer | Freewing Yak-130 70 mm | Trainer airframe by design: thick less-swept wing, large tail, flaps. Description confirms gentle stall with wing-rocking warning and nose-down break. |
| Batteries | Zeee 6S 3300 mAh EC5, qty 2 | 4000 mAh XT60 unavailable in time. 3300 is inside Freewing's 3000–4000 range. Bought EC5→XT60 adapters for the trainer. |
| Receiver | RadioMaster ER8, qty 1 | ELRS **PWM** — most ELRS receivers are CRSF-only and will not drive servos directly. Cut from 2 to 1 at $50 each; spare only shortens downtime, doesn't prevent loss. |
| Transmitter | RadioMaster TX16S MKII, ELRS, **Mode 2** | Mode 2 = throttle left. US standard, assumed by every tutorial. |
| Charger | ISDT D2 MK2 | AC input, 2×200 W dual output, charges both packs simultaneously. |

**Deferred deliberately:** bench power supply (the $120 unit only covers partial
throttle; a 24 V/100 A unit is $400+ — try batteries with watt logging first), optical
tachometer (check ESC telemetry first), all rake hardware until Phase 3.

---

## 6. Thrust stand design

**Beam orientation: VERTICAL.** The TAL220 is a bending-beam cell and responds to force
applied *perpendicular* to its long axis. Mounting it horizontally with thrust acting
along its length produces almost no output.

- Fixed end bolted to baseplate (M5 + spacer), free end up carrying the fan bracket (M4 + spacer)
- Thrust acts horizontally = perpendicular to beam = sensitive axis
- Fan weight acts along the beam = axial = barely sensed. Dead weight mostly drops out.

**Moment rejection works in one plane only.** A single-point cell cancels the moment
from off-centre loading *along* the beam axis — that is why kitchen scales read the same
anywhere on the platform. Offsetting the fan **sideways**, out of that plane, is not
rejected and goes straight into the reading as error. Keep the fan axis in the beam's
plane and laterally centred.

**Spacers at both ends.** The beam must be free to flex. 3–5 mm is plenty; typical
deflection at rated capacity is a few tenths of a millimetre. Contact anywhere along the
beam means noise instead of readings.

**Nothing but the beam's free end touches the moving block.** Every stray wire or tie is
a parallel load path carrying force around the sensor. Route ESC wires with a generous
service loop.

**Flow clearances.** No structure within 1–2 fan diameters ahead of the inlet lip — at
static conditions the fan draws from a hemisphere. Elevate the fan so the exhaust jet
clears the baseplate entirely; a 65 m/s jet impinging on your own structure generates a
force that looks like a real effect.

**Portable by design.** One permanent bolted sandwich; only the C-clamps engage and
disengage. Calibration slope survives transport; zero will shift, which the tare handles.
Tall stand + thrust at the top = overturning moment, so clamps are not optional.

---

## 7. Experimental design

**One variable at a time.** Constant thrust tube, swappable nozzles. Joint upstream of
where contraction begins, with a spigot register so the interior is continuous — a 0.5 mm
step at that station trips a boundary layer already near separation. Contraction length
fixed at 1.0 D across all nozzles, so only exit area varies.

**Print all variants identically:** same material, layer height, orientation, ideally the
same session. Then **caliper every exit** — a 0.3 mm deviation on a 25 mm exit is a 2.4%
area error, larger than the difference between the 80% and 78% cases.

**Add a fifth constant-area nozzle** (100%, no contraction). Free extra data point that
makes the mass-flow-versus-exhaust-velocity tradeoff visible at both ends.

**Knowing what NOT to test.** Estimate effect size before adding a variable. If the
predicted difference is below measurement uncertainty (±2%), you cannot resolve it and
running the test produces noise you will be tempted to interpret. Contraction length over
0.75–1.5 D predicts well under 1% — screen it once with a single extra part, document it
as insignificant, and fix it.

**Phase 1 is bare fan + bellmouth only.** No S-duct. That measurement is the denominator
for every later result.

---

## 8. Corrections log

Recording these because the reasoning matters more than the conclusion.

| Date | Was | Corrected to | Why |
|---|---|---|---|
| 2026-09 | PR thresholds 0.95 / 0.98 | 0.995 / 0.998, and prefer K | Turbine practice. At PR 0.95 you lose 5,000 Pa against a 2,600 Pa fan rise — the aircraft would not move. |
| 2026-09 | MPXV7002DP | Sensirion SDP810 | ±50 Pa error against a 150–400 Pa signal |
| 2026-09 | Stock INA226 module | INA226 + external 100 A shunt | Onboard shunt maxes at 20 A |
| 2026-09 | Horizontal load cell beam | Vertical beam | Bending-beam cells respond perpendicular to the long axis, not axially |
| 2026-09 | ER8 ~$25–35 | ~$50 | Bad estimate; PWM receivers cost more than CRSF |
| 2026-09 | XT90 connectors | EC5 | XT90 ~90 A is under the ~100 A continuous draw |
| 2026-09 | "ER8G has gyro" | G = Glider (slim package); GV adds Vario | No stabilisation in either |

---

## 9. Open questions

- **Test space.** AE department faculty sponsor is the better ask than a room — it also
  unlocks tunnel time (Phase 3), Fluent license (Phase 4), and a recommendation letter.
  **Wait until Phase 1 data exists** before asking; "I got ±1.8% repeatability" lands
  differently than a description of intentions. Interim: parking garage (enclosed, no
  wind, nobody cares about noise) or calm mornings with a logged wind cutoff.
- **Current sensing:** external shunt (accurate, in the power path) vs ACS758 Hall
  (isolated, simpler, ~2%). Decide with Group B.
- **Tunnel access** for Phase 3. The largest credibility gap in the project is that
  static testing does not reproduce flight inlet conditions — at zero forward speed
  there is no forebody boundary layer to divert, which is the whole reason the diverter
  exists. Static numbers will flatter the design. Acknowledge it in the report regardless;
  a reviewer who sees the limitation named trusts the rest more, not less.
- **Adapter direction count** — check whether the 4-pack is four of one direction or two
  of each. If short, buy EC5 charge leads for the charger rather than more adapters.

---

## 10. Career framing

The stated goal is the measurement. The real goal is defence-relevant experience, which
is why the project is shaped as a phased test program with gates, uncertainty budgets,
and formal reports rather than as a fast RC jet.

**Decision test:** does this get me closer to defensible measured data? If yes, do it.
If it is about the airplane being cool or fast, it waits until after there are results.

Documentation layers, in order of how often they are touched: engineering log (every
session, markdown, append-only) → git commits (every session) → test reports (one per
phase, docx) → paper (last, a synthesis of the reports, not new writing).
