// appstart.cpp
// main routines for mancala game
// Copyright Matthew Chandler 2014

#include <iostream>

#include <cstdlib>
#include <ctime>

#include <gtkmm/application.h>

#include "gui.h"

int main(int argc, char * argv[])
{
    // initialize random seed
    srand(time(0));

    // set up and launch a GTK window
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv,
        "org.matt.mancala", Gio::APPLICATION_NON_UNIQUE);

    Mancala::Win m_win;

    return app->run(m_win);
}
