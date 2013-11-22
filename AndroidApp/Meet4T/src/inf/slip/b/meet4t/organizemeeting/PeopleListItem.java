package inf.slip.b.meet4t.organizemeeting;

public class PeopleListItem {
		 
		private String name;
	 
		private boolean selected;
	 
		public PeopleListItem(String name, boolean selected) {
			super();
			this.name = name;
			this.selected = selected;
		}

		public String getName() {
			return name;
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
