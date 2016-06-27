#include "ImSeries.hpp"
#include "NiaCore.hpp"
//#include <gtkmm-2.4/gtkmm.h>

int main(int argc, char** argv)
{
  std::vector<ImFrame*> frames = ImFrame::load("/Users/asood/Documents/neuroscience/scratch/test1001_0.tiff");
  ImStack* stack = new ImStack(1,3);
  stack->insert(frames.at(0),0,0);
  stack->insert(frames.at(0),0,1);
  stack->insert(frames.at(0),0,2);
  ImSeries* series = new ImSeries(1,1);
  series->insert(stack,0,0);

  Gtk::Main kit(argc,argv);
  NiaCore nc(series);
  Gtk::Main::run(nc);

  return 0;
}
