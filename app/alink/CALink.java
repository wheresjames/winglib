
package winglib.app.alink;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;

public class CALink extends Activity
{
    //--------------------------------------------------------------
	// onCreate()
    //--------------------------------------------------------------
    @Override public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );

        TextView  tv = new TextView( this );
        tv.setText( getString() );
        setContentView( tv );

    }

	/// Native function
    public native String getString();

    static 
	{
        System.loadLibrary( "alink" );
    }
}
