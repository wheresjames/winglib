#pragma once

class CSqirrMeshAnimator
{
public:

    /// Default constructor
    CSqirrMeshAnimator(void);

    /// Destructor
    ~CSqirrMeshAnimator(void);

    /// Release resources
    void Destroy();

    /// Detaches without restoring original positions
    void Detach();

    /// Restores original vertex positions
    int Restore();

    /// Returns vertex buffers and size
    int GetMetrics( irr::video::S3DVertex **pOriginal, irr::video::S3DVertex **pCurrent, unsigned int &uPoints, unsigned int *puPitch = NULL );

    /// Initializes
    int Init( irr::scene::IMeshSceneNode *pNode );

    /// Initializes with callback function
    int Init( irr::scene::IMeshSceneNode *pNode, SquirrelObject &soF, HSQUIRRELVM hVm, long lSpeed )
    {
        if ( !Init( pNode ) )
            return 0;

        m_psoCallback = new SquirrelObject( hVm );
        if ( !m_psoCallback )
            return 0;

        *m_psoCallback = soF;

        m_lSpeed = lSpeed;

        return 1;
    }

    /// Runs the animator
    int Run( irr::scene::ISceneManager *pSm, float fClock );

    /// Process the vertex buffers the wave function
    virtual int ProcessVertices( irr::scene::ISceneManager *pSm, float fClock,
                                  irr::video::S3DVertex *pO, irr::video::S3DVertex *pV, unsigned int uPoints )
    {   return 0; }

    /// Execute squirrel process
    int Run( irr::scene::ISceneManager *pSm, float fClock, sqbind::CSqEngine *pSe );

private:

    /// Original vertex positions
    irr::core::array< irr::video::S3DVertex >   m_vertOriginal;

    /// The node we're modifying
    irr::scene::IMeshSceneNode                  *m_pNode;

    /// Squirrel callback function
    SquirrelObject                              *m_psoCallback;

	/// Pointer to engine handling the callback
	sqbind::CSqEngine 							*m_pSe;

    /// Speed
    long                                        m_lSpeed;

    /// Tick count
    long                                        m_lTick;

};
