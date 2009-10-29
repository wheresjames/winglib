// stdafx.cpp

#include "stdafx.h"

// CSqIrrlicht
SQBIND_REGISTER_CLASS_BEGIN( CSqIrrlicht, CSqIrrlicht )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, SetCallback )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, Init )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, Draw )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddSkyDome )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, SetStereo )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, GetStereo )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, SetREyeKey )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, GetREyeKey )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, SetLEyeKey )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, GetLEyeKey )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, SetAmbientLight )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddLight )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddCamera )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddCube )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddMesh )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddSphere )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddPlane )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, LoadTexture )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, CreateTexture )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddRotateAnimator )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddGrid )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddMeshAnimator )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, ScreenToPlane )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, NodeAtScreen )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, getLineIntersect )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, getIntersect )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, getMouseIntersect )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, getPickPoint )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, getNodeScreenPos )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, getScreenPos )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, screenToWorld )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, screenToWorldBox )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, screenToPlane )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, FillVolume )

	SQBIND_CONST( irr, EMIE_LMOUSE_PRESSED_DOWN )
	SQBIND_CONST( irr, EMIE_LMOUSE_LEFT_UP )
	SQBIND_CONST( irr, EMIE_MMOUSE_PRESSED_DOWN )
	SQBIND_CONST( irr, EMIE_MMOUSE_LEFT_UP )
	SQBIND_CONST( irr, EMIE_RMOUSE_PRESSED_DOWN )
	SQBIND_CONST( irr, EMIE_RMOUSE_LEFT_UP )
	SQBIND_CONST( irr, EMIE_MOUSE_MOVED )
	SQBIND_CONST( irr, EMIE_MOUSE_WHEEL )

SQBIND_REGISTER_CLASS_END()

// CSqirrNode
SQBIND_REGISTER_CLASS_BEGIN( CSqirrNode, CSqirrNode )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, IsValid )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, AddAnimator )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, SetPosition )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, GetPosition )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, FaceTarget )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, FaceTargetY )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, FaceTargetX )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, Move )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, GetExtent )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, GetAbsoluteExtent )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, SetScale )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, GetScale )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, SetMaterialFlag )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, SetMaterialType )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, SetTexture )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, SetParent )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, SetVisible )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, IsVisible )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, SetDebugDataVisible )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, SetID )
	SQBIND_MEMBER_FUNCTION( CSqirrNode, GetID )


	// Material flags
	SQBIND_CONST( irr::video, EMF_WIREFRAME )
	SQBIND_CONST( irr::video, EMF_POINTCLOUD )
	SQBIND_CONST( irr::video, EMF_GOURAUD_SHADING )
	SQBIND_CONST( irr::video, EMF_LIGHTING )
	SQBIND_CONST( irr::video, EMF_ZBUFFER )
	SQBIND_CONST( irr::video, EMF_ZWRITE_ENABLE )
	SQBIND_CONST( irr::video, EMF_BACK_FACE_CULLING )
	SQBIND_CONST( irr::video, EMF_BILINEAR_FILTER )
	SQBIND_CONST( irr::video, EMF_TRILINEAR_FILTER )
	SQBIND_CONST( irr::video, EMF_ANISOTROPIC_FILTER )
	SQBIND_CONST( irr::video, EMF_FOG_ENABLE )
	SQBIND_CONST( irr::video, EMF_NORMALIZE_NORMALS )
	SQBIND_CONST( irr::video, EMF_TEXTURE_WRAP )
	SQBIND_CONST( irr::video, EMF_MATERIAL_FLAG_COUNT )

	// Material types
	SQBIND_CONST( irr::video, EMT_SOLID )
	SQBIND_CONST( irr::video, EMT_SOLID_2_LAYER )
	SQBIND_CONST( irr::video, EMT_LIGHTMAP )
	SQBIND_CONST( irr::video, EMT_LIGHTMAP_ADD )
	SQBIND_CONST( irr::video, EMT_LIGHTMAP_M2 )
	SQBIND_CONST( irr::video, EMT_LIGHTMAP_M4 )
	SQBIND_CONST( irr::video, EMT_LIGHTMAP_LIGHTING )
	SQBIND_CONST( irr::video, EMT_LIGHTMAP_LIGHTING_M2 )
	SQBIND_CONST( irr::video, EMT_LIGHTMAP_LIGHTING_M4 )
	SQBIND_CONST( irr::video, EMT_DETAIL_MAP )
	SQBIND_CONST( irr::video, EMT_SPHERE_MAP )
	SQBIND_CONST( irr::video, EMT_REFLECTION_2_LAYER )
	SQBIND_CONST( irr::video, EMT_TRANSPARENT_ADD_COLOR )
	SQBIND_CONST( irr::video, EMT_TRANSPARENT_ALPHA_CHANNEL )
	SQBIND_CONST( irr::video, EMT_TRANSPARENT_ALPHA_CHANNEL_REF )
	SQBIND_CONST( irr::video, EMT_TRANSPARENT_VERTEX_ALPHA )
	SQBIND_CONST( irr::video, EMT_TRANSPARENT_REFLECTION_2_LAYER )
	SQBIND_CONST( irr::video, EMT_NORMAL_MAP_SOLID )
	SQBIND_CONST( irr::video, EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR )
	SQBIND_CONST( irr::video, EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA )
	SQBIND_CONST( irr::video, EMT_PARALLAX_MAP_SOLID )
	SQBIND_CONST( irr::video, EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR )
	SQBIND_CONST( irr::video, EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA )
	SQBIND_CONST( irr::video, EMT_ONETEXTURE_BLEND )

	// Debug data
	SQBIND_CONST( irr::scene, EDS_BBOX )
	SQBIND_CONST( irr::scene, EDS_BBOX_BUFFERS )
	SQBIND_CONST( irr::scene, EDS_FULL )
	SQBIND_CONST( irr::scene, EDS_HALF_TRANSPARENCY )
	SQBIND_CONST( irr::scene, EDS_MESH_WIRE_OVERLAY )
	SQBIND_CONST( irr::scene, EDS_NORMALS )
	SQBIND_CONST( irr::scene, EDS_OFF )
	SQBIND_CONST( irr::scene, EDS_SKELETON )

SQBIND_REGISTER_CLASS_END()

// CSqirrCamera
SQBIND_REGISTER_CLASS_BEGIN( CSqirrCamera, CSqirrCamera )
	SQBIND_MEMBER_FUNCTION( CSqirrCamera, UpdateCamera )
	SQBIND_MEMBER_FUNCTION( CSqirrCamera, SetTarget )
	SQBIND_MEMBER_FUNCTION( CSqirrCamera, GetTarget )
	SQBIND_MEMBER_FUNCTION( CSqirrCamera, SyncTargetToRotation )
	SQBIND_MEMBER_FUNCTION( CSqirrCamera, SetFov )
	SQBIND_MEMBER_FUNCTION( CSqirrCamera, SetAspectRatio )
	SQBIND_MEMBER_FUNCTION( CSqirrCamera, SetLens )
	SQBIND_MEMBER_FUNCTION( CSqirrCamera, SetFarValue )
	SQBIND_MEMBER_FUNCTION( CSqirrCamera, SetNearValue )
SQBIND_REGISTER_CLASS_END()

// CSqirrAnimator
SQBIND_REGISTER_CLASS_BEGIN( CSqirrAnimator, CSqirrAnimator )
SQBIND_REGISTER_CLASS_END()

// CSqirrTexture
SQBIND_REGISTER_CLASS_BEGIN( CSqirrTexture, CSqirrTexture )
	SQBIND_MEMBER_FUNCTION( CSqirrTexture, GetWidth )
	SQBIND_MEMBER_FUNCTION( CSqirrTexture, GetHeight )
	SQBIND_MEMBER_FUNCTION( CSqirrTexture, Lock )
	SQBIND_MEMBER_FUNCTION( CSqirrTexture, Unlock )
SQBIND_REGISTER_CLASS_END()

// CSqirrFont
SQBIND_REGISTER_CLASS_BEGIN( CSqirrFont, CSqirrFont )
	SQBIND_MEMBER_FUNCTION( CSqirrFont, set )
	SQBIND_MEMBER_FUNCTION( CSqirrFont, Render )
SQBIND_REGISTER_CLASS_END()

// CSqirrColor
SQBIND_REGISTER_CLASS_BEGIN( CSqirrColor, CSqirrColor )
	SQBIND_MEMBER_FUNCTION( CSqirrColor, r )
	SQBIND_MEMBER_FUNCTION( CSqirrColor, g )
	SQBIND_MEMBER_FUNCTION( CSqirrColor, b )
	SQBIND_MEMBER_FUNCTION( CSqirrColor, a )
SQBIND_REGISTER_CLASS_END()

// CSqirrColorf
SQBIND_REGISTER_CLASS_BEGIN( CSqirrColorf, CSqirrColorf )
	SQBIND_MEMBER_FUNCTION( CSqirrColorf, r )
	SQBIND_MEMBER_FUNCTION( CSqirrColorf, g )
	SQBIND_MEMBER_FUNCTION( CSqirrColorf, b )
	SQBIND_MEMBER_FUNCTION( CSqirrColorf, a )
SQBIND_REGISTER_CLASS_END()

// CSqirrVector3d
SQBIND_REGISTER_CLASS_BEGIN( CSqirrVector3d, CSqirrVector3d )
	SQBIND_MEMBER_FUNCTION( CSqirrVector3d, x )
	SQBIND_MEMBER_FUNCTION( CSqirrVector3d, y )
	SQBIND_MEMBER_FUNCTION( CSqirrVector3d, z )
	SQBIND_MEMBER_FUNCTION( CSqirrVector3d, set )
	SQBIND_MEMBER_FUNCTION( CSqirrVector3d, inflate )
SQBIND_REGISTER_CLASS_END()

// CSqirrVector2d
SQBIND_REGISTER_CLASS_BEGIN( CSqirrVector2d, CSqirrVector2d )
	SQBIND_MEMBER_FUNCTION( CSqirrVector2d, x )
	SQBIND_MEMBER_FUNCTION( CSqirrVector2d, y )
	SQBIND_MEMBER_FUNCTION( CSqirrVector2d, set )
	SQBIND_MEMBER_FUNCTION( CSqirrVector2d, inflate )
SQBIND_REGISTER_CLASS_END()

// CSqirrBoundingBox3d
SQBIND_REGISTER_CLASS_BEGIN( CSqirrBoundingBox3d, CSqirrBoundingBox3d )
	SQBIND_MEMBER_FUNCTION( CSqirrBoundingBox3d, top )
	SQBIND_MEMBER_FUNCTION( CSqirrBoundingBox3d, left )
	SQBIND_MEMBER_FUNCTION( CSqirrBoundingBox3d, front )
	SQBIND_MEMBER_FUNCTION( CSqirrBoundingBox3d, right )
	SQBIND_MEMBER_FUNCTION( CSqirrBoundingBox3d, bottom )
	SQBIND_MEMBER_FUNCTION( CSqirrBoundingBox3d, back )
	SQBIND_MEMBER_FUNCTION( CSqirrBoundingBox3d, set )
	SQBIND_MEMBER_FUNCTION( CSqirrBoundingBox3d, inflate )
SQBIND_REGISTER_CLASS_END()

// CSqirrRect2d
SQBIND_REGISTER_CLASS_BEGIN( CSqirrRect2d, CSqirrRect2d )
	SQBIND_MEMBER_FUNCTION( CSqirrRect2d, top )
	SQBIND_MEMBER_FUNCTION( CSqirrRect2d, left )
	SQBIND_MEMBER_FUNCTION( CSqirrRect2d, right )
	SQBIND_MEMBER_FUNCTION( CSqirrRect2d, bottom )
	SQBIND_MEMBER_FUNCTION( CSqirrRect2d, lt )
	SQBIND_MEMBER_FUNCTION( CSqirrRect2d, rb )
	SQBIND_MEMBER_FUNCTION( CSqirrRect2d, set )
	SQBIND_MEMBER_FUNCTION( CSqirrRect2d, inflate )
SQBIND_REGISTER_CLASS_END()


// CSqirrLine
SQBIND_REGISTER_CLASS_BEGIN( CSqirrLine, CSqirrLine )
	SQBIND_MEMBER_FUNCTION( CSqirrLine, start )
	SQBIND_MEMBER_FUNCTION( CSqirrLine, end )
	SQBIND_MEMBER_FUNCTION( CSqirrLine, set )
	SQBIND_MEMBER_FUNCTION( CSqirrLine, inflate )
	SQBIND_MEMBER_FUNCTION( CSqirrLine, FromPointAndRotation )
	SQBIND_MEMBER_FUNCTION( CSqirrLine, GetRotation )
	SQBIND_MEMBER_FUNCTION( CSqirrLine, GetVector )
	SQBIND_MEMBER_FUNCTION( CSqirrLine, GetNormal )
SQBIND_REGISTER_CLASS_END()

// CSqirrVectorArray
SQBIND_REGISTER_CLASS_BEGIN( CSqirrVertexArray, CSqirrVertexArray )
	SQBIND_MEMBER_FUNCTION( CSqirrVertexArray, x )
	SQBIND_MEMBER_FUNCTION( CSqirrVertexArray, y )
	SQBIND_MEMBER_FUNCTION( CSqirrVertexArray, z )
	SQBIND_MEMBER_FUNCTION( CSqirrVertexArray, set )
	SQBIND_MEMBER_FUNCTION( CSqirrVertexArray, inflate )
SQBIND_REGISTER_CLASS_END()

// CSqirrAnimator
SQBIND_REGISTER_CLASS_BEGIN( CSqirrPhysics, CSqirrPhysics )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_irrlicht( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqirrColor );
    SQBIND_EXPORT( x_vm, CSqirrColorf );
    SQBIND_EXPORT( x_vm, CSqirrVector3d );
    SQBIND_EXPORT( x_vm, CSqirrVector2d );
    SQBIND_EXPORT( x_vm, CSqirrVertexArray );
    SQBIND_EXPORT( x_vm, CSqirrBoundingBox3d );
    SQBIND_EXPORT( x_vm, CSqirrRect2d );

    SQBIND_EXPORT( x_vm, CSqirrAnimator );
    SQBIND_EXPORT( x_vm, CSqirrTexture );
    SQBIND_EXPORT( x_vm, CSqirrFont );

    SQBIND_EXPORT( x_vm, CSqirrNode );
    SQBIND_EXPORT( x_vm, CSqirrCamera );

    SQBIND_EXPORT( x_vm, CSqIrrlicht );
}

#if defined( SQBIND_STATIC )
	#include "sqirr_font.cpp"
	#include "sq_irrlicht.cpp"
	#include "sqirr_mesh_animator.cpp"
	#include "sq_physics.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_irrlicht( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
