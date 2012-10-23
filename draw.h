// draw.h
// graphics for mancala
// Copyright Matthew Chandler 2012

#ifndef __MANCALA_DRAW_H__
#define __MANCALA_DRAW_H__

#include <gtkmm/drawingarea.h>

#include "board.h"


namespace Mancala
{
    class Draw: public Gtk::DrawingArea
    {
    public:
        Draw(const int Num_bowls = 6, const int Num_beads = 4, const int Ai_depth = 10);

        // main drawing routine
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

        // state vars
        Player hint_player;
        bool show_hint;
        int hint_i;

        // board data
        Mancala::Board b;

    private:
        // helper function to draw an image
        void draw_img(const Cairo::RefPtr<Cairo::Context>& cr, const Glib::RefPtr<Gdk::Pixbuf>&  img,
            const double translate_x, const double translate_y, const double scale_x, const double scale_y);
        // helper function to draw a number
        void draw_num(const Cairo::RefPtr<Cairo::Context>& cr, const Pango::FontDescription & font, const int num,
            const double x, const double y);

        // Images
        Glib::RefPtr<Gdk::Pixbuf> bg_store;
        Glib::RefPtr<Gdk::Pixbuf> bg_bowl;
        Glib::RefPtr<Gdk::Pixbuf> bg_board;
        Glib::RefPtr<Gdk::Pixbuf> hint_img;
        Glib::RefPtr<Gdk::Pixbuf> bead_s_img;
        std::vector<Glib::RefPtr<Gdk::Pixbuf>> bead_imgs;
    };
}
#endif // __MANCALA_DRAW_H__
