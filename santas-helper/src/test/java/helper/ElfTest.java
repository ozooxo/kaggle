package helper;

import static org.junit.Assert.*;

import org.junit.Test;

public class ElfTest {

	@Test
	public void testMakeToy() {
		Elf elf = new Elf(27);
		Toy toy = new Toy("3,2014 1 1 12 0,600");
		
		elf.makeToy(toy, new Date("2014 1 2 9 0"));
		assertEquals(toy.toString(), "3,27,2014 1 2 9 0,600");
		assertEquals(elf.available_from, new Date("2014 1 3 9 0"));
		assertEquals(elf.productivity, Math.pow(1.02, 600/60), 0.01);
		
		elf.makeToy(toy, elf.available_from);
		assertEquals(elf.available_from.minuteUntilEndOfDay(), 600-(int)Math.ceil(600/Math.pow(1.02, 600/60)));
		assertEquals(elf.productivity, Math.pow(1.02, (600+600/Math.pow(1.02, 600/60))/60), 0.001);
	}

}
