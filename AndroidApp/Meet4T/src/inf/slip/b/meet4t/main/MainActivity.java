package inf.slip.b.meet4t.main;

import inf.slip.b.meet4t.R;
import inf.slip.b.meet4t.bluetooth.DeviceScanActivity;
import inf.slip.b.meet4t.organizemeeting.InvitePeopleActivity;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.StrictMode;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity {

	private static final int REQUEST_ENABLE_BT = 2; // random # >0
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
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
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
	}

	private void bleNotification(int message) {
	    Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
		
	}

	private void invitePeople() {
		Intent intent = new Intent(this, InvitePeopleActivity.class);
		startActivity(intent);
	}
}
