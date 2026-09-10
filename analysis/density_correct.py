#!/usr/bin/env python3
"""
Correct measured thrust to standard sea level density (1.225 kg/m^3).

Thrust scales directly with air density, which swings ~10% between a cold
morning and a hot afternoon. Data taken on different days is NOT comparable
until corrected. This is the single most common way amateur thrust testing
produces numbers that look like real effects but are not.

    T_corrected = T_measured * (1.225 / rho_measured)

Usage:
    python density_correct.py run.csv --temp 22.4 --press 1008 --rh 44
"""
import argparse
import pandas as pd

R_DRY = 287.058     # J/(kg K)
R_VAP = 461.495     # J/(kg K)
RHO_STD = 1.225     # kg/m^3


def saturation_vapor_pressure(t_c):
    """Tetens equation, returns Pa."""
    return 610.78 * 10 ** (7.5 * t_c / (t_c + 237.3))


def humid_air_density(t_c, press_hpa, rh_pct):
    t_k = t_c + 273.15
    p_total = press_hpa * 100.0
    p_vapor = (rh_pct / 100.0) * saturation_vapor_pressure(t_c)
    p_dry = p_total - p_vapor
    return p_dry / (R_DRY * t_k) + p_vapor / (R_VAP * t_k)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv")
    ap.add_argument("--temp", type=float, required=True, help="deg C")
    ap.add_argument("--press", type=float, required=True, help="hPa (station pressure)")
    ap.add_argument("--rh", type=float, default=50.0, help="percent")
    ap.add_argument("--col", default="thrust_g")
    ap.add_argument("-o", "--out", default=None)
    args = ap.parse_args()

    rho = humid_air_density(args.temp, args.press, args.rh)
    factor = RHO_STD / rho

    df = pd.read_csv(args.csv)
    df["rho_kgm3"] = rho
    df["thrust_g_corr"] = df[args.col] * factor

    print("rho measured   = %.4f kg/m^3" % rho)
    print("correction     = x %.4f" % factor)
    print("mean raw       = %.1f g" % df[args.col].mean())
    print("mean corrected = %.1f g" % df["thrust_g_corr"].mean())

    out = args.out or args.csv.replace(".csv", "_corrected.csv")
    df.to_csv(out, index=False)
    print("wrote %s" % out)


if __name__ == "__main__":
    main()
