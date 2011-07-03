package com.moreharts.android.rubydroid;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.content.Intent;

public class RubyDroidHomeActivity extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

	public void openConsoleClicked(View v)
	{
		Intent consoleIntent = new Intent(this, ConsoleActivity.class);
		startActivity(consoleIntent);
	}

	public void loadFileClicked(View v)
	{

	}
}
