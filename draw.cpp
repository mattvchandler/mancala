// draw.cpp
// graphics for mancala
// Copyright Matthew Chandler 2012

#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdlib>

#include <gdkmm/general.h>
#include <glibmm/fileutils.h>
#include <glibmm/main.h>

#include "draw.h"

namespace Mancala
{
    Draw::Draw(const int Num_bowls, const int Num_beads, const int Ai_depth,
            const bool Extra_rule, const bool Capture_rule, const bool Collect_rule):
        hint_player(Mancala::PLAYER_1),
        show_hint(false),
        hint_i(0),
        b(Num_bowls, Num_beads, Ai_depth, Extra_rule, Capture_rule, Collect_rule)
    {
        // signal on mouse click
        add_events(Gdk::BUTTON_PRESS_MASK);

        // load images from disk
        try
        {
            bg_store = Gdk::Pixbuf::create_from_file("img/bg_store.png");
            bg_bowl = Gdk::Pixbuf::create_from_file("img/bg_bowl.png");
            bg_board = Gdk::Pixbuf::create_from_file("img/bg_board.png");
            hint_img = Gdk::Pixbuf::create_from_file("img/hint.png");
            bead_s_img = Gdk::Pixbuf::create_from_file("img/bead_s.png");
            bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_red.png"));
            bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_green.png"));
            bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_blue.png"));
            bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_yellow.png"));
            bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_magenta.png"));
            bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_cyan.png"));
        }
        catch(const Glib::FileError& ex)
        {
            std::cerr<<"File Error: "<<ex.what()<<std::endl;
        }
        catch(const Gdk::PixbufError& ex)
        {
            std::cerr<<"Pixbuf Error: "<< ex.what()<<std::endl;
        }
    }

    // helper function to draw an image
    void Draw::draw_img(const Cairo::RefPtr<Cairo::Context>& cr, const Glib::RefPtr<Gdk::Pixbuf>&  img,
        const double translate_x, const double translate_y, const double scale_x, const double scale_y)
    {
        cr->save();
        cr->translate(translate_x, translate_y);
        cr->scale(scale_x, scale_y);
        Gdk::Cairo::set_source_pixbuf(cr, img);
        cr->paint();
        cr->restore();
    }

    // helper function to draw a number
    void Draw::draw_num(const Cairo::RefPtr<Cairo::Context>& cr, const Pango::FontDescription & font,
        const int num, const double x, const double y)
    {
        std::ostringstream str;
        int txt_w, txt_h;
        str<<num;
        Glib::RefPtr<Pango::Layout> txt = create_pango_layout(str.str());
        txt->set_font_description(font);
        txt->get_pixel_size(txt_w, txt_h);
        cr->move_to(x - .5 * txt_w, y - .5 * txt_h);
        txt->show_in_cairo_context(cr);
    }

    // main drawing routine
    bool Draw::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        int num_cells = b.num_bowls + 2;
        double inv_num_cells = 1.0 / num_cells;

        Gtk::Allocation alloc = get_allocation();

        Pango::FontDescription font("Monospace");
        font.set_size(std::min(alloc.get_width(), alloc.get_height()) * .1 * Pango::SCALE);
        cr->set_source_rgb(0.0, 0.0, 0.0);

        // draw bg
        draw_img(cr, bg_board, 0.0, 0.0, alloc.get_width() / (bg_board->get_width() - .5),
            alloc.get_height() / (bg_board->get_height() - .5));

        // draw bg for left store
        draw_img(cr, bg_store, 0.0, 0.0, alloc.get_width() / (bg_store->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bg_store->get_height() - .5));

        // l store beads
        for(auto & j: b.l_store.beads)
        {
            draw_img(cr, bead_imgs[j.color_i], alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                alloc.get_width() / (bead_imgs[j.color_i]->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bead_imgs[j.color_i]->get_height() - .5) * .5);
            draw_img(cr, bead_s_img, alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                alloc.get_width() / (bead_s_img->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bead_s_img->get_height() - .5) * .5);
        }

        // draw # for left store
        draw_num(cr, font, b.l_store.beads.size(), alloc.get_width() * .5 * inv_num_cells, 
            alloc.get_height() * .5);

        // draw bg for right store
        draw_img(cr, bg_store, alloc.get_width() * (1.0 - inv_num_cells), 0,
            alloc.get_width() / (bg_store->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bg_store->get_height() - .5));

        // r store beads
        for(auto & j: b.r_store.beads)
        {
            draw_img(cr, bead_imgs[j.color_i], alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                alloc.get_width() / (bead_imgs[j.color_i]->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bead_imgs[j.color_i]->get_height() - .5) * .5);
            draw_img(cr, bead_s_img, alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                alloc.get_width() / (bead_s_img->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bead_s_img->get_height() - .5) * .5);
        }

        // draw # for right store
        draw_num(cr, font, b.r_store.beads.size(), alloc.get_width() * (1.0 - .5 * inv_num_cells),
            alloc.get_height() * .5);

        // draw bowls
        for(size_t i = 0; i < b.top_row.size(); ++i)
        {
            // draw upper row hint
            if(hint_player == PLAYER_2 && show_hint && hint_i == (int)i)
            {
                draw_img(cr, hint_img, alloc.get_width() * (i + 1) * inv_num_cells, 0.0,
                    alloc.get_width() / (hint_img->get_width() - .5) * inv_num_cells,
                    alloc.get_height() / (hint_img->get_height() - .5) * .5);
            }

            // upper row bgs
            draw_img(cr, bg_bowl, alloc.get_width() * (i + 1) * inv_num_cells, 0,
                alloc.get_width() / (bg_bowl->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bg_bowl->get_height() - .5) * .5);

            // upper row beads
            for(auto & j: b.top_row[i].beads)
            {
                draw_img(cr, bead_imgs[j.color_i], alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                    alloc.get_width() / (bead_imgs[j.color_i]->get_width() - .5) * inv_num_cells,
                    alloc.get_height() / (bead_imgs[j.color_i]->get_height() - .5) * .5);
                draw_img(cr, bead_s_img, alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                    alloc.get_width() / (bead_s_img->get_width() - .5) * inv_num_cells,
                    alloc.get_height() / (bead_s_img->get_height() - .5) * .5);
            }

            // upper row txt
            draw_num(cr, font, b.top_row[i].beads.size(), alloc.get_width() * (2 * i + 3) * .5 * inv_num_cells,
                alloc.get_height() * .25);

            // draw lower row hint
            if(hint_player == PLAYER_1 && show_hint && hint_i == (int)i)
            {
                draw_img(cr, hint_img, alloc.get_width() * (i + 1) * inv_num_cells, .5 * alloc.get_height(),
                    alloc.get_width() / (hint_img->get_width() - .5) * inv_num_cells,
                    alloc.get_height() / (hint_img->get_height() - .5) * .5);
            }

            // lower row bgs
            draw_img(cr, bg_bowl, alloc.get_width() * (i + 1) * inv_num_cells, .5 * alloc.get_height(),
                alloc.get_width() / (bg_bowl->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bg_bowl->get_height() - .5) * .5);

            // lower row beads
            for(auto & j: b.bottom_row[i].beads)
            {
                draw_img(cr, bead_imgs[j.color_i], alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                    alloc.get_width() / (bead_imgs[j.color_i]->get_width() - .5) * inv_num_cells,
                    alloc.get_height() / (bead_imgs[j.color_i]->get_height() - .5) * .5);
                draw_img(cr, bead_s_img, alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                    alloc.get_width() / (bead_s_img->get_width() - .5) * inv_num_cells,
                    alloc.get_height() / (bead_s_img->get_height() - .5) * .5);
            }

            // lower row txt
            draw_num(cr, font, b.bottom_row[i].beads.size(), alloc.get_width() * (2 * i + 3) * .5 * inv_num_cells,
                alloc.get_height() * .75);
        }
        return true;
    }
}
