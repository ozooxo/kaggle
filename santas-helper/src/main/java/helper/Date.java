package helper;

class Date implements Comparable<Date> {
	
	static final int hour_start = 9;
	static final int hour_end = 19;
	
	static final int[] days_in_month = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	
	private int year;
	private int month;
	private int day;
	
	private int hour;
	private int minute;
	
	Date (int year, int month, int day, int hour, int minute) {
		this.year = year;
		this.month = month;
		this.day = day;
		
		this.hour = hour;
		this.minute = minute;
	}
	
	Date (String input) {
		String[] words = input.split(" ");
		year = Integer.parseInt(words[0]);
		month = Integer.parseInt(words[1]);
		day = Integer.parseInt(words[2]);
		hour = Integer.parseInt(words[3]);
		minute = Integer.parseInt(words[4]);
	}
	
	@Override
	public boolean equals (Object obj) {
		if (!(obj instanceof Date)) return false;
		if (obj == this) return true;

		Date that = (Date) obj;
		if (this.year != that.year) return false;
		if (this.month != that.month) return false;
		if (this.day != that.day) return false;
		if (this.hour != that.hour) return false;
		if (this.minute != that.minute) return false;
		
		return true;
	}
	
	@Override
	public int compareTo (Date that) {
		if (that == this) return 0;

		if (this.year < that.year) return -1;
		if (this.year > that.year) return 1;
		if (this.month < that.month) return -1;
		if (this.month > that.month) return 1;
		if (this.day < that.day) return -1;
		if (this.day > that.day) return 1;
		if (this.hour < that.hour) return -1;
		if (this.hour > that.hour) return 1;
		if (this.minute < that.minute) return -1;
		if (this.minute > that.minute) return 1;
		
		return 0;
	}
	
	@Override
	public String toString () {
		return year+" "+month+" "+day+" "+hour+" "+minute;
	}
	
	Date addMonth (int duration_month) {
		/*
		 * This is not always right. For example,
		 * Date(2014, 1, 31, 9, 3).addMonth(1) --> Date(2014, 2, 31, 9, 3)
		 * However, since addDay only use addMonth(1) and also count the date from the
		 * beginning of the month, addDay/Hour/Minute are always right.
		 */
		return new Date (
				year + (month + duration_month - 1)/12,
				(month + duration_month - 1)%12 + 1,
				day,
				hour,
				minute);
	}
	
	Date addDay (int duration_day) {
		if (day + duration_day <= days_in_month[month]) {
			return new Date(year, month, day + duration_day, hour, minute);
		}
		else {
			int remain_day = (day + duration_day) - days_in_month[month];
			return new Date(year, month, 0, hour, minute).addMonth(1).addDay(remain_day);
		}
	}
	
	Date addHour (int duration_hour) {
		if (hour + duration_hour < hour_end) {
			return new Date(year, month, day, hour + duration_hour, minute);
		}
		else {
			return new Date(
					year, 
					month, 
					day, 
					hour_start+(hour + duration_hour - hour_start)%10, 
					minute
					).addDay((hour + duration_hour - hour_start)/10);
		}
	}
	
	Date addMinute (int duration_minute) {
		if (minute + duration_minute < 60) {
			return new Date(year, month, day, hour, minute + duration_minute);
		}
		else {
			return new Date(
					year, 
					month, 
					day, 
					hour, 
					(minute + duration_minute)%60
					).addHour((minute + duration_minute)/60);
		}
	}
	
	Date nextDay () {
		Date nextday = this.addDay(1);
		nextday.hour = 9;
		nextday.minute = 0;
		return nextday;
	}
	
	int minuteUntilEndOfDay () {
		return (hour_end - hour - 1)*60 + (60 - minute);
	}
}
