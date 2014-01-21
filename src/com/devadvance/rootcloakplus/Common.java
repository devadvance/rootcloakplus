package com.devadvance.rootcloakplus;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.content.Context;

public class Common {
	public static String getAppNameByPID(Context context, int pid){
	    ActivityManager manager 
	               = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);

	    for(RunningAppProcessInfo processInfo : manager.getRunningAppProcesses()){
	        if(processInfo.pid == pid){
	            return processInfo.processName;
	        }
	    }
	    return "";
	}
	
	public static String getProcessName(int pid) {
		if (pid == 0) {
			return null;
		}

		String fileLine = null;
		String[] splitLine;
		String processName = null;
		
		FileInputStream fstream = null;
		BufferedReader bufreader = null;
		try {
			fstream = new FileInputStream("/proc/" + pid + "/cmdline");
			bufreader = new BufferedReader(new InputStreamReader(fstream));
			if ((fileLine = bufreader.readLine()) != null)   {
				splitLine = fileLine.split("\\s+");
				processName = splitLine[0];
			}
			
			bufreader.close();
		} catch (FileNotFoundException e) {
			return null;
		} catch (IOException e) {
			return null;
		}

		return processName;
	}
	
	public static String getProcessName() {
		String fileLine = null;
		String processName = null;
		
		FileInputStream fstream = null;
		BufferedReader bufreader = null;
		try {
			fstream = new FileInputStream("/proc/self/cmdline");
			bufreader = new BufferedReader(new InputStreamReader(fstream));
			if ((fileLine = bufreader.readLine()) != null)   {
				processName = fileLine.substring(0, fileLine.indexOf('\0'));
			}
			
			bufreader.close();
		} catch (FileNotFoundException e) {
			return null;
		} catch (IOException e) {
			return null;
		}

		return processName;
		
	}
	
	
	
	public static final String PREFS_APPS = "CustomizeApps";
	public static final String PREFS_KEYWORDS = "CustomizeKeywords";
	public static final String PREFS_COMMANDS = "CustomizeCommands";
	public static final String PACKAGE_NAME = "com.devadvance.rootcloakplus";
	public static final String TAG = "RootCloakPlus";
	public static final String[] DEFAULT_BLACKLIST_ARRAY = {"com.fde.DomesticDigitalCopy",
		"com.directv.application.android.go.production",
		"com.res.bby",
		"dk.excitor.dmemail",
		"com.BHTV", 
		"com.bradfordnetworks.bma", 
		"com.apriva.mobile.bams", 
		"com.apriva.mobile.aprivapay", 
		"pl.pkobp.iko", 
		"au.com.auspost", 
		"com.rogers.citytv.phone", 
		"com.zenprise", 
		"net.flixster.android", 
		"com.starfinanz.smob.android.sfinanzstatus", 
		"com.ovidos.yuppi", 
		"klb.android.lovelive", 
		"com.incube.epub", 
		"com.airwatch.androidagent", 
		"com.zappware.twintv.d3",
		"com.starfinanz.mobile.android.pushtan",
		"com.stofa.webtv",
		"com.barclays.android.barclaysmobilebanking",
		"com.bskyb.skygo",
		"com.hanaskcard.rocomo.potal",
		"com.hanabank.ebk.channel.android.hananbank",
		"com.ahnlab.v3mobileplus",
		"com.good.android.gfe",
		"it.phoenixspa.inbank",
		"dk.tv2.tv2play",
		"com.enterproid.divideinstaller",
		"com.isis.mclient.verizon.activity",
		"com.isis.mclient.atnt.activity",
		"com.isis.mclient.atnt.activity",
		"be.telenet.yelo",
		"no.rdml.android.mobiletv",
		"uk.co.barclays.barclayshomeowner",
		"com.mcafee.apps.emmagent",
		"com.virginmedia.tvanywhere",
		"com.amis.mobiatv",
		"it.telecomitalia.cubovision",
		"nl.ziggo.android.tv",
		"com.orange.fr.ocs",
		"com.adb.android.app.iti",
		"com.mobileiron", 
		"be.belgacom.mobile.tveverywhere", 
		"com.asobimo.celes_gp_en",
		"com.starfinanz.smob.android.sbanking",
		"com.channel4.ondemand"};
	public static final List<String> DEFAULT_BLACKLIST_ENTRIES = new ArrayList<String> (Arrays.asList(DEFAULT_BLACKLIST_ARRAY));
	public static final String DEBUG_KEY = "DEBUGGERPREF";
	
	public static final String APP_LIST_KEY = "APPS_LIST"; // Uses the name LIST for legacy purposes
	
	public static final String KEYWORD_SET_KEY = "KEYWORD_SET";
	public static final String COMMAND_SET_KEY = "APPS_SET";
	
	public static final String[] DEFAULT_KEYWORD_LIST = new String[] { "supersu", "superuser", "Superuser", 
		"noshufou", "xposed", "rootcloak", 
		"chainfire", "titanium", "Titanium", 
		"substrate", "greenify", "amphoras",
		"app_process.orig", ".tmpsu","-ROOTCLOAK"};
	public static final List<String> DEFAULT_KEYWORD_ENTRIES = new ArrayList<String> (Arrays.asList(DEFAULT_KEYWORD_LIST));
	public static final Set<String> DEFAULT_KEYWORD_SET = new HashSet<String>(Arrays.asList(DEFAULT_KEYWORD_LIST));
	public static final String[] DEFAULT_COMMAND_LIST = new String[] { "su", "which", "busybox", "pm", "am", "sh", "ps", "logcat"};
	public static final Set<String> DEFAULT_COMMAND_SET = new HashSet<String>(Arrays.asList(DEFAULT_COMMAND_LIST));
	
	public static final String[] DEFAULT_GREP_ARRAY = new String[] { "|", "grep", "-v", "supersu", "|", "grep", "-v", "superuser", "|", "grep", "-v", "Superuser", 
		"|", "grep", "-v", "noshufou", "|", "grep", "-v", "xposed", "|", "grep", "-v", "rootcloak", 
		"|", "grep", "-v", "chainfire", "|", "grep", "-v", "titanium", "|", "grep", "-v", "Titanium", 
		"|", "grep", "-v", "substrate", "|", "grep", "-v", "greenify", "|", "grep", "-v", "root", "|", "grep", "-v", "su"};
	public static final List<String> DEFAULT_GREP_ENTRIES = new ArrayList<String> (Arrays.asList(DEFAULT_GREP_ARRAY));
}