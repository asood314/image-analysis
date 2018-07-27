#include "Scanner.hpp"
//----------Linux/Mac-----------
//#include <sys/time.h>
//#include <sys/resource.h>
//----------Linux/Mac-----------
//#include <gtkmm-2.4/gtkmm.h>

int main(int argc, char** argv)
{
  //----------Linux/Mac-----------
  //struct rlimit lim;
  //lim.rlim_cur = RLIM_INFINITY;
  //lim.rlim_max = RLIM_INFINITY;
  //setrlimit(RLIMIT_CPU,&lim);
  //----------Linux/Mac-----------

  Gtk::Main kit(argc,argv);
  nia::niaVersion = 7;
  Scanner s(argc, argv);
  Gtk::Main::run(s);

  return 0;
}
