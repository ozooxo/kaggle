package helper;

import org.junit.Test;

public class SmallToyUsageDistribution {

	@Test
	public void test() {
		
		int[] count = new int[2401];
		
		for (int minute = 0; minute < 600; ++minute) {
			Elf elf = new Elf(1);
			elf.productivity = 0.25;
			elf.available_from = Constants.elfStartTime.addMinute(minute);
			while (elf.productivity < 4) {
				if (elf.available_from.minuteUntilEndOfDay() < 4) elf.newDay(4);
				int duration = (int)Math.floor(elf.available_from.minuteUntilEndOfDay()*elf.productivity);
				//System.out.println(duration);
				++count[duration];
				Toy toy = new Toy(1, elf.available_from, duration);
				elf.makeToy(toy, elf.available_from);
			}
		}
		
		for (int i = 0; i < 2401; ++i) {
			System.out.println(i+", "+count[i]);
		}
	}
}
