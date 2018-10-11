/*
 * Public interface of libcellwriter
 *
 */

#ifndef INK_LIBRARY_H
#define INK_LIBRARY_H 1

#ifdef __cplusplus
extern "C" {
#endif

typedef void *InkEngine;

extern InkEngine createInkEngine();
extern int setInkEngineProfile(InkEngine engine, char *profile, int size);
extern char *getInkEngineProfile(InkEngine engine, int *size);
extern void deleteInkEngine(InkEngine engine);

typedef void *InkGlyph;

extern InkGlyph createInkGlyph(InkEngine engine);
extern int addInkGlyphPoint(InkGlyph glyph, float x, float y, float pressure);
extern int recognizeInkGlyph(InkGlyph glyph);
extern void trainInkGlyph(InkGlyph glyph, int unicode);
extern void deleteInkGlyph(InkGlyph glyph);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace Ink
{

class Engine;

class Glyph
{
public:
    explicit Glyph(InkEngine engine) { glyph = createInkGlyph(engine); }
    ~Glyph() { deleteInkGlyph(glyph); }

public:
    bool isValid() const { return glyph != nullptr; }
    bool addPoint(float x, float y, float pressure = 1.0)
            { return addInkGlyphPoint(glyph, x, y, pressure) != 0; }
    bool endStroke()
            { return addInkGlyphPoint(glyph, 0.0f, 0.0f, 0.0f) != 0; }
    int recognize() const { return recognizeInkGlyph(glyph); }
    void train(int unicode) const { trainInkGlyph(glyph, unicode); }

private:
    InkGlyph glyph;
};

class Engine
{
public:
    Engine() { engine = createInkEngine(); }
    ~Engine() { deleteInkEngine (engine); }

public:
    bool isValid() const { return engine != nullptr; }
    bool setProfile(char *profile, int size)
            { return setInkEngineProfile(engine, profile, size) != 0; }
    char *profile(int &size) const
            { return getInkEngineProfile(engine, &size); }

    Glyph createGlyph() { return Glyph(engine); }

private:
    InkEngine engine;
};

} // namespace Ink
#endif

#endif
