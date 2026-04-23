# MSCS-632 Assignment 8 — Family Tree Program in Prolog

**Course:** MSCS-632 Advanced Programming Languages  
**Author:** Parthasarathi Ponnapalli  
**University:** University of the Cumberlands

---

## Overview

A three-generation South Indian family tree — 20 people across  
two lineages connected by 26 parent facts. All relationships are  
derived by logical inference; no relationship data is stored directly.

```
Generation 1:
  Venkataraman & Kamakshi  → Anand, Selvam
  Subramaniam  & Meenakshi → Mythili, Vimala

Generation 2:
  Anand    & Kavitha  → Karthik, Divya
  Selvam   & Geetha   → Surya, Nithya
  Rajan    & Mythili  → Arun, Preethi
  Prasanna & Vimala   → Deepak, Saranya
```

---

## Files

```
Assignment_8/
├── README.md
├── family_tree.pl      ← Prolog facts + rules
└── run_prolog.bat      ← Double-click to run on Windows
```

---

## Rules Implemented

| Rule | Description |
|------|-------------|
| `father/2`, `mother/2` | Direct parent by gender |
| `child/2`, `son/2`, `daughter/2` | Inverse of parent |
| `grandparent/2` | Two-step parent chain |
| `sibling/2` | Shared parent, different person |
| `cousin/2` | Parents are siblings |
| `uncle/2`, `aunt/2` | Sibling of a parent |
| `ancestor/2` | **Recursive** — climbs to any depth |
| `descendant/2` | Reverses ancestor |

---

## How to Run

### Prerequisites

Download **SWI-Prolog** (Windows installer):  
→ https://www.swi-prolog.org/download/stable

### Windows — double-click

```
run_prolog.bat
```

Runs all 15 queries and saves output to `output_prolog.txt`.

### Manual — SWI-Prolog REPL

```prolog
?- [family_tree].
?- child(X, anand).
?- grandparent(X, karthik).
?- cousin(karthik, surya).
?- setof(X, ancestor(X, karthik), L).
?- setof(X, descendant(X, venkataraman), L).
```

---

## Sample Output (key queries)

```
Q1  : Children of Anand             → divya, karthik
Q3  : Grandparents of Karthik       → kamakshi, venkataraman
Q6  : cousin(karthik, surya)?       → true
Q7  : All cousins of Karthik        → nithya, surya
Q9  : Aunt of Arun                  → vimala
Q11 : Ancestors of Karthik          → anand, kamakshi, kavitha, venkataraman
Q12 : Descendants of Venkataraman   → anand, divya, karthik, nithya, selvam, surya
```

---

## Note on Duplicate Results

The `sibling`, `cousin`, and `uncle/aunt` rules can return duplicate  
bindings because each child has two parents in the fact base.  
Use `setof/3` at query time to get clean deduplicated results:

```prolog
?- setof(X, sibling(X, anand), L).
L = [selvam].
```

---

## References

- Clocksin, W. F., & Mellish, C. S. (2003). *Programming in Prolog* (5th ed.). Springer.  
- Sterling, L., & Shapiro, E. (1994). *The art of Prolog* (2nd ed.). MIT Press.
