#ifndef BATCH_WINDOW_HPP
#define BATCH_WINDOW_HPP

#include <gtkmm-2.4/gtkmm.h>
#include "NiaBuffer.hpp"

class BatchWindow : public Gtk::Window
{

protected:
  int m_pending,m_completed;
  Gtk::ProgressBar m_progressBar;
  Gtk::HSeparator m_hsep;
  Gtk::VBox m_vbox;
  Gtk::HBox m_hbox;
  Gtk::TextView m_textView;
  Glib::RefPtr<Gtk::TextBuffer> m_textBuffer;
  Gtk::ScrolledWindow m_scroll;
  Gtk::Button m_okButton;

  void on_ok_clicked(){ hide(); }

public:
  BatchWindow();
  virtual ~BatchWindow(){}
  void launch(int ntasks);
  void taskCompleted();
  bool refresh();
  
};

#endif
