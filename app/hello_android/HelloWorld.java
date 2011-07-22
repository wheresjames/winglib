
package winglib.app.hello_world;

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
		tv.setText( "Hello World!" );
        setContentView( tv );

    }

	/// Native function
    public native String getString();

    /* this is used to load the 'hello-jni' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.HelloJni/lib/libhello-jni.so at
     * installation time by the package manager.
     */
    static 
	{
        System.loadLibrary( "hello-world" );
    }
}
