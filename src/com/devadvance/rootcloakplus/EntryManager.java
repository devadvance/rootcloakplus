package com.devadvance.rootcloakplus;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class EntryManager {

	public static final int BLACKLIST_FILE = 1001;
	public static final int KEYWORD_FILE = 2002;
	public static final int COMMAND_FILE = 3003;
	public static final int FILE_FILE = 4004;
	
	// THIS MEANS IT WILL NOT WORK ON DEVICES WITH MULTIPLE USERS POTENTIALLY!!! NEED TO FIX IN THE FUTURE!!!
	private static final String BLACKLIST_FILENAME = "/data/data/com.devadvance.rootcloakplus/blacklist-ROOTCLOAK";
	private static final String KEYWORD_FILENAME = "/data/data/com.devadvance.rootcloakplus/keyword-ROOTCLOAK";
	private static final String COMMAND_FILENAME = "/data/data/com.devadvance.rootcloakplus/command-ROOTCLOAK";
	private static final String FILE_FILENAME = "/data/data/com.devadvance.rootcloakplus/file-ROOTCLOAK";

	public static boolean addEntryToFile(int whichFile, String entryToAdd) {
		// First line of the file contains a count of the number of entries.
		// In the native code this makes the array allocation easier.
		String filename;
		ArrayList<String> entries;

		switch (whichFile) {
		case BLACKLIST_FILE:
			filename = BLACKLIST_FILENAME;
			break;
		case KEYWORD_FILE:
			filename = KEYWORD_FILENAME;
			break;
		case COMMAND_FILE:
			filename = COMMAND_FILENAME;
			break;
		case FILE_FILE:
			filename = FILE_FILENAME;
			break;
		default:
			return false;
		}
		
		entries = readEntriesFromFile(filename);
		if (entries == null) {
			entries = new ArrayList<String>();
		}
		
		if (entries.contains(entryToAdd)) {
			return true; // Return true if the entry is already on the list. Maybe change later?
		} else {
			entries.add(entryToAdd);
		}
		
		if (writeEntriesToFile(filename, entries)) {
			return true;
		}

		return false;
	}
	
	/**
	 * Public version of this method.
	 * @param whichFile
	 * @return
	 */
	public static boolean writeEntriesToFile(int whichFile, List<String> entries) {
		String filename;
		
		switch (whichFile) {
		case BLACKLIST_FILE:
			filename = BLACKLIST_FILENAME;
			break;
		case KEYWORD_FILE:
			filename = KEYWORD_FILENAME;
			break;
		case COMMAND_FILE:
			filename = COMMAND_FILENAME;
			break;
		case FILE_FILE:
			filename = FILE_FILENAME;
			break;
		default:
			return false;
		}
		
		return writeEntriesToFile(filename, entries);
	}
	

	private static boolean writeEntriesToFile(String filename, List<String> entries) {
		if (filename == null || entries == null) {
			return false;
		}

		FileWriter fstream = null;
		BufferedWriter bufWriter = null;

		try {

			File theFile = new File(filename);

			if(!theFile.exists()) {// Create the file if it does not exist.
				theFile.createNewFile();
			}
			theFile.setReadable(true, false); // Set world readable

			fstream = new FileWriter(filename, false);
			bufWriter = new BufferedWriter(fstream);
			
			bufWriter.write("" + entries.size() + "\n");
			for (String entry : entries) {
				bufWriter.write(entry + "\n");
			}
			
			bufWriter.close();

		} catch (IOException e) {
			return false;
		}
		
		return true;
	}
	
	/**
	 * Public version of this method.
	 * @param whichFile
	 * @return
	 */
	public static ArrayList<String> readEntriesFromFile(int whichFile) {
		String filename;
		
		switch (whichFile) {
		case BLACKLIST_FILE:
			filename = BLACKLIST_FILENAME;
			break;
		case KEYWORD_FILE:
			filename = KEYWORD_FILENAME;
			break;
		case COMMAND_FILE:
			filename = COMMAND_FILENAME;
			break;
		case FILE_FILE:
			filename = FILE_FILENAME;
			break;
		default:
			return null;
		}
		
		return readEntriesFromFile(filename);
	}

	/**
	 * Gets all the lines of a file as an ArrayList<String>.
	 * @param filename Name of the file to read from.
	 * @return ArrayList<String> containing all of the lines from the file. null value if it doesn't exist, an Exception occurs, or filename is null.
	 */
	private static ArrayList<String> readEntriesFromFile(String filename) {
		if (filename == null) {
			return null;
		}

		ArrayList<String> fileData = new ArrayList<String>();
		String fileLine = null;

		// Open the file
		FileInputStream fstream = null;
		BufferedReader bufreader = null;
		try {
			fstream = new FileInputStream(filename);
			bufreader = new BufferedReader(new InputStreamReader(fstream));
			//Read File Line By Line
			boolean firstLine = true;
			while ((fileLine = bufreader.readLine()) != null)   {
				// Print the content on the console
				//System.out.println (fileLine);
				if (firstLine) {
					firstLine = false;
					continue;
				}
				fileData.add(fileLine);
			}

			//Close the input stream
			bufreader.close();
		} catch (FileNotFoundException e) {
			return null;
		} catch (IOException e) {
			return null;
		}

		return fileData;
	}

}
