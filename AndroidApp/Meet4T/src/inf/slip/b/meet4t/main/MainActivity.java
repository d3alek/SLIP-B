package inf.slip.b.meet4t.main;

import inf.slip.b.meet4t.R;
import inf.slip.b.meet4t.bluetooth.BluetoothLeService;
import inf.slip.b.meet4t.bluetooth.DeviceScanActivity;
import inf.slip.b.meet4t.bump.BumpFileActivity;
import inf.slip.b.meet4t.organizemeeting.InvitePeopleActivity;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.StrictMode;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity {

	private static final int REQUEST_ENABLE_BT = 2; // random # >0
	private static final int REQUEST_SET_OWN_MUG = 1;
	private BluetoothAdapter mBluetoothAdapter;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitNetwork().build();
		StrictMode.setThreadPolicy(policy);
		setContentView(R.layout.activity_main);
		final Button enableButton = (Button) findViewById(R.id.enable_button);
		enableButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	enableBLE();
        		}
        });
		final Button disableButton = (Button) findViewById(R.id.disable_button);
		disableButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	disableBLE();
        		}
        });
		if (BLEenabled()) {
			enableButton.setVisibility(View.INVISIBLE);
			disableButton.setVisibility(View.VISIBLE);
		} else {
			enableButton.setVisibility(View.VISIBLE);
			disableButton.setVisibility(View.INVISIBLE);
		}
		final Button scanButton = (Button) findViewById(R.id.search_devices_button);
		scanButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	scanDevices();
        		}
        });
		final Button invitePeopleButton = (Button) findViewById(R.id.invite_people_button);
		invitePeopleButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	invitePeople();
        		}
        });
		final Button bumpFileButton = (Button) findViewById(R.id.bump_button);
		bumpFileButton.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				Intent i = new Intent(MainActivity.this, BumpFileActivity.class);
				startActivity(i);
			}
		});
		ConnectToOwnMug();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	@Override
	public boolean onPrepareOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		if (BLEenabled()) {
			menu.findItem(R.id.menu_BLE_on).setVisible(true);
			menu.removeItem(R.id.menu_BLE_off);
		} else {
			menu.removeItem(R.id.menu_BLE_on);
			menu.findItem(R.id.menu_BLE_off).setVisible(true);
		}
		menu.findItem(R.id.menu_refresh).setVisible(false);
		menu.findItem(R.id.menu_scan).setVisible(false);
		menu.findItem(R.id.menu_stop).setVisible(false);
		return true;
	}

	@Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_BLE_off:
            	enableBLE();
                break;
            case R.id.menu_BLE_on:
            	disableBLE();
            	invalidateOptionsMenu();
                break;
            case R.id.action_set_own_mug:
            	Intent intent = new Intent(this, DeviceScanActivity.class);
            	startActivityForResult(intent, REQUEST_SET_OWN_MUG);
            	break;
        }
        return true;
    }

	private void scanDevices() {
		Intent intent = new Intent(this, DeviceScanActivity.class);
		startActivity(intent);
	}

	private void enableBLE() {
		// Checking whether BLE is supported on the device. Then
		if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
			bleNotification(R.string.ble_not_supported);
			finish();
		}
		// Initializing Bluetooth adapter.
		final BluetoothManager bluetoothManager =
		        (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
		mBluetoothAdapter = bluetoothManager.getAdapter();
		// Ensures Bluetooth is available on the device and it is enabled. If not,
		// displays a dialog requesting user permission to enable Bluetooth.
		if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
		    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
		    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
		}
	}
	
	private void disableBLE() {
		// Checking whether BLE is supported on the device. Then
		if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
			bleNotification(R.string.ble_not_supported);
			finish();
		}
		// Initializing Bluetooth adapter.
		final BluetoothManager bluetoothManager =
		        (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
		mBluetoothAdapter = bluetoothManager.getAdapter();
		// Ensures Bluetooth is available on the device and it is enabled. If not,
		// displays a dialog requesting user permission to enable Bluetooth.
		if (mBluetoothAdapter.isEnabled()) {
			if  (!mBluetoothAdapter.disable()) {
				bleNotification(R.string.ble_disable_fail);
			}
		}
		findViewById(R.id.enable_button).setVisibility(View.VISIBLE);
		findViewById(R.id.disable_button).setVisibility(View.INVISIBLE);
	}

	private boolean BLEenabled() {
		// Checking whether BLE is supported on the device. Then
		if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
			bleNotification(R.string.ble_not_supported);
			finish();
		}
		// Initializing Bluetooth adapter.
		final BluetoothManager bluetoothManager =
		        (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
		mBluetoothAdapter = bluetoothManager.getAdapter();
		return mBluetoothAdapter.isEnabled();
	}

	@Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data){
    	super.onActivityResult(requestCode, resultCode, data);
    	if (resultCode != RESULT_OK) {
    		return;
    	}
    	if (requestCode == REQUEST_ENABLE_BT) {
    		if (BLEenabled()) {
    			findViewById(R.id.enable_button).setVisibility(View.INVISIBLE);
    			findViewById(R.id.disable_button).setVisibility(View.VISIBLE);
    			invalidateOptionsMenu();
    		}
    	}
    	if (requestCode == REQUEST_SET_OWN_MUG) {
    		SharedPreferences sharedPref = this.getPreferences(Context.MODE_PRIVATE);
    		SharedPreferences.Editor editor = sharedPref.edit();
        	Bundle extras = data.getExtras();
        	if (extras.containsKey(getString(R.string.own_mug))){
        		editor.putString(getString(R.string.own_mug), extras.getString(getString(R.string.own_mug)));
        		editor.commit();
        	}
    	}
	}
	
	private void ConnectToOwnMug() {
		if (! BLEenabled()) {
			return;
		}
		SharedPreferences sharedPref = this.getPreferences(Context.MODE_PRIVATE);
		if (sharedPref.contains(getString(R.string.own_mug))) {
			String ownMug = sharedPref.getString(getString(R.string.own_mug), null);
			if (ownMug != null) {
				new BluetoothLeService().connect(ownMug);
			}
		}
		
	}
	
	private void bleNotification(int message) {
	    Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
	}

	private void invitePeople() {
		Intent intent = new Intent(this, InvitePeopleActivity.class);
		startActivity(intent);
	}
}
