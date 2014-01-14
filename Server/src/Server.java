import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.util.Arrays;
import java.util.List;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

/**
 * This class runs a server that responds to 
 * HTTP request messages.
 * 
 * To retrieve information from this class, 
 * you need to provide the correct ID that will
 * give you the required privileges.
 */
public class Server 
{

	/**
	 * Main method to start the server.
	 * @param args Initial program arguments
	 * @throws Exception
	 */
	public static void main(String[] args) throws Exception 
	{
		HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
		server.createContext("/test", new MyHandler());
		server.setExecutor(null); // creates a default executor
		server.start();
	}
	
	/**
	 * Constructor that starts a new Server.
	 * Throws IOException when failing to create 
	 * an HTTP server.
	 * 
	 * @throws IOException  
	 */	
	public Server() throws IOException
	{
		HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
		server.createContext("/test", new MyHandler());
		server.setExecutor(null); // creates a default executor
		server.start();
	}

	/**
	 * This static class is responsible for dealing 
	 * with HTTP requests.
	 * It implements the required
	 * --com.sun.net.httpserver.HttpHandler--
	 * interface.
	 */
	private static class MyHandler implements HttpHandler 
	{
		
	/*
	 ====================================================
	 INTERFACE IMPLEMENTATION
	 ====================================================
	 */
		/**
		 * This method handles the HTTP exchange.
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		public void handle(HttpExchange t) throws IOException 
		{
			analyzePacket(t);
		}

		
	/*
	 ====================================================
	 PRIVATE HELPERS
	 ====================================================
	 */
		/**
		 * This method is the top-most method that analyses the
		 * packet first, and then delegates the work to the
		 * responsible methods.
		 * Every single packet will go through this method.
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		private static void analyzePacket(HttpExchange t) throws IOException
		{
			if (!t.getRequestMethod().equals( "POST" )) {
				sendGenericResponse(t);
				return;
			}
			
			// put the stream into a string
			String packetBody = IoUtils.getStringFromInputStream(t.getRequestBody()); 
			packetBody = packetBody.toLowerCase();
			System.out.println(packetBody);
			
			// ensure that we have kettle Id.
			if (!DB.IsKettleId(packetBody))
			{
				System.out.println("Not a kettle id.");
				sendGenericResponse(t);
				return;
			}
			
			String message = getMessageRemainder(packetBody);
			String firstWord = getFirstWordFromPacket(message);
			
			if (firstWord.equals("cups")) {
				sendCupIds(t);
			} else if (firstWord.equals("numbers")) {
				sendNumbers(t);
			} else if (firstWord.equals("number")) {
				sendNumber(t, message);
			} else if (firstWord.equals("names")) {
				sendNames(t);
			} else if (firstWord.equals("name")) {
				sendName(t,message);
			} else {
				sendCupIds(t);
			}
				
		}
		
		/**
		 * This method sends all the cup identifiers as a response.
		 * 
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		private static void sendCupIds(HttpExchange t) throws IOException
		{
			List<String> cupIds = DB.GetAllCupIds();
			
			// create the message using all the strings within the list
			String response = cupIds.get(0);
			for (int i = 1; i < cupIds.size(); i++) {
				response = response + ", " + cupIds.get(i);
			}
			
			// send the response
			t.sendResponseHeaders(200, response.length());
			OutputStream os = t.getResponseBody();
			os.write(response.getBytes());
			os.close();
			
			// TODO: remove. For debugging
			System.out.println("Message: " + response);
		}
		
		/**
		 * This method replies to the HTTP exchange by 
		 * sending the numbers of all users in the data base.
		 * 
		 * CONSIDER REMOVING: too much data exposure
		 * 
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		private static void sendNumbers(HttpExchange t) throws IOException
		{
			List<String> numbers = DB.GetNumbersByCupIds(DB.GetAllCupIds());
			
			// create the message using all the strings within the list
			String response = numbers.get(0);
			for (int i = 1; i < numbers.size(); i++) {
				response = response + ", " + numbers.get(i);
			}
			
			// send the response
			t.sendResponseHeaders(200, response.length());
			OutputStream os = t.getResponseBody();
			os.write(response.getBytes());
			os.close();
			
			// TODO: remove. For debugging
			System.out.println("Message: " + response);
		}
		
		/**
		 * This method replies to the HTTP exchange by 
		 * sending the numbers of the users that are 
		 * identified by the given cup identifiers.
		 * 
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		private static void sendNumber(HttpExchange t, String message) throws IOException
		{
			// error checking : first message word must be "number"
			if (message.length() == 0 || !message.substring(0, message.indexOf(' ')).equals("number"))
			{
				sendGenericResponse(t);
				return;
			}
			
			// ensure that the Ids are correctly structured
			String cupIdsString = message.substring(message.indexOf(' ')+1);
			String[] cupIds = cupIdsString.split(" ");
			
			List<String> numbers = DB.GetNumbersByCupIds(Arrays.asList(cupIds));
			if (numbers.size() == 0) {
				sendGenericResponse(t);
				return;
			}
			
			// create the message using all the strings within the list
			String response = numbers.get(0);
			for (int i = 1; i < numbers.size(); i++) {
				response = response + ", " + numbers.get(i);
			}
			
			// send the response
			t.sendResponseHeaders(200, response.length());
			OutputStream os = t.getResponseBody();
			os.write(response.getBytes());
			os.close();
			
			// TODO: remove. For debugging
			System.out.println("Message: " + response);
		}
		
		/**
		 * This method replies to the HTTP exchange by 
		 * sending the names of all users in the data base.
		 * 
		 * CONSIDER REMOVING: too much exposure
		 * 
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		private static void sendNames(HttpExchange t) throws IOException
		{
			List<String> names = DB.GetNamesByCupIds(DB.GetAllCupIds());
			
			// create the message using all the strings within the list
			String curName = names.get(0);
			final StringBuilder upName = new StringBuilder(curName.length());
				upName.append(Character.toUpperCase(curName.charAt(0))).append(curName.substring(1));

			String response = ""+upName;
			for (int i = 1; i < names.size(); i++) {
				String currentName = names.get(i);
				final StringBuilder upperName = new StringBuilder(currentName.length());
				upperName.append(Character.toUpperCase(currentName.charAt(0))).append(currentName.substring(1));
				response = response + ", " + upperName;
			}
			
			// send the response
			t.sendResponseHeaders(200, response.length());
			OutputStream os = t.getResponseBody();
			os.write(response.getBytes());
			os.close();
			
			// TODO: remove. For debugging
			System.out.println("Message: " + response);
		}
		
		/**
		 * This method replies to the HTTP exchange by 
		 * sending the names of the users that are 
		 * identified by the given cup identifiers.
		 * 
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		private static void sendName(HttpExchange t, String message) throws IOException 
		{
			// error checking : first message word must be "name"
			if (message.length() == 0 || !message.substring(0, message.indexOf(' ')).equals("name"))
			{
				sendGenericResponse(t);
				return;
			}
			
			// ensure that the Ids are correctly structured
			String cupIdsString = message.substring(message.indexOf(' ')+1);
			String[] cupIds = cupIdsString.split(" ");
			
			List<String> names = DB.GetNamesByCupIds(Arrays.asList(cupIds));
			if (names.size() == 0) {
				sendGenericResponse(t);
				return;
			}
			
			// create the message using all the strings within the list
			String curName = names.get(0);
			final StringBuilder upName = new StringBuilder(curName.length());
				upName.append(Character.toUpperCase(curName.charAt(0))).append(curName.substring(1));

			String response = "" + upName;
			for (int i = 1; i < names.size(); i++) {
				String currentName = names.get(i);
				final StringBuilder upperName = new StringBuilder(currentName.length());
				upperName.append(Character.toUpperCase(currentName.charAt(0))).append(currentName.substring(1));
				response = response + ", " + upperName;
			}
			
			// send the response
			t.sendResponseHeaders(200, response.length());
			OutputStream os = t.getResponseBody();
			os.write(response.getBytes());
			os.close();
			
			// TODO: remove. For debugging
			System.out.println("Message: " + response);
		}
		
		/**
		 * This method replies to the HTTP exchange by  
		 * sending a generic response.
		 * 
		 * This method will be invoked whenever the 
		 * provided packet of the HTTP exchange is 
		 * not in the correct format and no data is 
		 * allowed to be exchanged. 
		 * 
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		private static void sendGenericResponse(HttpExchange t) throws IOException
		{
			String response = "This is a generic response.";
			t.sendResponseHeaders(200, response.length());
			OutputStream os = t.getResponseBody();
			os.write(response.getBytes());
			os.close();

			// TODO: remove. For debugging
			System.out.println("Message: " + response);
		}
		
		/**
		 * This method gets the first word from a string.
		 * The identifier that splits the string is a space.
		 * 
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		private static String getFirstWordFromPacket(String packet) 
		{
			if (packet.length() == 0)
				return " ";
			
			String firstWord = packet.substring(0, packet.indexOf(' '));
			return firstWord;
		}
		
		/**
		 * This method removes the first word from a string and 
		 * then returns the remaining message.
		 * The identifier that splits the string is a space. 
		 * 
		 * @param t HttpExchange to handle
		 * @throws IOException
		 */
		private static String getMessageRemainder(String packet)
		{
			if (packet.length() == 0)
				return " ";
			
			String message = packet.substring(packet.indexOf(' ') + 1);
			return message;
		}
		
	}
}
