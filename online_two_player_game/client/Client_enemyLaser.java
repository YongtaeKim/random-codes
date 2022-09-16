import java.awt.Color;
import java.awt.Graphics;

public class Client_enemyLaser {

    private int x;
    private int y;
    private int speed;
   
    public Client_enemyLaser(int x, int y)
    {
       this.x = x;
       this.y = y;
       speed = 10;     
    }
    
     public void updateFrame()
    {    
     y -= speed;  //opponent bullet goes opposite direction
    }
    
    public int getY(){
       return y;
    }
    
    public int getX(){
       return x;
    }
   
    public void render(Graphics g){
      g.drawImage(Client_Image.Laser_XWing,x,y, 25, 42,null);
    }
}