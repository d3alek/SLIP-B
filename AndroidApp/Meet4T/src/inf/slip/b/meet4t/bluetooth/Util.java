package inf.slip.b.meet4t.bluetooth;

public class Util {

	/**
	 * Auxiliary method that turns an array of bytes into a string.
	 */
    public static String bytArrayToHex(byte[] array) {
    	StringBuilder sb = new StringBuilder();
    	for (byte b : array) {
    		sb.append(String.format("%02x", b&0xff));
    	}
    	return sb.toString();
    }

}
