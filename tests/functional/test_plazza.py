#!/usr/bin/env python3

import re
import os
import sys
import subprocess

BIN = os.environ.get("PLAZZA_BIN", "./plazza")
TIMEOUT = 10
_passed = 0
_failed = 0
_failures = []

GRN = "\033[92m"
RED = "\033[91m"
CYN = "\033[96m"
RST = "\033[0m"
BLD = "\033[1m"

FAST = (0.01, 5, 100)


def run(*args):
    try:
        r = subprocess.run(
            [BIN] + [str(a) for a in args],
            input="", capture_output=True, text=True, timeout=TIMEOUT,
        )
        return r.returncode, r.stdout.splitlines(), r.stderr.splitlines()
    except subprocess.TimeoutExpired:
        return -1, [], []


def send(plazza_args, order):
    try:
        r = subprocess.run(
            [BIN] + [str(a) for a in plazza_args],
            input=order + "\n", capture_output=True, text=True, timeout=TIMEOUT,
        )
        return r.returncode, r.stdout.splitlines(), r.stderr.splitlines()
    except subprocess.TimeoutExpired:
        return -1, [], []


def ok(name):
    global _passed
    _passed += 1
    print(f"  {GRN}✓{RST} {name}")


def fail(name, detail=""):
    global _failed
    _failed += 1
    _failures.append((name, detail))
    print(f"  {RED}✗{RST} {name}" + (f"  [{detail}]" if detail else ""))


def chk(name, condition, detail=""):
    if condition:
        ok(name)
    else:
        fail(name, detail)


def section(title):
    print(f"\n{CYN}{BLD}── {title} ──{RST}")


def ready_count(lines):
    return sum(1 for l in lines if l.startswith("[READY]"))


def ready_lines(lines):
    return [l for l in lines if l.startswith("[READY]")]


section("1. Wrong argument count")

for label, args in [
    ("no args", []),
    ("1 arg", ["1"]),
    ("2 args", ["1", "2"]),
    ("4 args", ["1", "2", "3", "4"]),
    ("5 args", ["1", "2", "3", "4", "5"]),
]:
    code, out, err = run(*args)
    all_lines = out + err
    chk(f"exit 84 with {label}", code == 84, f"got {code}")
    chk(f"usage printed with {label}",
        any("Usage" in l or "usage" in l for l in all_lines),
        str(all_lines[:2]))

section("2. Multiplier invalid values")

for label, val in [
    ("multiplier=0", 0),
    ("multiplier=-1", -1),
    ("multiplier=-0.5", -0.5),
    ("multiplier=abc", "abc"),
    ("multiplier=--1", "--1"),
]:
    code, _, _ = run(val, 2, 100)
    chk(f"exit 84 with {label}", code == 84, f"got {code}")

section("3. nb_cooks invalid values")

for label, val in [
    ("nb_cooks=0", 0),
    ("nb_cooks=-1", -1),
    ("nb_cooks=-10", -10),
    ("nb_cooks=abc", "abc"),
]:
    code, _, _ = run(1, val, 100)
    chk(f"exit 84 with {label}", code == 84, f"got {code}")

section("4. replenish_time invalid values")

for label, val in [
    ("replenish=0", 0),
    ("replenish=-1", -1),
    ("replenish=-500", -500),
    ("replenish=abc", "abc"),
]:
    code, _, _ = run(1, 2, val)
    chk(f"exit 84 with {label}", code == 84, f"got {code}")

section("5. Valid parameters exit 0")

for label, args in [
    ("1 2 1000", [1, 2, 1000]),
    ("0.1 2 500", [0.1, 2, 500]),
    ("0.01 5 100", [0.01, 5, 100]),
    ("0.5 3 200", [0.5, 3, 200]),
    ("2 1 1000", [2, 1, 1000]),
    ("1 10 500", [1, 10, 500]),
    ("0.001 2 50", [0.001, 2, 50]),
    ("10 2 1000", [10, 2, 1000]),
]:
    code, _, _ = send(args, "")
    chk(f"exit 0 with {label}", code == 0, f"got {code}")

section("6. Usage message format")

code, out, err = run()
all_lines = out + err
chk("usage mentions multiplier",
    any("multiplier" in l for l in all_lines), str(all_lines))
chk("usage mentions nb_cooks",
    any("nb_cooks" in l for l in all_lines), str(all_lines))
chk("usage mentions replenish",
    any("replenish" in l for l in all_lines), str(all_lines))
chk("exit 84 with no args", code == 84, f"got {code}")

section("7. Basic orders produce READY")

for pizza_type, size, label in [
    ("americana", "M", "americana M"),
    ("margarita", "S", "margarita S"),
    ("regina", "L", "regina L"),
    ("fantasia", "XL", "fantasia XL"),
]:
    code, out, _ = send(FAST, f"{pizza_type} {size} x1")
    chk(f"exit 0 for {label}", code == 0, f"got {code}")
    chk(f"1 READY for {label}", ready_count(out) == 1,
        f"got {ready_count(out)}: {out}")

section("8. All pizza types in READY output")

for pizza_type in ["americana", "margarita", "regina", "fantasia"]:
    code, out, _ = send(FAST, f"{pizza_type} M x1")
    chk(f"[READY] {pizza_type} present",
        any(f"[READY] {pizza_type}" in l for l in out),
        str(out))

section("9. All pizza sizes produce READY")

for size in ["S", "M", "L", "XL", "XXL"]:
    code, out, _ = send(FAST, f"americana {size} x1")
    chk(f"READY for size {size}", ready_count(out) == 1,
        f"got {ready_count(out)}: {out}")

section("10. READY message exact format")

code, out, _ = send(FAST, "americana M x1")
chk("READY starts with [READY]",
    any(l.startswith("[READY]") for l in out), str(out))
chk("READY contains pizza type",
    any("[READY] americana" in l for l in out), str(out))
chk("READY contains pizza size",
    any("[READY] americana M" in l for l in out), str(out))

section("11. Quantity parsing x1..x9")

for qty in [1, 2, 3, 5, 9]:
    code, out, _ = send(FAST, f"margarita S x{qty}")
    chk(f"x{qty} → {qty} READY", ready_count(out) == qty,
        f"got {ready_count(out)}")

section("12. Semicolon separator")

code, out, _ = send(FAST, "americana M x1; margarita S x1")
chk("2 pizzas via ; → 2 READY", ready_count(out) == 2,
    f"got {ready_count(out)}: {out}")

code, out, _ = send(FAST, "americana M x1; margarita S x1; regina L x1")
chk("3 pizzas via ; → 3 READY", ready_count(out) == 3,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "margarita S x2; americana M x2")
chk("x2 + x2 via ; → 4 READY", ready_count(out) == 4,
    f"got {ready_count(out)}")

section("13. Invalid pizza type")

code, out, _ = send(FAST, "hawaiian M x1")
chk("unknown type → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")
chk("unknown type → exit 0", code == 0, f"got {code}")

code, out, _ = send(FAST, "pizza M x1")
chk("'pizza' type → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "calzone L x1")
chk("'calzone' type → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

section("14. Invalid pizza size")

code, out, _ = send(FAST, "americana XXXL x1")
chk("unknown size → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")
chk("unknown size → exit 0", code == 0, f"got {code}")

code, out, _ = send(FAST, "americana medium x1")
chk("'medium' size → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "americana small x1")
chk("'small' size → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

section("15. Invalid quantity format")

code, out, _ = send(FAST, "americana M 2")
chk("qty without x → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "americana M x0")
chk("x0 → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "americana M x")
chk("bare 'x' → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "americana M x-1")
chk("x-1 → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

section("16. Empty and whitespace lines")

code, out, _ = send(FAST, "")
chk("empty line → exit 0", code == 0, f"got {code}")
chk("empty line → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "   ")
chk("whitespace line → exit 0", code == 0, f"got {code}")

section("17. Status command - no kitchens")

code, out, _ = send(FAST, "status")
chk("status no kitchens → exit 0", code == 0, f"got {code}")
chk("status no kitchens → 'No kitchens running.'",
    any("No kitchens running" in l for l in out), str(out))

section("18. Status command - with active kitchen")

code, out, _ = send(FAST, "americana M x1\nstatus")
chk("status with kitchen → exit 0", code == 0, f"got {code}")
chk("status shows 'Kitchen #1'",
    any("Kitchen #1" in l for l in out), str(out))
chk("status shows cook count",
    any("cooks" in l for l in out), str(out))
chk("status shows pizzas in queue",
    any("pizzas" in l for l in out), str(out))

section("20. Status shows ingredient stock")

code, out, _ = send(FAST, "americana M x1\nstatus")
chk("status shows 'Stock:'",
    any("Stock:" in l for l in out), str(out))
chk("status shows Dough",
    any("Dough" in l for l in out), str(out))
chk("status shows Tomato",
    any("Tomato" in l for l in out), str(out))
chk("status shows all 9 ingredients",
    sum(1 for l in out for ing in
        ["Dough", "Tomato", "Gruyere", "Ham", "Mushrooms",
         "Steak", "Eggplant", "Goat cheese", "Chief love"]
        if ing in l) >= 9, str(out))

section("21. Load balancing - 2 kitchens spawn")

code, out, _ = send((0.01, 3, 100), "margarita S x7\nstatus")
chk("7 orders with 3 cooks → Kitchen #2 exists",
    any("Kitchen #2" in l for l in out), str(out))
chk("7 orders all complete → 7 READY", ready_count(out) == 7,
    f"got {ready_count(out)}")

code, out, _ = send((0.01, 3, 100), "margarita S x6\nstatus")
chk("6 orders = capacity of 1 kitchen → only Kitchen #1",
    any("Kitchen #1" in l for l in out) and
    not any("Kitchen #2" in l for l in out), str(out))

section("22. READY count matches ordered quantity")

for qty in [1, 3, 5]:
    code, out, _ = send(FAST, f"americana M x{qty}")
    chk(f"x{qty} → exactly {qty} READY", ready_count(out) == qty,
        f"got {ready_count(out)}")

code, out, _ = send(FAST, "americana M x2; margarita S x2")
chk("x2 + x2 → exactly 4 READY", ready_count(out) == 4,
    f"got {ready_count(out)}")

section("23. Exit code for normal operations")

code, _, _ = send(FAST, "americana M x1")
chk("order → exit 0", code == 0, f"got {code}")

code, _, _ = send(FAST, "status")
chk("status only → exit 0", code == 0, f"got {code}")

code, _, _ = send(FAST, "")
chk("empty stdin → exit 0", code == 0, f"got {code}")

code, _, _ = send(FAST, "americana M x1; margarita S x2")
chk("multi order → exit 0", code == 0, f"got {code}")

code, _, _ = send(FAST, "badpizza M x1")
chk("invalid order only → exit 0", code == 0, f"got {code}")

section("24. No stderr for valid operations")

code, _, err = send(FAST, "americana M x1")
chk("valid order → no stderr", err == [], repr(err))

code, _, err = send(FAST, "status")
chk("status → no stderr", err == [], repr(err))

section("25. Mixed valid and invalid orders")

code, out, _ = send(FAST, "americana M x1; badpizza S x1")
chk("valid + invalid → 1 READY", ready_count(out) == 1,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "americana M x1; americana XXXL x1")
chk("valid + bad size → 1 READY", ready_count(out) == 1,
    f"got {ready_count(out)}")

section("26. Extra whitespace in order")

code, out, _ = send(FAST, "  americana  M  x1  ")
chk("extra spaces → 1 READY", ready_count(out) == 1,
    f"got {ready_count(out)}: {out}")

code, out, _ = send(FAST, "americana M x1  ;  margarita S x1")
chk("spaces around ; → 2 READY", ready_count(out) == 2,
    f"got {ready_count(out)}")

section("27. Case sensitivity")

code, out, _ = send(FAST, "AMERICANA M x1")
chk("uppercase type → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "Americana M x1")
chk("titlecase type → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

section("28. Large quantities")

code, out, _ = send((0.01, 10, 100), "margarita S x10")
chk("x10 → 10 READY", ready_count(out) == 10,
    f"got {ready_count(out)}")

code, out, _ = send((0.01, 10, 100), "americana M x20")
chk("x20 → 20 READY", ready_count(out) == 20,
    f"got {ready_count(out)}")

section("29. All 4 types in one order line")

code, out, _ = send(FAST,
    "americana M x1; margarita S x1; regina L x1; fantasia XL x1")
chk("4 different types → 4 READY", ready_count(out) == 4,
    f"got {ready_count(out)}")
chk("[READY] americana present",
    any("[READY] americana" in l for l in out), str(ready_lines(out)))
chk("[READY] margarita present",
    any("[READY] margarita" in l for l in out), str(ready_lines(out)))
chk("[READY] regina present",
    any("[READY] regina" in l for l in out), str(ready_lines(out)))
chk("[READY] fantasia present",
    any("[READY] fantasia" in l for l in out), str(ready_lines(out)))

section("30. Multiplier affects baking time")

code, out, _ = send((0.01, 2, 100), "margarita S x1")
chk("0.01 multiplier → terminates fast", code != -1, "TIMEOUT")

code, out, _ = send((5, 2, 100), "margarita S x1")
chk("5x multiplier → still terminates", code != -1, "TIMEOUT")

section("31. Multiple lines of orders")

code, out, _ = send(FAST, "americana M x1\nmargarita S x1")
chk("2 order lines → 2 READY", ready_count(out) == 2,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "americana M x1\nmargarita S x1\nregina L x1")
chk("3 order lines → 3 READY", ready_count(out) == 3,
    f"got {ready_count(out)}")

section("32. READY count per order exactly matches")

code, out, _ = send(FAST, "margarita S x3")
chk("x3 → exactly 3 READY lines", ready_count(out) == 3,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "margarita S x3")
rl = ready_lines(out)
chk("all 3 READY lines say margarita S",
    all("[READY] margarita S" in l for l in rl),
    str(rl))

section("33. Status shows correct cook count")

code, out, _ = send((0.01, 2, 100), "americana M x1\nstatus")
chk("nb_cooks=2 → '2 cooks' in status",
    any("2 cooks" in l for l in out), str(out))

code, out, _ = send((0.01, 4, 100), "americana M x1\nstatus")
chk("nb_cooks=4 → '4 cooks' in status",
    any("4 cooks" in l for l in out), str(out))

section("34. Consecutive semicolons")

code, out, _ = send(FAST, "americana M x1;; americana M x1")
chk(";; between orders → empty part skipped, still 2 READY",
    ready_count(out) == 2, f"got {ready_count(out)}")

code, out, _ = send(FAST, ";;;")
chk("only semicolons → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

section("35. Wrong order token count")

code, out, _ = send(FAST, "americana x1")
chk("missing size token → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "americana")
chk("type only → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

code, out, _ = send(FAST, "americana M")
chk("type + size only → 0 READY", ready_count(out) == 0,
    f"got {ready_count(out)}")

section("36. All sizes for all types")

for t, s in [("regina", "M"), ("margarita", "XXL"),
              ("fantasia", "S"), ("americana", "XL")]:
    code, out, _ = send(FAST, f"{t} {s} x1")
    chk(f"{t} {s} → 1 READY", ready_count(out) == 1,
        f"got {ready_count(out)}: {out}")

section("37. Status ingredient names")

code, out, _ = send((0.01, 2, 100), "americana M x1\nstatus")
for ing in ["Dough", "Tomato", "Gruyere", "Ham", "Mushrooms"]:
    chk(f"status shows {ing}",
        any(ing in l for l in out), str(out))

section("38. Multiplier formats accepted")

for label, val in [("1", 1), ("1.0", 1.0), ("0.5", 0.5), ("0.1", 0.1)]:
    code, _, _ = send((val, 2, 100), "")
    chk(f"multiplier={label} is valid", code == 0, f"got {code}")

section("39. Performance - terminates in time")

for label, args, order in [
    ("1 pizza fast", (0.01, 2, 100), "americana M x1"),
    ("10 pizzas fast", (0.01, 5, 100), "margarita S x10"),
    ("20 pizzas fast", (0.01, 5, 100), "americana M x20"),
    ("mixed 12 pizzas", (0.01, 5, 100),
     "americana M x3; margarita S x3; regina L x3; fantasia XL x3"),
    ("50 pizzas fast", (0.01, 10, 100), "margarita S x50"),
]:
    code, out, _ = send(args, order)
    chk(f"terminates in {TIMEOUT}s: {label}", code != -1, "TIMEOUT")

section("40. Complex scenarios")

code, out, _ = send((0.01, 3, 100),
    "americana M x1; margarita S x2; regina L x1; fantasia XL x1")
chk("5 mixed pizzas → 5 READY", ready_count(out) == 5,
    f"got {ready_count(out)}")
chk("exit 0", code == 0, f"got {code}")

code, out, _ = send((0.01, 2, 100),
    "fantasia M x3\namericana S x2\nmargarita L x1")
chk("3 order lines mixed types → 6 READY", ready_count(out) == 6,
    f"got {ready_count(out)}")

code, out, _ = send((0.01, 3, 50),
    "americana M x1\nstatus\nmargarita S x1")
chk("interleaved status + orders → 2 READY", ready_count(out) >= 2,
    f"got {ready_count(out)}: {out}")

section("41. READY format for each size")

for size in ["S", "M", "L", "XL", "XXL"]:
    code, out, _ = send(FAST, f"americana {size} x1")
    chk(f"[READY] americana {size} exact format",
        any(f"[READY] americana {size}" == l for l in out), str(out))

section("42. nb_cooks edge values")

code, _, _ = send((0.01, 1, 100), "americana M x1")
chk("nb_cooks=1 → exit 0", code == 0, f"got {code}")

code, out, _ = send((0.01, 1, 100), "americana M x1")
chk("nb_cooks=1 → 1 READY", ready_count(out) == 1,
    f"got {ready_count(out)}")

code, _, _ = send((0.01, 10, 100), "americana M x5")
chk("nb_cooks=10 → exit 0", code == 0, f"got {code}")

section("43. replenish_time edge values")

code, _, _ = send((0.01, 2, 1), "americana M x1")
chk("replenish=1ms → exit 0", code == 0, f"got {code}")

code, _, _ = send((0.01, 2, 99999), "americana M x1")
chk("replenish=99999ms → exit 0", code == 0, f"got {code}")

section("44. Status shows busy cook count")

code, out, _ = send((0.01, 2, 100), "americana M x1\nstatus")
chk("status contains 'busy' keyword",
    any("busy" in l for l in out), str(out))

section("45. x1 quantity for each pizza type")

for pizza_type in ["americana", "margarita", "regina", "fantasia"]:
    code, out, _ = send(FAST, f"{pizza_type} S x1")
    chk(f"{pizza_type} S x1 → exactly 1 READY",
        ready_count(out) == 1, f"got {ready_count(out)}: {out}")

section("46. Invalid order doesn't crash")

for label, order in [
    ("all invalid", "badtype BADSIZE xbad"),
    ("extra tokens", "americana M x1 extra"),
    ("only numbers", "1 2 3"),
]:
    code, out, _ = send(FAST, order)
    chk(f"{label} → exit 0 no crash", code == 0, f"got {code}")

section("47. Stress test")

code, out, _ = send((0.01, 10, 50), "margarita S x50")
chk("50 margaritas → 50 READY", ready_count(out) == 50,
    f"got {ready_count(out)}")
chk("50 margaritas → exit 0", code == 0, f"got {code}")

section("48. Multiplier boundary 0.001")

code, out, _ = send((0.001, 2, 100), "americana M x1")
chk("multiplier=0.001 → exit 0", code == 0, f"got {code}")
chk("multiplier=0.001 → 1 READY", ready_count(out) == 1,
    f"got {ready_count(out)}")

section("49. READY type field matches ordered type")

for t in ["americana", "margarita", "regina", "fantasia"]:
    code, out, _ = send(FAST, f"{t} M x1")
    rl = ready_lines(out)
    chk(f"READY for {t} says {t}",
        any(f"[READY] {t}" in l for l in rl),
        str(rl))

section("50. READY size field matches ordered size")

for size in ["S", "M", "L", "XL", "XXL"]:
    code, out, _ = send(FAST, f"americana {size} x1")
    rl = ready_lines(out)
    chk(f"READY for americana {size} says {size}",
        any(f"[READY] americana {size}" in l for l in rl),
        str(rl))

section("51. Multiple kitchens - both complete all pizzas")

code, out, _ = send((0.01, 2, 100), "americana M x10")
chk("10 pizzas with 2 cooks → all 10 READY", ready_count(out) == 10,
    f"got {ready_count(out)}")
chk("exit 0", code == 0, f"got {code}")

code, out, _ = send((0.01, 2, 100), "margarita S x5; americana M x5")
chk("10 pizzas 2 types → 10 READY", ready_count(out) == 10,
    f"got {ready_count(out)}")

section("52. Partial invalid order line - valid part still cooked")

code, out, _ = send(FAST, "americana M x2; NOTAPIZZA S x1; margarita S x1")
chk("2 valid + 1 invalid → 3 READY", ready_count(out) == 3,
    f"got {ready_count(out)}")
chk("exit 0", code == 0, f"got {code}")

section("53. Status kitchen number increments")

code, out, _ = send((0.01, 2, 100), "margarita S x5\nstatus")
chk("5 pizzas with 2 cooks → 2 kitchens, Kitchen #1 present",
    any("Kitchen #1" in l for l in out), str(out))
chk("5 pizzas with 2 cooks → Kitchen #2 present",
    any("Kitchen #2" in l for l in out), str(out))

section("54. Fantasia and regina cook times (relative)")

code, out, _ = send((0.01, 2, 100), "margarita S x4")
chk("4 margaritas → 4 READY", ready_count(out) == 4,
    f"got {ready_count(out)}")

code, out, _ = send((0.01, 2, 100), "fantasia M x4")
chk("4 fantasia → 4 READY", ready_count(out) == 4,
    f"got {ready_count(out)}")

code, out, _ = send((0.01, 2, 100), "regina L x4")
chk("4 regina → 4 READY", ready_count(out) == 4,
    f"got {ready_count(out)}")

section("55. Order with quantity x1 for every type and size combo (sample)")

combos = [
    ("americana", "S"), ("margarita", "M"), ("regina", "XL"),
    ("fantasia", "XXL"), ("americana", "L"),
]
for t, s in combos:
    code, out, _ = send(FAST, f"{t} {s} x1")
    chk(f"{t} {s} → [READY] {t} {s}",
        any(f"[READY] {t} {s}" in l for l in out),
        str(out))

section("56. Quantity x4 for each type")

for t in ["americana", "margarita", "regina", "fantasia"]:
    code, out, _ = send(FAST, f"{t} M x4")
    chk(f"{t} x4 → 4 READY", ready_count(out) == 4,
        f"got {ready_count(out)}")

chk("no READY for empty order",
    ready_count(send(FAST, "")[1]) == 0)

total = _passed + _failed
print(f"\n{BLD}{'=' * 55}{RST}")
print(f"{BLD}Results: {_passed}/{total} passed{RST}")
if _failures:
    print(f"{RED}{_failed} FAILED:{RST}")
    for name, detail in _failures:
        print(f"  - {name}" + (f"  [{detail}]" if detail else ""))
else:
    print(f"{GRN}All tests passed!{RST}")
sys.exit(0 if _failed == 0 else 1)
