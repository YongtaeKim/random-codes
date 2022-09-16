import java.awt.Color;
import java.awt.Graphics;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;


public class Server_enemyPlayer{

	private int x;
	private int y;

	private long current ;
	private long delay;

	public Server_enemyPlayer(int x,int y){
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
				projectSWServer.laser_client.add(new Server_enemyLaser(x+80,y+140));// Draw Laser for client player
			}
			current = System.nanoTime();
		}  
	}

	public void render(Graphics g){ 
		g.drawImage(Server_Image.TIE_Fighter,x,y, 180, 151,null);
	}
	public void renderBlast(Graphics g){
		g.drawImage(Server_Image.blast0,x,y, 300, 300,null);
	}
	public int getY(){
		return y;
	}
	public int getX(){
		return x;
	} 

}
