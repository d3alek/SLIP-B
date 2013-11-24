/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package inf.slip.b.meet4t.bluetooth;

import inf.slip.b.meet4t.R;
import inf.slip.b.meet4t.organizemeeting.InvitePeopleActivity;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.UUID;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ExpandableListView;
import android.widget.SimpleExpandableListAdapter;
import android.widget.TextView;
import android.widget.Toast;

/**
 * For a given BLE device, this Activity provides the user interface to connect, display data,
 * and display GATT services and characteristics supported by the device.  The Activity
 * communicates with {@code BluetoothLeService}, which in turn interacts with the
 * Bluetooth LE API.
 */
public class DeviceControlActivity extends Activity {
	private final static String SERVICE_I_WANT = "00001523-1212-efde-1523-785feabcd123";
	private final static String PENDING_CHARACTERISTIC = "00001526-1212-efde-1523-785feabcd123";
	private final static String AVAILABLE_CHARACTERISTIC = "00001524-1212-efde-1523-785feabcd123";
	private final static String DECLINED_CHARACTERISTIC = "00001525-1212-efde-1523-785feabcd123";
	private final static String TAG = DeviceControlActivity.class.getSimpleName();

	public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
	public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";

	private TextView mConnectionState;
	private TextView mDataField;
	private String mDeviceName;
	private String mDeviceAddress;
	private ExpandableListView mGattServicesList;
	private BluetoothLeService mBluetoothLeService;
	private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =
			new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
	private boolean mConnected = false;
	//    private BluetoothGattCharacteristic mNotifyCharacteristic;

	private final String LIST_NAME = "NAME";
	private final String LIST_UUID = "UUID";

	// Code to manage Service lifecycle.
	private final ServiceConnection mServiceConnection = new ServiceConnection() {

		@Override
		public void onServiceConnected(ComponentName componentName, IBinder service) {
			mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
			if (!mBluetoothLeService.initialize()) {
				Log.e(TAG, "Unable to initialize Bluetooth");
				finish();
			}
			// Automatically connects to the device upon successful start-up initialization.
			mBluetoothLeService.connect(mDeviceAddress);
		}

		@Override
		public void onServiceDisconnected(ComponentName componentName) {
			mBluetoothLeService = null;
		}
	};

	// Handles various events fired by the Service.
	// ACTION_GATT_CONNECTED: connected to a GATT server.
	// ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
	// ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
	// ACTION_DATA_AVAILABLE: received data from the device.  This can be a result of read
	//                        or notification operations.
	private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			final String action = intent.getAction();
			Log.i("Cat", "action: " + action);
			if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
				mConnected = true;
				updateConnectionState(R.string.connected);
				invalidateOptionsMenu();
				displayGattServices(mBluetoothLeService.getSupportedGattServices());
			} else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
				mConnected = false;
				updateConnectionState(R.string.disconnected);
				invalidateOptionsMenu();
				clearUI();
			} else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
				// Show all the supported services and characteristics on the user interface.
				displayGattServices(mBluetoothLeService.getSupportedGattServices());

				inviteNextMug();
			} else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
				Log.i("Cat", "action data available");
				String data = intent.getStringExtra(BluetoothLeService.EXTRA_DATA);
				BluetoothGattCharacteristic changedCharacteristic = mBluetoothLeService.getCharacteristicByUuid(UUID.fromString(data));
				mBluetoothLeService.readCharacteristic(changedCharacteristic);
				Log.i("Cat", "Value is " + changedCharacteristic.getValue());
				Log.i("Cat", data + "'s new value is: " + changedCharacteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT32, 0));
				if (data.equals(PENDING_CHARACTERISTIC)) {
					inviteNextMug();
				}
			} else if (action.equals("WRITE_SUCCESS")) {
				Log.i("Cat", "got write success");
				String data = intent.getStringExtra(BluetoothLeService.EXTRA_DATA);
				if (data.equals(PENDING_CHARACTERISTIC)) {
					inviteNextMug();
				}
			}
		}
	};

	// If a given GATT characteristic is selected, check for supported features.  This sample
	// demonstrates 'Read' and 'Notify' features.  See
	// http://d.android.com/reference/android/bluetooth/BluetoothGatt.html for the complete
	// list of supported characteristic features.
	private final ExpandableListView.OnChildClickListener servicesListClickListner =
			new ExpandableListView.OnChildClickListener() {
		@Override
		public boolean onChildClick(ExpandableListView parent, View v, int groupPosition,
				int childPosition, long id) {
			if (mGattCharacteristics != null) {
				final BluetoothGattCharacteristic characteristic =
						mGattCharacteristics.get(groupPosition).get(childPosition);
				Log.i(TAG, "Clicked characteristic's value is " + characteristic.getValue());
				if ( characteristic.getValue() == null) {
					Log.i(TAG, "Value is null");
				}
				else {
					StringBuilder sb = new StringBuilder();
					for(byte byteChar : characteristic.getValue())
						sb.append(String.format("%02x ", byteChar));
					Log.i(TAG, "Clicked characteristic's value is " + sb.toString());
				}
				inviteNextMug();
				return true;
			}
			return false;
		}
	};

	private void clearUI() {
		mGattServicesList.setAdapter((SimpleExpandableListAdapter) null);
		mDataField.setText(R.string.no_data);
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.gatt_services_characteristics);

		final Intent intent = getIntent();
		mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
		mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);

		// Sets up UI references.
		((TextView) findViewById(R.id.device_address)).setText(mDeviceAddress);
		mGattServicesList = (ExpandableListView) findViewById(R.id.gatt_services_list);
		mGattServicesList.setOnChildClickListener(servicesListClickListner);
		mConnectionState = (TextView) findViewById(R.id.connection_state);
		mDataField = (TextView) findViewById(R.id.data_value);

		getActionBar().setTitle(mDeviceName);
		getActionBar().setDisplayHomeAsUpEnabled(true);
		Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
		bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
		invitePeople();
	}

	@Override
	protected void onResume() {
		super.onResume();
		registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
		if (mBluetoothLeService != null) {
			final boolean result = mBluetoothLeService.connect(mDeviceAddress);
			Log.d(TAG, "Connect request result=" + result);
		}
	}

	@Override
	protected void onPause() {
		lastSent = false;
		super.onPause();
		unregisterReceiver(mGattUpdateReceiver);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		unbindService(mServiceConnection);
		mBluetoothLeService = null;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.gatt_services, menu);
		if (mConnected) {
			menu.findItem(R.id.menu_connect).setVisible(false);
			menu.findItem(R.id.menu_disconnect).setVisible(true);
		} else {
			menu.findItem(R.id.menu_connect).setVisible(true);
			menu.findItem(R.id.menu_disconnect).setVisible(false);
		}
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch(item.getItemId()) {
		case R.id.menu_connect:
			mBluetoothLeService.connect(mDeviceAddress);
			return true;
		case R.id.menu_disconnect:
			mBluetoothLeService.disconnect();
			return true;
		case android.R.id.home:
			onBackPressed();
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	private int requestCode = 1;
	private ArrayList<String> mugQueue;
	private boolean lastSent;

	private void invitePeople(){
		Intent intent = new Intent(this, InvitePeopleActivity.class);
		startActivityForResult(intent, requestCode);
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data){
		super.onActivityResult(requestCode, resultCode, data);
		if (resultCode != RESULT_OK) {
			return;
		}
		Bundle extras = data.getExtras();
		if (extras.containsKey("invitees")){
			String invitees = extras.getString("invitees");
			mugQueue  = parseInviteesList(invitees);
			BluetoothGattService service = getServiceIWant();
			if (service == null) {
				Log.i("Cat", "service is null");
				return;
			};
		} else {
			Toast.makeText(this, "Not inviting anyone?", Toast.LENGTH_LONG).show();
		}
	}

	private ArrayList<String> parseInviteesList(String invitees) {
		invitees = invitees.substring(1, invitees.length() - 1);
		ArrayList<String> result = new ArrayList<String>(Arrays.asList(invitees.split(", ")));
		return result;
	}

	private BluetoothGattService getServiceIWant(){
		return mBluetoothLeService.getServiceByUuid(SERVICE_I_WANT);
	}

	private void updateConnectionState(final int resourceId) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				mConnectionState.setText(resourceId);
			}
		});
	}

	// Demonstrates how to iterate through the supported GATT Services/Characteristics.
	// In this sample, we populate the data structure that is bound to the ExpandableListView
	// on the UI.
	private boolean displayGattServices(List<BluetoothGattService> gattServices) {
		if (gattServices == null) {
			Toast.makeText(this, "gattServices is null", Toast.LENGTH_LONG).show();
			return false;
		}
		//Toast.makeText(this, "gattServices is NOT null, len: " + gattServices.size(), Toast.LENGTH_LONG).show();
		String uuid = null;
		String serviceUuid = null;
		String unknownServiceString = getResources().getString(R.string.unknown_service);
		String unknownCharaString = getResources().getString(R.string.unknown_characteristic);
		ArrayList<HashMap<String, String>> gattServiceData = new ArrayList<HashMap<String, String>>();
		ArrayList<ArrayList<HashMap<String, String>>> gattCharacteristicData
		= new ArrayList<ArrayList<HashMap<String, String>>>();
		mGattCharacteristics = new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
		boolean foundIt = false;

		// Loops through available GATT Services.
		for (BluetoothGattService gattService : gattServices) {
			HashMap<String, String> currentServiceData = new HashMap<String, String>();
			serviceUuid = gattService.getUuid().toString();
			if (serviceUuid.equals(SERVICE_I_WANT)){
				Log.i(TAG, "SERVICE I WANT FOUND");
				foundIt = true;
			}
			currentServiceData.put(
					LIST_NAME, SampleGattAttributes.lookup(serviceUuid, unknownServiceString));
			currentServiceData.put(LIST_UUID, serviceUuid);
			gattServiceData.add(currentServiceData);

			ArrayList<HashMap<String, String>> gattCharacteristicGroupData =
					new ArrayList<HashMap<String, String>>();
			List<BluetoothGattCharacteristic> gattCharacteristics =
					gattService.getCharacteristics();
			ArrayList<BluetoothGattCharacteristic> charas =
					new ArrayList<BluetoothGattCharacteristic>();

			// Loops through available Characteristics.
			for (BluetoothGattCharacteristic gattCharacteristic : gattCharacteristics) {
				Log.i(TAG, "Looping over gattCharacteristics for the service " + uuid);
				charas.add(gattCharacteristic);
				HashMap<String, String> currentCharaData = new HashMap<String, String>();
				uuid = gattCharacteristic.getUuid().toString();
				currentCharaData.put(
						LIST_NAME, SampleGattAttributes.lookup(uuid, unknownCharaString));
				currentCharaData.put(LIST_UUID, uuid);
				gattCharacteristicGroupData.add(currentCharaData);
				//                if (serviceUuid.equals(SERVICE_I_WANT)) {
				if (uuid.equals("00001526-1212-efde-1523-785feabcd123")) {
					mBluetoothLeService.setCharacteristicNotification(gattCharacteristic, true);
				}
			}
			mGattCharacteristics.add(charas);

			gattCharacteristicData.add(gattCharacteristicGroupData);

		}

		SimpleExpandableListAdapter gattServiceAdapter = new SimpleExpandableListAdapter(
				this,
				gattServiceData,
				android.R.layout.simple_expandable_list_item_2,
				new String[] {LIST_NAME, LIST_UUID},
				new int[] { android.R.id.text1, android.R.id.text2 },
				gattCharacteristicData,
				android.R.layout.simple_expandable_list_item_2,
				new String[] {LIST_NAME, LIST_UUID},
				new int[] { android.R.id.text1, android.R.id.text2 }
				);
		mGattServicesList.setAdapter(gattServiceAdapter);
		return foundIt;
	}

	private static IntentFilter makeGattUpdateIntentFilter() {
		final IntentFilter intentFilter = new IntentFilter();
		intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
		intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
		intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
		intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
		intentFilter.addAction("WRITE_SUCCESS");
		return intentFilter;
	}

	/**
	 * Returns the pending characteristic if it exists, and null otherwise.
	 * 
	 * @param services
	 * @return
	 */
	private BluetoothGattCharacteristic getPendingCharacteristic() {
		BluetoothGattService gattService = mBluetoothLeService.getServiceByUuid(SERVICE_I_WANT);
		if (gattService == null) {
			Log.i(TAG, "Can't see gatt service " + SERVICE_I_WANT);
		}
		else {
			Log.d("Cat", "My method found: " + gattService.getUuid());
		}
		if (gattService == null) {
			return null;
		}
		return gattService.getCharacteristic(UUID.fromString(PENDING_CHARACTERISTIC));
	}

	private String getNextMug() {
		if (mugQueue.size() >= 1) {
			return mugQueue.remove(0);
		}
		return null;
	}

	private void inviteNextMug() {
		BluetoothGattCharacteristic c = getPendingCharacteristic();
		if (c == null) {
			Log.i(TAG, "inviteNextMug characteristic is null");
			return;
		} else {
			Log.d("Cat", "Setting characteristic: " + c.getUuid().toString());
		}
		c.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
		String nextMug = getNextMug();
		Log.d("Cat", "NextMug: " + nextMug);
		if(nextMug != null) {
			nextMug = nextMug + "0000000000000000".substring(nextMug.length());
			c.setValue(nextMug);
			Log.d("Cat", "String value before writing:  " + c.getStringValue(0));
			mBluetoothLeService.writeCharacteristic(c);
			Toast.makeText(getApplicationContext(), "" + nextMug, Toast.LENGTH_SHORT).show();
		} else if (!lastSent) {
			Log.i(TAG, "Sending last");
			lastSent = true;
			Toast.makeText(getApplicationContext(), "no mugs to invite", Toast.LENGTH_SHORT).show();
			nextMug = "1111111111111111";
			c.setValue(nextMug);
			mBluetoothLeService.writeCharacteristic(c);
		}
	}

}

