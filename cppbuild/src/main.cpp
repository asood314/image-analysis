#include "ImSeries.hpp"
#include "NiaCore.hpp"
//#include <gtkmm-2.4/gtkmm.h>

int main(int argc, char** argv)
{

  Gtk::Main kit(argc,argv);
  NiaCore nc;
  nia::niaVersion = 2;
  Gtk::Main::run(nc);

  return 0;
}
