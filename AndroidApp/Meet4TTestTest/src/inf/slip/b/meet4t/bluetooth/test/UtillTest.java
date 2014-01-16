package inf.slip.b.meet4t.bluetooth.test;

import inf.slip.b.meet4t.bluetooth.Util;
import junit.framework.TestCase;

public class UtillTest extends TestCase {

	public void testByteToString() {
		byte[] array = "aa".getBytes();
		assertEquals(Util.bytArrayToHex(array), "AA", Util.bytArrayToHex(array));
	}
}
