
package winglib.app.helloworld;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;

public class HelloWorld extends Activity
{
    //--------------------------------------------------------------
	// onCreate()
    //--------------------------------------------------------------
    @Override public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );

        TextView  tv = new TextView( this );
        tv.setText( getString() );
		// tv.setText( "Hello World! - Love java" );
        setContentView( tv );

    }

	/// Native function
    public native String getString();

    static 
	{
        System.loadLibrary( "hello_android" );
    }
}
