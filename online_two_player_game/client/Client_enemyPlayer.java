import java.awt.Color;
import java.awt.Graphics;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

public class Client_enemyPlayer{

	private int x;
	private int y;

	private long current ;
	private long delay;

	public Client_enemyPlayer(int x,int y){
		this.x = x;
		this.y = y;
	}
	public void initialize(){
		current = System.nanoTime();
		delay = 100;
	}

	public void updateFrame(int X_recieved, int Y_recieved, int fire){
		x=X_recieved;
		y=Y_recieved; 

		if(fire==1)
		{
			long breaks = (System.nanoTime()-current)/1000000;
			if(breaks > delay){
				projectSWClient.laser_server.add(new Client_enemyLaser(x+90,y-20));// Draw Laser for server player
			}
			current = System.nanoTime();
		}
	}

	public void render(Graphics g){ 
		g.drawImage(Client_Image.XWing,x,y, 190, 206,null);
	}
	public void renderBlast(Graphics g){
		g.drawImage(Client_Image.blast0,x,y, 300, 300,null);
	}
	public int getY(){
		return y;
	}
	public int getX(){
		return x;
	}

}
