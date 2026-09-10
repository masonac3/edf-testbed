#!/usr/bin/env python3
"""
Fit the load cell calibration from a mass sweep.

Usage:
    python calibrate.py cal_sweep.csv

Input CSV with columns: mass_g,raw,direction
  direction is 'up' or 'down' so hysteresis can be reported separately.

Prints countsPerGram and zeroOffset to paste into the firmware, plus the
linearity and hysteresis numbers that belong in your uncertainty budget.
"""
import sys
import numpy as np
import pandas as pd


def main(path):
    df = pd.read_csv(path)
    x = df["mass_g"].to_numpy(float)
    y = df["raw"].to_numpy(float)

    slope, intercept = np.polyfit(x, y, 1)
    pred = slope * x + intercept
    resid = y - pred
    ss_res = float(np.sum(resid ** 2))
    ss_tot = float(np.sum((y - y.mean()) ** 2))
    r2 = 1 - ss_res / ss_tot if ss_tot else float("nan")

    full_scale_counts = slope * x.max()
    max_dev_counts = float(np.max(np.abs(resid)))
    nonlin_pct_fs = 100 * max_dev_counts / full_scale_counts

    print("countsPerGram = %.4f" % slope)
    print("zeroOffset    = %d" % round(intercept))
    print("R^2           = %.6f" % r2)
    print("nonlinearity  = %.3f %% FS  (max deviation %.0f counts)"
          % (nonlin_pct_fs, max_dev_counts))

    if "direction" in df.columns:
        up = df[df["direction"] == "up"].set_index("mass_g")["raw"]
        dn = df[df["direction"] == "down"].set_index("mass_g")["raw"]
        common = up.index.intersection(dn.index)
        if len(common):
            diff = (dn[common] - up[common]).abs()
            hyst_pct_fs = 100 * float(diff.max()) / full_scale_counts
            print("hysteresis    = %.3f %% FS  (max %.0f counts at %g g)"
                  % (hyst_pct_fs, diff.max(), diff.idxmax()))
        else:
            print("hysteresis    = no matching up/down masses found")
    else:
        print("hysteresis    = no 'direction' column, skipped")

    print()
    print("Paste into thrust_stand.ino:")
    print("  float countsPerGram = %.4f;" % slope)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
