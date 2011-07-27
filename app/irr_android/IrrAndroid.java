
package winglib.app.irrandroid;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;

public class IrrAndroid extends Activity
{
    //--------------------------------------------------------------
	// onCreate()
    //--------------------------------------------------------------
    @Override public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );
        TextView  tv = new TextView( this );
        setContentView( tv );

    }

	/// Native function
    public native String nativeInit();

    static 
	{
        System.loadLibrary( "irr_android" );
    }
}
