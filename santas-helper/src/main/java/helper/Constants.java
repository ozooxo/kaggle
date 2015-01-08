package helper;

class Constants {

	static Date elfStartTime = new Date(2014, 10, 7, 12, 0); 
	// To make elfsPack2400 phase uniform, it cannot be too small
	// In this setup, toys in region 2771~2842 cannot be packed, but the difference should be minor.
	
	static double keepProductivityIncreaseFactor = 1.187951; // -log(1.02)/log(0.9)
	static double keepProductifityCutoffFactor = 0.841735; // 1/(-log(1.02)/log(0.9))
}
