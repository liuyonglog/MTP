import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;
import java.util.Vector;

public class HTP {
	static final int TRAIN_NUM = 103;
	static final int TEST_NUM = 103;
	static Vector<Heart> heartList = null;
	static Vector<NonHeart> nonHeartList = null;
	
	public static void main(String args[]) throws IOException {
		
		heartList = new Vector<Heart>();
		nonHeartList = new Vector<NonHeart>();
		//trainingSetInit();
		//outputResultTrainingSet();
		testSetInit();
		outputResultTestSet();

	}
	
	public static void outputResultTestSet() throws IOException {
		String record = new String();
		
		FileWriter fw = new FileWriter("e:/HeartTestSet.txt");
		for (int i = 0; i < heartList.size(); i++) {
			record = heartList.get(i).age + " ";
			record += heartList.get(i).priorSurgeryStatus + " "; 
			record += heartList.get(i).mismatchOfAlleles + " ";
			record += heartList.get(i).mismatchOfAntigen + " ";
			record += heartList.get(i).mismatchScore + " ";
			record += heartList.get(i).reject;
			record += "*";
			fw.write(record);
			
			record = null;
		}
		fw.close();
		
		fw = new FileWriter("e:/NonHeartTestSet.txt");
		for (int i = 0; i < nonHeartList.size(); i++) {
			record = nonHeartList.get(i).age + " ";
			record += nonHeartList.get(i).priorSurgeryStatus + " ";
			record += nonHeartList.get(i).dead;
			record += "*";
			fw.write(record);
			record = null;
		}
		
		fw.close();
	}
	
	public static void testSetInit() {
		//index start from 0
		Scanner sc = new Scanner(HTP.class.getResourceAsStream("testSet.txt"));
		while(sc.hasNext()){
			Heart heartInstance = new Heart();
			NonHeart nonHeartInstance = new NonHeart();
			
			//flag = true if we got a heart instance
			int ID = sc.nextInt();
System.out.println("ID:" + ID);			
			int acceptanceYear = sc.nextInt();
			int age = sc.nextInt();
			int dead = sc.nextInt();
			int surviveTime = sc.nextInt();
			int prior = sc.nextInt();
			int transStatus = sc.nextInt();
			String token = sc.next();
			String matchToken = "NA";
			if(token.matches(matchToken)) {
System.out.println("Yes");
				nonHeartInstance.age = age;
				nonHeartInstance.dead = dead;
				nonHeartInstance.priorSurgeryStatus = prior;
				nonHeartList.add(nonHeartInstance);
			} else {
				heartInstance.age = age;
				heartInstance.reject = dead;
				heartInstance.priorSurgeryStatus = prior;
				heartInstance.mismatchOfAlleles = sc.nextInt();
				heartInstance.mismatchOfAntigen = sc.nextInt();
				heartInstance.mismatchScore = sc.nextDouble();
				heartList.add(heartInstance);
			}
		}
		sc.close();
		System.out.println("done reading files");
	}
	
	public static void outputResultTrainingSet() throws IOException {
		String record = new String();
		
		FileWriter fw = new FileWriter("e:/HeartTrainingSet.txt");
		for (int i = 0; i < heartList.size(); i++) {
			record = heartList.get(i).age + " ";
			record += heartList.get(i).priorSurgeryStatus + " "; 
			record += heartList.get(i).mismatchOfAlleles + " ";
			record += heartList.get(i).mismatchOfAntigen + " ";
			record += heartList.get(i).mismatchScore + " ";
			record += heartList.get(i).reject;
			record += "*";
			fw.write(record);
			
			record = null;
		}
		fw.close();
		
		fw = new FileWriter("e:/NonHeartTrainingSet.txt");
		for (int i = 0; i < nonHeartList.size(); i++) {
			record = nonHeartList.get(i).age + " ";
			record += nonHeartList.get(i).priorSurgeryStatus + " ";
			record += nonHeartList.get(i).dead;
			record += "*";
			fw.write(record);
			record = null;
		}
		
		fw.close();
	}
	
	public static void trainingSetInit() {
		//index start from 0
		Scanner sc = new Scanner(HTP.class.getResourceAsStream("trainingSet.txt"));
		while(sc.hasNext()){
			Heart heartInstance = new Heart();
			NonHeart nonHeartInstance = new NonHeart();
			
			//flag = true if we got a heart instance
			int ID = sc.nextInt();
			System.out.println("ID:" + ID);			
			int birthDay = sc.nextInt();
			int birthMonth = sc.nextInt();
			int birthYear = sc.nextInt();
			int acceptDay = sc.nextInt();
			int acceptMonth = sc.nextInt();
			int acceptYear = sc.nextInt();
			
			//cal age
			int age = acceptYear - birthYear;
			String transplantDate = sc.next();
			String NA = "NA";
			if(transplantDate.matches("NA")) {
				nonHeartInstance.age = age;
				int i = 5;
				while ( i > 0 ) {
					sc.next();
					i--;
				}
				nonHeartInstance.dead = sc.nextInt();
				//System.out.println("dead:" + nonHeartInstance.dead);
				nonHeartInstance.priorSurgeryStatus = sc.nextInt();
				i = 4;
				while ( i > 0 ) {
					sc.next();
					i--;
				}
				
				nonHeartList.add(nonHeartInstance);
				
			} else {
				
				heartInstance.age = age;
				int i = 6;
				while ( i != 0 ) {
					sc.next();
					i--;
				}
				heartInstance.priorSurgeryStatus = sc.nextInt();
				String error = sc.next();
				if(error.matches("NA")) {
					i = 3;//TODO
					while ( i != 0 ) {
						sc.next();
						i--;
					}
					
				} else {
					heartInstance.mismatchOfAlleles = Integer.parseInt(error);
					heartInstance.mismatchOfAntigen = sc.nextInt();
					heartInstance.mismatchScore = sc.nextDouble();
					heartInstance.reject = sc.nextInt();
					heartList.add(heartInstance);
				}
					
				
			}
			
				
		}
		sc.close();
		System.out.println("done reading files");
	}
		
}
