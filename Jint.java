import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.StringTokenizer;
import java.util.concurrent.ConcurrentHashMap;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JTextArea;

public class Jint extends JPanel implements KeyListener
{
    private ConcurrentHashMap<String,Object> vars;
    private JTextArea pastArea;
    private JTextArea nextArea;
    
    public Jint()
    {
        vars = new ConcurrentHashMap<String,Object>();
        pastArea = new JTextArea(1,100);
        pastArea.setEditable(false);
        nextArea = new JTextArea(1,100);
        nextArea.addKeyListener(this);
        setLayout(new BoxLayout(this,BoxLayout.Y_AXIS));
        add(pastArea);
        add(nextArea);
    }
    
    public void keyPressed(KeyEvent e){}
    
    public void keyReleased(KeyEvent e){}
    
    public void keyTyped(KeyEvent e)
    {
        if(e.getKeyChar() == '\n'){
            String msg = nextArea.getText();
            if(interpretText(msg)){
                pastArea.setText(pastArea.getText() + msg);
                nextArea.setText("");
            }
        }
    }
    
    public void setVar(String name, Object value)
    {
        vars.put(name,value);
    }
    
    public void delete(String name)
    {
        vars.remove(name);
    }
    
    public Object getVar(String name){ return vars.get(name); }
    
    public boolean interpretText(String command)
    {
        if((command.contains(";") || command.contains("{")) && !(command.endsWith("\n\n"))) return false;
        StringTokenizer st = new StringTokenizer(command," \n{}().;");
        return true;
    }
}