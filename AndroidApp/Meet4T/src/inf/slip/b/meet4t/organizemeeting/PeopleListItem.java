package inf.slip.b.meet4t.organizemeeting;

public class PeopleListItem {
		 
		private String name;
		private String mugID;
	 
		private boolean selected;
	 
		public PeopleListItem(String info, boolean selected) {
			super();
			parse(info);
			this.selected = selected;
		}
	 
		private void parse(String info) {
			String[] parsedInfo = info.split(": ");
			this.name = parsedInfo[0];
			this.mugID = parsedInfo[1];
		}

		public String getName() {
			return name;
		}

		public String getMugID() {
			return mugID;
		}
	 
		public void setName(String name) {
			this.name = name;
		}
	 
		public boolean isSelected() {
			return selected;
		}
	 
		public void setSelected(boolean selected) {
			this.selected = selected;
		}
	 
	}
