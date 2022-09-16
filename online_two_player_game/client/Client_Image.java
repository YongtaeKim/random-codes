import java.awt.image.BufferedImage;
import java.io.IOException;
import javax.imageio.ImageIO;

public class Client_Image {

	public static BufferedImage background,TIE_Fighter,XWing,Laser_TIE_Fighter,Laser_XWing,blast0;

	public static void initialize()
	{		
		background = imageLoader("SpaceBG.png");
		TIE_Fighter = imageLoader("TIE_Fighter.png");    
		XWing = imageLoader("XWing2.png");
		Laser_TIE_Fighter = imageLoader("Green_Laser.png"); 
		Laser_XWing = imageLoader("Red_Laser.png");
		blast0 = imageLoader("blast0.png");
	}

	public static BufferedImage imageLoader(String path){
		try {
			return 
					ImageIO.read(Client_Image.class.getResource(path));
		} catch (IOException e) {
			System.out.println("Image load failed!");
			e.printStackTrace();
			System.exit(1);
		}
		return null;     
	}

}