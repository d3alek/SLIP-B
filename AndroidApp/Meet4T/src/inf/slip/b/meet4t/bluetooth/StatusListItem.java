package inf.slip.b.meet4t.bluetooth;


public class StatusListItem {
		 
		public enum MugStatus {NOT_YET_INVITED, WAITING_REPLY, ACCEPTED, DECLINED};

		private String name;
		private String mugID;
	 
		private MugStatus mugStatus;
	 
		public StatusListItem(String name, String mugID, MugStatus status) {
			super();
			this.name = name;
			this.mugID = mugID;
			this.mugStatus = status;
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
	 
		public MugStatus getMugStatus() {
			return this.mugStatus;
		}
		
		public void setMugStatus(MugStatus status) {
			this.mugStatus = status;
		}
	 
	}
