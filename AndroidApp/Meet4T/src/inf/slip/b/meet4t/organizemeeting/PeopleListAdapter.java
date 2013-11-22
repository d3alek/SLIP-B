package inf.slip.b.meet4t.organizemeeting;

import inf.slip.b.meet4t.R;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.io.IOUtils;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.TextView;

public class PeopleListAdapter extends BaseAdapter implements OnClickListener {

	private static final String NAME_SEPARATOR = ", ";

	/** The inflator used to inflate the XML layout */
	private LayoutInflater inflator;

	/** A list containing some sample data to show. */
	private List<PeopleListItem> dataList;

	public PeopleListAdapter(LayoutInflater inflator) {
		super();
		this.inflator = inflator;
		dataList = new ArrayList<PeopleListItem>();
		List<String> people = getPeople();
		for (String person : people) {
			dataList.add(new PeopleListItem(person, false));
		}
	}

	@Override
	public int getCount() {
		return dataList.size();
	}

	@Override
	public Object getItem(int position) {
		return dataList.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View view, ViewGroup viewGroup) {

		// We only create the view if its needed
		if (view == null) {
			view = inflator.inflate(R.layout.invitepeople_item, null);

			// Set the click listener for the checkbox
			view.findViewById(R.id.checkBox1).setOnClickListener(this);
		}

		PeopleListItem data = (PeopleListItem) getItem(position);

		// Set the example text and the state of the checkbox
		CheckBox cb = (CheckBox) view.findViewById(R.id.checkBox1);
		cb.setChecked(data.isSelected());
		// We tag the data object to retrieve it on the click listener.
		cb.setTag(data);

		TextView tv = (TextView) view.findViewById(R.id.textView1);
		tv.setText(data.getName());

		return view;
	}

	@Override
	/** Will be called when a checkbox has been clicked. */
	public void onClick(View view) {
		PeopleListItem data = (PeopleListItem) view.getTag();
		data.setSelected(((CheckBox) view).isChecked());
	}
	
	private List<String> getPeople() {
		URL url;
		String received = null;
		String toSend = "kettle1 names";
		try {
			url = new URL("http://54.201.81.197:8080/test");
			// Getting names
			HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
			urlConnection.setDoOutput(true);
			urlConnection.getOutputStream().write(toSend.getBytes());
			// REading response
			InputStream in = new BufferedInputStream(urlConnection.getInputStream());
			received = IOUtils.toString(in, "UTF-8");
			Log.i("Cat", "From server: " + received);
			urlConnection.disconnect();
		} catch (MalformedURLException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		if (received == null) {
			return new ArrayList<String>();
		}
		return Arrays.asList(received.split(NAME_SEPARATOR));
	}
	
	public String getMugIdsOfSelectedPeople() {
		String requestBody = "kettle1 mug";
		// Add invitees names to request body
		for (PeopleListItem person : dataList) {
			if (person.isSelected()) {
				requestBody = requestBody + " " + person.getName();
			}
		}
		URL url;
		try {
			url = new URL("http://54.201.81.197:8080/test");
			// Sending request
			HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
			urlConnection.setDoOutput(true);
			urlConnection.getOutputStream().write(requestBody.getBytes());
			// Reading response
			InputStream in = new BufferedInputStream(urlConnection.getInputStream());
			String received = IOUtils.toString(in, "UTF-8");
			urlConnection.disconnect();
			return received;
		} catch (MalformedURLException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}
}
