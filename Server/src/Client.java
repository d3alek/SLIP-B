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
	
	// runner method
	public static void main(String[] args) {
		
//		try {
//
//            doStuff();
//            
//        } catch (UnknownHostException e) {
//            System.err.println("Don't know about host: localhost.");
//            System.err.println(e);
//            System.exit(1);
//        } catch (IOException e) {
//            System.err.println("Couldn't get I/O for the connection to: taranis.");
//            System.err.println(e);
//            System.exit(1);
//        }
//        
        try {
        	
        	sendGet();
        	
        } catch (Exception e) {
        	System.out.println("Exception: " + e);
        }

        System.out.println("Finito.");
	}
	
	
	private static void doStuff() throws UnknownHostException, IOException 
	{
		socket = new Socket("localhost", 8000);
		System.out.println("Socket obtained.");
        out = new PrintWriter(socket.getOutputStream(), true);
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        
        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
        String fromServer;
        String fromUser;

        while ((fromServer = in.readLine()) != null) {
            System.out.println("Server: " + fromServer);
            if (fromServer.equals("Bye."))
                break;

            fromUser = stdIn.readLine();
            if (fromUser != null) {
                System.out.println("Client: " + fromUser);
                out.println(fromUser);
            }
        }

        out.close();
        in.close();
        stdIn.close();
        socket.close();
	}
	
	// HTTP GET request
	private static void sendGet() throws Exception {
 
		String url = "http://129.215.58.114:8080/test";
 
		URL obj = new URL(url);
		HttpURLConnection con = (HttpURLConnection) obj.openConnection();
	
		con.setDoOutput(true);
		// optional default is GET
		con.setRequestMethod("POST");
		
		//add request header
		con.setRequestProperty("User-Agent", "Mozilla/5.0");
 
		// body
		OutputStreamWriter writer = new OutputStreamWriter(con.getOutputStream());
		writer.write("kettle1");
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
