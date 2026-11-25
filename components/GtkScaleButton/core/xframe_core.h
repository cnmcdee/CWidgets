#pragma once

//
// xframe_core.h - shared core for the XFrame widget library.
//
// Everything in here follows the format of the "Writing Your Own
// X-11 Framework Basics" articles (hotconfig.com):
//
//   Part 1 - Display / Screen / Window boilerplate, the GC as a
//            "pen", the drawing primitives, and the Canvas class.
//   Part 2 - widgets are standalone classes that record display_,
//            screen_, window_ and gc_ at construction time.
//   Part 3 - SStyle_Manager tracks named styles in a vector.
//   Part 4 - a polished widget is built from primitives: two fill
//            rectangles + four fill arcs make a rounded body, four
//            lines + four draw arcs make its outline, and text is
//            centred with XTextWidth + font ascent / descent.
//
// Two small completions versus the articles, needed by Part 4's own
// draw code: SStyle gains lineColor_ and text_color_, and the style
// manager gains the delete_style() function Part 3 left as a reader
// challenge, plus bounds-safe getters.
//

#include <X11/Xlib.h>

#include <X11/Xutil.h>

#include <X11/keysym.h>

#include <X11/fonts/font.h>

#include <algorithm>

#include <cstdint>

#include <cstdlib>

#include <cstring>

#include <functional>

#include <iostream>

#include <string>

#include <vector>

//region SStyle

struct Srgb

{

    int r;

    int g;

    int b;

};

struct SStyle

{

    unsigned long fillColor_ = 0;

    unsigned long outlineColor_ = 0;

    unsigned long lineColor_ = 0;

    unsigned long text_color_ = 0;

    unsigned long radius_ = 0;

    unsigned long lineWidth_ = 1;

    XFontStruct *font_ = nullptr;

    std::string style_name_;

};

//endregion

//region SStyle_Manager

class SStyle_Manager

{

private:

    std::vector<SStyle> styles_;

    Display *display_;

    Window window_;

    int screen_;

    GC gc_;

    int active_style = 0; // just set the default to first passed.

public:

    SStyle_Manager(Display *display, Window window, int screen, GC gc)

    {

        display_ = display;

        window_ = window;

        screen_ = screen;

        gc_ = gc;

    }

    // NOTE (test agent, Nov 26 2026): do NOT XFreeFont here. Every demo
    // main calls XCloseDisplay() before this destructor runs (stack
    // unwinding after main returns), so XFreeFont would operate on a
    // closed Display and corrupt the heap (SIGSEGV/SIGABRT inside
    // XFreeFont on every clean exit). The fonts are process-lifetime
    // resources; the X server reclaims them when the client disconnects.
    // Use delete_style() to free a single style while the display is
    // still open.
    ~SStyle_Manager()

    {

        styles_.clear();

    }

    int add_style(std::string styleName, Srgb fillColor, Srgb outlineColor, int radius, int linewidth, std::string fontName)

    {

        return add_style_ex(styleName, fillColor, outlineColor, outlineColor, Srgb{0, 0, 0}, radius, linewidth, fontName);

    }

    int add_style_ex(std::string styleName, Srgb fillColor, Srgb outlineColor, Srgb lineColor, Srgb textColor, int radius, int linewidth, std::string fontName)

    {

        SStyle nstyle;

        nstyle.style_name_ = std::move(styleName);

        nstyle.fillColor_ = get_color_Srgb(fillColor);

        nstyle.outlineColor_ = get_color_Srgb(outlineColor);

        nstyle.lineColor_ = get_color_Srgb(lineColor);

        nstyle.text_color_ = get_color_Srgb(textColor);

        nstyle.radius_ = radius;

        nstyle.lineWidth_ = linewidth;

        // Define fallback fonts to try if the requested font fails

        const char* fallbackFonts[] = { "fixed", "helvetica" };

        size_t numFallbacks = sizeof(fallbackFonts) / sizeof(*fallbackFonts);

        nstyle.font_ = nullptr;

        // Attempt to load the primary font (if provided and not empty)

        if (!fontName.empty()) {

            nstyle.font_ = XLoadQueryFont(display_, fontName.c_str());

        }

        // If the primary failed, iterate through fallback fonts until one succeeds or all are exhausted

        if (!nstyle.font_) {

            for (size_t i = 0; i < numFallbacks && !nstyle.font_; ++i) {

                nstyle.font_ = XLoadQueryFont(display_, fallbackFonts[i]);

            }

        }

        // Final validation: if no font loaded, report which fonts were attempted and exit

        if (!nstyle.font_)

        {

            printf("Failed to load any font (tried: ");

            if (!fontName.empty()) printf("%s", fontName.c_str());

            for (size_t i = 0; i < numFallbacks; ++i) {

                if (!fontName.empty() || i > 0) printf(", ");

                printf("%s", fallbackFonts[i]);

            }

            printf(")\n");

            return -1;

        }

        styles_.push_back(nstyle);

        return 0;

    }

    // The delete function Part 3 left as a challenge: remove a style
    // by name, freeing its font, and keep active_style in range.

    int delete_style(std::string styleName)

    {

        for (size_t i = 0; i < styles_.size(); ++i)

        {

            if (styles_[i].style_name_ == styleName)

            {

                if (styles_[i].font_)

                {

                    XFreeFont(display_, styles_[i].font_);

                }

                styles_.erase(styles_.begin() + static_cast<long>(i));

                if (active_style >= (int)styles_.size())

                {

                    active_style = styles_.empty() ? 0 : (int)styles_.size() - 1;

                }

                return 0;

            }

        }

        return -1; // Not found

    }

    int style_count()

    {

        return (int)styles_.size();

    }

    unsigned long get_color_Srgb(Srgb color)

    {

        XColor xcolor;

        xcolor.red = color.r << 8;

        xcolor.green = color.g << 8;

        xcolor.blue = color.b << 8;

        if (XAllocColor(display_, DefaultColormap(display_, screen_), &xcolor))

        {

            return xcolor.pixel;

        } else {

            printf("Failed to allocate color! RGB(%d,%d,%d)\n", color.r, color.g, color.b);

            return BlackPixel(display_, screen_);  // Fallback

        }

    }

    unsigned long get_color_rgb(unsigned char r, unsigned char g, unsigned char b)

    {

        XColor xcolor;

        xcolor.red = r << 8;

        xcolor.green = g << 8;

        xcolor.blue = b << 8;

        if (XAllocColor(display_, DefaultColormap(display_, screen_), &xcolor))

        {

            return xcolor.pixel;

        } else {

            printf("Failed to allocate color! RGB(%d,%d,%d)\n", r, g, b);

            return BlackPixel(display_, screen_);  // Fallback

        }

    }

    int set_active_style_byName(std::string styleName)

    {

        auto it = std::find_if(styles_.begin(), styles_.end(),

            [&](const SStyle& s) { return s.style_name_ == styleName; });

        if (it != styles_.end())

        {

            active_style = static_cast<int>(std::distance(styles_.begin(), it));

            return active_style;

        }

        return -1; // Not found

    }

    SStyle get_active_style_byName(std::string styleName)

    {

        auto it = std::find_if(styles_.begin(), styles_.end(),

            [&](const SStyle& s) { return s.style_name_ == styleName; });

        if (it != styles_.end())

        {

            return *it;

        }

        return get_active_style();

    }

    SStyle get_active_style()

    {

        if (styles_.empty())

        {

            return SStyle{};

        }

        if (active_style < 0 || active_style >= (int)styles_.size())

        {

            active_style = 0;

        }

        return styles_[active_style];

    }

    SStyle get_index_style(int index)

    {

        if (index >= 0 && index < (int)styles_.size())

        {

            return styles_[index];

        }

        return get_active_style();

    }

    // Resolve the style a widget should draw with: its own named
    // style when set, otherwise the manager's active style.

    SStyle resolve(const std::string& styleName)

    {

        if (!styleName.empty())

        {

            return get_active_style_byName(styleName);

        }

        return get_active_style();

    }

};

//endregion

//region Drawing helpers (Part 4 construction, shared by all widgets)

namespace xf

{

    inline void set_line_width(Display* display, GC gc, int lineWidth)

    {

        unsigned long valuemask = GCLineWidth;

        XGCValues gc_values;

        gc_values.line_width = lineWidth;

        XChangeGC(display, gc, valuemask, &gc_values);

    }

    // The Part 4 rounded body: Rectangle A + Rectangle B + 4 fill arcs.

    inline void fill_rounded_rect(Display* display, Window window, GC gc, const SStyle& style, int x1, int y1, int x2, int y2)

    {

        int r = (int)style.radius_;

        int w = x2 - x1;

        int h = y2 - y1;

        if (r <= 0 || w <= 2 * r || h <= 2 * r)

        {

            XFillRectangle(display, window, gc, x1, y1, w, h);

            return;

        }

        XSetForeground(display, gc, style.fillColor_);

        //Rectangle A

        XFillRectangle(display, window, gc, x1 + r, y1, w - 2 * r, h);

        //Rectangle B

        XFillRectangle(display, window, gc, x1, y1 + r, w, h - 2 * r);

        //region Arc C Top Left

        XFillArc(display, window, gc, x1, y1, 2 * r, 2 * r, 90 * 64, 90 * 64);

        //region Arc D Top Right

        XFillArc(display, window, gc, x2 - 2 * r, y1, 2 * r, 2 * r, 0, 90 * 64);

        //region Arc E Bottom Right

        XFillArc(display, window, gc, x2 - 2 * r, y2 - 2 * r, 2 * r, 2 * r, 270 * 64, 90 * 64);

        //region Arc F Bottom Left

        XFillArc(display, window, gc, x1, y2 - 2 * r, 2 * r, 2 * r, 180 * 64, 90 * 64);

        //endregion

    }

    // The Part 4 outline: 4 line bars + 4 draw arcs, in lineColor_.

    inline void draw_rounded_outline(Display* display, Window window, GC gc, const SStyle& style, int x1, int y1, int x2, int y2)

    {

        int r = (int)style.radius_;

        XSetForeground(display, gc, style.lineColor_);

        set_line_width(display, gc, (int)style.lineWidth_);

        if (r <= 0)

        {

            XDrawRectangle(display, window, gc, x1, y1, x2 - x1, y2 - y1);

            return;

        }

        // 4 Line Bars

        XDrawLine(display, window, gc, x1 + r, y1, x2 - r, y1);

        XDrawLine(display, window, gc, x2, y1 + r, x2, y2 - r);

        XDrawLine(display, window, gc, x1, y1 + r, x1, y2 - r);

        XDrawLine(display, window, gc, x1 + r, y2, x2 - r, y2);

        // 4 Arcs

        XDrawArc(display, window, gc, x1, y1, 2 * r, 2 * r, 90 * 64, 90 * 64);

        //region Arc D Top Right

        XDrawArc(display, window, gc, x2 - 2 * r, y1, 2 * r, 2 * r, 0, 90 * 64);

        //region Arc E Bottom Right

        XDrawArc(display, window, gc, x2 - 2 * r, y2 - 2 * r, 2 * r, 2 * r, 270 * 64, 90 * 64);

        //region Arc F Bottom Left

        XDrawArc(display, window, gc, x1, y2 - 2 * r, 2 * r, 2 * r, 180 * 64, 90 * 64);

        //endregion

    }

    // Convenience: body + outline in one call, the polished look.

    inline void draw_polished(Display* display, Window window, GC gc, const SStyle& style, int x1, int y1, int x2, int y2)

    {

        fill_rounded_rect(display, window, gc, style, x1, y1, x2, y2);

        draw_rounded_outline(display, window, gc, style, x1, y1, x2, y2);

    }

    inline int text_width(const SStyle& style, const std::string& text)

    {

        if (!style.font_)

        {

            return (int)text.size() * 8;

        }

        return XTextWidth(style.font_, text.c_str(), (int)text.length());

    }

    // Part 4 centred text: XTextWidth + ascent / descent.

    inline void draw_text_centered(Display* display, Window window, GC gc, const SStyle& style, int x1, int y1, int x2, int y2, const std::string& text)

    {

        if (!style.font_ || text.empty())

        {

            return;

        }

        XSetFont(display, gc, style.font_->fid);

        XSetForeground(display, gc, style.text_color_);

        int tw = XTextWidth(style.font_, text.c_str(), (int)text.length());

        int text_x = x1 + ((x2 - x1) - tw) / 2;

        int text_y = y1 + ((y2 - y1) + style.font_->ascent - style.font_->descent) / 2;

        XDrawString(display, window, gc, text_x, text_y, text.c_str(), (int)text.length());

    }

    // Left-aligned text, vertically centred in the given band.

    inline void draw_text_left(Display* display, Window window, GC gc, const SStyle& style, int x, int y1, int y2, const std::string& text)

    {

        if (!style.font_ || text.empty())

        {

            return;

        }

        XSetFont(display, gc, style.font_->fid);

        XSetForeground(display, gc, style.text_color_);

        int text_y = y1 + ((y2 - y1) + style.font_->ascent - style.font_->descent) / 2;

        XDrawString(display, window, gc, x, text_y, text.c_str(), (int)text.length());

    }

    // Baseline text at an explicit position (labels, grid cells...).

    inline void draw_text_at(Display* display, Window window, GC gc, const SStyle& style, int x, int baseline_y, const std::string& text)

    {

        if (!style.font_ || text.empty())

        {

            return;

        }

        XSetFont(display, gc, style.font_->fid);

        XSetForeground(display, gc, style.text_color_);

        XDrawString(display, window, gc, x, baseline_y, text.c_str(), (int)text.length());

    }

    inline void draw_checkmark(Display* display, Window window, GC gc, unsigned long color, int cx, int cy, int size)

    {

        XSetForeground(display, gc, color);

        set_line_width(display, gc, 2);

        int s = size / 2;

        XDrawLine(display, window, gc, cx - s, cy, cx - s / 3, cy + s);

        XDrawLine(display, window, gc, cx - s / 3, cy + s, cx + s, cy - s);

        set_line_width(display, gc, 1);

    }

    inline void draw_triangle_down(Display* display, Window window, GC gc, unsigned long color, int cx, int cy, int size)

    {

        XSetForeground(display, gc, color);

        for (int i = 0; i < size; ++i)

        {

            XDrawLine(display, window, gc, cx - (size - i), cy + i, cx + (size - i), cy + i);

        }

    }

    inline void draw_triangle_up(Display* display, Window window, GC gc, unsigned long color, int cx, int cy, int size)

    {

        XSetForeground(display, gc, color);

        for (int i = 0; i < size; ++i)

        {

            XDrawLine(display, window, gc, cx - (size - i), cy - i, cx + (size - i), cy - i);

        }

    }

    inline void draw_triangle_right(Display* display, Window window, GC gc, unsigned long color, int cx, int cy, int size)

    {

        XSetForeground(display, gc, color);

        for (int i = 0; i < size; ++i)

        {

            XDrawLine(display, window, gc, cx + i, cy - (size - i), cx + i, cy + (size - i));

        }

    }

}

//endregion

//region Region Canvas (from Part 1)

class Canvas

{

private:

    Display* display = nullptr;

    Window window = 0;

    GC gc = nullptr;

    XImage* image = nullptr;

    int width = 0;

    int height = 0;

    std::vector<uint32_t> pixels;

public:

    Canvas(Display* display, Window window, GC graphicsContext, int width, int height) : display(display), window(window), gc(graphicsContext)

    {

        resize(width, height);

    }

    ~Canvas() { destroyImage(); }

    void resize(int newWidth, int newHeight)

    {

        width = std::max(1, newWidth);

        height = std::max(1, newHeight);

        pixels.resize(static_cast<size_t>(width) * height);

        clear(0x202020);

        destroyImage();

        Visual* visual = DefaultVisual(display, DefaultScreen(display));

        int depth = DefaultDepth(display, DefaultScreen(display));

        image = XCreateImage(display, visual, depth, ZPixmap, 0, nullptr, width, height, 32, 0);

        if (!image)

        {

            std::exit(1);

        }

        image->data = static_cast<char*>(std::malloc(image->bytes_per_line * height));

        if (!image->data)

        {

            std::exit(1);

        }

    }

    int getWidth() const

    {

        return width;

    }

    int getHeight() const

    {

        return height;

    }

    // Direct access to the raw framebuffer.

    uint32_t* data()

    {

        return pixels.data();

    }

    const uint32_t* data() const

    {

        return pixels.data();

    }

    void clear(uint32_t rgb)

    {

        std::fill(pixels.begin(), pixels.end(), rgb);

    }

    void setPixel(int x, int y, uint32_t rgb)

    {

        if (x < 0 || x >= width || y < 0 || y >= height)

        {

            return;

        }

        pixels[static_cast<size_t>(y) * width + x] = rgb;

    }

    uint32_t getPixel(int x, int y) const

    {

        if (x < 0 || x >= width || y < 0 || y >= height)

        {

            return 0;

        }

        return pixels[static_cast<size_t>(y) * width + x];

    }

    void fillRect(int x, int y, int w, int h, uint32_t rgb)

    {

        int x0 = std::max(0, x);

        int y0 = std::max(0, y);

        int x1 = std::min(width, x + w);

        int y1 = std::min(height, y + h);

        for (int py = y0; py < y1; ++py)

        {

            for (int px = x0; px < x1; ++px)

            {

                setPixel(px, py, rgb);

            }

        }

    }

    void drawGradient()

    {

        for (int y = 0; y < height; ++y)

        {

            for (int x = 0; x < width; ++x)

            {

                uint8_t red = static_cast<uint8_t>(255.0 * x / std::max(1, width - 1));

                uint8_t green = static_cast<uint8_t>(255.0 * y / std::max(1, height - 1));

                uint8_t blue = 100;

                uint32_t color = (static_cast<uint32_t>(red) << 16) | (static_cast<uint32_t>(green) << 8) | blue;

                setPixel(x, y, color);

            }

        }

    }

    void present()

    {

        for (int y = 0; y < height; ++y)

        {

            for (int x = 0; x < width; ++x)

            {

                uint32_t rgb = getPixel(x, y);

                unsigned long x11Pixel = ((rgb >> 16) & 0xff) << 16 | ((rgb >> 8) & 0xff) << 8 | (rgb & 0xff);

                XPutPixel(image, x, y, x11Pixel);

            }

        }

        XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);

        XFlush(display);

    }

private:

    void destroyImage()

    {

        if (image)

        {

            XDestroyImage(image);

            image = nullptr;

        }

    }

};

//endregion
