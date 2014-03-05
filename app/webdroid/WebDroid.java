
package winglib.app.webdroid;

import winglib.app.webdroid.R;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.content.Context;
import android.webkit.WebView;
import android.webkit.WebChromeClient;
import android.app.AlertDialog;
import android.content.DialogInterface;

public class WebDroid extends Activity
{
	WebView m_wv;
	
    //--------------------------------------------------------------
	// onKeyDown()
    //--------------------------------------------------------------
	@Override 
	public boolean onKeyDown( int keyCode, KeyEvent event )
	{
		if( KeyEvent.ACTION_DOWN == event.getAction() )
		{
			switch( keyCode )
			{
				case KeyEvent.KEYCODE_BACK:
				
					if( m_wv.canGoBack() )
					{
						m_wv.goBack();
					} // end if
					
					else
					{
						finish();
					} // end else
			
					return true;
			} // end switch

		} // end if
			
		return super.onKeyDown( keyCode, event );
	}

    //--------------------------------------------------------------
	// onCreate()
    //--------------------------------------------------------------
    @Override public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );

		// Set the layout
		setContentView( R.layout.main );

		// Setup the web view
		m_wv = (WebView)findViewById( R.id.webview );
		if ( null == m_wv ) 
			return;

		// Enable javascript
		m_wv.getSettings().setJavaScriptEnabled( true );
					
		// Enable web pages to get position
		m_wv.getSettings().setGeolocationEnabled( true );
		
		final Context thisCtx = this;		
		m_wv.setWebChromeClient( 
			new WebChromeClient() 
			{
				@Override
				public void onGeolocationPermissionsShowPrompt(
								String origin, 
								android.webkit.GeolocationPermissions.Callback callback ) 
				{
					callback.invoke( origin, true, false ); 
				}
				
				@Override
				public boolean onJsAlert(WebView view, String url, String message, final android.webkit.JsResult result) 
				{  
					new AlertDialog.Builder( thisCtx )
						.setTitle( getResources().getString( R.string.app_name ) )
						.setMessage( message )
						.setPositiveButton( android.R.string.ok,
								new AlertDialog.OnClickListener()
								{  
									public void onClick( DialogInterface dialog, int which )
									{  
										result.confirm();  
									}  
								})
						.setCancelable( false )
						.create()
						.show(); 
					  
					return true;  
				};  
			} );

		// Load the default page
		m_wv.loadUrl( getResources().getString( R.string.root_url ) );

	}

}
