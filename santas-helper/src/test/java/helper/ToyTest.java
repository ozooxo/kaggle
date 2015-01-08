package helper;

import static org.junit.Assert.*;

import org.junit.Test;

public class ToyTest {

	@Test
	public void testIO() {
		Toy toy = new Toy("3,2014 1 1 12 0,34");
		toy.makeAndReturnDuration(new Date("2014 1 1 13 0"), 15, 1);
		assertEquals(toy.toString(), "3,15,2014 1 1 13 0,34");
	}

}
