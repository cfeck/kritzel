#include "libcellwriter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "common.h"
#include "recognize.h"

/**
 * Engine
 *
 */
struct Engine
{
    char *static_profile;
    int static_size;
    int glyphs;
};

InkEngine createInkEngine()
{
    struct Engine *e = (struct Engine *) malloc(sizeof(struct Engine));
    if (!e) {
        return 0;
    }

    recognize_init();
    e->glyphs = 0;

    return (InkEngine) e;
}

int setInkEngineProfile(InkEngine engine, char *profile, int size)
{
    /* TODO read profile
    sample_read();
     */
    return 0;
}

char *getInkEngineProfile(InkEngine engine, int *size)
{
    /* TODO write profile
    sample_write();
     */
    return 0;
}

void deleteInkEngine(InkEngine engine)
{
    struct Engine *e = (struct Engine *) engine;
    if (!e) {
        return;
    }

    if (e->glyphs) {
        fprintf(stderr, "WARNING: ink engine leaked %d glyphs\n", e->glyphs);
    }

    free(e);
}

/**
 * Glyph
 *
 */

struct GlyphPoint
{
    float x, y;
    float pressure;
};

struct Glyph
{
    struct Engine *engine;
    Sample sample;
    int allocated;
    int used;
    struct GlyphPoint *points;
};

InkGlyph createInkGlyph(InkEngine engine)
{
    struct Engine *e = (struct Engine *) engine;
    if (!e) {
        return 0;
    }
    struct Glyph *g = (struct Glyph *) malloc(sizeof(struct Glyph));
    if (!g) {
        return 0;
    }
    g->engine = e;
    memset(&g->sample, 0, sizeof(Sample));
    g->allocated = 32;
    g->used = 0;
    g->points = (struct GlyphPoint *) malloc(g->allocated * sizeof(struct GlyphPoint));
    if (!g->points) {
        free(g);
        return 0;
    }
    ++e->glyphs;

    return (InkGlyph) g;
}

int addInkGlyphPoint(InkGlyph glyph, float x, float y, float pressure)
{
    struct Glyph *g = (struct Glyph *) glyph;
    if (!g) {
        return FALSE;
    }
    if (g->used == g->allocated) {
        /* increase storage by 50% each time a reallocation is required */
        int new_allocated = g->allocated + (g->allocated >> 1);
        /* prevent overflow */
        if (new_allocated > (1 << 14)) {
            return FALSE;
        }
        struct GlyphPoint *new_points = (struct GlyphPoint *) realloc(g->points,
                new_allocated * sizeof(struct GlyphPoint));
        if (!new_points) {
            return FALSE;
        }
        g->allocated = new_allocated;
        g->points = new_points;
    }
    struct GlyphPoint *point = &g->points[g->used];
    point->x = x;
    point->y = y;
    point->pressure = pressure;
    ++g->used;
    return TRUE;
}

static void prepareSample(struct Glyph *g)
{
    struct GlyphPoint *points = g->points;
    int n = g->used;
    int drawing = FALSE;
    while (n > 0) {
        if (points->pressure > 0.0) {
            if (!input || !input->len) {
                /* new sample */
                input = &g->sample;
                clear_sample(input);
            }
            if (!drawing) {
                /* new stroke */
                if (input->len >= STROKES_MAX) {
                    break;
                }
                input->strokes[input->len] = stroke_new(0);
                ++input->len;
                drawing = TRUE;
            }
            draw_stroke(&input->strokes[input->len - 1], (int) points->x, (int) points->y);
        } else {
            if (drawing) {
                /* end current stroke */
                Stroke *stroke = input->strokes[input->len - 1];
                smooth_stroke(stroke);
                simplify_stroke(stroke);
                process_stroke(stroke);
            }
            drawing = FALSE;
        }
        --n;
        ++points;
    }
}

#define ALTERNATES 5

int recognizeInkGlyph(InkGlyph glyph)
{
    struct Glyph *g = (struct Glyph *) glyph;
    if (!g) {
        return 0;
    }

    prepareSample(g);
    if (!input) {
        return 0;
    }
    if (input->len == 0 || input->len >= STROKES_MAX) {
        clear_sample(input);
        input = NULL;
        return 0;
    }

    int result = 0;
    if (input->strokes[0] && input->strokes[0]->len) {
        Sample *alts[ALTERNATES];
        int num_alts = ALTERNATES;
        recognize_sample(input, &alts[0], num_alts);
        result = input->ch;
#if 1
        fprintf(stderr, "recognize result: %d ('%c')\n", result, result);
        int i, j;
        if (result != 0) {
            for (i = 0; i < ALTERNATES; ++i) {
                if (!alts[i]) {
                    break;
                }
                fprintf(stderr, "alternative %d: %d ('%c') rating %d, ratings: ", i + 1, alts[i]->ch, alts[i]->ch, alts[i]->rating);
                for (j = 0; j < ENGINES; ++j) {
                    fprintf(stderr, "%d ", alts[i]->ratings[j]);
                }
                fprintf(stderr, "\n");
            }
        }
#endif
    }
    clear_sample(input);
    input = NULL;
    return result;
}

void trainInkGlyph(InkGlyph glyph, int unicode)
{
    struct Glyph *g = (struct Glyph *) glyph;
    if (!g) {
        return;
    }

    prepareSample(g);
    if (!input) {
        return;
    }
    if (input->len == 0 || input->len >= STROKES_MAX) {
        clear_sample(input);
        input = NULL;
        return;
    }

    if (input->strokes[0] && input->strokes[0]->len) {
        input->ch = unicode;
        train_sample(input, TRUE);
    }
    clear_sample(input);
    input = NULL;
}

void deleteInkGlyph(InkGlyph glyph)
{
    struct Glyph *g = (struct Glyph *) glyph;
    if (!g) {
        return;
    }
    --g->engine->glyphs;
    free(g->points);
    free(g);
}

