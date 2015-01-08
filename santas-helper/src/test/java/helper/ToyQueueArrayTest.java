package helper;

import static org.junit.Assert.*;

import java.util.LinkedList;

import org.junit.Test;

public class ToyQueueArrayTest {

	@Test
	public void testAdd() {
		ToyQueueArray toyQueueArray = new ToyQueueArray();
		toyQueueArray.add(new Toy("3,2014 1 1 12 0,34"));
	}
	
	@Test
	public void testPollLargest() {
		Toy toy1 = new Toy("3,2014 1 1 12 0,34");
		Toy toy2 = new Toy("3,2014 1 1 13 0,134");
		ToyQueueArray toyQueueArray;
		
		toyQueueArray = new ToyQueueArray();
		toyQueueArray.add(toy1);
		toyQueueArray.add(toy2);
		assertEquals(toyQueueArray.pollLargest(new Date("2014 1 1 14 0")), toy2);
		
		toyQueueArray = new ToyQueueArray();
		toyQueueArray.add(toy1);
		toyQueueArray.add(toy2);
		assertEquals(toyQueueArray.pollLargest(new Date("2014 1 1 12 15")), toy1);
	}

	@Test
	public void testPollIncreaseProductivity() {
		Toy toy1 = new Toy("3,2014 1 1 12 0,34");
		ToyQueueArray toyQueueArray;
		
		toyQueueArray = new ToyQueueArray();
		toyQueueArray.add(toy1);
		assertEquals(toyQueueArray.pollIncreaseProductivity(new Date("2014 1 1 18 26"), 1), toy1);
		
		toyQueueArray = new ToyQueueArray();
		toyQueueArray.add(toy1);
		assertEquals(toyQueueArray.pollIncreaseProductivity(new Date("2014 1 1 18 25"), 1), toy1);
		
		toyQueueArray = new ToyQueueArray();
		toyQueueArray.add(toy1);
		assertEquals(toyQueueArray.pollIncreaseProductivity(new Date("2014 1 1 18 27"), 1), null);
		
		toyQueueArray = new ToyQueueArray();
		toyQueueArray.add(toy1);
		//assertEquals(toyQueueArray.pollIncreaseProductivity(new Date("2014 1 1 18 24"), 1), toy1);
		
		toyQueueArray = new ToyQueueArray();
		toyQueueArray.add(toy1);
		assertEquals(toyQueueArray.pollIncreaseProductivity(new Date("2014 1 1 11 0"), 1), null);
		//assertEquals(toyQueueArray.pollIncreaseProductivity(new Date("2014 1 1 12 0"), 1), toy1);
		assertEquals(toyQueueArray.pollIncreaseProductivity(new Date("2014 1 1 13 0"), 1), null);
	}
	
	@Test
	public void testPollKeepProductivity() {
		Toy toy0 = new Toy("3,2014 1 1 12 0,2850");
		Toy toy1 = new Toy("3,2014 1 1 12 0,2851");
		Toy toy2 = new Toy("3,2014 1 1 12 0,2852");
		Toy toy3 = new Toy("3,2014 1 1 12 0,2853");
		ToyQueueArray toyQueueArray;
		
		toyQueueArray = new ToyQueueArray();
		toyQueueArray.add(toy0);
		toyQueueArray.add(toy1);
		toyQueueArray.add(toy2);
		toyQueueArray.add(toy3);
		assertEquals(toyQueueArray.pollKeepProductivity(new Date("2014 1 2 9 0"), 4), toy1);
	}
}
