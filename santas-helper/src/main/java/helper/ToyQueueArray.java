package helper;

class ToyQueueArray {
	
	static final int queue_size = 22390;
	static int largest = queue_size-1;

	ToyQueue[] array = new ToyQueue[queue_size];
	
	ToyQueueArray () {
		for (int i = 0; i < queue_size; ++i) { // can't use iterator in here for initialization
			array[i] = new ToyQueue();
		}
	}

	boolean add (Toy toy) {
		if (toy.duration <= 10000) return array[toy.duration].toys.add(toy);
		else if (toy.duration <= queue_size-2) return array[toy.duration-(int)(Math.random()*1200)].toys.add(toy);
		//if (toy.duration <= queue_size-2) return array[toy.duration].toys.add(toy);
		else return array[queue_size-1].toys.add(toy);
	}
	
	void moveInArray (int indexFrom, int indexTo, int amount) {
		for (int i = 0; i < amount; ++i) {
			Toy toy = array[indexFrom].toys.poll();
			array[indexTo].toys.add(toy);
		}
	}
	
	Toy pollLargest (Date time) {
		for (int duration = largest; duration >= 0; --duration) {
			Toy toy = array[duration].toys.peek();
			if (toy != null) {
				if (toy.arrival_time.compareTo(time) <= 0) {
					largest = duration;
					return array[duration].toys.poll();
				}
			}
		}
		return null;
	}
	
	Toy pollIncreaseProductivity (Date time, double productivity) {
		int duration = (int)Math.floor(time.minuteUntilEndOfDay()*productivity);
		for (int i = 0; i < 116; ++i) {
			Toy toy = array[duration].toys.peek();
			if (toy != null) {
				if (toy.arrival_time.compareTo(time) <= 0) {
					return array[duration].toys.poll();
				}
			}
			--duration;
			if (duration <= 0) break;
		}
		return null;
	}
	
	Toy pollKeepProductivity (Date time, double productivity) {
		int duration = (int)Math.floor(Constants.keepProductivityIncreaseFactor*time.minuteUntilEndOfDay()*productivity);
		while (duration > (int)Math.floor(Constants.keepProductifityCutoffFactor*600*productivity)) {
			Toy toy = array[duration].toys.peek();
			if (toy != null) {
				if (toy.arrival_time.compareTo(time) <= 0) {
					return array[duration].toys.poll();
				}
			}
			--duration;
		}
		return null;
	}
	
	void printSize () {
		for (int i = 0; i < queue_size; ++i) {
			System.out.println(i+"\t"+array[i].toys.size());
		}
	}
	
	void printSize (int start, int end) {
		for (int i = start; i < Math.min(end, queue_size); ++i) {
			System.out.println("duration: "+i+"\t"+array[i].toys.size());
		}
	}
	
	void printAllCount () {
		int allCount = 0;
		for (int i = 0; i < queue_size; ++i) {
			allCount += array[i].toys.size();
		}
		System.out.println("all count unpacked: "+allCount);
	}
	
	void printRemainTime (int threshold_duration) {
		int remainTime = 0;
		for (int i = threshold_duration; i < Math.min(threshold_duration, queue_size); ++i) {
			remainTime += i*array[i].toys.size();
		}
		System.out.println("remain time <="+threshold_duration+"min: "+remainTime+"min");
	}
}
