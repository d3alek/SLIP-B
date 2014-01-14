import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;


public class BreakServer {
	public static void main(String[] args)
	{
		// default size that changes if command line arguments are present
		int threadsToCreate = 100;
		if (args.length != 0){
			threadsToCreate = Integer.parseInt(args[0]);
		}
		
		System.out.println("Creating " + threadsToCreate + " threads.");
		
		List<Thread> threads = new ArrayList<Thread>(threadsToCreate);
		
		// push all threads into the list
		for (int i = 0; i < threadsToCreate; i++) {
			threads.add(new Thread(new RequestData()));
		}
		
		// start all threads
		for (int i = 0; i < threads.size(); i++) {
			threads.get(i).start();
		}
		
		System.out.println("Done.");
	}
	
	private static class RequestData implements Runnable 
	{
		@Override
		public void run() 
		{
			try {
				sendCups();
			} catch (Exception e)
			{
				System.out.println("Exception: " + e.toString());
			}
		}
		
		// HTTP GET request
		private static void sendCups() throws Exception {
			
			String url = "http://54.201.94.129:8080/test";
	 
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
		}
		
	}
}
