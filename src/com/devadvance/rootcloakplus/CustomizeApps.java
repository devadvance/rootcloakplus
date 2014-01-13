package com.devadvance.rootcloakplus;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import android.os.Bundle;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.support.v4.app.NavUtils;
import android.preference.PreferenceActivity;

public class CustomizeApps extends PreferenceActivity {
	
	//Set<String> appSet;
	//String[] appList;
	List<String> blacklistEntries;
	String[] blacklistArray;
	//boolean isFirstRun;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_customize_apps);
		// Show the Up button in the action bar.
		setupActionBar();
		
		loadList();
		
	}
	
	public void onListItemClick( ListView parent, View v, int position, long id) { 
    	final int positionFinal = position;
    	new AlertDialog.Builder(CustomizeApps.this)
	    .setTitle("Remove App")
	    .setMessage("Are you sure you want to remove this app?")
	    .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
	        public void onClick(DialogInterface dialog, int whichButton) {
	        	removeApp(positionFinal);
	        }
	    }).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
	        public void onClick(DialogInterface dialog, int whichButton) {
	            // Do nothing.
	        }
	    }).show();
    	
    }

	/**
	 * Set up the {@link android.app.ActionBar}.
	 */
	private void setupActionBar() {

		getActionBar().setDisplayHomeAsUpEnabled(true);

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.customize_apps, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case android.R.id.home:
			// This ID represents the Home or Up button. In the case of this
			// activity, the Up button is shown. Use NavUtils to allow users
			// to navigate up one level in the application structure. For
			// more details, see the Navigation pattern on Android Design:
			//
			// http://developer.android.com/design/patterns/navigation.html#up-vs-back
			//
			NavUtils.navigateUpFromSameTask(this);
			return true;
			
		case R.id.action_new:
			final PackageManager pm = getPackageManager();
			//get a list of installed apps.
			final List<ApplicationInfo> packages = pm.getInstalledApplications(PackageManager.GET_META_DATA);
			final String[] names = new String[packages.size()];
			final HashMap<String, String> nameMap = new HashMap<String,String>();
			int i = 0;
			for (ApplicationInfo info : packages) {
				//names[i] = info.packageName;
				names[i] = (String)info.loadLabel(pm) + "\n(" + info.packageName + ")";
				nameMap.put(names[i], info.packageName);
				i++;
			}
			Arrays.sort(names);
			
			new AlertDialog.Builder(this).setTitle("Add an App")
			.setItems((CharSequence[]) names, new DialogInterface.OnClickListener() {
	               public void onClick(DialogInterface dialog, int which) {
	            	   addApp(nameMap.get(names[which]));
	           }
	    }).show();
			
			return true;
		case R.id.action_new_custom:
			final EditText input = new EditText(this);
			new AlertDialog.Builder(CustomizeApps.this)
		    .setTitle("Add App")
		    .setMessage("Input the app package name:")
		    .setView(input)
		    .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
		        public void onClick(DialogInterface dialog, int whichButton) {
		        	addApp(input.getText().toString());
		        }
		    }).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
		        public void onClick(DialogInterface dialog, int whichButton) {
		            // Do nothing.
		        }
		    }).show();
			return true;
		case R.id.action_load_defaults:
			loadDefaultsWithConfirm();
			return true;
		case R.id.action_clear_list:
			clearList();
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	
	private void loadDefaults() {
		blacklistEntries = Common.DEFAULT_BLACKLIST_ENTRIES;
		EntryManager.writeEntriesToFile(EntryManager.BLACKLIST_FILE, blacklistEntries);
		loadList();
	}

	private void loadDefaultsWithConfirm() {
		AlertDialog.Builder builder = new AlertDialog.Builder(CustomizeApps.this)
		.setTitle("Reset apps to default?")
		.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				loadDefaults();
			}
		});
		builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				// Do nothing on cancel
			}
		}).show();
	}
	
	private void loadList() {
		blacklistEntries = EntryManager.readEntriesFromFile(EntryManager.BLACKLIST_FILE);
		
		if (blacklistEntries == null) { // If the file isn't there
			blacklistEntries = Common.DEFAULT_BLACKLIST_ENTRIES;
			EntryManager.writeEntriesToFile(EntryManager.BLACKLIST_FILE, blacklistEntries);
		}
		
		blacklistArray = blacklistEntries.toArray(new String[0]);
		Arrays.sort(blacklistArray);
		
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
		        android.R.layout.simple_list_item_1, blacklistArray);
		// Bind to our new adapter.
		setListAdapter(adapter);
	}
	
	private void clearList() {
		AlertDialog.Builder builder = new AlertDialog.Builder(CustomizeApps.this)
		.setTitle("Proceed to clear all apps?")
		.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				blacklistEntries = new ArrayList<String>();
				EntryManager.writeEntriesToFile(EntryManager.BLACKLIST_FILE, blacklistEntries);
				loadList();
			}
		});
		builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				// Do nothing on cancel
			}
		}).show();
	}
	
	private void addApp(String appName) {
		if (!(blacklistEntries.contains(appName))) {
			EntryManager.addEntryToFile(EntryManager.BLACKLIST_FILE, appName);
			loadList();
		}
	}
	
	private void removeApp(int position) {
		String tempName = blacklistArray[position];
		blacklistEntries.remove(tempName);
		EntryManager.writeEntriesToFile(EntryManager.BLACKLIST_FILE, blacklistEntries);
		loadList();
	}

}
