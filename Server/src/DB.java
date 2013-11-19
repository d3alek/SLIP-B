import java.util.ArrayList;
import java.util.Arrays;
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
		String idSubString = id.substring(0, id.indexOf(' '));
		
		// loop until kettle id is found.
		for (int i = 0; i < _kettleIds.size();i++) {
			String kettleId = _kettleIds.get(i);
			String kettleId2 = kettleId + " ";
			if (id.equals(kettleId) 
					|| id.equals(kettleId2)
					|| idSubString.equals(kettleId) 
					|| idSubString.equals(kettleId2))
			{
				return true;
			}
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
		
		List<String> contactsF = FilterString(contacts);
		List<String> numbers = new ArrayList<String>();
		
		for(int i = 0; i < contactsF.size(); i++) {
			String contact = contactsF.get(i);
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
		
		return GetNumbersByContacts(GetNamesByCupIds(FilterString(cupIds)));
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
		
		List<String> cupIdsF = FilterString(cupIds);
		List<String> names = new ArrayList<String>();
		
		for(int i = 0; i < cupIdsF.size(); i++) {
			String cupId = cupIdsF.get(i);
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
	
	private static List<String> FilterString(List<String> strings)
	{
		for (int i = 0; i < strings.size(); i++ ) {
			String s = strings.get(i);
			s.replaceAll("[^a-zA-Z0-9]","");
			strings.set(i, s);
		}
		return strings;
	}
	
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
		_cupIdToContact.put("m255", "alek");
		_cupIdToContact.put("m015", "catalina");
		_cupIdToContact.put("m018", "ewan");
		_cupIdToContact.put("m043", "georgi");
		_cupIdToContact.put("m077", "markus");
		
		_contactToNumber.put("alek", "07777777777");
		_contactToNumber.put("catalina", "07777777777");
		_contactToNumber.put("ewan", "07777777777");
		_contactToNumber.put("georgi", "07777777777");
		_contactToNumber.put("markus", "07777777777");
	}
}
