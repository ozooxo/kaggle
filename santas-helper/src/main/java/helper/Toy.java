package helper;

class Toy implements Comparable<Toy> {

	private int id;
	Date arrival_time;
	int duration;
	
	int elf_id;
	Date starting_time;
	int actual_duration;
	
	Toy (int id, Date arrival_time, int duration){
		this.id = id;
		this.arrival_time = arrival_time;
		this.duration = duration;
	}
	
	Toy (String input) {
		String[] id_time_duration = input.split(",");
		id = Integer.parseInt(id_time_duration[0]);
		arrival_time = new Date(id_time_duration[1]);
		duration = Integer.parseInt(id_time_duration[2]);
	}
	
	@Override
	public String toString () {
		return id+","+elf_id+","+starting_time+","+actual_duration;
	}
	
	@Override
	public int compareTo(Toy that) {
		int result = starting_time.compareTo(that.starting_time);
		if (result != 0) return result;
		else return Integer.compare(that.id, that.id);
	}
	
	public String allInfo () {
		return id+"\t"+arrival_time+"\t"+duration+"\t"+elf_id+"\t"+starting_time+"\t"+actual_duration;
	}
	
	int makeAndReturnDuration (Date starting_time, int elf_id, double productivity) {
		if (this.arrival_time.compareTo(starting_time) > 0) {
			System.err.println("Error: Start too early!");
			return 0;
		}
		
		this.starting_time = starting_time;
		this.elf_id = elf_id;
		this.actual_duration = (int)Math.ceil(1.0*duration/productivity);
		
		return duration;
	}
}
