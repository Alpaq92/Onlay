#ifndef LAY_INCLUDE_HEADER
#define LAY_INCLUDE_HEADER

#include <stdint.h>

#ifndef LAY_EXPORT
#define LAY_EXPORT extern
#endif

#ifndef LAY_ASSERT
#include <assert.h>
#define LAY_ASSERT assert
#endif

#if defined(__GNUC__) || defined(__clang__)
#define LAY_STATIC_INLINE __attribute__((always_inline)) static inline
#elif defined(_MSC_VER)
#define LAY_STATIC_INLINE __forceinline static
#else
#define LAY_STATIC_INLINE inline static
#endif

typedef uint32_t lay_id;
#if LAY_FLOAT == 1
typedef float lay_scalar;
#else
typedef int16_t lay_scalar;
#endif

#define LAY_INVALID_ID UINT32_MAX

typedef struct lay_vec4 { lay_scalar v[4]; } lay_vec4;
typedef struct lay_vec2 { lay_scalar v[2]; } lay_vec2;

typedef struct lay_item_t {
    uint32_t flags;
    lay_id first_child;
    lay_id next_sibling;
    lay_vec4 margins;
    lay_vec4 paddings;
    lay_vec2 size;
    lay_scalar gap;
    float weight;
} lay_item_t;

typedef struct lay_context {
    lay_item_t *items;
    lay_vec4 *rects;
    lay_id capacity;
    lay_id count;
} lay_context;

typedef enum lay_box_flags {
    LAY_ROW = 0x002,
    LAY_COLUMN = 0x003,

    LAY_LAYOUT = 0x000,
    LAY_FLEX = 0x002,

    LAY_NOWRAP = 0x000,
    LAY_WRAP = 0x004,

    LAY_START = 0x008,
    LAY_MIDDLE = 0x000,
    LAY_END = 0x010,
    LAY_JUSTIFY = 0x018
} lay_box_flags;

typedef enum lay_layout_flags {
    LAY_LEFT = 0x020,
    LAY_TOP = 0x040,
    LAY_RIGHT = 0x080,
    LAY_BOTTOM = 0x100,
    LAY_HFILL = 0x0a0,
    LAY_VFILL = 0x140,
    /* Centered, offset by the left or top margin. */
    LAY_HCENTER = 0x000,
    LAY_VCENTER = 0x000,
    LAY_CENTER = 0x000,
    LAY_FILL = 0x1e0,
    /* A wrapping container also sets this on each item that starts a line. */
    LAY_BREAK = 0x200
} lay_layout_flags;

enum {
    LAY_USERMASK = 0x7fff0000,
    LAY_ANY = 0x7fffffff
};

enum {
    LAY_ITEM_BOX_MODEL_MASK = 0x000007,
    LAY_ITEM_BOX_MASK       = 0x00001F,
    LAY_ITEM_LAYOUT_MASK = 0x0003E0,
    LAY_ITEM_INSERTED   = 0x400,
    LAY_ITEM_HFIXED      = 0x800,
    LAY_ITEM_VFIXED      = 0x1000,
    LAY_ITEM_FIXED_MASK  = LAY_ITEM_HFIXED | LAY_ITEM_VFIXED,

    LAY_ITEM_COMPARE_MASK = LAY_ITEM_BOX_MODEL_MASK
        | (LAY_ITEM_LAYOUT_MASK & ~(uint32_t)LAY_BREAK)
        | LAY_USERMASK
};

LAY_STATIC_INLINE lay_vec4 lay_vec4_xyzw(lay_scalar x, lay_scalar y, lay_scalar z, lay_scalar w)
{
    lay_vec4 result;
    result.v[0] = x;
    result.v[1] = y;
    result.v[2] = z;
    result.v[3] = w;
    return result;
}

LAY_EXPORT void lay_init_context(lay_context *ctx);
LAY_EXPORT void lay_reserve_items_capacity(lay_context *ctx, lay_id count);
LAY_EXPORT void lay_destroy_context(lay_context *ctx);
LAY_EXPORT void lay_reset_context(lay_context *ctx);

/* Adding items after this may reallocate, and every rect read must follow it. */
LAY_EXPORT void lay_run_context(lay_context *ctx);
LAY_EXPORT void lay_run_item(lay_context *ctx, lay_id item);

/* A run that wraps leaves LAY_BREAK on the items it broke at; clear them
   before running a context again at another size. */
LAY_EXPORT void lay_clear_item_break(lay_context *ctx, lay_id item);

LAY_EXPORT lay_id lay_items_count(lay_context *ctx);
LAY_EXPORT lay_id lay_items_capacity(lay_context *ctx);
LAY_EXPORT lay_id lay_item(lay_context *ctx);

/* lay_insert walks the parent's children to find the last; lay_append after
   a known sibling does not. */
LAY_EXPORT void lay_insert(lay_context *ctx, lay_id parent, lay_id child);
LAY_EXPORT void lay_append(lay_context *ctx, lay_id earlier, lay_id later);
LAY_EXPORT void lay_push(lay_context *ctx, lay_id parent, lay_id child);

LAY_EXPORT lay_vec2 lay_get_size(lay_context *ctx, lay_id item);
LAY_EXPORT void lay_get_size_xy(lay_context *ctx, lay_id item, lay_scalar *x, lay_scalar *y);
LAY_EXPORT void lay_set_size(lay_context *ctx, lay_id item, lay_vec2 size);
LAY_EXPORT void lay_set_size_xy(lay_context *ctx, lay_id item, lay_scalar width, lay_scalar height);

LAY_EXPORT void lay_set_contain(lay_context *ctx, lay_id item, uint32_t flags);
LAY_EXPORT void lay_set_behave(lay_context *ctx, lay_id item, uint32_t flags);

/* Between children only, never before the first or after the last. */
LAY_EXPORT void lay_set_gap(lay_context *ctx, lay_id item, lay_scalar gap);
LAY_EXPORT lay_scalar lay_get_gap(lay_context *ctx, lay_id item);

/* A filling item's share of the space left over, against its filling
   siblings'; 0 reads as 1. Its own size is a floor kept before sharing. */
LAY_EXPORT void lay_set_weight(lay_context *ctx, lay_id item, float weight);
LAY_EXPORT float lay_get_weight(lay_context *ctx, lay_id item);

LAY_EXPORT lay_vec4 lay_get_margins(lay_context *ctx, lay_id item);
LAY_EXPORT void lay_get_margins_ltrb(lay_context *ctx, lay_id item, lay_scalar *l, lay_scalar *t, lay_scalar *r, lay_scalar *b);
LAY_EXPORT void lay_set_margins(lay_context *ctx, lay_id item, lay_vec4 ltrb);
LAY_EXPORT void lay_set_margins_ltrb(lay_context *ctx, lay_id item, lay_scalar l, lay_scalar t, lay_scalar r, lay_scalar b);

LAY_EXPORT lay_vec4 lay_get_paddings(lay_context *ctx, lay_id item);
LAY_EXPORT void lay_get_paddings_ltrb(lay_context *ctx, lay_id item, lay_scalar *l, lay_scalar *t, lay_scalar *r, lay_scalar *b);
LAY_EXPORT void lay_set_paddings(lay_context *ctx, lay_id item, lay_vec4 ltrb);
LAY_EXPORT void lay_set_paddings_ltrb(lay_context *ctx, lay_id item, lay_scalar l, lay_scalar t, lay_scalar r, lay_scalar b);

/* Valid until the next reallocation; keep the id, not the pointer. */
LAY_STATIC_INLINE lay_item_t *lay_get_item(const lay_context *ctx, lay_id id)
{
    LAY_ASSERT(id != LAY_INVALID_ID && id < ctx->count);
    return ctx->items + id;
}

LAY_STATIC_INLINE lay_id lay_first_child(const lay_context *ctx, lay_id id)
{
    const lay_item_t *pitem = lay_get_item(ctx, id);
    return pitem->first_child;
}

LAY_STATIC_INLINE lay_id lay_next_sibling(const lay_context *ctx, lay_id id)
{
    const lay_item_t *pitem = lay_get_item(ctx, id);
    return pitem->next_sibling;
}

LAY_STATIC_INLINE lay_vec4 lay_get_rect(const lay_context *ctx, lay_id id)
{
    LAY_ASSERT(id != LAY_INVALID_ID && id < ctx->count);
    return ctx->rects[id];
}

LAY_STATIC_INLINE void lay_get_rect_xywh(
        const lay_context *ctx, lay_id id,
        lay_scalar *x, lay_scalar *y, lay_scalar *width, lay_scalar *height)
{
    LAY_ASSERT(id != LAY_INVALID_ID && id < ctx->count);
    lay_vec4 rect = ctx->rects[id];
    *x = rect.v[0];
    *y = rect.v[1];
    *width = rect.v[2];
    *height = rect.v[3];
}

#undef LAY_EXPORT
#undef LAY_STATIC_INLINE

#endif // LAY_INCLUDE_HEADER

#ifdef LAY_IMPLEMENTATION

#include <stddef.h>
#include <stdbool.h>

#ifndef LAY_REALLOC
#include <stdlib.h>
#define LAY_REALLOC(_block, _size) realloc(_block, _size)
#define LAY_FREE(_block) free(_block)
#endif

#ifndef LAY_MEMSET
#include <string.h>
#define LAY_MEMSET(_dst, _val, _size) memset(_dst, _val, _size)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define LAY_FORCE_INLINE __attribute__((always_inline)) inline
#ifdef __cplusplus
#define LAY_RESTRICT __restrict
#else
#define LAY_RESTRICT restrict
#endif // __cplusplus
#elif defined(_MSC_VER)
#define LAY_FORCE_INLINE __forceinline
#define LAY_RESTRICT __restrict
#else
#define LAY_FORCE_INLINE inline
#ifdef __cplusplus
#define LAY_RESTRICT
#else
#define LAY_RESTRICT restrict
#endif // __cplusplus
#endif

static LAY_FORCE_INLINE lay_scalar lay_scalar_max(lay_scalar a, lay_scalar b)
{ return a > b ? a : b; }
static LAY_FORCE_INLINE lay_scalar lay_scalar_min(lay_scalar a, lay_scalar b)
{ return a < b ? a : b; }
static LAY_FORCE_INLINE float lay_float_max(float a, float b)
{ return a > b ? a : b; }
static LAY_FORCE_INLINE float lay_float_min(float a, float b)
{ return a < b ? a : b; }

void lay_init_context(lay_context *ctx)
{
    ctx->capacity = 0;
    ctx->count = 0;
    ctx->items = NULL;
    ctx->rects = NULL;
}

/* Items and their rects share one block, rects after the last item. */
static void lay_set_capacity(lay_context *ctx, lay_id capacity)
{
    const size_t item_size = sizeof(lay_item_t) + sizeof(lay_vec4);
    ctx->capacity = capacity;
    ctx->items = (lay_item_t*)LAY_REALLOC(ctx->items, ctx->capacity * item_size);
    ctx->rects = (lay_vec4*)(ctx->items + ctx->capacity);
}

void lay_reserve_items_capacity(lay_context *ctx, lay_id count)
{
    if (count >= ctx->capacity)
        lay_set_capacity(ctx, count);
}

void lay_destroy_context(lay_context *ctx)
{
    if (ctx->items != NULL) {
        LAY_FREE(ctx->items);
        ctx->items = NULL;
        ctx->rects = NULL;
    }
}

void lay_reset_context(lay_context *ctx)
{ ctx->count = 0; }

static void lay_calc_size(lay_context *ctx, lay_id item, int dim);
static void lay_arrange(lay_context *ctx, lay_id item, int dim);

void lay_run_context(lay_context *ctx)
{
    LAY_ASSERT(ctx != NULL);

    if (ctx->count > 0) {
        lay_run_item(ctx, 0);
    }
}

void lay_run_item(lay_context *ctx, lay_id item)
{
    LAY_ASSERT(ctx != NULL);

    lay_calc_size(ctx, item, 0);
    lay_arrange(ctx, item, 0);
    lay_calc_size(ctx, item, 1);
    lay_arrange(ctx, item, 1);
}

void lay_clear_item_break(lay_context *ctx, lay_id item)
{
    LAY_ASSERT(ctx != NULL);
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->flags = pitem->flags & ~(uint32_t)LAY_BREAK;
}

lay_id lay_items_count(lay_context *ctx)
{
    LAY_ASSERT(ctx != NULL);
    return ctx->count;
}

lay_id lay_items_capacity(lay_context *ctx)
{
    LAY_ASSERT(ctx != NULL);
    return ctx->capacity;
}

lay_id lay_item(lay_context *ctx)
{
    lay_id idx = ctx->count++;

    if (idx >= ctx->capacity)
        lay_set_capacity(ctx, ctx->capacity < 1 ? 32 : (ctx->capacity * 4));

    lay_item_t *item = lay_get_item(ctx, idx);
    LAY_MEMSET(item, 0, sizeof(lay_item_t));
    item->first_child = LAY_INVALID_ID;
    item->next_sibling = LAY_INVALID_ID;
    LAY_MEMSET(&ctx->rects[idx], 0, sizeof(lay_vec4));
    return idx;
}

static LAY_FORCE_INLINE
void lay_append_by_ptr(
        lay_item_t *LAY_RESTRICT pearlier,
        lay_id later, lay_item_t *LAY_RESTRICT plater)
{
    plater->next_sibling = pearlier->next_sibling;
    plater->flags |= LAY_ITEM_INSERTED;
    pearlier->next_sibling = later;
}

lay_id lay_last_child(const lay_context *ctx, lay_id parent)
{
    lay_item_t *pparent = lay_get_item(ctx, parent);
    lay_id child = pparent->first_child;
    if (child == LAY_INVALID_ID) return LAY_INVALID_ID;
    lay_item_t *pchild = lay_get_item(ctx, child);
    lay_id result = child;
    for (;;) {
        lay_id next = pchild->next_sibling;
        if (next == LAY_INVALID_ID) break;
        result = next;
        pchild = lay_get_item(ctx, next);
    }
    return result;
}

void lay_append(lay_context *ctx, lay_id earlier, lay_id later)
{
    LAY_ASSERT(later != 0);
    LAY_ASSERT(earlier != later);
    lay_item_t *LAY_RESTRICT pearlier = lay_get_item(ctx, earlier);
    lay_item_t *LAY_RESTRICT plater = lay_get_item(ctx, later);
    lay_append_by_ptr(pearlier, later, plater);
}

void lay_insert(lay_context *ctx, lay_id parent, lay_id child)
{
    LAY_ASSERT(child != 0);
    LAY_ASSERT(parent != child);
    lay_item_t *LAY_RESTRICT pparent = lay_get_item(ctx, parent);
    lay_item_t *LAY_RESTRICT pchild = lay_get_item(ctx, child);
    LAY_ASSERT(!(pchild->flags & LAY_ITEM_INSERTED));
    if (pparent->first_child == LAY_INVALID_ID) {
        pparent->first_child = child;
        pchild->flags |= LAY_ITEM_INSERTED;
    } else {
        lay_id next = pparent->first_child;
        lay_item_t *LAY_RESTRICT pnext = lay_get_item(ctx, next);
        for (;;) {
            next = pnext->next_sibling;
            if (next == LAY_INVALID_ID) break;
            pnext = lay_get_item(ctx, next);
        }
        lay_append_by_ptr(pnext, child, pchild);
    }
}

void lay_push(lay_context *ctx, lay_id parent, lay_id new_child)
{
    LAY_ASSERT(new_child != 0);
    LAY_ASSERT(parent != new_child);
    lay_item_t *LAY_RESTRICT pparent = lay_get_item(ctx, parent);
    lay_id old_child = pparent->first_child;
    lay_item_t *LAY_RESTRICT pchild = lay_get_item(ctx, new_child);
    LAY_ASSERT(!(pchild->flags & LAY_ITEM_INSERTED));
    pparent->first_child = new_child;
    pchild->flags |= LAY_ITEM_INSERTED;
    pchild->next_sibling = old_child;
}

lay_vec2 lay_get_size(lay_context *ctx, lay_id item)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    return pitem->size;
}

void lay_get_size_xy(
        lay_context *ctx, lay_id item,
        lay_scalar *x, lay_scalar *y)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    lay_vec2 size = pitem->size;
    *x = size.v[0];
    *y = size.v[1];
}

void lay_set_size(lay_context *ctx, lay_id item, lay_vec2 size)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->size = size;
    uint32_t flags = pitem->flags;
    if (size.v[0] == 0)
        flags &= ~(uint32_t)LAY_ITEM_HFIXED;
    else
        flags |= LAY_ITEM_HFIXED;
    if (size.v[1] == 0)
        flags &= ~(uint32_t)LAY_ITEM_VFIXED;
    else
        flags |= LAY_ITEM_VFIXED;
    pitem->flags = flags;
}

void lay_set_size_xy(
        lay_context *ctx, lay_id item,
        lay_scalar width, lay_scalar height)
{
    lay_vec2 size;
    size.v[0] = width;
    size.v[1] = height;
    lay_set_size(ctx, item, size);
}

void lay_set_gap(lay_context *ctx, lay_id item, lay_scalar gap)
{
    lay_get_item(ctx, item)->gap = gap;
}

lay_scalar lay_get_gap(lay_context *ctx, lay_id item)
{
    return lay_get_item(ctx, item)->gap;
}

void lay_set_weight(lay_context *ctx, lay_id item, float weight)
{
    LAY_ASSERT(weight >= 0.0f);
    lay_get_item(ctx, item)->weight = weight;
}

float lay_get_weight(lay_context *ctx, lay_id item)
{
    return lay_get_item(ctx, item)->weight;
}

void lay_set_behave(lay_context *ctx, lay_id item, uint32_t flags)
{
    LAY_ASSERT((flags & LAY_ITEM_LAYOUT_MASK) == flags);
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->flags = (pitem->flags & ~(uint32_t)LAY_ITEM_LAYOUT_MASK) | flags;
}

void lay_set_contain(lay_context *ctx, lay_id item, uint32_t flags)
{
    LAY_ASSERT((flags & LAY_ITEM_BOX_MASK) == flags);
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->flags = (pitem->flags & ~(uint32_t)LAY_ITEM_BOX_MASK) | flags;
}

void lay_set_margins(lay_context *ctx, lay_id item, lay_vec4 ltrb)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->margins = ltrb;
}

void lay_set_margins_ltrb(
        lay_context *ctx, lay_id item,
        lay_scalar l, lay_scalar t, lay_scalar r, lay_scalar b)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->margins.v[0] = l;
    pitem->margins.v[1] = t;
    pitem->margins.v[2] = r;
    pitem->margins.v[3] = b;
}

void lay_set_paddings(lay_context *ctx, lay_id item, lay_vec4 ltrb)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->paddings = ltrb;
}

void lay_set_paddings_ltrb(
        lay_context *ctx, lay_id item,
        lay_scalar l, lay_scalar t, lay_scalar r, lay_scalar b)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->paddings.v[0] = l;
    pitem->paddings.v[1] = t;
    pitem->paddings.v[2] = r;
    pitem->paddings.v[3] = b;
}

lay_vec4 lay_get_paddings(lay_context *ctx, lay_id item)
{ return lay_get_item(ctx, item)->paddings; }

void lay_get_paddings_ltrb(
        lay_context *ctx, lay_id item,
        lay_scalar *l, lay_scalar *t, lay_scalar *r, lay_scalar *b)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    lay_vec4 paddings = pitem->paddings;
    *l = paddings.v[0];
    *t = paddings.v[1];
    *r = paddings.v[2];
    *b = paddings.v[3];
}

lay_vec4 lay_get_margins(lay_context *ctx, lay_id item)
{ return lay_get_item(ctx, item)->margins; }

void lay_get_margins_ltrb(
        lay_context *ctx, lay_id item,
        lay_scalar *l, lay_scalar *t, lay_scalar *r, lay_scalar *b)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    lay_vec4 margins = pitem->margins;
    *l = margins.v[0];
    *t = margins.v[1];
    *r = margins.v[2];
    *b = margins.v[3];
}

static LAY_FORCE_INLINE
lay_scalar lay_calc_overlayed_size(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *LAY_RESTRICT pitem = lay_get_item(ctx, item);
    lay_scalar need_size = 0;
    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID) {
        lay_item_t *pchild = lay_get_item(ctx, child);
        lay_vec4 rect = ctx->rects[child];
        lay_scalar child_size = rect.v[dim] + rect.v[2 + dim] + pchild->margins.v[wdim];
        need_size = lay_scalar_max(need_size, child_size);
        child = pchild->next_sibling;
    }
    return pitem->paddings.v[dim] + pitem->paddings.v[wdim] + need_size;
}

static LAY_FORCE_INLINE
lay_scalar lay_calc_stacked_size(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *LAY_RESTRICT pitem = lay_get_item(ctx, item);
    lay_scalar need_size = 0;
    lay_id child = pitem->first_child;
    lay_id kids = 0;
    while (child != LAY_INVALID_ID) {
        lay_item_t *pchild = lay_get_item(ctx, child);
        lay_vec4 rect = ctx->rects[child];
        need_size += rect.v[dim] + rect.v[2 + dim] + pchild->margins.v[wdim];
        child = pchild->next_sibling;
        kids++;
    }
    if (kids > 1 && pitem->gap > 0)
        need_size += pitem->gap * (lay_scalar)(kids - 1);
    return pitem->paddings.v[dim] + pitem->paddings.v[wdim] + need_size;
}

static LAY_FORCE_INLINE
lay_scalar lay_calc_wrapped_overlayed_size(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *LAY_RESTRICT pitem = lay_get_item(ctx, item);
    lay_scalar need_size = 0;
    lay_scalar need_size2 = 0;
    lay_scalar breaks = 0;
    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID) {
        lay_item_t *pchild = lay_get_item(ctx, child);
        lay_vec4 rect = ctx->rects[child];
        if (pchild->flags & LAY_BREAK) {
            need_size2 += need_size;
            need_size = 0;
            breaks++;
        }
        lay_scalar child_size = rect.v[dim] + rect.v[2 + dim] + pchild->margins.v[wdim];
        need_size = lay_scalar_max(need_size, child_size);
        child = pchild->next_sibling;
    }
    if (pitem->gap > 0)
        need_size2 += pitem->gap * breaks;
    return pitem->paddings.v[dim] + pitem->paddings.v[wdim] + need_size2 + need_size;
}

static LAY_FORCE_INLINE
lay_scalar lay_calc_wrapped_stacked_size(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *LAY_RESTRICT pitem = lay_get_item(ctx, item);
    lay_scalar need_size = 0;
    lay_scalar need_size2 = 0;
    lay_id kids = 0;
    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID) {
        lay_item_t *pchild = lay_get_item(ctx, child);
        lay_vec4 rect = ctx->rects[child];
        if (pchild->flags & LAY_BREAK) {
            if (kids > 1 && pitem->gap > 0)
                need_size += pitem->gap * (lay_scalar)(kids - 1);
            need_size2 = lay_scalar_max(need_size2, need_size);
            need_size = 0;
            kids = 0;
        }
        need_size += rect.v[dim] + rect.v[2 + dim] + pchild->margins.v[wdim];
        kids++;
        child = pchild->next_sibling;
    }
    if (kids > 1 && pitem->gap > 0)
        need_size += pitem->gap * (lay_scalar)(kids - 1);
    return pitem->paddings.v[dim] + pitem->paddings.v[wdim] + lay_scalar_max(need_size2, need_size);
}

static void lay_calc_size(lay_context *ctx, lay_id item, int dim)
{
    lay_item_t *pitem = lay_get_item(ctx, item);

    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID) {
        lay_calc_size(ctx, child, dim);
        lay_item_t *pchild = lay_get_item(ctx, child);
        child = pchild->next_sibling;
    }

    ctx->rects[item].v[dim] = pitem->margins.v[dim];

    if (pitem->size.v[dim] != 0) {
        ctx->rects[item].v[2 + dim] = pitem->size.v[dim];
        return;
    }

    lay_scalar cal_size;
    switch (pitem->flags & LAY_ITEM_BOX_MODEL_MASK) {
    case LAY_COLUMN|LAY_WRAP:
        if (dim)
            cal_size = lay_calc_stacked_size(ctx, item, 1);
        else
            cal_size = lay_calc_overlayed_size(ctx, item, 0);
        break;
    case LAY_ROW|LAY_WRAP:
        if (!dim)
            cal_size = lay_calc_wrapped_stacked_size(ctx, item, 0);
        else
            cal_size = lay_calc_wrapped_overlayed_size(ctx, item, 1);
        break;
    case LAY_COLUMN:
    case LAY_ROW:
        if ((pitem->flags & 1) == (uint32_t)dim)
            cal_size = lay_calc_stacked_size(ctx, item, dim);
        else
            cal_size = lay_calc_overlayed_size(ctx, item, dim);
        break;
    default:
        cal_size = lay_calc_overlayed_size(ctx, item, dim);
        break;
    }

    ctx->rects[item].v[2 + dim] = cal_size;
}

static LAY_FORCE_INLINE
void lay_arrange_stacked(
            lay_context *ctx, lay_id item, int dim, bool wrap)
{
    const int wdim = dim + 2;
    lay_item_t *pitem = lay_get_item(ctx, item);

    const uint32_t item_flags = pitem->flags;
    const lay_scalar gap = pitem->gap;
    lay_vec4 rect = ctx->rects[item];
    lay_scalar space = rect.v[2 + dim] - pitem->paddings.v[dim]
                                       - pitem->paddings.v[wdim];

    float max_x2 = (float)(rect.v[dim] + space + pitem->paddings.v[dim]);

    lay_id start_child = pitem->first_child;
    while (start_child != LAY_INVALID_ID) {
        lay_scalar used = 0;
        uint32_t count = 0;
        float weight_total = 0.0f;
        uint32_t squeezed_count = 0;
        uint32_t total = 0;
        bool hardbreak = false;
        lay_id child = start_child;
        lay_id end_child = LAY_INVALID_ID;
        while (child != LAY_INVALID_ID) {
            lay_item_t *pchild = lay_get_item(ctx, child);
            const uint32_t child_flags = pchild->flags;
            const uint32_t flags = (child_flags & LAY_ITEM_LAYOUT_MASK) >> dim;
            const uint32_t fflags = (child_flags & LAY_ITEM_FIXED_MASK) >> dim;
            const lay_vec4 child_margins = pchild->margins;
            lay_vec4 child_rect = ctx->rects[child];
            lay_scalar extend = used;
            if (total) extend += gap;
            if ((flags & LAY_HFILL) == LAY_HFILL) {
                ++count;
                weight_total += pchild->weight > 0.0f ? pchild->weight : 1.0f;
                extend += child_rect.v[dim] + pchild->size.v[dim]
                        + child_margins.v[wdim];
            } else {
                if ((fflags & LAY_ITEM_HFIXED) != LAY_ITEM_HFIXED)
                    ++squeezed_count;
                extend += child_rect.v[dim] + child_rect.v[2 + dim] + child_margins.v[wdim];
            }
            if (wrap && (
                    total && ((extend > space) ||
                    (child_flags & LAY_BREAK)))) {
                end_child = child;
                hardbreak = (child_flags & LAY_BREAK) == LAY_BREAK;
                pchild->flags = child_flags | LAY_BREAK;
                break;
            } else {
                used = extend;
                child = pchild->next_sibling;
            }
            ++total;
        }

        lay_scalar extra_space = space - used;
        float filler = 0.0f;
        float spacer = 0.0f;
        float extra_margin = 0.0f;
        float eater = 0.0f;

        if (extra_space > 0) {
            if (count > 0)
                filler = (float)extra_space / weight_total;
            else if (total > 0) {
                switch (item_flags & LAY_JUSTIFY) {
                case LAY_JUSTIFY:
                    /* Not on a wrapped row's last line, nor before a manual break. */
                    if (!wrap || ((end_child != LAY_INVALID_ID) && !hardbreak))
                        spacer = (float)extra_space / (float)(total - 1);
                    break;
                case LAY_START:
                    break;
                case LAY_END:
                    extra_margin = extra_space;
                    break;
                default:
                    extra_margin = extra_space / 2.0f;
                    break;
                }
            }
        }
#ifdef LAY_FLOAT
        /* In floats extra_space can be a hair below zero with nothing to
           squeeze, which would divide by zero. */
        else if (!wrap && (squeezed_count > 0))
#else
        else if (!wrap && (extra_space < 0))
#endif
            eater = (float)extra_space / (float)squeezed_count;

        float x = (float)rect.v[dim] + pitem->paddings.v[dim];
        float x1;
        child = start_child;
        while (child != end_child) {
            lay_scalar ix0, ix1;
            lay_item_t *pchild = lay_get_item(ctx, child);
            const uint32_t child_flags = pchild->flags;
            const uint32_t flags = (child_flags & LAY_ITEM_LAYOUT_MASK) >> dim;
            const uint32_t fflags = (child_flags & LAY_ITEM_FIXED_MASK) >> dim;
            const lay_vec4 child_margins = pchild->margins;
            lay_vec4 child_rect = ctx->rects[child];

            x += (float)child_rect.v[dim] + extra_margin;
            if ((flags & LAY_HFILL) == LAY_HFILL)
                x1 = x + (float)pchild->size.v[dim]
                       + filler * (pchild->weight > 0.0f ? pchild->weight : 1.0f);
            else if ((fflags & LAY_ITEM_HFIXED) == LAY_ITEM_HFIXED)
                x1 = x + (float)child_rect.v[2 + dim];
            else
                x1 = x + lay_float_max(0.0f, (float)child_rect.v[2 + dim] + eater);

            ix0 = (lay_scalar)x;
            if (wrap)
                ix1 = (lay_scalar)lay_float_min(max_x2 - (float)child_margins.v[wdim], x1);
            else
                ix1 = (lay_scalar)x1;
            child_rect.v[dim] = ix0;
            child_rect.v[dim + 2] = ix1 - ix0;
            ctx->rects[child] = child_rect;
            x = x1 + (float)child_margins.v[wdim];
            child = pchild->next_sibling;
            extra_margin = spacer + (float)gap;
        }

        start_child = end_child;
    }
}

static LAY_FORCE_INLINE
void lay_arrange_overlay(lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *pitem = lay_get_item(ctx, item);
    const lay_vec4 rect = ctx->rects[item];
    const lay_scalar offset = rect.v[dim] + pitem->paddings.v[dim];
    const lay_scalar space = rect.v[2 + dim] - pitem->paddings.v[dim]
                                             - pitem->paddings.v[wdim];

    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID) {
        lay_item_t *pchild = lay_get_item(ctx, child);
        const uint32_t b_flags = (pchild->flags & LAY_ITEM_LAYOUT_MASK) >> dim;
        const lay_vec4 child_margins = pchild->margins;
        lay_vec4 child_rect = ctx->rects[child];

        switch (b_flags & LAY_HFILL) {
        case LAY_HCENTER:
            child_rect.v[dim] += (space - child_rect.v[2 + dim]) / 2 - child_margins.v[wdim];
            break;
        case LAY_RIGHT:
            child_rect.v[dim] += space - child_rect.v[2 + dim] - child_margins.v[dim] - child_margins.v[wdim];
            break;
        case LAY_HFILL:
            child_rect.v[2 + dim] = lay_scalar_max(0, space - child_rect.v[dim] - child_margins.v[wdim]);
            break;
        default:
            break;
        }

        child_rect.v[dim] += offset;
        ctx->rects[child] = child_rect;
        child = pchild->next_sibling;
    }
}

static LAY_FORCE_INLINE
void lay_arrange_overlay_squeezed_range(
        lay_context *ctx, int dim,
        lay_id start_item, lay_id end_item,
        lay_scalar offset, lay_scalar space)
{
    int wdim = dim + 2;
    lay_id item = start_item;
    while (item != end_item) {
        lay_item_t *pitem = lay_get_item(ctx, item);
        const uint32_t b_flags = (pitem->flags & LAY_ITEM_LAYOUT_MASK) >> dim;
        const lay_vec4 margins = pitem->margins;
        lay_vec4 rect = ctx->rects[item];
        lay_scalar min_size = lay_scalar_max(0, space - rect.v[dim] - margins.v[wdim]);
        switch (b_flags & LAY_HFILL) {
            case LAY_HCENTER:
                rect.v[2 + dim] = lay_scalar_min(rect.v[2 + dim], min_size);
                rect.v[dim] += (space - rect.v[2 + dim]) / 2 - margins.v[wdim];
                break;
            case LAY_RIGHT:
                rect.v[2 + dim] = lay_scalar_min(rect.v[2 + dim], min_size);
                rect.v[dim] = space - rect.v[2 + dim] - margins.v[wdim];
                break;
            case LAY_HFILL:
                rect.v[2 + dim] = min_size;
                break;
            default:
                rect.v[2 + dim] = lay_scalar_min(rect.v[2 + dim], min_size);
                break;
        }
        rect.v[dim] += offset;
        ctx->rects[item] = rect;
        item = pitem->next_sibling;
    }
}

static LAY_FORCE_INLINE
lay_scalar lay_arrange_wrapped_overlay_squeezed(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *pitem = lay_get_item(ctx, item);
    lay_scalar offset = ctx->rects[item].v[dim] + pitem->paddings.v[dim];
    lay_scalar need_size = 0;
    lay_id child = pitem->first_child;
    lay_id start_child = child;
    while (child != LAY_INVALID_ID) {
        lay_item_t *pchild = lay_get_item(ctx, child);
        if (pchild->flags & LAY_BREAK) {
            lay_arrange_overlay_squeezed_range(ctx, dim, start_child, child, offset, need_size);
            offset += need_size + pitem->gap;
            start_child = child;
            need_size = 0;
        }
        const lay_vec4 rect = ctx->rects[child];
        lay_scalar child_size = rect.v[dim] + rect.v[2 + dim] + pchild->margins.v[wdim];
        need_size = lay_scalar_max(need_size, child_size);
        child = pchild->next_sibling;
    }
    lay_arrange_overlay_squeezed_range(ctx, dim, start_child, LAY_INVALID_ID, offset, need_size);
    offset += need_size;
    return offset;
}

static void lay_arrange(lay_context *ctx, lay_id item, int dim)
{
    lay_item_t *pitem = lay_get_item(ctx, item);

    const uint32_t flags = pitem->flags;
    switch (flags & LAY_ITEM_BOX_MODEL_MASK) {
    case LAY_COLUMN | LAY_WRAP:
        if (dim != 0) {
            lay_arrange_stacked(ctx, item, 1, true);
            lay_scalar offset = lay_arrange_wrapped_overlay_squeezed(ctx, item, 0);
            ctx->rects[item].v[2 + 0] = offset - ctx->rects[item].v[0];
        }
        break;
    case LAY_ROW | LAY_WRAP:
        if (dim == 0)
            lay_arrange_stacked(ctx, item, 0, true);
        else
            lay_arrange_wrapped_overlay_squeezed(ctx, item, 1);
        break;
    case LAY_COLUMN:
    case LAY_ROW:
        if ((flags & 1) == (uint32_t)dim) {
            lay_arrange_stacked(ctx, item, dim, false);
        } else {
            const lay_vec4 rect = ctx->rects[item];
            lay_arrange_overlay_squeezed_range(
                ctx, dim, pitem->first_child, LAY_INVALID_ID,
                rect.v[dim] + pitem->paddings.v[dim],
                rect.v[2 + dim] - pitem->paddings.v[dim]
                               - pitem->paddings.v[2 + dim]);
        }
        break;
    default:
        lay_arrange_overlay(ctx, item, dim);
        break;
    }
    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID) {
        lay_arrange(ctx, child, dim);
        lay_item_t *pchild = lay_get_item(ctx, child);
        child = pchild->next_sibling;
    }
}

#endif // LAY_IMPLEMENTATION
