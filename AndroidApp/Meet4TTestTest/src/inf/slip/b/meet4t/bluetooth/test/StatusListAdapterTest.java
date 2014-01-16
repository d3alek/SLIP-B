package inf.slip.b.meet4t.bluetooth.test;

import inf.slip.b.meet4t.bluetooth.StatusListAdapter;
import inf.slip.b.meet4t.bluetooth.StatusListItem;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;
import android.util.Pair;

public class StatusListAdapterTest extends TestCase {
	
	private StatusListAdapter adapter;

	protected void setUp() throws Exception {
		super.setUp();
		ArrayList<Pair<String, String>> people = new ArrayList<Pair<String, String>>();
		people.add(new Pair<String, String>("John Doe", "mug01"));
		people.add(new Pair<String, String>("Ana Smith", "mug56"));
		people.add(new Pair<String, String>("Sam Johnson", "mug78"));
		adapter = new StatusListAdapter(null, people, false);
	}

	protected void tearDown() throws Exception {
		super.tearDown();
		adapter = null;
	}

	public void testStatusListAdapter() {
		ArrayList<Pair<String, String>> people = new ArrayList<Pair<String, String>>();
		try {
			new StatusListAdapter(null, people, true);
		} catch (Exception e) {
			fail("Constructor failed");
		}
		
	}

	public void testGetCount() {
		assertEquals(3, adapter.getCount());
	}

	public void testGetItem() {
		assertEquals("Ana Smith", ((StatusListItem) adapter.getItem(1)).getName());
	}

	public void testGetItemById() {
		assertEquals(null, adapter.getItemById("mug57"));
		assertEquals("Ana Smith", ((StatusListItem) adapter.getItemById("mug56")).getName());
	}

	public void testAddItems() {
		assertEquals(3, adapter.getCount());
		List<Pair<String, String>> morePeople = new ArrayList<Pair<String, String>>();
		morePeople.add(new Pair<String, String>("Mike All", "mug06"));
		adapter.addItems(morePeople);
		assertEquals(4, adapter.getCount());
	}

}
