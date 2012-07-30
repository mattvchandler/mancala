// appstart.cpp
// main routines for mancala game
// Copyright Matthew Chandler 2012

#include <iostream>

#include <cstdlib>
#include <ctime>

#include <gtkmm/application.h>

#include "gui.h"
#include "mancala.h"

int main(int argc, char * argv[])
{
    // initialize random seed
    srand(time(0));

    // set up and launch a GTK window
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "mancala.mancala");

    Mancala_win m_win;

    return app->run(m_win);
}
