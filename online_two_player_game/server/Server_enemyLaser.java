import java.awt.Color;
import java.awt.Graphics;

public class Server_enemyLaser {

    private int x;
    private int y;
    private int speed;
   
    public Server_enemyLaser(int x, int y)
    {
       this.x = x;
       this.y = y;
       speed = 10;
      
    }
    
    public void updateFrame()
    {    
     y += speed;  //goes opposite direction
    }
    
    public int getY(){
       return y;
    }
    
    public int getX(){
       return x;
    }
   
    public void render(Graphics g){
      g.drawImage(Server_Image.Laser_TIE_Fighter,x,y, 26, 42,null);
    }
}