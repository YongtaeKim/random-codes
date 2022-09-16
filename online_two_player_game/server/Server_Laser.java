import java.awt.Color;
import java.awt.Graphics;

public class Server_Laser {

    private int x;
    private int y;
    private int speed;
   
    public Server_Laser(int x, int y)
    {
       this.x = x;
       this.y = y;
       speed = 10;
      
    }
    
    public void updateFrame()
    {    
     y -= speed;  // bullet goes forward
    }
    
      //return y coordinate
    public int getY(){
       return y;
    }
    //return y coordinate
    public int getX(){
       return x;
    }
   
    public void render(Graphics g){
      g.drawImage(Server_Image.Laser_XWing,x,y, 25, 42,null);
    }
}