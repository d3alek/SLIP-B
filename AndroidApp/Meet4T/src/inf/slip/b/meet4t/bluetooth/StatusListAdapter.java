package inf.slip.b.meet4t.bluetooth;

import inf.slip.b.meet4t.R;
import inf.slip.b.meet4t.bluetooth.StatusListItem.MugStatus;

import java.util.ArrayList;
import java.util.List;

import android.util.Log;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

public class StatusListAdapter extends BaseAdapter implements OnClickListener {

	/** The inflator used to inflate the XML layout */
	private LayoutInflater inflator;

	/** A list containing some sample data to show. */
	private List<StatusListItem> dataList;

	public StatusListAdapter(LayoutInflater inflator, List<Pair<String, String>> people) {
		super();
		this.inflator = inflator;
		dataList = new ArrayList<StatusListItem>();
		for (int i = 0; i < people.size(); i++) {
			dataList.add(new StatusListItem(people.get(i).first, people.get(i).second, MugStatus.NOT_YET_INVITED));
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
			view = inflator.inflate(R.layout.device_ack_list_item, null);

			// Set the click listener for the checkbox
			view.findViewById(R.id.invitee).setOnClickListener(this);
		}

		StatusListItem data = (StatusListItem) getItem(position);

		switch (data.getMugStatus()) {
		case NOT_YET_INVITED:
			view.findViewById(R.id.not_invited_yet).setVisibility(View.VISIBLE);
			view.findViewById(R.id.invited).setVisibility(View.INVISIBLE);
			view.findViewById(R.id.invited_accepted).setVisibility(View.INVISIBLE);
			view.findViewById(R.id.invited_declined).setVisibility(View.INVISIBLE);
			break;
		case WAITING_REPLY:
			view.findViewById(R.id.not_invited_yet).setVisibility(View.INVISIBLE);
			view.findViewById(R.id.invited).setVisibility(View.VISIBLE);
			view.findViewById(R.id.invited_accepted).setVisibility(View.INVISIBLE);
			view.findViewById(R.id.invited_declined).setVisibility(View.INVISIBLE);
			break;
		case ACCEPTED:
			view.findViewById(R.id.not_invited_yet).setVisibility(View.INVISIBLE);
			view.findViewById(R.id.invited).setVisibility(View.INVISIBLE);
			view.findViewById(R.id.invited_accepted).setVisibility(View.VISIBLE);
			view.findViewById(R.id.invited_declined).setVisibility(View.INVISIBLE);
			break;
		case DECLINED:
			view.findViewById(R.id.not_invited_yet).setVisibility(View.INVISIBLE);
			view.findViewById(R.id.invited).setVisibility(View.INVISIBLE);
			view.findViewById(R.id.invited_accepted).setVisibility(View.INVISIBLE);
			view.findViewById(R.id.invited_declined).setVisibility(View.VISIBLE);
			break;
		default:
			break;
		}
		TextView tv = (TextView) view.findViewById(R.id.invitee);
		tv.setText(data.getName());
		
		return view;
	}

	@Override
	public void onClick(View view) {
		StatusListItem data = (StatusListItem) view.getTag();
		Log.d("Cat", "clicked " + data.getName());
	}

	public StatusListItem getItemById(String mugID) {
		for (StatusListItem item : dataList) {
			if (item.getMugID().equals(mugID)) {
				return item;
			}
		}
		return null;
	}

	public void addItems(List<Pair<String, String>> people) {
		dataList = new ArrayList<StatusListItem>();
		for (int i = 0; i < people.size(); i++) {
			dataList.add(new StatusListItem(people.get(i).first, people.get(i).second, MugStatus.NOT_YET_INVITED));
		}
	}
}
