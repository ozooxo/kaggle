package helper;

import static org.junit.Assert.*;

import org.junit.Test;

public class DateTest {

	@Test
	public void testToString() {
		String date = "2014 12 26 15 38";
		assertTrue(new Date(date).toString().equals(date));
		//System.out.println(new Date(2014, 12, 26, 15, 38).toString());
	}
	
	@Test
	public void testCompareTo() {
		assertTrue(new Date(2014, 11, 30, 18, 58).compareTo(new Date(2015, 11, 30, 18, 58)) < 0);
	}
	
	@Test
	public void testAddMinute() {
		//System.out.println(new Date(2014, 12, 31, 0, 2).addMinute(1));
		
		assertTrue(new Date(2014, 12, 26, 15, 38).addMinute(15).equals(new Date(2014, 12, 26, 15, 53)));
		assertTrue(new Date(2014, 12, 26, 15, 38).addMinute(25).equals(new Date(2014, 12, 26, 16, 03)));
		assertTrue(new Date(2014, 12, 26, 18, 58).addMinute(2).equals(new Date(2014, 12, 27, 9, 0)));
		assertTrue(new Date(2014, 12, 26, 18, 58).addMinute(3).equals(new Date(2014, 12, 27, 9, 1)));
		assertTrue(new Date(2014, 12, 31, 18, 58).addMinute(3).equals(new Date(2015, 1, 1, 9, 1)));
		assertTrue(new Date(2014, 12, 31, 18, 58).addMinute(31*10*60+3).equals(new Date(2015, 2, 1, 9, 01)));
		assertTrue(new Date(2014, 12, 31, 18, 58).addMinute(365*10*60+3).equals(new Date(2016, 1, 1, 9, 01)));
		assertTrue(new Date(2014, 12, 31, 18, 58).addMinute(364*10*60+3).equals(new Date(2015, 12, 31, 9, 01)));
	}
	
	@Test
	public void testAddDay() {
		//System.out.println(new Date(2014, 12, 30, 18, 58).addDay(3+31));
		
		assertTrue(new Date(2014, 12, 30, 18, 58).addDay(3).equals(new Date(2015, 1, 2, 18, 58)));
		assertTrue(new Date(2014, 12, 30, 18, 58).addDay(3+31).equals(new Date(2015, 2, 2, 18, 58)));
		assertTrue(new Date(2014, 12, 30, 18, 58).addDay(3+31+28).equals(new Date(2015, 3, 2, 18, 58)));
	}
	
	@Test
	public void testAddMonth() {
		//System.out.println(new Date(2014, 11, 30, 18, 58).addMonth(2));
		
		assertTrue(new Date(2014, 11, 30, 18, 58).addMonth(1).equals(new Date(2014, 12, 30, 18, 58)));
		assertTrue(new Date(2014, 11, 30, 18, 58).addMonth(2).equals(new Date(2015, 01, 30, 18, 58)));
	}
	
	@Test
	public void testNextDay() {
		assertTrue(new Date(2014, 12, 30, 18, 58).nextDay().equals(new Date(2014, 12, 31, 9, 0)));
		assertTrue(new Date(2014, 12, 31, 18, 58).nextDay().equals(new Date(2015, 1, 1, 9, 0)));
		assertTrue(new Date(2014, 12, 31, 18, 58).nextDay().nextDay().equals(new Date(2015, 1, 2, 9, 0)));
	}
	
	@Test
	public void testMinuteUntilEndOfDay() {
		assertEquals(new Date(2014, 12, 30, 18, 58).minuteUntilEndOfDay(), 2);
		assertEquals(new Date(2014, 12, 30, 17, 58).minuteUntilEndOfDay(), 62);
		assertEquals(new Date(2014, 12, 30, 9, 0).minuteUntilEndOfDay(), 600);
		assertEquals(new Date(2014, 10, 7, 14, 0).minuteUntilEndOfDay(), 300);
	}
}
