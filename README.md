# Onlay

A 2D layout engine in C99. It computes rectangles and does nothing else — it
draws nothing, owns no widgets, handles no input, and knows nothing about the
program using it.

A fork of [randrew/layout](https://github.com/randrew/layout), which is itself
a rewrite of the layout engine from
[oui](https://bitbucket.org/duangle/oui-blendish). The original is excellent
and unmaintained; this fork exists to build as C99 on every compiler and to
add the three things a declarative UI needed.

```c
lay_context ctx;
lay_init_context(&ctx);
lay_reserve_items_capacity(&ctx, 256);

lay_id card = lay_item(&ctx);
lay_set_size_xy(&ctx, card, 420, 354);
lay_set_contain(&ctx, card, LAY_COLUMN | LAY_START);
lay_set_paddings_ltrb(&ctx, card, 24, 24, 24, 24);
lay_set_gap(&ctx, card, 10);

lay_id row = lay_item(&ctx);
lay_set_behave(&ctx, row, LAY_HFILL);
lay_set_weight(&ctx, row, 2.0f);      /* twice a sibling's share */
lay_insert(&ctx, card, row);

lay_run_context(&ctx);
lay_vec4 r = lay_get_rect(&ctx, row);   /* r.v[0..3] is x, y, w, h */
lay_destroy_context(&ctx);
```

Single header. `#define LAY_IMPLEMENTATION` in exactly one translation unit.
`#define LAY_FLOAT 1` for float coordinates instead of `int16_t`.

## What is different from randrew/layout

**It is C99 everywhere.** Upstream stores rects in a GCC `vector_size` type and
falls back to a C++ class with `operator[]` on MSVC — which forces the whole
library to be compiled as C++ on that toolchain. Both are replaced by a struct
holding an array. The algorithm still indexes by dimension, and the only change
to the API is that a rect is read as `r.v[0]` rather than `r[0]`.

**Weighted tracks.** `lay_set_weight(ctx, item, w)` gives a filling item its
share of the leftover space relative to its siblings, where upstream splits it
equally. Weight 0 reads as 1, so an item that says nothing behaves as before. A
size set on a weighted item is a floor rather than a size: "at least 80 wide,
then grow" is `lay_set_size_xy(..., 80, h)` with `LAY_HFILL`. That is one rule
covering what CSS spells as three — a fixed track, a minimum that grows, and a
fraction.

**Gaps.** `lay_set_gap(ctx, item, n)` puts space between an item's children
without putting it outside them, so a row of three has two gaps and no leading
or trailing one. It applies down a wrapped row's lines as well as along them,
and it is counted when a container is sized to its contents — otherwise a
container came out exactly one gap per child too short.

**Padding**, from [randrew/layout#23](https://github.com/randrew/layout/pull/23)
by [codecat](https://github.com/codecat), ported onto the struct rect. A margin
is space outside an item and a padding is space inside it: a padding insets the
rect an item lays its children into, and is added when that item is sized to
its contents.

Every change is marked `// ONLAY:` in the source with the reason next to it.

## Tests

`test_onlay.c` builds standalone and checks the arithmetic against numbers
taken from a running application rather than from this library — a login card,
weighted columns, floors that grow, template rows, and padding against margin.
Upstream's own `test_layout.c` is not carried: it assumes the vector rect.

```
cc -DLAY_FLOAT=1 -o onlaytest test_onlay.c && ./onlaytest
```

## Licence

MIT, as upstream. See `LICENSE` — the original copyright is Andrew Richards's
and is unchanged.
