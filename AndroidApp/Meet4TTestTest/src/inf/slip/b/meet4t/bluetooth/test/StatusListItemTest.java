package inf.slip.b.meet4t.bluetooth.test;

import inf.slip.b.meet4t.bluetooth.StatusListItem;
import inf.slip.b.meet4t.bluetooth.StatusListItem.MugStatus;
import junit.framework.TestCase;

public class StatusListItemTest extends TestCase {
	
	private StatusListItem item;

	protected void setUp() throws Exception {
		super.setUp();
		item = new StatusListItem("Ana", "mug15", MugStatus.WAITING_REPLY);
	}

	protected void tearDown() throws Exception {
		super.tearDown();
		item = null;
	}

	public void testStatusListItem() {
		try {
			new StatusListItem(null, null, null);
			new StatusListItem("Ben", "mug73", MugStatus.ACCEPTED);
		} catch (Exception e) {
			fail("Not yet implemented");
		}
	}

	public void testGetName() {
		assertEquals("Ana", item.getName());
	}

	public void testSetName() {
		item.setName("Charlie");
		assertEquals("Charlie", item.getName());
	}

	public void testGetMugID() {
		assertEquals("mug15", item.getMugID());
	}

	public void testGetMugStatus() {
		assertEquals(MugStatus.WAITING_REPLY, item.getMugStatus());
	}

	public void testSetMugStatus() {
		item.setMugStatus(MugStatus.DECLINED);
		assertEquals(MugStatus.DECLINED, item.getMugStatus());
	}

}
