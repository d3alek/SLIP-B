
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Assert;
import org.junit.Test;

public class AllTests {
	@SuppressWarnings("unused")
	private static Server _server;
	
    private static final String serverURL = "http://54.201.94.129:8080/test";
	
	@BeforeClass
	public static void setUp() throws Exception 
	{
		try 
		{
		_server = new Server();
		} 
		catch (IOException e) {
			Assert.fail();
		}
		
	}

	@AfterClass
	public static void tearDown() throws Exception 
	{
		// nothing to tear-down
	}
	
	@Test
	public void sendCups() throws Exception {
		// Getting all cups
		String url = serverURL;
 
		URL obj = new URL(url);
		HttpURLConnection con = (HttpURLConnection) obj.openConnection();
	
		con.setDoOutput(true);
		// optional default is GET
		con.setRequestMethod("POST");
		
		//add request header
		con.setRequestProperty("User-Agent", "Mozilla/5.0");
 
		// body
		OutputStreamWriter writer = new OutputStreamWriter(con.getOutputStream());
		writer.write("485322d4e0f44c3d cups");
		writer.close();
 
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		String answer = "f1b15f3fa8ac20dd, fb743c997802e631, 69839aac6e58a5a1, 3433085d22f83506, 62304f49124a3fdf, 36c2ec472b92679e, 3a2fe9250288e39b, 36a5c5d5cc28739a, 40605342121cbcde, 4f4710d9fd4673ee, a2a7b4dd06bfe1b8, 2df30ba1413c78b1, 347a2a8b1f697a7f, d163bbdd530ec035, 99c61bcc3229a470";
		String responseString = response.toString();
		
		Assert.assertTrue(answer.equals(responseString));
	}
	
	@Test
	public void sendNames() throws Exception {
		// Getting all names
		String url = serverURL;
 
		URL obj = new URL(url);
		HttpURLConnection con = (HttpURLConnection) obj.openConnection();
	
		con.setDoOutput(true);
		// optional default is GET
		con.setRequestMethod("POST");
		
		//add request header
		con.setRequestProperty("User-Agent", "Mozilla/5.0");
 
		// body
		OutputStreamWriter writer = new OutputStreamWriter(con.getOutputStream());
		writer.write("485322d4e0f44c3d names");
		writer.close();
		
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		String responseString = response.toString();
		String answer = "Haralampii, Rhuaridh, Markus, Angus, Georgi, Strahil, Morag, Catalina, Ghita, Franz, Ewan, Radu, Fritz, Alek, Gigel";
		
		Assert.assertTrue(answer.equals(responseString));
	}
	
	@Test
	public void sendName() throws Exception {
		// Getting single name
		String url = serverURL;
 
		URL obj = new URL(url);
		HttpURLConnection con = (HttpURLConnection) obj.openConnection();
	
		con.setDoOutput(true);
		// optional default is GET
		con.setRequestMethod("POST");
		
		//add request header
		con.setRequestProperty("User-Agent", "Mozilla/5.0");
 
		// body
		OutputStreamWriter writer = new OutputStreamWriter(con.getOutputStream());
		writer.write("485322d4e0f44c3d name d163bbdd530ec035");
		writer.close();
		
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		String responseString = response.toString();
		String answer = "Alek";
		
		Assert.assertTrue(answer.equals(responseString));
	}
	
	@Test
	public void sendNumbers() throws Exception {
		// Getting all numbers
		
		String url = serverURL;
 
		URL obj = new URL(url);
		HttpURLConnection con = (HttpURLConnection) obj.openConnection();
	
		con.setDoOutput(true);
		// optional default is GET
		con.setRequestMethod("POST");
		
		//add request header
		con.setRequestProperty("User-Agent", "Mozilla/5.0");
 
		// body
		OutputStreamWriter writer = new OutputStreamWriter(con.getOutputStream());
		writer.write("485322d4e0f44c3d numbers");
		writer.close();
		 
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		String responseString = response.toString();
		String answer = "07812854785, 07164789452, 07853868475, 07246791234, 07164831325, 07156984578, 07965555555, 07787654321, 07246571235, 07446792584, 07984613457, 07877777777, 07564973158, 07812345678, 07675558966";
		
		Assert.assertTrue(responseString.equals(answer));
 
	}
	
	@Test
	public void sendNumber() throws Exception {
		// Getting single number
		
		String url = serverURL;
 
		URL obj = new URL(url);
		HttpURLConnection con = (HttpURLConnection) obj.openConnection();
	
		con.setDoOutput(true);
		// optional default is GET
		con.setRequestMethod("POST");
		
		//add request header
		con.setRequestProperty("User-Agent", "Mozilla/5.0");
 
		// body
		OutputStreamWriter writer = new OutputStreamWriter(con.getOutputStream());
		writer.write("485322d4e0f44c3d number 36a5c5d5cc28739a");
		writer.close();
 
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		String responseString = response.toString();
		String answer = "07787654321";
		
		Assert.assertTrue(responseString.equals(answer));		
	}

}
