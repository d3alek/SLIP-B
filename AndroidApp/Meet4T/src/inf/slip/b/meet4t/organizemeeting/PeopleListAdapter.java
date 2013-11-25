package inf.slip.b.meet4t.organizemeeting;

import inf.slip.b.meet4t.R;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.io.IOUtils;

import android.util.Log;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.TextView;

public class PeopleListAdapter extends BaseAdapter implements OnClickListener {

	private static final String SEPARATOR = ", ";

	/** The inflator used to inflate the XML layout */
	private LayoutInflater inflator;

	/** A list containing some sample data to show. */
	private List<PeopleListItem> dataList;

	public PeopleListAdapter(LayoutInflater inflator) {
		super();
		this.inflator = inflator;
		dataList = new ArrayList<PeopleListItem>();
		List<Pair<String, String>> people = getPeople();
		for (int i = 0; i < people.size(); i++) {
			dataList.add(new PeopleListItem(people.get(i).first, people.get(i).second, false));
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
	
	private ArrayList<Pair<String, String>> getPeople() {
		URL url;
		String[] names;
		String[] mugs;
		String received = null;
		String namesRequest = "kettle2 names";
		String mugIDsRequest = "kettle2 cups";
		try {
			url = new URL("http://54.201.81.197:8080/test");
			// Getting names
			HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
			urlConnection.setDoOutput(true);
			urlConnection.getOutputStream().write(namesRequest.getBytes());
			// Reading response
			InputStream in = new BufferedInputStream(urlConnection.getInputStream());
			received = IOUtils.toString(in, "UTF-8");
			Log.i("Cat", "From server: " + received);
			urlConnection.disconnect();
			names = received.split(SEPARATOR);
			// Getting cup ids
			urlConnection = (HttpURLConnection) url.openConnection();
			urlConnection.setDoOutput(true);
			urlConnection.getOutputStream().write(mugIDsRequest.getBytes());
			// Reading response
			in = new BufferedInputStream(urlConnection.getInputStream());
			received = IOUtils.toString(in, "UTF-8");
			Log.i("Cat", "From server: " + received);
			urlConnection.disconnect();
			mugs = received.split(SEPARATOR);
			return pairUp(names, mugs);
		} catch (MalformedURLException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return new ArrayList<Pair<String,String>>();
	}
	
	private ArrayList<Pair<String, String>> pairUp(String[] names, String[] mugs) {
		ArrayList<Pair<String, String>> pairs = new ArrayList<Pair<String, String>>();
		for (int i = 0; i < names.length; i++) {
			pairs.add(new Pair<String, String>(names[i], mugs[i]));
		}
		return pairs;
	}

	public String getSelectedPeopleString() {
		List<String> selectedPeople = new ArrayList<String>();
		// Add invitees names to request body
		for (PeopleListItem person : dataList) {
			if (person.isSelected()) {
				selectedPeople.add(person.getName() + ":" + person.getMugID());
			}
		}
		if (selectedPeople.size() > 0) {
			return selectedPeople.toString();
		}
		return null;
	}
}
