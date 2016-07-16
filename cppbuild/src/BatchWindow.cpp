#include "BatchWindow.hpp"

BatchWindow::BatchWindow() :
  m_tasks(0),m_completed(0),
  m_hours(0),m_minutes(0),m_seconds(0),
  m_label("Job Progress:"),
  m_okButton("OK")
{
  set_default_size(400,150);
  set_title("Batch Window");
  m_progressBar.set_fraction(0.0);
  m_okButton.signal_clicked().connect(sigc::mem_fun(*this, &BatchWindow::on_ok_clicked));
  m_textBuffer = Gtk::TextBuffer::create();
  m_textBuffer->set_text("");
  m_textView.set_buffer(m_textBuffer);
  m_scroll.add(m_textView);
  m_hbox.pack_start(m_label,Gtk::PACK_SHRINK,5);
  m_hbox.pack_start(m_progressBar,Gtk::PACK_EXPAND_WIDGET);
  m_hbox.pack_start(m_okButton,Gtk::PACK_SHRINK,10);
  m_vbox.pack_start(m_hbox,Gtk::PACK_SHRINK);
  m_vbox.pack_start(m_hsep,Gtk::PACK_SHRINK,10);
  m_vbox.pack_start(m_scroll,Gtk::PACK_EXPAND_WIDGET);
  add(m_vbox);
}

void BatchWindow::launch(int ntasks)
{
  m_tasks = ntasks;
  m_completed = 0;
  m_hours = 0;
  m_minutes = 0;
  m_seconds = 0;
  Glib::signal_timeout().connect(sigc::mem_fun(*this, &BatchWindow::refresh), 1000);
  show();
  show_all_children();
}

void BatchWindow::taskCompleted()
{
  nia::nout.lock();
  m_completed++;
  double frac = ((double)m_completed) / m_tasks;
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
    Gtk::TextBuffer::iterator it = m_textBuffer->end();
    m_textView.scroll_to(it);
  }
  nia::nout.unlock();
  m_seconds++;
  if(m_seconds == 60){
    m_seconds = 0;
    m_minutes++;
    if(m_minutes == 60){
      m_minutes = 0;
      m_hours++;
    }
  }
  std::ostringstream s;
  s << m_completed << "/" << m_tasks << " (";
  if(m_hours < 10) s << "0";
  s << m_hours << ":";
  if(m_minutes <10) s << "0";
  s << m_minutes << ":";
  if(m_seconds < 10) s << "0";
  s << m_seconds << ")";
  m_progressBar.set_text(s.str());
  return m_completed < m_tasks;
}
