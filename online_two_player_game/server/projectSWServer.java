
import java.awt.*;
import java.awt.image.BufferStrategy;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.*;

/*
 * 
 * Title: Star Wars X-Wing vs TIE-Fighter
 * 
 * How to Run(Server):
 * >java projectSWServer [port]
 * - Then wait until the Client side connects.
 * 
 * How to play:
 * Move with MOUSE, Press SPACEBAR to fire a laser.
 * 
 * This server side player will play the X-Wing (bottom)
 * Simple goal - Shoot enemy player with laser to point a score.
 * 
 * Programmed to continuous run until player choose to exit.
 * 
 * 
 */

public class projectSWServer implements Runnable{


	private String title;
	private int width;
	private int height;
	private Thread  thread;
	private boolean running;
	private BufferStrategy buffer;
	private Graphics g;

	private Server_Player player_server; 
	private Server_enemyPlayer player_client; 
	public static ArrayList<Server_Laser> laser_server;
	public static ArrayList<Server_enemyLaser> laser_client;

	private long current,delay,now;
	private int score;
	private boolean game_start=true;

	private Server_Window game_window;
	// Game Window Size
	public static final int gameWidth = 800;
	public static final int gameHeight = 1000;

	private int player_client_change_x;
	private int player_client_change_y;
	private int player_client_shoot;
	private int player_client_score;

	static ServerSocket serversocket = null;
	static Socket socket = null;

	static BufferedReader inFromClient;
	static DataOutputStream outToClient;

	public projectSWServer(String title,int width,int height){
		this.title = title;
		this.width = width;
		this.height = height;
	} // Create Game window with given size

	public void initialize(){

		game_window = new Server_Window(title,width,height); // Display Game Window
		Server_Image.initialize(); // Load Object Images

		player_server = new Server_Player((gameWidth/2),(gameHeight-300));
		player_server.initialize();

		player_client = new Server_enemyPlayer((gameWidth/2),(120));
		player_client.initialize();

		laser_server = new ArrayList<Server_Laser>(); 
		laser_client = new ArrayList<Server_enemyLaser>();
		current = System.nanoTime();
		now = System.nanoTime();
		delay = 1200;
		score = 0;
	}

	public synchronized void start(){
		running = true; 
		thread = new Thread(this);
		thread.start();
	}

	public synchronized void stop(){
		if(!(running))
			return;
		running = false;
		try {
			thread.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	// Function for sending and receiving packet(game data) with another player.
	public void updateFrame(){

		// Update player position and laser shot(game data) every frame
		int player_server_X = player_server.getX();
		int player_server_Y = player_server.getY();
		int player_server_Fire = player_server.getFire();

		// Four types of data are used
		try{
			// send the game data to client
			outToClient.writeBytes(String.valueOf(score)+'\n'); // score
			outToClient.writeBytes(String.valueOf(player_server_X)+'\n'); //x position
			outToClient.writeBytes(String.valueOf(player_server_Y)+'\n'); //y position
			outToClient.writeBytes(String.valueOf(player_server_Fire)+'\n'); // fire
		}
		catch(Exception e){            
		}

		try{
			// get the game data from client
			player_client_score=Integer.parseInt(inFromClient.readLine());// enemy player score
			player_client_change_x=Integer.parseInt(inFromClient.readLine());// enemy player score
			player_client_change_y=Integer.parseInt(inFromClient.readLine());// enemy player y
			player_client_shoot=Integer.parseInt(inFromClient.readLine());// enemy player fire
		}
		catch(Exception e){
		}

		// Client player frame
		player_server.updateFrame();
		// Server player frame
		player_client.updateFrame(player_client_change_x, player_client_change_y,player_client_shoot);//update player2

		//Create Laser
		for(int i = 0; i<laser_server.size();i++){
			laser_server.get(i).updateFrame();
		}

		//Create Opponent player Laser
		for(int i = 0; i<laser_client.size();i++){
			laser_client.get(i).updateFrame();


		}

	}

	// Graphic Rendering Function
	public void render(Graphics  g){

		//Display canvas
		buffer = game_window.getCanvas().getBufferStrategy();
		if(buffer == null){
			game_window.getCanvas().createBufferStrategy(3);
			return;
		}

		// Draw Game Screen
		g = buffer.getDrawGraphics();
		g.clearRect(0, 0, width, height);

		//Draw Screen Objects
		g.drawImage(Server_Image.background,0,0, gameWidth, gameHeight,null);


		if(game_start){
			player_server.render(g);

			if(game_start)//client player alive
			{
				player_client.render(g);
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
			}

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

			// Apply player co-ordinates
			int player_server_X = player_server.getX();
			int player_server_Y = player_server.getY();

			// Apply enemy player co-ordinates
			int player_client_X = player_client.getX();
			int player_client_Y = player_client.getY();

			// Enemy player shot by laser. Add score and also remove the laser
			for(int m = 0; m < laser_server.size();m++)
			{
				int laser_server_X = laser_server.get(m).getX();
				int laser_server_Y = laser_server.get(m).getY(); 


				if( player_client_X - 10 < laser_server_X && player_client_X + 190 > laser_server_X &&
						player_client_Y < laser_server_Y && player_client_Y + 150 > laser_server_Y ){
					laser_server.remove(m);
					m--;
					score++;
					player_client.renderBlast(g); // Show blast image.
				}
			}

			// Player shot by laser. Add score and also remove the laser
			for(int n = 0; n < laser_client.size();n++)
			{ 
				int laser_client_X = laser_client.get(n).getX();
				int laser_client_Y = laser_client.get(n).getY();

				if( player_server_X < laser_client_X && player_server_X + 160 > laser_client_X &&
						player_server_Y < laser_client_Y && player_server_Y + 140 > laser_client_Y ){
					laser_client.remove(n);
					n--;
					player_client_score++;
					player_server.renderBlast(g); // Show blast image.

				}
			}

			// display each players scores
			g.setColor(Color.YELLOW);
			g.setFont(new Font("arial",Font.BOLD, 22));
			g.drawString("YOUR SCORE : "+score, 40,gameHeight-40);
			g.drawString("ENEMY SCORE : "+player_client_score, 40,40);
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

		int arg_Port = Integer.parseInt(args[0]);

		try{
			// Input Port as arguments
			serversocket = new ServerSocket(arg_Port);
			socket = serversocket.accept(); // Waits for client.

			// Input from the client player
			inFromClient = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			// Output to the client player
			outToClient = new DataOutputStream(socket.getOutputStream());
		}
		catch(Exception e){
			System.out.println("Connection problem!");
		}

		projectSWServer game = new projectSWServer("Project Star Wars Server",gameWidth,gameHeight);  
		game.start();

	}

}
