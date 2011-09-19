
package winglib.app.irrandroid;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class IrrAndroid extends Activity
{
    //--------------------------------------------------------------
	// onCreate()
    //--------------------------------------------------------------
    @Override public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );
        mGLView = new GLSurfaceView( this );
        mGLView.setRenderer( new IrrRenderer() );
        setContentView( mGLView );
    }

    @Override
    protected void onPause() 
	{
        super.onPause();
        mGLView.onPause();
    }

    @Override
    protected void onResume() 
	{
        super.onResume();
        mGLView.onResume();
    }

    static 
	{
        System.loadLibrary( "irr_android" );
    }
	
	private GLSurfaceView mGLView;
}

class IrrRenderer implements GLSurfaceView.Renderer 
{
    public void onSurfaceCreated( GL10 gl, EGLConfig config )
	{
		nativeInit();
    }

    public void onSurfaceChanged( GL10 gl, int w, int h )
	{
        gl.glViewport( 0, 0, w, h );
    }

    public void onDrawFrame( GL10 gl )
	{
//        gl.glClear( GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT );
		nativeDraw();
    }

	/// Native function
    public native String nativeInit();
    public native String nativeDraw();

}
