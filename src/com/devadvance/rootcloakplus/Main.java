package com.devadvance.rootcloakplus;


import java.io.File;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import android.app.ActivityManager.RecentTaskInfo;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.app.ActivityManager.RunningServiceInfo;
import android.app.ActivityManager.RunningTaskInfo;
import android.content.pm.ApplicationInfo;
import android.util.Log;

import com.devadvance.rootcloakplus.EntryManager;
import com.saurik.substrate.MS;
import com.saurik.substrate.MS.MethodAlteration;

public class Main {
	private static ArrayList<String> blacklistEntries = EntryManager.readEntriesFromFile(EntryManager.BLACKLIST_FILE);

	@SuppressWarnings({ "unchecked", "rawtypes" })
	static void initialize() {
		Log.i(Common.TAG, "Extension initializing: RCP");
		initForPackageManager();
		initForActivityManager();
		initForRuntime();
		initForProcessBuilder();
		Log.i(Common.TAG, "Extension initialized: RCP");
	}

	static void initForPackageManager() {
		Class _class;
		Method method;
		Constructor constructor;
		try {
			_class = Class.forName("android.app.ApplicationPackageManager");
			method = _class.getMethod("getInstalledApplications", Integer.TYPE);
			MS.hookMethod(_class, method, new MethodAlteration<Object, Object>() {
				public Object invoked(Object _this, Object... args) throws Throwable {
					final String testName = Common.getProcessName();
					if (testName == null || !isBlacklisted(testName)) {
						//Log.d(Common.TAG, "hookMethod, and testName: " + testName);
						return invoke(_this, args);
					}

					Log.i(Common.TAG, "Blacklisted app: " + testName);

					List<ApplicationInfo> packages = (List<ApplicationInfo>) invoke(_this, args);
					Iterator<ApplicationInfo> iter = packages.iterator();
					ApplicationInfo tempAppInfo;
					String tempPackageName;
					while(iter.hasNext()) {
						tempAppInfo = iter.next();
						tempPackageName = tempAppInfo.packageName;
						if(tempPackageName != null && stringContainsFromCollection(tempPackageName, Common.DEFAULT_KEYWORD_ENTRIES)) {
							iter.remove();
						}
					}

					return packages;
				}
			});
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		try {
			_class = Class.forName("android.app.ApplicationPackageManager");
			method = _class.getMethod("getPackageInfo", String.class, Integer.TYPE);
			MS.hookMethod(_class, method, new MethodAlteration<Object, Object>() {
				public Object invoked(Object _this, Object... args) throws Throwable {
					final String testName = Common.getProcessName();
					if (testName == null || !isBlacklisted(testName)) {
						//Log.d(Common.TAG, "hookMethod, and testName: " + testName);
						return invoke(_this, args);
					}

					Log.i(Common.TAG, "Blacklisted app: " + testName);

					String packageName = (String) args[0];
					if(packageName != null && stringContainsFromCollection(packageName, Common.DEFAULT_KEYWORD_ENTRIES)) {
						packageName = "FAKEPACKAGENAME";
						args[0] = packageName;
					}

					return invoke(_this, args);
				}
			});
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}


	}

	static void initForActivityManager() {
		Class _class;
		Method method;
		Constructor constructor;

		try {
			_class = Class.forName("android.app.ActivityManager");
			method = _class.getMethod("getRunningAppProcesses");
			MS.hookMethod(_class, method, new MethodAlteration<Object, Object>() {
				public Object invoked(Object _this, Object... args) throws Throwable {
					final String testName = Common.getProcessName();
					if (testName == null || !isBlacklisted(testName)) {
						//Log.d(Common.TAG, "hookMethod, and testName: " + testName);
						return invoke(_this, args);
					}

					Log.i(Common.TAG, "Blacklisted app: " + testName);

					List<RunningAppProcessInfo> processes = (List<RunningAppProcessInfo>) invoke(_this, args);
					Iterator<RunningAppProcessInfo> iter = processes.iterator();
					RunningAppProcessInfo tempAppInfo;
					String tempProcessName;
					while(iter.hasNext()) {
						tempAppInfo = iter.next();
						tempProcessName = tempAppInfo.processName;
						if(tempProcessName != null && stringContainsFromCollection(tempProcessName, Common.DEFAULT_KEYWORD_ENTRIES)) {
							iter.remove();
						}
					}

					return processes;
				}
			});
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		try {
			_class = Class.forName("android.app.ActivityManager");
			method = _class.getMethod("getRunningTasks", Integer.TYPE);
			MS.hookMethod(_class, method, new MethodAlteration<Object, Object>() {
				public Object invoked(Object _this, Object... args) throws Throwable {
					final String testName = Common.getProcessName();
					if (testName == null || !isBlacklisted(testName)) {
						//Log.d(Common.TAG, "hookMethod, and testName: " + testName);
						return invoke(_this, args);
					}

					Log.i(Common.TAG, "Blacklisted app: " + testName);

					List<RunningTaskInfo> tasks = (List<RunningTaskInfo>) invoke(_this, args);
					Iterator<RunningTaskInfo> iter = tasks.iterator();
					RunningTaskInfo tempTaskInfo;
					String tempBaseActivity;
					while(iter.hasNext()) {
						tempTaskInfo = iter.next();
						tempBaseActivity = tempTaskInfo.baseActivity.flattenToString();
						if(tempBaseActivity != null && stringContainsFromCollection(tempBaseActivity, Common.DEFAULT_KEYWORD_ENTRIES)) {
							iter.remove();
						}
					}

					return tasks;
				}
			});
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		try {
			_class = Class.forName("android.app.ActivityManager");
			method = _class.getMethod("getRunningServices", Integer.TYPE);
			MS.hookMethod(_class, method, new MethodAlteration<Object, Object>() {
				public Object invoked(Object _this, Object... args) throws Throwable {
					final String testName = Common.getProcessName();
					if (testName == null || !isBlacklisted(testName)) {
						//Log.d(Common.TAG, "hookMethod, and testName: " + testName);
						return invoke(_this, args);
					}

					Log.i(Common.TAG, "Blacklisted app: " + testName);

					List<RunningServiceInfo> services = (List<RunningServiceInfo>) invoke(_this, args);
					Iterator<RunningServiceInfo> iter = services.iterator();
					RunningServiceInfo tempServiceInfo;
					String tempProcess;
					while(iter.hasNext()) {
						tempServiceInfo = iter.next();
						tempProcess = tempServiceInfo.process;
						if(tempProcess != null && stringContainsFromCollection(tempProcess, Common.DEFAULT_KEYWORD_ENTRIES)) {
							iter.remove();
						}
					}

					return services;
				}
			});
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}


		try {
			_class = Class.forName("android.app.ActivityManager");
			method = _class.getMethod("getRecentTasks", Integer.TYPE, Integer.TYPE);
			MS.hookMethod(_class, method, new MethodAlteration<Object, Object>() {
				public Object invoked(Object _this, Object... args) throws Throwable {
					final String testName = Common.getProcessName();
					if (testName == null || !isBlacklisted(testName)) {
						//Log.d(Common.TAG, "hookMethod, and testName: " + testName);
						return invoke(_this, args);
					}

					Log.i(Common.TAG, "Blacklisted app: " + testName);

					List<RecentTaskInfo> tasks = (List<RecentTaskInfo>) invoke(_this, args);
					Iterator<RecentTaskInfo> iter = tasks.iterator();
					RecentTaskInfo tempTaskInfo;
					String name;
					while(iter.hasNext()) {
						tempTaskInfo = iter.next();
						name = tempTaskInfo.toString();
						if(name != null && stringContainsFromCollection(name, Common.DEFAULT_KEYWORD_ENTRIES)) {
							iter.remove();
						}
					}

					return tasks;
				}
			});
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	static void initForRuntime() {
		Class _class;
		Method method;
		Constructor constructor;
		try {
			_class = Class.forName("java.lang.Runtime");
			method = _class.getMethod("exec", String[].class, String[].class, File.class);
			MS.hookMethod(_class, method, new MethodAlteration<Object, Object>() {
				public Object invoked(Object _this, Object... args) throws Throwable {
					final String testName = Common.getProcessName();
					if (testName == null || !isBlacklisted(testName)) {
						//Log.d(Common.TAG, "hookMethod, and testName: " + testName);
						return invoke(_this, args);
					}

					Log.i(Common.TAG, "Blacklisted app: " + testName);
					String[] execArray = (String[]) args[0];
					if ((execArray != null)  && (execArray.length >= 1)) {
//						String tempString = "Exec Command:";
//						for (String temp : execArray) {
//							tempString = tempString + " " + temp;
//						}
//						Log.d(Common.TAG, tempString);

						if (execArray[0] == "su") {
							throw new IOException();
						}
						if (execArray[0] == "ps") {
							args[0] = buildGrepArraySingle(execArray, true);
						} else if (execArray[0] == "ls") {
							args[0] = buildGrepArraySingle(execArray, true);
						} else if (execArray[0] == "pm" ) {
							if (execArray.length >= 3 && execArray[1].equalsIgnoreCase("list") && execArray[2].equalsIgnoreCase("packages")) {
								args[0] = buildGrepArraySingle(execArray, true);
							} else if (execArray.length >= 3 && (execArray[1].equalsIgnoreCase("dump") || execArray[1].equalsIgnoreCase("path"))) {
								// If getting dumping or getting the path, don't let it work if it contains any of the keywords
								if (stringContainsFromCollection(execArray[2], Common.DEFAULT_KEYWORD_ENTRIES)) {
									args[0] = new String[] {execArray[0], execArray[1], "FAKE_PACKAGE"};
								}
							}
						} else if (execArray[0] == "sh") {
							//String[] tempArray = {"FAKECOMMAND", "FAKE"};
							if (execArray.length >= 3 && execArray[1] != null && execArray[1].equalsIgnoreCase("-c")) {
								if (execArray[2] != null && (execArray[2] == "ps" || execArray[2] == "pm"|| execArray[2] == "")) {
									args[0] = buildGrepArraySingle(execArray, false);
								} else if (execArray[2] != null && execArray[2] == "su") {
									String[] tempArray = {"sh", "-c", "suz"};
									args[0] = tempArray;
								} else {
									args[0] = buildGrepArraySingle(execArray, false);
								}
							} else {
								//args[0] = buildGrepArray(execArray, false);
								throw new IOException();
							}
						}

					}

//					String[] finishedArgs = (String[]) args[0];
//
//					String tempString = "Finished exec Command:";
//					for (String temp : finishedArgs) {
//						tempString = tempString + " " + temp;
//					}
//					Log.d(Common.TAG, tempString);

					return invoke(_this, args);
				}
			});
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	
	// Note to self: in the future, instead of doing any of this, change it to hook start(), and then have it check using command() if it is a bad arg
	// Otherwise the app doign the detection could detect it!!!
	static void initForProcessBuilder() {
		Class _class;
		Method method;
		Constructor constructor;
		try {
			_class = Class.forName("java.lang.ProcessBuilder");
			constructor = _class.getConstructor(List.class);
			MS.hookMethod(_class, constructor, new MethodAlteration<Object, Object>() {
				public Object invoked(Object _this, Object... args) throws Throwable {
					final String testName = Common.getProcessName();
					if (testName == null || !isBlacklisted(testName)) {
						//Log.d(Common.TAG, "hookMethod, and testName: " + testName);
						return invoke(_this, args);
					}

					Log.i(Common.TAG, "Blacklisted app: " + testName);


					List<String> execList = (List<String>) args[0];
					if ((execList != null)  && (execList.size() >= 1)) {

						ArrayList<String> finalArgs = new ArrayList<String>();

						if (execList.get(0) == "ps") {
							args[0] = buildGrepListSingle(execList, true);
						} else if (execList.get(0) == "ls") {
							args[0] = buildGrepListSingle(execList, true);
						} else if (execList.get(0) == "pm" ) {
							if (execList.size() >= 3 && execList.get(1).equalsIgnoreCase("list") && execList.get(2).equalsIgnoreCase("packages")) {
								args[0] = buildGrepListSingle(execList, true);
							} else if (execList.size() >= 3 && (execList.get(1).equalsIgnoreCase("dump") || execList.get(1).equalsIgnoreCase("path"))) {
								// If getting dumping or getting the path, don't let it work if it contains any of the keywords
								if (stringContainsFromCollection(execList.get(2), Common.DEFAULT_KEYWORD_ENTRIES)) {
									List<String> tempList = new ArrayList<String> (Arrays.asList(new String[] {execList.get(0), execList.get(1), "FAKE_PACKAGE"}));
									args[0] = tempList;
								}
							}
						} else if (execList.get(0) == "sh") {
							if (execList.size() >= 3 && execList.get(1) != null && execList.get(1).equalsIgnoreCase("-c")) {
								if (execList.get(2) != null && (execList.get(2) == "ps" || execList.get(2) == "pm"|| execList.get(2) == "")) {
									args[0] = buildGrepListSingle(execList, false);
								} else if (execList.get(2) != null && execList.get(2) == "su") {
									List<String> tempList = new ArrayList<String> (Arrays.asList(new String[] {"sh", "-c", "suz"}));
									args[0] = tempList;
								} else {
									args[0] = buildGrepListSingle(execList, false);
								}
							} else {
								List<String> tempList = new ArrayList<String> (Arrays.asList(new String[] {"FAKECOMMAND", "FAKE"}));
								args[0] = tempList;
							}
						}

					}

					return invoke(_this, args);
				}
			});
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}


		// For ProcessBuilder command
		try {
			_class = Class.forName("java.lang.ProcessBuilder");
			method = _class.getMethod("command", List.class);
			MS.hookMethod(_class, method, new MethodAlteration<Object, Object>() {
				public Object invoked(Object _this, Object... args) throws Throwable {
					final String testName = Common.getProcessName();
					if (testName == null || !isBlacklisted(testName)) {
						//Log.d(Common.TAG, "hookMethod, and testName: " + testName);
						return invoke(_this, args);
					}

					Log.i(Common.TAG, "Blacklisted app: " + testName);


					List<String> execList = (List<String>) args[0];
					if ((execList != null)  && (execList.size() >= 1)) {
						ArrayList<String> finalArgs = new ArrayList<String>();

						if (execList.get(0) == "ps") {
							args[0] = buildGrepListSingle(execList, true);
						} else if (execList.get(0) == "ls") {
							args[0] = buildGrepListSingle(execList, true);
						} else if (execList.get(0) == "pm" ) {
							if (execList.size() >= 3 && execList.get(1).equalsIgnoreCase("list") && execList.get(2).equalsIgnoreCase("packages")) {
								args[0] = buildGrepListSingle(execList, true);
							} else if (execList.size() >= 3 && (execList.get(1).equalsIgnoreCase("dump") || execList.get(1).equalsIgnoreCase("path"))) {
								// If getting dumping or getting the path, don't let it work if it contains any of the keywords
								if (stringContainsFromCollection(execList.get(2), Common.DEFAULT_KEYWORD_ENTRIES)) {
									List<String> tempList = new ArrayList<String> (Arrays.asList(new String[] {execList.get(0), execList.get(1), "FAKE_PACKAGE"}));
									args[0] = tempList;
								}
							}
						} else if (execList.get(0) == "sh") {
							if (execList.size() >= 3 && execList.get(1) != null && execList.get(1).equalsIgnoreCase("-c")) {
								if (execList.get(2) != null && (execList.get(2) == "ps" || execList.get(2) == "pm"|| execList.get(2) == "")) {
									args[0] = buildGrepListSingle(execList, false);
								} else if (execList.get(2) != null && execList.get(2) == "su") {
									List<String> tempList = new ArrayList<String> (Arrays.asList(new String[] {"sh", "-c", "suz"}));
									args[0] = tempList;
								} else {
									args[0] = buildGrepListSingle(execList, false);
								}
							} else {
								List<String> tempList = new ArrayList<String> (Arrays.asList(new String[] {"FAKECOMMAND", "FAKE"}));
								args[0] = tempList;
							}
						}

					}

					return invoke(_this, args);
				}
			});
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}


	/*
	 * Common use methods.
	 */
	private static boolean stringContainsFromCollection(String base, Collection<String> values) {
		if (base != null && values != null) {
			for (String tempString : values) {
				if (base.contains(tempString)){
					return true;
				}
			}
		}

		return false;
	}

	private static boolean isBlacklisted(String name) {
		if (blacklistEntries == null || blacklistEntries.size() == 0) {
			if (Common.DEFAULT_BLACKLIST_ENTRIES.contains(name)) {
				return true;
			}
		} else {
			if (blacklistEntries.contains(name)) {
				return true;
			}
		}

		return false;
	}

	private static List<String> buildGrepList(List<String> original, boolean addSH) {
		List<String> originalList = new ArrayList<String>();
		if (addSH) {
			originalList.add("sh");
			originalList.add("-c");
		}
		originalList.addAll(original);
		originalList.addAll(Common.DEFAULT_GREP_ENTRIES);
		return originalList;
	}

	private static List<String> buildGrepListSingle(List<String> original, boolean addSH) {
		StringBuilder builder = new StringBuilder();
		List<String> originalList = new ArrayList<String>();
		if (addSH) {
			originalList.add("sh");
			originalList.add("-c");
		}
		for (String temp : original) {
			builder.append(" ");
			builder.append(temp);
		}
		//originalList.addAll(Arrays.asList(original));
		for (String temp : Common.DEFAULT_GREP_ENTRIES) {
			builder.append(" ");
			builder.append(temp);
		}
		//originalList.addAll(Common.DEFAULT_GREP_ENTRIES);
		originalList.add(builder.toString());
		return originalList;
	}

	private static String[] buildGrepArray(String[] original, boolean addSH) {
		ArrayList<String> originalList = new ArrayList<String>();
		if (addSH) {
			originalList.add("sh");
			originalList.add("-c");
		}
		originalList.addAll(Arrays.asList(original));
		originalList.addAll(Common.DEFAULT_GREP_ENTRIES);
		return originalList.toArray(new String[0]);
	}

	private static String[] buildGrepArraySingle(String[] original, boolean addSH) {
		StringBuilder builder = new StringBuilder();
		ArrayList<String> originalList = new ArrayList<String>();
		if (addSH) {
			originalList.add("sh");
			originalList.add("-c");
		}
		for (String temp : original) {
			builder.append(" ");
			builder.append(temp);
		}
		//originalList.addAll(Arrays.asList(original));
		for (String temp : Common.DEFAULT_GREP_ENTRIES) {
			builder.append(" ");
			builder.append(temp);
		}
		//originalList.addAll(Common.DEFAULT_GREP_ENTRIES);
		originalList.add(builder.toString());
		return originalList.toArray(new String[0]);
	}
}