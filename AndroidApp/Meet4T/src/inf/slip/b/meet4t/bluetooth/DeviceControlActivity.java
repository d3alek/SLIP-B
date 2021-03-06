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
import inf.slip.b.meet4t.bluetooth.StatusListItem.MugStatus;
import inf.slip.b.meet4t.main.MainActivity;
import inf.slip.b.meet4t.organizemeeting.InvitePeopleActivity;

import java.util.ArrayList;
import java.util.UUID;

import android.app.ListActivity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.util.Pair;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ExpandableListView;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

/**
 * For a given BLE device, this Activity provides the user interface to connect, display data,
 * and display GATT services and characteristics supported by the device.  The Activity
 * communicates with {@code BluetoothLeService}, which in turn interacts with the
 * Bluetooth LE API.
 */
public class DeviceControlActivity extends ListActivity {
	private final static String SERVICE_I_WANT = "00001523-1212-efde-1523-785feabcd123";
	private final static String PENDING_CHARACTERISTIC = "00001526-1212-efde-1523-785feabcd123";
	private final static String AVAILABLE_CHARACTERISTIC = "00001524-1212-efde-1523-785feabcd123";
	private final static String DECLINED_CHARACTERISTIC = "00001525-1212-efde-1523-785feabcd123";
	private final static String TAG = DeviceControlActivity.class.getSimpleName();

    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";
	private static final long RECONNECT_ATTEMPT_INTERVAL = 10000;
	
	private static final String END_OF_MUG_QUEUE = "1111111111111111";
	private static final int REQUEST_INVITEES = 3;
	private static final long RESEND_ATTEMPT_INTERVAL = 1000;
	private static final String DONE_ACKNOWLEDGEMENTS_VALUE = "00 00 00 00 00 00 00 FF ";
	private static final String RECEIVED_INVITATION_VALUE = "00 ";


	private Handler mHandler;
//    private TextView mConnectionState;
    private TextView mDataField;
    private String mDeviceName;
    private String mDeviceAddress;
    private ExpandableListView mGattServicesList;
    private BluetoothLeService mBluetoothLeService;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =
            new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    private boolean mConnected = false;
    private boolean isDemo;
    private String mode = null;

    private StatusListAdapter adapter;

    private boolean waitingConfirmations = false;

    // Code to manage Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            else {
            	Log.i("Cat", "Bluetooth initialized");
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
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                invalidateOptionsMenu();
                tryToReconnect();
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.
                mBluetoothLeService.setCharacteristicNotification(getPendingCharacteristic(), true);
                inviteNextMug();
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                String data = intent.getStringExtra(BluetoothLeService.EXTRA_DATA);
                BluetoothGattCharacteristic changedCharacteristic = mBluetoothLeService.getCharacteristicByUuid(UUID.fromString(data));
                mBluetoothLeService.readCharacteristic(changedCharacteristic);
                if (!data.equals(PENDING_CHARACTERISTIC)) {
                	return ;
                }
                if (waitingConfirmations) {
                	getConfirmation();
                } else {
                	mHandler.postDelayed(new Runnable() {
                		@Override
                		public void run() {
                			inviteNextMug();
                		}
                	}, 5000);
                }
            } else if (action.equals("WRITE_SUCCESS")) {
            	String data = intent.getStringExtra(BluetoothLeService.EXTRA_DATA);
            	if (data.equals(PENDING_CHARACTERISTIC)) {
	                inviteNextMug();
                }
            }
        }
    };

    @Override
    protected void onListItemClick(ListView l, View view, int  position, long id) {
    	StatusListItem item = (StatusListItem) adapter.getItem(position);
    	if (item.getMugStatus() == MugStatus.NOT_YET_INVITED) {
    		item.setMugStatus(MugStatus.ACCEPTED);
    	} else {
    		item.setMugStatus(MugStatus.NOT_YET_INVITED);
    	}
    	adapter.notifyDataSetChanged();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        isDemo = getIntent().hasExtra(MainActivity.EXTRAS_MODE);
        if (isDemo) {
        	mode = getIntent().getExtras().getString(MainActivity.EXTRAS_MODE);
        }
        setContentView(R.layout.activity_control_device);
        if (!isDemo || getString(R.string.demo2).equals(mode)) {
        	getWindow().setBackgroundDrawableResource(R.drawable.light_blue_bg);
        } else {
        	getWindow().setBackgroundDrawableResource(R.drawable.canvas_bg_2);
        }
        adapter = new StatusListAdapter(getLayoutInflater(), getPeople(null),
        		(!isDemo || getString(R.string.demo2).equals(mode)));
        getListView().setAdapter(adapter);
        final Intent intent = getIntent();
        mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);
        mHandler = new Handler();
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
        if (!isDemo || getString(R.string.demo2).equals(mode)) {
        	ImageView v = (ImageView) getWindow().findViewById(R.id.main_bg);
        	v.setImageResource(R.drawable.light_blue_bg);
        } else {
        	getWindow().setBackgroundDrawableResource(R.drawable.canvas_bg_2);
        }
    }

    @Override
    protected void onPause() {
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
	private ArrayList<String> mugQueue;
	private ArrayList<String> mugQueue2;
	private ArrayList<Pair<String,String>> inviteesList;

    private void invitePeople(){
    	Intent intent = new Intent(this, InvitePeopleActivity.class);
    	if (isDemo) {
    		intent.putExtra(MainActivity.EXTRAS_MODE, mode);
    	}
    	startActivityForResult(intent, REQUEST_INVITEES);
    }
 
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data){
    	super.onActivityResult(requestCode, resultCode, data);
    	if (resultCode != RESULT_OK) {
    		return;
    	}
    	try {
    		if (requestCode == REQUEST_INVITEES) {
    			Bundle extras = data.getExtras();
    			if (extras.containsKey("invitees")){
    				String invitees = extras.getString("invitees");
    				inviteesList = getPeople(invitees);
    				mugQueue = getMugs(inviteesList);
    				mugQueue.add(END_OF_MUG_QUEUE);
    				mugQueue2 = getMugs(inviteesList);
    				mugQueue2.add(END_OF_MUG_QUEUE);
    				try {
    					adapter.addItems(inviteesList);
    					adapter.notifyDataSetChanged();
    				} catch (Exception e) {
    					// Do nothing
    				}
    				if (isDemo) {
    					mHandler.postDelayed(new Runnable() {
    						@Override
    						public void run() {
    							confirmListItem(mugQueue.get(0));
    						}
    					}, 3000);
    					mHandler.postDelayed(new Runnable() {
    						@Override
    						public void run() {
    							confirmListItem(mugQueue.get(2));
    						}
    					}, 3500);
    					mHandler.postDelayed(new Runnable() {
    						@Override
    						public void run() {
    							confirmListItem(mugQueue.get(1));
    						}
    					}, 4000);
    					mHandler.postDelayed(new Runnable() {
    						@Override
    						public void run() {
    							adapter.assumeEveryoneElseDeclined();
    						}
    					}, 5000);
    				}
    			} else {
    				// Do nothing
    			}
    		}
    	} catch (Exception e) {
    		// Do nothing
    	}
    }

    private void tryToReconnect() {
    	mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
            	if (mBluetoothLeService != null) {
                    final boolean result = mBluetoothLeService.connect(mDeviceAddress);
                    Log.d(TAG, "Connect request result = " + result);
                    if (result) {
                    	Log.d("Cat", "Reconnected to the device");
                    	return;
                    }
                }
            	tryToReconnect();
            }
        }, RECONNECT_ATTEMPT_INTERVAL);
    }
    
    private ArrayList<Pair<String, String>> getPeople(String invitees) {
		if (invitees == null) {
			ArrayList<Pair<String,String>> c =  new ArrayList<Pair<String,String>>();
			c.add(new Pair<String, String>("Name", "Mug"));
			return c;
			
		}
		invitees = invitees.substring(1, invitees.length() - 1);
		String[] nameAndMug = invitees.split(", ");
		ArrayList<Pair<String, String>> pairs = new ArrayList<Pair<String, String>>();
		for (String pair : nameAndMug) {
			String[] splittedPair = pair.split(":");
			pairs.add(new Pair<String, String>(splittedPair[0], splittedPair[1]));
		}
		return pairs;
	}
    
    private ArrayList<String> getMugs(ArrayList<Pair<String, String>> people) {
    	ArrayList<String> mugs = new ArrayList<String> ();
    	for (Pair<String, String> pair : people) {
    		mugs.add(pair.second);
    	}
    	return mugs;
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
    		Log.d("Cat", "Found characteristic: " + gattService.getUuid());
    	}
    	if (gattService == null) {
        	return null;
        }
        return gattService.getCharacteristic(UUID.fromString(PENDING_CHARACTERISTIC));
    }

    /**
     * Returns the id of the next mug, and removes it from the queue. Returns null if there is no mug to invite.
     */
    private String getNextMug() {
    	if (mugQueue == null) {
    		return null;
    	}
    	if (mugQueue.size() >= 1) {
    		return mugQueue.remove(0);
    	}
    	return null;
    }

    /**
     * Sets the Pending characteristic to the id of the next mug to be invited.
     */
    private void inviteNextMug() {
    	BluetoothGattCharacteristic c = getPendingCharacteristic();
    	if (c == null) {
    		Log.i(TAG, "inviteNextMug characteristic is null");
    		return;
    	}
    	c.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
		String nextMug = getNextMug();
		Log.d("Cat", "NextMug: " + nextMug);
		if(nextMug != null) {
			nextMug = nextMug + "0000000000000000".substring(nextMug.length());
			c.setValue(nextMug);
    		mBluetoothLeService.writeCharacteristic(c);
		} else {
			waitingConfirmations = true;
		}
    }

    Runnable sendEOQlater = new Runnable() {
		@Override
		public void run() {
			BluetoothGattCharacteristic characteristic = getPendingCharacteristic();
			characteristic.setValue(END_OF_MUG_QUEUE);
			mBluetoothLeService.writeCharacteristic(characteristic);
			Log.i("Cat", "Sent end of queue again ");
		}
	};

    private void getConfirmation() {
    	BluetoothGattCharacteristic characteristic = getPendingCharacteristic();
    	String value = getStringFromCharacteristic(characteristic);
    	if (value.equals(RECEIVED_INVITATION_VALUE)) {
    		mHandler.removeCallbacks(sendEOQlater);
    		mHandler.postDelayed(sendEOQlater, RESEND_ATTEMPT_INTERVAL);
    	} else if (value.equals(DONE_ACKNOWLEDGEMENTS_VALUE)) {
    		adapter.assumeEveryoneElseDeclined();
    	} else {
    		String confirmedMug = value.replaceAll(" ", "");
    		confirmedMug = confirmedMug.toLowerCase();
    		confirmListItem(confirmedMug);
    	}
    }


	private void confirmListItem(String confirmedMug) {
		StatusListItem item = (StatusListItem) adapter.getItemById(confirmedMug);
		if (item == null) {
			return ; 
		}
    	if (item.getMugStatus() != MugStatus.ACCEPTED) {
    		item.setMugStatus(MugStatus.ACCEPTED);
    		adapter.notifyDataSetChanged();
    	}
	}

	private String getStringFromCharacteristic(BluetoothGattCharacteristic characteristic) {
		String value;
        final byte[] data = characteristic.getValue();
        if (data != null && data.length > 0) {
            final StringBuilder stringBuilder = new StringBuilder(data.length);
            for(byte byteChar : data)
                stringBuilder.insert(0, String.format("%02X ", byteChar));
            value = stringBuilder.toString();
        } else {
        	value = " data is null ";
        }
    	return value;
	}
}

