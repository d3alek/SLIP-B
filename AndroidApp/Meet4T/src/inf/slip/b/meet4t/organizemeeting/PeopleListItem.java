package inf.slip.b.meet4t.organizemeeting;

public class PeopleListItem {
		 
		private String name;
		private String mugID;
	 
		private boolean selected;
	 
		public PeopleListItem(String name, String mugID, boolean selected) {
			super();
			this.name = name;
			this.mugID = mugID;
			this.selected = selected;
		}

		public String getName() {
			return name;
		}
	 
		public void setName(String name) {
			this.name = name;
		}

		public String getMugID() {
			return this.mugID;
		}
	 
		public boolean isSelected() {
			return selected;
		}
	 
		public void setSelected(boolean selected) {
			this.selected = selected;
		}
	 
	}
