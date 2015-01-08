package helper;

import java.io.*;
import java.util.*;

class Algorithm {

	static public class OrderByElfAvalibility implements Comparator<Elf> {
		@Override
		public int compare(Elf elf1, Elf elf2) {
			int result = elf1.available_from.compareTo(elf2.available_from);
			if (result != 0) return result;
			else return Integer.compare(elf1.id, elf2.id);
		}
	}
	
	static void readFile (String filePath, ToyQueueArray toysArray) throws IOException {
		
		BufferedReader inputStream = null;
		try {
			inputStream = new BufferedReader(new FileReader(filePath));
			inputStream.readLine(); // skip the header
			
			String line;
			while (inputStream != null) {
				line = inputStream.readLine();
				if (line == null) break;
				toysArray.add(new Toy(line));
			}
		}
		finally {
			if (inputStream != null) inputStream.close();
		}
	}
	
	public static void main(String[] args) throws IOException {
		
		//String filePath = "/home/beta/Documents/PlayGround/kaggle/santas-helper/data/toys_rev2_sample10.csv";
		//int sampleFactor = 10;
		
		String filePath = "/home/beta/Documents/PlayGround/kaggle/santas-helper/data/toys_rev2.csv";
		int sampleFactor = 1;
		
		int elfNumber = 900/sampleFactor;
		
		ArrayList<Toy> packingList = new ArrayList();
		ToyQueueArray toysArray = new ToyQueueArray();
		readFile (filePath, toysArray);
		
		//toysArray.printSize();
		toysArray.printAllCount();
		
		TreeSet<Elf> elfsIncrease = new TreeSet(new OrderByElfAvalibility());
		for (int i = 1; i <= elfNumber; ++i) {
			elfsIncrease.add(new Elf(i));
		}
		
		//for (Elf elf : elfsIncrease) System.out.println(elf);
		//toysArray.printSize(21000, 22390);
		
		///*
		TreeSet<Elf> elfsPack2400 = new TreeSet(new OrderByElfAvalibility());
		while (! elfsIncrease.isEmpty()) {
			Elf elf = elfsIncrease.pollFirst();
			Toy toy = toysArray.pollIncreaseProductivity(elf.available_from, elf.productivity);
			if (toy == null) {
				elfsPack2400.add(elf);
			}
			else {
				elf.makeToy(toy, elf.available_from);
				packingList.add(toy);
				if (elf.productivity >= 4) elfsPack2400.add(elf);
				else elfsIncrease.add(elf);
			}
		}
		
		//for (Elf elf : elfsIncrease) System.out.println(elf);
		//for (Elf elf : elfsPack2400) System.out.println(elf);
		//for (Toy toy : packingList) System.out.println(toy.allInfo());
		//toysArray.printSize(1900, 2000);
		toysArray.printAllCount();
		System.out.println("packingList size: "+packingList.size());
		
		TreeSet<Elf> elfsPackLarge = new TreeSet(new OrderByElfAvalibility());
		while (! elfsPack2400.isEmpty()) {
			Elf elf = elfsPack2400.pollFirst();
			Toy toy = toysArray.pollKeepProductivity(elf.available_from, elf.productivity);
			if (toy == null) {
				elfsPackLarge.add(elf);
			}
			else {
				elf.makeToy(toy, elf.available_from);
				elf.newDay(150);
				packingList.add(toy);
				elfsPack2400.add(elf);
			}
		}
		
		//toysArray.printSize(0, 2400);
		//toysArray.printSize((int)(4*600*Constants.keepProductifityCutoffFactor)-5, (int)(4*600*Constants.keepProductivityIncreaseFactor)+5);
		//for (Elf elf : elfsPack2400) System.out.println(elf);
		//for (Elf elf : elfsPackLarge) System.out.println(elf); // if change Constants.elfStartTime, should check this one is distributed kind of uniformly.
		//for (Toy toy : packingList) System.out.println(toy.allInfo());
		toysArray.printAllCount();
		System.out.println("packingList size: "+packingList.size());
		
		//toysArray.moveInArray(1591, 1590, 60);
		//toysArray.moveInArray(1591, 1593, 40);
		
		//toysArray.moveInArray(1594, 1595, 60);
		//toysArray.moveInArray(1594, 1598, 40);
		toysArray.moveInArray(1594, 1598, 40);
		toysArray.moveInArray(1631, 1638, 20);
		toysArray.moveInArray(1631, 1641, 20);
		toysArray.moveInArray(1631, 1645, 20);
		toysArray.moveInArray(1631, 1646, 20);
		toysArray.moveInArray(1631, 1648, 20);
		
		while (! elfsPackLarge.isEmpty()) {
			Elf elf = elfsPackLarge.pollFirst();
			Toy toy = toysArray.pollLargest(elf.available_from);
			if (toy == null) {
				//System.err.println("Packing finished... Impossible! (null pollLargest)");
				break;
			}
			elf.makeToy(toy, elf.available_from);
			packingList.add(toy);

			while (true) {
				int remainMinute = elf.available_from.minuteUntilEndOfDay();
				toy = toysArray.pollIncreaseProductivity(elf.available_from, elf.productivity);
				if (toy == null) {
					if (1.0*remainMinute*elf.productivity < 20) elf.newDay(remainMinute+1);
					else {
						elfsPackLarge.add(elf);
						break;
					}
				}
				else {
					elf.makeToy(toy, elf.available_from);
					packingList.add(toy);
					if (elf.productivity >= 4) {
						elfsPackLarge.add(elf);
						break;
					}
				}
			}
		}
		
		//toysArray.printSize(140, 2400);
		//toysArray.printSize(10000, ToyQueueArray.queue_size);
		
		//toysArray.printRemainTime(150);
		//for (Elf elf : elfsPackLarge) System.out.println(elf);
		//for (Toy toy : packingList) System.out.println(toy.allInfo());
		toysArray.printAllCount();
		System.out.println("packingList size: "+packingList.size());
		
		Collections.sort(packingList);
		String outputPath = "/home/beta/Documents/PlayGround/kaggle/santas-helper/data/output.csv";
		PrintWriter fileStream = null;
		try {
			fileStream = new PrintWriter(new FileWriter(outputPath));
			fileStream.println("ToyId,ElfId,StartTime,Duration");
			for (Toy toy : packingList) {
				fileStream.println(toy);
			}
		}
		finally {
			if (fileStream != null) {
				fileStream.close();
			}
		}
		//*/
	}
}
