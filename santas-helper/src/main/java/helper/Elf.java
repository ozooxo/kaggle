package helper;

import java.util.*;

class Elf implements Comparable<Elf> {

	int id;
	double productivity;
	Date available_from;
	
	Elf (int id) {
		this.id = id;
		this.productivity = 1;
		this.available_from = Constants.elfStartTime.addMinute(id*91%600);
	}
	
	@Override
	public String toString() {
		return id+","+productivity+","+available_from;
	}
	
	@Override
	public int compareTo (Elf that) {
		return Integer.compare(this.id, that.id);
	}
	
	private void updateProductivity (Date starting_time, double minute_needed) {
		int minute_available = starting_time.minuteUntilEndOfDay();
		if (minute_available >= minute_needed) {
			productivity *= Math.pow(1.02, 1.0*minute_needed/60);
			if (productivity > 4) productivity = 4;
			return;
		}
		else {
			double minute_unsanctioned = minute_needed - minute_available;
			productivity *= Math.pow(1.02, 1.0*minute_available/60);
			productivity *= Math.pow(0.9, 1.0*minute_unsanctioned/60);
			if (productivity > 4) productivity = 4;
			if (productivity < 0.25) productivity = 0.25;
			return;
		}
	}
	
	void makeToy (Toy toy, Date starting_time) {
		int duration = toy.makeAndReturnDuration(starting_time, id, productivity);
		int minute_needed = (int)Math.ceil(1.0*duration/productivity);
		available_from = starting_time.addMinute(minute_needed);
		
		updateProductivity(starting_time, minute_needed);
	}
	
	void newDay (int tolarant_duration) {
		if (available_from.minuteUntilEndOfDay() < tolarant_duration) {
			available_from = available_from.nextDay();
		}
	}
}
