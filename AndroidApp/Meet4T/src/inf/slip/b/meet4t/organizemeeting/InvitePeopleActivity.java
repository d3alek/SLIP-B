package inf.slip.b.meet4t.organizemeeting;


import inf.slip.b.meet4t.R;
import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class InvitePeopleActivity extends ListActivity {
	
	PeopleListAdapter adapter;
		 
		@Override
		protected void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);
			setContentView(R.layout.activity_invitepeople);
			adapter = new PeopleListAdapter(
					getLayoutInflater());
			final Button invitePeopleButton = (Button) findViewById(R.id.invite_people);
			invitePeopleButton.setOnClickListener(new View.OnClickListener() {
	            public void onClick(View v) {
	            	invitePeople(adapter.getSelectedPeopleString());
	        		}
	        });
			getListView().setAdapter(adapter);

		}

		private void invitePeople(String selectedPeople) {
			Intent i = getIntent();
			if (selectedPeople != null) {
				i.putExtra("invitees", selectedPeople);
			}
			setResult(RESULT_OK, i);
			finish();
		}
	}
