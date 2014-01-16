package inf.slip.b.meet4t.organizemeeting.test;

import inf.slip.b.meet4t.organizemeeting.PeopleListItem;
import junit.framework.TestCase;

public class PeopleListItemTest extends TestCase {

	private PeopleListItem item;

	protected void setUp() throws Exception {
		super.setUp();
		item = new PeopleListItem("Sam", "mug13", true);
	}

	protected void tearDown() throws Exception {
		super.tearDown();
		item = null;
	}

	public void testPeopleListItem() {
		try {
			new PeopleListItem(null, null, true);
			new PeopleListItem("Anne", null, false);
			new PeopleListItem(null, "mug45", false);
			new PeopleListItem("Anne", "mug45", true);
		} catch (Exception e) {
			fail("Constructor failed");
		}
	}

	public void testGetName() {
		assertEquals("Sam", item.getName());
	}

	public void testSetName() {
		item.setName("Jim");
		assertEquals("Jim", item.getName());
	}

	public void testGetMugID() {
		assertEquals("mug13", item.getMugID());
	}

	public void testIsSelected() {
		assertEquals(true, item.isSelected());
	}

	public void testSetSelected() {
		item.setSelected(false);
		assertEquals(false, item.isSelected());
	}

}
