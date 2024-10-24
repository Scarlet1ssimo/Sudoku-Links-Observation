# An Observation Helper for Sudoku

## Highlights

| Demo 00 | Demo 21 |
| --- | --- |
| ![Demo/00.png](/tmp/00.png) | ![Demo/21.png](/tmp/21.png) |

## Usage

First, run `solver.cpp` over a state of a sudoku puzzle.

Create a `data.in` appending the output of the solver after the same sudoku state.

Run `visualizer.py` to see all strong-weak links in `/tmp`.

## Annotations

- Red links for strong links
- Green links for weak links
- Blue highlighted candidates are numbers involved in observation
- Red candidate is the one to eliminate

## License

MIT License