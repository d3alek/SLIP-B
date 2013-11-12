import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.Socket;
import java.net.URL;
import java.net.UnknownHostException;


public class Client {
	private static Socket socket;
	private static PrintWriter out;
    private static BufferedReader in;
    private static final String serverURL = "http://129.215.59.128:8080/test";
	
	// runner method
	public static void main(String[] args) {
	
        try {
        	
        	if (args[0].equals("1"))
        		sendCups();
        	else if (args[0].equals("2"))
        		sendNames();
        	else if (args[0].equals("3"))
    			sendName();
        	else if (args[0].equals("4"))
				sendNumbers();
        	else if (args[0].equals("5"))
				sendNumber();
			else 
				System.out.println("Program use: java Client [1-5]");
        				
        	
        } catch (Exception e) {
        	System.out.println("Exception: " + e);
        }

        System.out.println("Finito.");
	}
	
	// HTTP GET request
	private static void sendCups() throws Exception {
		System.out.println("Getting all cups");
		
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
		writer.write("kettle1 cups");
		writer.close();
		
		int responseCode = con.getResponseCode();
		System.out.println("\nSending 'POST' request to URL : " + url);
		System.out.println("With body: kettle1");
		System.out.println("Response Code : " + responseCode);
 
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		//print result
		System.out.println(response.toString());
 
	}
	
	// HTTP GET request
	private static void sendNames() throws Exception {
		System.out.println("Getting all names");
		
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
		writer.write("kettle1 names");
		writer.close();
		
		int responseCode = con.getResponseCode();
		System.out.println("\nSending 'POST' request to URL : " + url);
		System.out.println("With body: kettle1");
		System.out.println("Response Code : " + responseCode);
 
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		//print result
		System.out.println(response.toString());
 
	}
	
	// HTTP GET request
	private static void sendName() throws Exception {
		System.out.println("Getting single name");
		
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
		writer.write("kettle1 name m077");
		writer.close();
		
		int responseCode = con.getResponseCode();
		System.out.println("\nSending 'POST' request to URL : " + url);
		System.out.println("With body: kettle1");
		System.out.println("Response Code : " + responseCode);
 
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		//print result
		System.out.println(response.toString());
 
	}
	
	// HTTP GET request
	private static void sendNumbers() throws Exception {
		System.out.println("Getting all numbers");
		
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
		writer.write("kettle1 numbers");
		writer.close();
		
		int responseCode = con.getResponseCode();
		System.out.println("\nSending 'POST' request to URL : " + url);
		System.out.println("With body: kettle1");
		System.out.println("Response Code : " + responseCode);
 
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		//print result
		System.out.println(response.toString());
 
	}
	
	// HTTP GET request
	private static void sendNumber() throws Exception {
		System.out.println("Getting single number");
		
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
		writer.write("kettle1 number m015");
		writer.close();
		
		int responseCode = con.getResponseCode();
		System.out.println("\nSending 'POST' request to URL : " + url);
		System.out.println("With body: kettle1");
		System.out.println("Response Code : " + responseCode);
 
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(con.getInputStream()));
		String inputLine;
		StringBuffer response = new StringBuffer();
 
		while ((inputLine = in.readLine()) != null) {
			response.append(inputLine);
		}
		in.close();
 
		//print result
		System.out.println(response.toString());
 
	}
	
}
