//gui.h
//GUI for mancala game, using gtkmm
//Copyright Matthew Chandler 2012

#ifndef __MANCALA_GUI_H__
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>

#include "mancala.h"

class Mancala_win: public Gtk::Window
{
public:
    Mancala_win();

protected:
    //containers
    Gtk::Box main_h_box;

private:
    //the actual mancala board
    Board b;
};

#define __MANCALA_GUI_H__
#endif //__MANCALA_GUI_H__
