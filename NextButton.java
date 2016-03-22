import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;
import javax.swing.Icon;
import javax.swing.JButton;

public class NextButton extends JButton
{
    public class TriangleIcon implements Icon
    {
        private boolean forward;
        
        public TriangleIcon(boolean fr)
        {
            forward = fr;
        }
        
        public int getIconHeight(){ return 16; }
        
        public int getIconWidth(){ return 16; }
        
        public void paintIcon(Component c, Graphics g, int x, int y)
        {
            g.setColor(Color.black);
            if(forward){
                int[] xs = {x,x+14,x};
                int[] ys = {y,y+8,y+16};
                g.fillPolygon(xs,ys,3);
            }
            else{
                int[] xs = {x+14,x,x+14};
                int[] ys = {y,y+8,y+16};
                g.fillPolygon(xs,ys,3);
            }
        }
    }
    
    public NextButton(boolean fr)
    {
        super();
        setIcon(new TriangleIcon(fr));
    }
}