#pragma once
#include "core/xframe_core.h"
#include "../StockData.h"
#include <cmath>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
// StockTextAnnotation — XFrame Stock Market drawing/annotation widget.
// Draws a light candlestick background from an OHLCVSeries, then its
// annotation in a strong colour. Attribution: XFrame.
class StockTextAnnotation {
private:
    Display* display_; int screen_; Window window_; GC gc_;
    SStyle_Manager* styles_; int x1_, y1_, x2_, y2_;
    std::string name_ = "StockTextAnnotation"; std::string text_ = "StockTextAnnotation";
    std::string style_name_; bool visible_ = true; bool enabled_ = true; bool focused_ = false;
    xframe::OHLCVSeries series_;
    struct Anchor { int bar = 0; double price = 0.0; };
    std::vector<Anchor> anchors_;
    Srgb annot_ = Srgb{160,90,0};
    std::string text__ = "Note";
    std::vector<std::pair<int,double>> brush_;
    bool dragging_ = false; int drag_anchor_ = -1;
    int axis_w_ = 56; int axis_h_ = 20;

public:
    StockTextAnnotation(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)
        : display_(display), screen_(screen), window_(window), gc_(gc), styles_(styles),
          x1_(x), y1_(y), x2_(x + width), y2_(y + height) { reset_anchors(); }
    ~StockTextAnnotation() {}
    void set_bounds(int x, int y, int w, int h) { x1_=x; y1_=y; x2_=x+w; y2_=y+h; }
    int width() { return x2_-x1_; } int height() { return y2_-y1_; }
    bool contains(int x, int y) { return visible_ && x>=x1_ && x<=x2_ && y>=y1_ && y<=y2_; }
    void set_visible(bool v) { visible_=v; } bool get_visible() { return visible_; }
    void set_enabled(bool e) { enabled_=e; } bool get_enabled() { return enabled_; }
    void set_style_name(std::string s) { style_name_=s; }
    void set_text(std::string t) { text_=t; text__=t; }
    std::string get_text() { return text_; }
    SStyle style() { return styles_->resolve(style_name_); }
    void set_series(const xframe::OHLCVSeries& s) { series_=s; reset_anchors(); }
    const xframe::OHLCVSeries& series() const { return series_; }
    int bar_count() { return (int)series_.size(); }
    void set_anchors(int b1, double p1, int b2, double p2) {
        anchors_.clear(); anchors_.push_back({b1,p1}); anchors_.push_back({b2,p2}); }
    void set_anchors3(int b1, double p1, int b2, double p2, int b3, double p3) {
        anchors_.clear(); anchors_.push_back({b1,p1}); anchors_.push_back({b2,p2}); anchors_.push_back({b3,p3}); }
    void set_anchor(int i, int bar, double price) {
        if (i>=0 && i<(int)anchors_.size()) anchors_[(size_t)i]={bar,price}; }
    Anchor anchor(int i) const { return (i>=0 && i<(int)anchors_.size()) ? anchors_[(size_t)i] : Anchor{}; }
    int anchor_count() { return (int)anchors_.size(); }
    void set_color(Srgb c) { annot_=c; } Srgb get_color() { return annot_; }
    void add_point(int bar, double price) { brush_.push_back({bar,price}); }
    void clear_points() { brush_.clear(); }
    int point_count() { return (int)brush_.size(); }
    void reset_anchors() {
        anchors_.clear();
        if (series_.empty()) { anchors_.push_back({0,100.0}); anchors_.push_back({10,100.0}); anchors_.push_back({20,100.0}); return; }
        int n=(int)series_.size();
        double lo=series_[0].low, hi=series_[0].high;
        for (auto& b: series_) { lo=std::min(lo,b.low); hi=std::max(hi,b.high); }
        int b1=std::min(5,n-1), b2=std::min(25,n-1), b3=std::min(15,n-1);
        anchors_.push_back({b1, series_[(size_t)b1].close});
        anchors_.push_back({b2, series_[(size_t)b2].close});
        anchors_.push_back({b3, (hi+lo)/2.0});
        // Fib widgets use anchors 0/1 as high/low; pitchfork uses all three.
        
    }
    void set_annotation_text(const std::string& t) { text__=t; text_=t; } std::string annotation_text() { return text__; }

    int plot_x() { return x1_+axis_w_; } int plot_y() { return y1_+6; }
    int plot_w() { return width()-axis_w_-8; } int plot_h() { return height()-axis_h_-12; }
    double item_w() { double c=series_.empty()?1.0:(double)series_.size(); return plot_w()/c; }
    int x_center(int i) { return plot_x()+(int)((i+0.5)*item_w()); }
    int x_at_bar(int i) { return plot_x()+(int)(i*item_w()); }
    void y_range(double& lo, double& hi) {
        lo=1e300; hi=-1e300;
        for (auto& b: series_) { lo=std::min(lo,b.low); hi=std::max(hi,b.high); }
        if (lo>hi) { lo=0.0; hi=1.0; return; }
        double pad=(hi-lo)*0.06+1e-9; lo-=pad; hi+=pad; }
    int y_at(double v, double lo, double hi) { return plot_y()+(int)((hi-v)/(hi-lo)*plot_h()); }
    double value_at_y(int y, double lo, double hi) { return hi-(double)(y-plot_y())/plot_h()*(hi-lo); }
    int bar_at_x(int x) { int b=(int)((x-plot_x())/item_w()); return std::max(0,std::min(std::max(0,(int)series_.size()-1),b)); }
    static std::string fmt(double v) { std::ostringstream o; o.precision(2); o<<std::fixed<<v; std::string s=o.str();
        while (!s.empty() && s.back()=='0') { s.pop_back(); }
        if (!s.empty() && s.back()=='.') { s.pop_back(); }
        return s; }
    void set_dash(bool on) { XGCValues gv; gv.line_style=on?LineOnOffDash:LineSolid; XChangeGC(display_,gc_,GCLineStyle,&gv); }
    void draw_candles(double lo, double hi) {
        int bw=std::max(1,(int)(item_w()*0.6));
        for (size_t i=0;i<series_.size();++i) { const auto& b=series_[i]; int cx=x_center((int)i);
            unsigned long c=styles_->get_color_rgb(b.close>=b.open?120:200, b.close>=b.open?190:120, b.close>=b.open?120:120);
            XSetForeground(display_,gc_,c);
            XDrawLine(display_,window_,gc_,cx,y_at(b.high,lo,hi),cx,y_at(b.low,lo,hi));
            int top=y_at(std::max(b.open,b.close),lo,hi), bot=y_at(std::min(b.open,b.close),lo,hi);
            XFillRectangle(display_,window_,gc_,cx-bw/2,top,bw,std::max(1,bot-top)); } }
    void draw() {
        if (!visible_) return;
        SStyle st=style();
        unsigned long white=styles_->get_color_rgb(255,255,255);
        unsigned long grid=styles_->get_color_rgb(228,228,228);
        unsigned long axis_col=styles_->get_color_rgb(150,150,150);
        XSetForeground(display_,gc_,white); XFillRectangle(display_,window_,gc_,x1_,y1_,width(),height());
        double lo=0.0, hi=1.0; y_range(lo,hi);
        int px=plot_x(), py=plot_y(), pw=plot_w(), ph=plot_h();
        double step=(hi-lo)/5.0; if(!(step>0)) step=1.0;
        for (int k=0;k<=5;++k) { double v=lo+k*step; int gy=y_at(v,lo,hi);
            XSetForeground(display_,gc_,grid); XDrawLine(display_,window_,gc_,px,gy,px+pw,gy);
            xf::draw_text_at(display_,window_,gc_,st,px-xf::text_width(st,fmt(v))-6,gy+4,fmt(v)); }
        draw_candles(lo,hi);
        unsigned long acol=styles_->get_color_Srgb(annot_);
        auto X=[&](int bar){ return x_center(bar); };
        auto Y=[&](double v){ return y_at(v,lo,hi); };
        (void)X; (void)Y;
        (void)px; (void)py; (void)pw; (void)ph;
        int cx=X(anchors_[0].bar), cy=Y(anchors_[0].price);
        std::string t=text__.empty()?text_:text__; int tw=xf::text_width(st,t);
        XSetForeground(display_,gc_,styles_->get_color_rgb(255,248,200)); XFillRectangle(display_,window_,gc_,cx,cy-18,tw+10,18);
        XSetForeground(display_,gc_,acol); XDrawRectangle(display_,window_,gc_,cx,cy-18,tw+10,18);
        XSetForeground(display_,gc_,styles_->get_color_rgb(20,20,20)); xf::draw_text_at(display_,window_,gc_,st,cx+5,cy-5,t);
        XFillArc(display_,window_,gc_,cx-3,cy-3,6,6,0,360*64);
        XSetForeground(display_,gc_,axis_col); XDrawRectangle(display_,window_,gc_,px,py,pw,ph);
        XSetForeground(display_,gc_,st.lineColor_); XDrawRectangle(display_,window_,gc_,x1_,y1_,width(),height());
        xf::draw_text_at(display_,window_,gc_,st,x1_+8,y1_+14,name_);
        XFlush(display_);
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x,y)||!enabled_) return false;
        focused_=true;
        if (button==1) {
            double lo=0.0, hi=1.0; y_range(lo,hi);
            int bar=bar_at_x(x); double price=value_at_y(y,lo,hi);
            if (!brush_.empty() || name_=="StockBrush") { add_point(bar,price); dragging_=true; drag_anchor_=-1; return true; }
            int best=-1; double bd=1e300;
            for (size_t i=0;i<anchors_.size();++i) { double d=std::abs(x-X_dummy(i))+std::abs(y-y_at(anchors_[i].price,lo,hi)); if(d<bd){bd=d;best=(int)i;} }
            if (best>=0) { anchors_[(size_t)best]={bar,price}; dragging_=true; drag_anchor_=best; }
            return true; }
        return false; }
    int X_dummy(size_t i) { return x_center(anchors_[i].bar); }
    bool on_button_release(int x, int y, unsigned int button) {
        (void)x; (void)y; if (button==1&&dragging_) { dragging_=false; drag_anchor_=-1; return true; } return false; }
    bool on_motion(int x, int y) {
        if (!enabled_) return false;
        if (dragging_ && drag_anchor_>=0) { double lo=0.0, hi=1.0; y_range(lo,hi);
            anchors_[(size_t)drag_anchor_]={bar_at_x(x), value_at_y(y,lo,hi)}; return true; }
        if (dragging_ && name_=="StockBrush") { double lo=0.0, hi=1.0; y_range(lo,hi); add_point(bar_at_x(x), value_at_y(y,lo,hi)); return true; }
        return true; }
    bool on_key(KeySym key, const char* str) {
        (void)str; if (!focused_||!enabled_) return false;
        double lo=0.0, hi=1.0; y_range(lo,hi); double step=(hi-lo)*0.02;
        switch (key) {
        case XK_Left: for (auto& a: anchors_) a.bar=std::max(0,a.bar-1); return true;
        case XK_Right: for (auto& a: anchors_) a.bar=std::min(std::max(0,(int)series_.size()-1),a.bar+1); return true;
        case XK_Up: for (auto& a: anchors_) a.price+=step; return true;
        case XK_Down: for (auto& a: anchors_) a.price-=step; return true;
        case XK_r: case XK_R: case XK_Home: reset_anchors(); return true;
        } return false; }
};
