import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.URI;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import com.sun.net.httpserver.Headers;
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
	
	public void LoopForever()
	{
		// isn't recursion just awesome.
		LoopForever();
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
		
		public void handle(HttpExchange t) throws IOException 
		{
			// Steps required
			// 1 - Get the details of the Http packet (body, uri, 
			// 		header, method)
			// 2 - Post the reply depending on the information in the 
			// 		received packet
			
			
			// calling this method closes the input stream and 
			// thus if you want to analyze the body, don't call this
			//printPacketDetails(t);
			
			analyzePacket(t);
		}

		
	/*
	 ====================================================
	 PRIVATE HELPERS
	 ====================================================
	 */
		
		private static void analyzePacket(HttpExchange t) throws IOException
		{
			if (!t.getRequestMethod().equals( "POST" )) {
				sendGenericResponse(t);
				return;
			}
			
			String packetBody = IoUtils.getStringFromInputStream(t.getRequestBody()); 
			System.out.println(packetBody);
			if (!DB.IsKettleId(packetBody))
			{
				System.out.println("Not a kettle id.");
				sendGenericResponse(t);
				return;
			}
			sendCupIds(t);
		}
		
		private static void sendCupIds(HttpExchange t) throws IOException
		{
			List<String> cupIds = DB.GetAllCupIds();
			
			// TODO do something smarter.
			if (cupIds.size()==0)
				return; 
			
			String response = cupIds.get(0);
			for (int i = 1; i < cupIds.size(); i++) {
				response = response + ", " + cupIds.get(i);
			}
			
			t.sendResponseHeaders(200, response.length());
			OutputStream os = t.getResponseBody();
			os.write(response.getBytes());
			os.close();
		}
		
		
		// stupid throws syntax
		private static void sendGenericResponse(HttpExchange t) throws IOException
		{
			System.out.println("Sending generic response.");
			String response = "This is a generic response.";
			t.sendResponseHeaders(200, response.length());
			OutputStream os = t.getResponseBody();
			os.write(response.getBytes());
			os.close();
		}
		
		// for debugging purposes
		private static void printPacketDetails(HttpExchange t) 
		{
			printPacketHeaders(t);
			printPacketBody(t);
			printPacketMethod(t);
			printPacketURI(t);
			
		}

		private static void printPacketHeaders(HttpExchange t)
		{
			System.out.println("----------PACKET HEADER----------");
			Headers h = t.getRequestHeaders();
			Set<String> keys = h.keySet();
			for (Iterator<String> iter = keys.iterator(); iter.hasNext();)
			{
				String headerElement = iter.next();

				System.out.print(headerElement + ": ");
				List<String> line = h.get(headerElement);
				for (int i = 0 ; i < line.size(); i++) {
					System.out.print(line.get(i) + ", ");
				}
				System.out.println(".");
			}
		}

		private static void printPacketBody(HttpExchange t)
		{
			System.out.println("----------PACKET BODY----------");
			InputStream body = t.getRequestBody();
			try 
			{
				IoUtils.copy(body, System.out);
				System.out.println("");
				body.close();
			} catch (Exception e)
			{
				return;
			}
		}

		private static void printPacketURI(HttpExchange t)
		{
			System.out.println("----------URI----------");
			URI packetUri = t.getRequestURI();
			System.out.println(packetUri.toString());
		}

		private static void printPacketMethod(HttpExchange t)
		{
			System.out.println("----------PACKET METHOD----------");
			String method = t.getRequestMethod();
			System.out.println(method);
		}
		
		

	}

}
