package inf.slip.b.meet4t.organizemeeting;


import inf.slip.b.meet4t.R;
import inf.slip.b.meet4t.main.MainActivity;
import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

public class InvitePeopleActivity extends ListActivity {

	private boolean isDemo = false;
	private String mode = null;
	
	PeopleListAdapter adapter;
		 
		@Override
		protected void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);
			setContentView(R.layout.activity_invitepeople);
			isDemo = getIntent().hasExtra(MainActivity.EXTRAS_MODE);
			if (isDemo) {
				mode = getIntent().getExtras().getString(MainActivity.EXTRAS_MODE);
			}
			if (!isDemo || getString(R.string.demo2).equals(mode)) {
	        	ImageView v = (ImageView) getWindow().findViewById(R.id.invite_people_list_bg);
	        	v.setImageResource(R.drawable.light_blue_bg);
	        } else {
	        	getWindow().setBackgroundDrawableResource(R.drawable.canvas_bg_2);
	        }
			adapter = new PeopleListAdapter(
					getLayoutInflater(), (!isDemo || getString(R.string.demo2).equals(mode)));
			Button invitePeopleButton = new Button(this);
			invitePeopleButton.setOnClickListener(new View.OnClickListener() {
	            public void onClick(View v) {
	            	invitePeople(adapter.getSelectedPeopleString());
	        		}
	        });
			invitePeopleButton.setText("Invite");
			getListView().addFooterView(invitePeopleButton);
			getListView().setAdapter(adapter);

		}

		private void invitePeople(String selectedPeople) {
			if (selectedPeople == null) {
				Toast.makeText(this, "You should invite people to your meeting", Toast.LENGTH_SHORT).show();
				return;
			}
			Intent i = getIntent();
			i.putExtra("invitees", selectedPeople);
			if (isDemo) {
				i.putExtra(MainActivity.EXTRAS_MODE, mode);
			}
			setResult(RESULT_OK, i);
			finish();
		}
	}
