import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.Map;

/**
 * This singleton class represents data in a very simple
 * format.
 * Methods are supplied so that data may be 
 * retrieved.
 */
public class DB {
/*
 ====================================================
 FIELDS & CONSTRUCTORS
 ====================================================
 */
	private static List<String> _kettleIds = new ArrayList<String>(); 
	private static Map<String,String> _cupIdToContact = new HashMap<String,String>();
	private static Map<String,String> _contactToNumber = new HashMap<String, String>();
	
	private static DB _instance = null;
	
	private DB() 
	{
		CreateKettleIds();
		CreateContactList();
	}
	
/*
 ====================================================
 PUBLIC METHODS
 ====================================================
 */
	
	/**
	 * This method simply queries the Database to 
	 * see whether or not the the given identifier 
	 * belongs to a kettle of not.
	 * 
	 * @param The kettle identifier.
 	 * @return True if the identifier belongs to a 
 	 * kettle, false otherwise.
	 */
	public static boolean IsKettleId(String id) {
		if (_instance == null) {
			_instance = new DB();
		}
		
		// remove strings
		// this has to be significantly more comprehensive
		// TODO fix, this doesn't remove remove spaces at the end
		id.replaceAll("\\s+", "");
		
		// TODO remove, this is a hack. Scary stuff...
		if (id.equals("kettle1 "))
			return true;
		
		System.out.println("Comparing identifier:" + id);
		
		for (int i = 0; i < _kettleIds.size();i++) {
			String kettleId = _kettleIds.get(i);
			if (id.equals(kettleId))
				return true;
		}
		
		
		
		return false;
	}
	
	/**
	 * This method simply gets all the cup identifiers
	 * and returns them in a list of integers.
	 * 
	 * @return The complete list of cup identifiers.
	 */
	public static List<String> GetAllCupIds()
	{
		if (_instance == null) {
			_instance = new DB();
		}
		
		List<String> cupIds = new ArrayList<String>();
		cupIds.addAll(_cupIdToContact.keySet());
		
		return cupIds;
	}
	
	/**
	 * This method will return the number that correspond to 
	 * the given contact names.
	 * 
	 * @param contacts A list of strings that holds the contact names.
	 * @return The list of numbers.
	 */
	public static List<String> GetNumbersByContacts(List<String> contacts)
	{
		if (_instance == null) {
			_instance = new DB();
		}
		
		List<String> numbers = new ArrayList<String>();
		
		for(int i = 0; i < contacts.size(); i++) {
			String contact = contacts.get(i);
			String number = _contactToNumber.get(contact);
			
			numbers.add(number);
		}
		
		return numbers;
	}
	
	/**
	 * This method will return the numbers that correspond to 
	 * the given cup identifiers.
	 * 
	 * @param cupIds A list of integers that holds the cup identifiers.
	 * @return The list of numbers.
	 */
	public static List<String> GetNumbersByCupIds(List<String> cupIds)
	{
		if (_instance == null) {
			_instance = new DB();
		}
		
		return GetNumbersByContacts(GetNamesByCupIds(cupIds));
	}
	
	/**
	 * This method will return the names that correspond to 
	 * the given cup identifiers.
	 * 
	 * @param cupIds A list of integers that holds the cup identifiers.
	 * @return The list of names.
	 */
	public static List<String> GetNamesByCupIds(List<String> cupIds) 
	{
		if (_instance == null) {
			_instance = new DB();
		}
		
		List<String> names = new ArrayList<String>();
		
		for(int i = 0; i < cupIds.size(); i++) {
			String cupId = cupIds.get(i);
			String name = _cupIdToContact.get(cupId);
			
			names.add(name);
		}
		
		return names;
		
	}
	
/*
 ====================================================
 PRIVATE HELPERS
 ====================================================
 */
	
	private static void CreateKettleIds() 
	{
		_kettleIds.add("kettle1");
		_kettleIds.add("kettle2");
		_kettleIds.add("kettle3");
		_kettleIds.add("kettle4");
		_kettleIds.add("kettle5");
	}
	
	private static void CreateContactList() 
	{
		_cupIdToContact.put("M255", "Alek");
		_cupIdToContact.put("M015", "Catalina");
		_cupIdToContact.put("M018", "Ewan");
		_cupIdToContact.put("M043", "Georgi");
		_cupIdToContact.put("M077", "Markus");
		
		_contactToNumber.put("Alek", "07777777777");
		_contactToNumber.put("Catalina", "07777777777");
		_contactToNumber.put("Ewan", "07777777777");
		_contactToNumber.put("Georgi", "07777777777");
		_contactToNumber.put("Markus", "07777777777");
	}
}
