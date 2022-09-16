import java.awt.MouseInfo;
import java.awt.Graphics;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;


public class Client_Player implements KeyListener{

	private int x, y, fire;
	private long current, delay;

	public Client_Player(int x,int y){
		this.x = x;
		this.y = y;
	}

	public void initialize(){
		Client_Window.frame.addKeyListener(this);
		current = System.nanoTime();
		delay = 100;
	}

	public void updateFrame()
	{
		// Control with mouse. Position Player on mouse pointer
		x = MouseInfo.getPointerInfo().getLocation().x - Client_Window.frame.getLocationOnScreen().x - 80;
		y = MouseInfo.getPointerInfo().getLocation().y - Client_Window.frame.getLocationOnScreen().y - 80;

		if(fire==1){
			// Put a delay on firing laser.
			long breaks = (System.nanoTime()-current)/1000000;
			if(breaks > delay){
				projectSWClient.laser_client.add(new Client_Laser(x+80,y+140));// Draw Laser
			}
			current = System.nanoTime();
		}  
	}

	public void render(Graphics g){ 
		g.drawImage(Client_Image.TIE_Fighter,x,y, 180, 151,null);
	}
	public void renderBlast(Graphics g){
		g.drawImage(Client_Image.blast0,x,y, 300, 300,null);
	}
	
	// Keyboard Spacebar is the fire key
	public void keyPressed(KeyEvent e) {
		int source = e.getKeyCode();
		if(source == KeyEvent.VK_SPACE){
			fire = 1;
		}
	}

	public void keyReleased(KeyEvent e) {
		int source = e.getKeyCode();
		if(source == KeyEvent.VK_SPACE){
			fire = 0;
		}
	}

	public void keyTyped(KeyEvent e) {

	}
	public int getY(){
		return y;
	}
	public int getX(){
		return x;
	}
	public int getFire(){
		return fire;
	}

}
