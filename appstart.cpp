// appstart.cpp
// main routines for mancala game
// Copyright Matthew Chandler 2012

#include <iostream>

#include <cstdlib>
#include <ctime>

#include <gtkmm/main.h>

#include "gui.h"

int main(int argc, char * argv[])
{
    // initialize random seed
    srand(time(0));

    // set up and launch a GTK window
    Gtk::Main kit(argc, argv);
    Mancala::Win m_win;

    Gtk::Main::run(m_win);
    return 0;
}
