
import java.awt.*;
import java.awt.image.BufferStrategy;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.Socket;
import java.util.*;

/*
 * 
 * Title: Star Wars X-Wing vs TIE-Fighter
 * 
 * How to Run(Client):
 * >java projectSWClient [Server IP] [Server port]
 * 
 * How to play:
 * Move with MOUSE, Press SPACEBAR to fire a laser.
 * 
 * This client side player will play the TIE-Fighter (upperside)
 * Simple goal - Shoot enemy player with laser to point a score.
 * 
 * Programmed to continuous run until player choose to exit.
 * 
 * 
 */

public class projectSWClient implements Runnable{


	private String title;
	private int width;
	private int height;
	private Thread  thread;
	private boolean running;
	private BufferStrategy buffer;
	private Graphics g;

	private Client_Player player_client; 
	private Client_enemyPlayer player_server;
	public static ArrayList<Client_Laser> laser_client;
	public static ArrayList<Client_enemyLaser> laser_server;

	private long current,delay,now;
	private int score;
	private boolean game_start=true;

	private Client_Window game_window;
	// Game Window Size
	public static final int gameWidth = 800;
	public static final int gameHeight = 1000;

	private int player_server_change_x;
	private int player_server_change_y;
	private int player_server_shoot;
	private int player_server_score;

	static BufferedReader inFromServer;
	static DataOutputStream outToServer;

	public projectSWClient(String title,int width,int height){
		this.title = title;
		this.width = width;
		this.height = height;
	} // Create Game window with given size

	public void initialize(){  

		game_window = new Client_Window(title,width,height); // Display Game Window
		Client_Image.initialize(); // Load Object Images

		player_client = new Client_Player((gameWidth/2),(120));
		player_client.initialize();

		player_server = new Client_enemyPlayer((gameWidth/2),(gameHeight-300));
		player_server.initialize();

		laser_client = new ArrayList<Client_Laser>(); 
		laser_server = new ArrayList<Client_enemyLaser>();
		current = System.nanoTime();
		now = System.nanoTime();
		delay = 1200;
		score = 0;
	}

	public synchronized void start(){
		running = true; // start the thread, run function call
		thread = new Thread(this);
		thread.start();
	}

	public synchronized void stop(){
		if(!(running))
			return;
		running = false; // stop run
		try {
			thread.join(); // stop thread
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	// Function for sending and receiving packet(game data) with another player.
	public void updateFrame(){

		// Update player position and laser shot(game data) every frame
		int player_client_X = player_client.getX();
		int player_client_Y = player_client.getY();
		int player_client_Fire = player_client.getFire();

		// Four types of data are used
		try{
			// send the game data to server
			outToServer.writeBytes(String.valueOf(score)+'\n');// enemy player score
			outToServer.writeBytes(String.valueOf(player_client_X)+'\n');// enemy player score
			outToServer.writeBytes(String.valueOf(player_client_Y)+'\n');// enemy player y
			outToServer.writeBytes(String.valueOf(player_client_Fire)+'\n');// enemy player fire
		}
		catch(Exception e){
		}

		try{
			// get the game data from server
			player_server_score=Integer.parseInt(inFromServer.readLine());//server player score
			player_server_change_x=Integer.parseInt(inFromServer.readLine());//server player x
			player_server_change_y=Integer.parseInt(inFromServer.readLine());//server player y
			player_server_shoot=Integer.parseInt(inFromServer.readLine());//server player fire
		}
		catch(Exception e){
		}


		// Client player frame
		player_client.updateFrame();
		// Server player frame
		player_server.updateFrame(player_server_change_x, player_server_change_y,player_server_shoot); 

		//Create Laser
		for(int i = 0; i<laser_client.size();i++){
			laser_client.get(i).updateFrame();
		}

		//Create Opponent player Laser
		for(int i = 0; i<laser_server.size();i++){
			laser_server.get(i).updateFrame();


		}

	}

	// Graphic Rendering Function
	public void render(Graphics  g){

		// Displaying the frame canvas
		buffer = game_window.getCanvas().getBufferStrategy();
		if(buffer == null){
			game_window.getCanvas().createBufferStrategy(3);
			return;
		}

		// Draw Game Screen
		g = buffer.getDrawGraphics();
		g.clearRect(0, 0, width, height);

		// Draw Screen Objects
		g.drawImage(Client_Image.background,0,0, gameWidth, gameHeight,null);

		if(game_start){
			player_client.render(g);

			if(game_start)
			{
				player_server.render(g);
				// Draw Laser
				for(int  i=0;i<laser_server.size(); i++){
					laser_server.get(i).render(g);
				}

				// Remove Laser that's out of screen
				for(int i = 0;i< laser_server.size();i++){
					if(laser_server.get(i).getY()<=0){
						laser_server.remove(i);
						i--;

					} 
				}
			}

			// Draw Laser
			for(int  i=0;i<laser_client.size(); i++){
				laser_client.get(i).render(g);
			}

			// Remove Laser that's out of screen
			for(int i = 0;i< laser_client.size();i++){
				if(laser_client.get(i).getY()>=gameHeight){
					laser_client.remove(i);
					i--;

				} 
			}

			// Apply player co-ordinates
			int player_client_X = player_client.getX();
			int player_client_Y = player_client.getY();

			// Apply enemy player co-ordinates
			int player_server_X = player_server.getX();
			int player_server_Y = player_server.getY();

			// Enemy player shot by laser. Add score and also remove the laser
			for(int m = 0; m < laser_client.size();m++)
			{
				int laser_client_X = laser_client.get(m).getX();
				int laser_client_Y = laser_client.get(m).getY(); 


				if( player_server_X < laser_client_X && player_server_X + 160 > laser_client_X &&
						player_server_Y < laser_client_Y && player_server_Y + 140 > laser_client_Y ){
					laser_client.remove(m);
					m--;
					score++;
					player_server.renderBlast(g); // Show blast image.
				}
			}

			// Player shot by laser. Add score and also remove the laser
			for(int n = 0; n < laser_server.size();n++)
			{ 
				int laser_server_X = laser_server.get(n).getX();
				int laser_server_Y = laser_server.get(n).getY();

				if( player_client_X - 10 < laser_server_X && player_client_X + 190 > laser_server_X &&
						player_client_Y < laser_server_Y && player_client_Y + 150 > laser_server_Y ){
					laser_server.remove(n);
					n--;
					player_server_score++;					
					player_client.renderBlast(g); // Show blast image.				

				}
			}

			// display each players scores
			g.setColor(Color.YELLOW);
			g.setFont(new Font("arial",Font.BOLD, 22));
			g.drawString("YOUR SCORE : "+score, 40,40);
			g.drawString("ENEMY SCORE : "+player_server_score, 40, gameHeight-40);
			g.drawString("X-Wing", 600,gameHeight-40);
			g.drawString("TIE-Fighter", 600,40);

		}

		buffer.show();
		g.dispose();

	}

	public void run() {
		initialize();
		int fps = 50; // Variable for adjust 1 frame per second.
		// this also decides packet transmission rates.
		// 50 fps is enough for this game.
		
		// Time delay before update next frame.
		// 1000000000 represents 1 second because it's using System.nanoTime.
		double timePerTick = 1000000000/fps;
		double delta = 0; 
		long current = System.nanoTime();

		while(running){
			// Frame rendering Frequency using delta value
			delta = delta + (System.nanoTime()-current)/timePerTick; 			
			current = System.nanoTime();

			// don't update frame until delta reaches 1.
			if(delta>=1){
				updateFrame(); // Update frame
				render(g); // Render the graphic.
				delta--; // Reset the delta value after the frame.
			}
		}
	}

	public static void main(String[] args) {

		String arg_IP = args[0];
		int arg_Port = Integer.parseInt(args[1]);

		try{
			// Input IP and Port as arguments
			InetAddress ip = InetAddress.getByName(arg_IP);
			Socket n = new Socket(ip, arg_Port); // Creating a socket at ip and port from argument

			// Input from the server player
			inFromServer = new BufferedReader(new InputStreamReader(n.getInputStream()));
			// Output to the server player
			outToServer = new DataOutputStream(n.getOutputStream()); 
		}
		catch(Exception e){
			System.out.println("Connection problem!");
		}

		projectSWClient game = new projectSWClient("Project Star Wars Client",gameWidth,gameHeight);  
		game.start();

	}

}
