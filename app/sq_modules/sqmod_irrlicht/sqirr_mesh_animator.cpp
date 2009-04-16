
#include "stdafx.h"

CSqirrMeshAnimator::CSqirrMeshAnimator(void)
{
    m_pNode = 0;
    m_psoCallback = 0;
    m_lTick = 0;
    m_lSpeed = 0;
    m_pSe = 0;
}

CSqirrMeshAnimator::~CSqirrMeshAnimator(void)
{   Destroy();
}

void CSqirrMeshAnimator::Destroy()
{
    Restore();
    Detach();
}

void CSqirrMeshAnimator::Detach()
{
    m_pNode = 0;
    m_lTick = 0;
    m_lSpeed = 0;
    m_pSe = 0;
    m_vertOriginal.clear();
    if ( m_psoCallback )
    {   delete m_psoCallback;
        m_psoCallback = 0;
    } // end if
}

int CSqirrMeshAnimator::GetMetrics( irr::video::S3DVertex **pOriginal, irr::video::S3DVertex **pCurrent, unsigned int &uPoints, unsigned int *puPitch )
{
    // Sanity checks
    if ( !m_pNode || !m_pNode->getMesh() || !m_pNode->getMesh()->getMeshBufferCount() )
        return 0;

    // Get the vertex buffer object
    irr::scene::IMeshBuffer *pMb = m_pNode->getMesh()->getMeshBuffer( 0 );
    if ( !pMb )
        return 0;

    // Get vertex buffer count
    uPoints = pMb->getVertexCount();
    if ( !uPoints )
        return 0;

    // Get the pitch
    if ( puPitch )
//        *puPitch = pMb->getVertexPitch();
        *puPitch = irr::video::getVertexPitchFromType( pMb->getVertexType() );

    // Original position buffer
    if ( pOriginal )
    {   *pOriginal = (irr::video::S3DVertex*)m_vertOriginal.pointer();
        if ( !*pOriginal )
            return 0;
    } // end if

    // Current vertex buffer
    if ( pCurrent )
    {   *pCurrent = (irr::video::S3DVertex*)pMb->getVertices();
        if ( !*pCurrent )
            return 0;
    } // end if

    return 1;
}

int CSqirrMeshAnimator::Restore()
{
    unsigned int uV = 0, uPitch = 0;
    irr::video::S3DVertex *pO = 0, *pV = 0;
    if ( !GetMetrics( &pO, &pV, uV, &uPitch ) )
        return 0;

    // Copy the vertex positions
    memcpy( pV, pO, uPitch * uV );

    return 1;
}

int CSqirrMeshAnimator::Init( irr::scene::IMeshSceneNode *pNode )
{
    // Lose the old thing
    Destroy();

    // Ensure mesh buffer params
    if ( !pNode || !pNode->getMesh() || !pNode->getMesh()->getMeshBufferCount() )
        return 0;

    unsigned int uV = pNode->getMesh()->getMeshBuffer( 0 )->getVertexCount();
    if ( !uV )
        return 0;

    // Save node pointer
    m_pNode = pNode;
    m_vertOriginal.set_used( uV );

    // Get buffer data
    unsigned int uPitch = 0;
    irr::video::S3DVertex *pO = 0, *pV = 0;
    if ( !GetMetrics( &pO, &pV, uV, &uPitch ) )
    {   Destroy(); return 0; }

    // Copy the vertex positions
    memcpy( pO, pV, uPitch * uV );

    return 1;
}

int CSqirrMeshAnimator::Run( irr::scene::ISceneManager *pSm, float fClock )
{
    // Speed check
    if ( m_lSpeed && m_lTick )
    {
        if ( --m_lTick )
            return 1;
        else
            m_lTick = m_lSpeed;

    } // end if

    unsigned int uV = 0, uPitch = 0;
    irr::video::S3DVertex *pO = 0, *pV = 0;
    if ( !GetMetrics( &pO, &pV, uV, &uPitch ) )
        return 0;

    // Process the vertices
    ProcessVertices( pSm, fClock, pO, pV, uV );

    pSm->getMeshManipulator()->recalculateNormals( m_pNode->getMesh()->getMeshBuffer( 0 ), false );
//    pSm->getMeshManipulator()->recalculateNormals( m_pNode->getMesh()->getMeshBuffer( 0 ), true );

    return 1;
}

int CSqirrMeshAnimator::Run( irr::scene::ISceneManager *pSm, float fClock, sqbind::CSqEngine *pSe )
{
	// Engine specified?
	if ( !pSe )
		pSe = m_pSe;
	else
		m_pSe = pSe;

    // Sanity check
    if ( !pSe || !m_psoCallback )
        return 0;

    unsigned int uV = 0, uPitch = 0;
    irr::video::S3DVertex *pO = 0, *pV = 0;
    if ( !GetMetrics( &pO, &pV, uV, &uPitch ) )
        return 0;

    // Process vertices
    pSe->Execute( *m_psoCallback, uV,
                  CSqirrVertexArray( pO, uV ),
                  CSqirrVertexArray( pV, uV ) );

    pSm->getMeshManipulator()->recalculateNormals( m_pNode->getMesh()->getMeshBuffer( 0 ), false );

    return 1;
}

