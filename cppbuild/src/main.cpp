#include "ImSeries.hpp"
#include "NiaCore.hpp"
//#include <gtkmm-2.4/gtkmm.h>

int main(int argc, char** argv)
{
  /*
  std::vector<ImFrame*> frames = ImFrame::load("/Users/asood/Documents/neuroscience/scratch/test1001_0.tiff");
  ImStack* stack = new ImStack(2,1);
  stack->insert(frames.at(0),0,0);
  stack->insert(frames.at(0),1,0);
  ImSeries* series = new ImSeries(1,1);
  series->insert(stack,0,0);
  */

  Gtk::Main kit(argc,argv);
  NiaCore nc;
  Gtk::Main::run(nc);

  return 0;
}
