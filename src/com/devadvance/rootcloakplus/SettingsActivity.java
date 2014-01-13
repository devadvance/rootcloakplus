package com.devadvance.rootcloakplus;

import android.os.Bundle;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.ListActivity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Resources;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

public class SettingsActivity extends ListActivity {
	SharedPreferences sharedPref;
	String[] menuItems;
	String instructionsString;
	String instructionsTitle;

	@SuppressLint("WorldReadableFiles")
	@SuppressWarnings("deprecation")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_settings);

		Resources res = getResources();
		menuItems = res.getStringArray(R.array.menu_array);
		instructionsString = res.getString(R.string.instructions1) + "\n\n"
				+ res.getString(R.string.instructions2) + "\n\n"
				+ res.getString(R.string.instructions3) + "\n\n"
				+ res.getString(R.string.instructions4);

		instructionsTitle = res.getString(R.string.instructions_title);
		
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_list_item_1, menuItems);
		setListAdapter(adapter);

		sharedPref = getSharedPreferences(Common.PREFS_APPS, MODE_WORLD_READABLE);
	}

	public void onListItemClick(ListView parent, View v, int position, long id) {
		Intent intent;
		switch (position) {
		case 0:
			intent = new Intent(this, CustomizeApps.class);
			startActivity(intent);
			break;
		case 1:
			Log.d(Common.PACKAGE_NAME, "Debug is now on");
			new AlertDialog.Builder(this)
					.setMessage(instructionsString)
					.setTitle(instructionsTitle).show();
			break;
		default:
			break;
		}
	}
}
