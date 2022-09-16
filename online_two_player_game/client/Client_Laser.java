import java.awt.Color;
import java.awt.Graphics;

public class Client_Laser {

    private int x;
    private int y;
    private int speed;
   
    public Client_Laser(int x, int y)
    {
       this.x = x;
       this.y = y;
       speed = 10;
      
    }
    public void updateFrame()
    {
     y += speed;  //Bullet goes forward
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
      g.drawImage(Client_Image.Laser_TIE_Fighter,x,y, 26, 42,null);
    }
}