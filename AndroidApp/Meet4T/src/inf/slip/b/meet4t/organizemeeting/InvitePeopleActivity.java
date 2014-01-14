package inf.slip.b.meet4t.organizemeeting;


import inf.slip.b.meet4t.R;
import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class InvitePeopleActivity extends ListActivity {
	
	PeopleListAdapter adapter;
		 
		@Override
		protected void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);
			setContentView(R.layout.activity_invitepeople);
			adapter = new PeopleListAdapter(
					getLayoutInflater());
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
			setResult(RESULT_OK, i);
			finish();
		}
	}
