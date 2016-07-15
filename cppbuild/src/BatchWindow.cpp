#include "BatchWindow.hpp"

BatchWindow::BatchWindow() :
  m_pending(0),m_completed(0),
  m_okButton("OK")
{
  set_default_size(400,300);
  m_progressBar.set_fraction(0.0);
  m_okButton.signal_clicked().connect(sigc::mem_fun(*this, &BatchWindow::on_ok_clicked));
  m_textBuffer = Gtk::TextBuffer::create();
  m_textBuffer->set_text("");
  m_textView.set_buffer(m_textBuffer);
  m_scroll.add(m_textView);
  m_hbox.pack_start(m_progressBar,Gtk::PACK_EXPAND_PADDING);
  m_hbox.pack_start(m_okButton,Gtk::PACK_EXPAND_PADDING);
  m_vbox.pack_start(m_hbox,Gtk::PACK_SHRINK);
  m_vbox.pack_start(m_hsep,Gtk::PACK_SHRINK,10);
  m_vbox.pack_start(m_scroll,Gtk::PACK_EXPAND_WIDGET);
  add(m_vbox);
}

void BatchWindow::launch(int ntasks)
{
  m_pending = ntasks;
  m_completed = 0;
  Glib::signal_timeout().connect(sigc::mem_fun(*this, &BatchWindow::refresh), 1000);
  show();
  show_all_children();
}

void BatchWindow::taskCompleted()
{
  nia::nout.lock();
  m_completed++;
  double frac = ((double)m_completed) / m_pending;
  m_progressBar.set_fraction(frac);
  nia::nout.unlock();
}

bool BatchWindow::refresh()
{
  nia::nout.lock();
  std::string msg = nia::nout.buffer();
  if(msg.length() > 1){
    m_textBuffer->insert(m_textBuffer->end(),msg);
    nia::nout.clear();
  }
  nia::nout.unlock();
  return true;
}
