 

#include "_gbmath.h"

namespace gbmath
{

 
model_view_camera::model_view_camera()
{
    m_mWorld.reset();  
    m_mModelRot.reset();  
    m_mModelLastRot.reset();  
    m_mCameraRotLast.reset();  
    m_vModelCenter = vec3(0,0,0);
    m_fRadius    = 5.0f;
    m_fDefaultRadius = 5.0f;
    m_fMinRadius = 1.0f;
    m_fMaxRadius = FLT_MAX;
    m_bLimitPitch = false;
    m_bEnablePositionMovement = false;
    m_bAttachCameraToModel = false;

    m_nRotateModelButtonMask  = MOUSE_LEFT_BUTTON;
    m_nZoomButtonMask         = MOUSE_WHEEL;
    m_nRotateCameraButtonMask = MOUSE_RIGHT_BUTTON;
    m_bDragSinceLastUpdate    = true;
}


// Update the view matrix & the model's world matrix based 
//       on user input & elapsed time
void model_view_camera::frameMove( float fElapsedTime )
{
    if( isKeyDown(m_aKeys[CAM_RESET]) )
        reset();

    // If no dragged has happend since last time FrameMove is called,
    // and no camera key is held down, then no need to handle again.
    if( !m_bDragSinceLastUpdate && 0 == m_cKeysDown )
        return;
    m_bDragSinceLastUpdate = false;

    // Get keyboard/mouse/gamepad input
    getInput( m_bEnablePositionMovement, m_nCurrentButtonMask != 0, true, false );

    // Get amount of velocity based on the keyboard input and drag (if any)
    updateVelocity( fElapsedTime );

    // Simple euler method to calculate position delta
    vec3 vPosDelta = m_vVelocity * fElapsedTime;

    // Change the radius from the camera to the model based on wheel scrolling
    if( m_nMouseWheelDelta && m_nZoomButtonMask == MOUSE_WHEEL )
        m_fRadius -= m_nMouseWheelDelta * m_fRadius * 0.1f;
    m_fRadius = __min( m_fMaxRadius, m_fRadius );
    m_fRadius = __max( m_fMinRadius, m_fRadius );
    m_nMouseWheelDelta = 0;

    // Get the inverse of the arcball's rotation matrix
    mat44 mCameraRot;
	mCameraRot = m_ViewArcBall.getRotationMatrix().inverted();


	// Transform vectors based on camera's rotation matrix
	vec3 vWorldUp, vWorldAhead;
	vec3 vLocalUp    = vec3(0,1,0);
	vec3 vLocalAhead = vec3(0,0,1);

 
	vWorldUp = vLocalUp;
	vWorldUp.transform_coord(mCameraRot);

 
	vWorldAhead =   vLocalAhead;
	vWorldAhead.transform_coord(mCameraRot);



    // Transform the position delta by the camera's rotation 
    vec3 vPosDeltaWorld;

	vPosDeltaWorld = vPosDelta;
	vPosDeltaWorld.transform_coord(mCameraRot);



    // Move the lookAt position 
    m_vLookAt += vPosDeltaWorld;
    if( m_bClipToBoundary )
        constrainToBoundary( &m_vLookAt );

    // Update the eye point based on a radius away from the lookAt position
    m_vEye = m_vLookAt - vWorldAhead * m_fRadius;

    // Update the view matrix
 
	if(m_bcameraLeftHandle)
	{
		m_mView.setViewLookAtLH(m_vEye, m_vLookAt, vWorldUp);
	}
	else
	{
		m_mView.setViewLookAtRH(m_vEye, m_vLookAt, vWorldUp);
	}


	mat44 mInvView;
 
	mInvView = m_mView.inverted();

	mInvView._41 = mInvView._42 = mInvView._43 = 0;


    mat44 mModelLastRotInv;
	mModelLastRotInv =	m_mModelLastRot.inverted();

    // Accumulate the delta of the arcball's rotation in view space.
    // Note that per-frame delta rotations could be problematic over long periods of time.
    mat44 mModelRot;
    mModelRot = m_WorldArcBall.getRotationMatrix();
    m_mModelRot *= m_mView * mModelLastRotInv * mModelRot * mInvView;

    if( m_ViewArcBall.isBeingDragged() && m_bAttachCameraToModel && !isKeyDown(m_aKeys[CAM_CONTROLDOWN]) )
    {

		// Attach camera to model by inverse of the model rotation
		mat44 mCameraLastRotInv;
		mCameraLastRotInv = m_mCameraRotLast.inverted();

		mat44 mCameraRotDelta = mCameraLastRotInv * mCameraRot; // local to world matrix
		m_mModelRot *= mCameraRotDelta;
    }
	
    m_mCameraRotLast = mCameraRot; 
    m_mModelLastRot = mModelRot;

    // Since we're accumulating delta rotations, we need to orthonormalize 
    // the matrix to prevent eventual matrix skew
    vec3* pXBasis = (vec3*) &m_mModelRot._11;
    vec3* pYBasis = (vec3*) &m_mModelRot._21;
    vec3* pZBasis = (vec3*) &m_mModelRot._31;

	pXBasis->normalize();
	*pYBasis = pZBasis->cross(*pXBasis);
	pYBasis->normalize();
	*pZBasis = pXBasis->cross(  *pYBasis  );


    // Translate the rotation matrix to the same position as the lookAt position
    m_mModelRot._41 = m_vLookAt.x;
    m_mModelRot._42 = m_vLookAt.y;
    m_mModelRot._43 = m_vLookAt.z;

    // Translate world matrix so its at the center of the model
    mat44 mTrans;
	mTrans.setTranslation( -m_vModelCenter.x, -m_vModelCenter.y, -m_vModelCenter.z   );

    m_mWorld = mTrans * m_mModelRot;
}


void model_view_camera::setDragRect( RECT &rc )
{
    base_camera::setDragRect( rc );

    m_WorldArcBall.setOffset( rc.left, rc.top );
    m_ViewArcBall.setOffset( rc.left, rc.top );
    setWindow( rc.right - rc.left, rc.bottom - rc.top );
}



// Reset the camera's position back to the default
void model_view_camera::reset()
{
	base_camera::reset();

	m_mWorld.reset(); //  D 3 DXMatrixIdentity( m_mWorld );
	m_mModelRot.reset(); //  D 3 DXMatrixIdentity( m_mModelRot );
	m_mModelLastRot.reset(); //  D 3 DXMatrixIdentity( m_mModelLastRot );    
	m_mCameraRotLast.reset(); //  D 3 DXMatrixIdentity( m_mCameraRotLast );    

	m_fRadius = m_fDefaultRadius;
	m_WorldArcBall.reset();
	m_ViewArcBall.reset();
}


// Override for setting the view parameters
void model_view_camera::setViewParams( const vec3& pvEyePt, const vec3& pvLookatPt )
{
	base_camera::setViewParams( pvEyePt, pvLookatPt );

	// Propogate changes to the member arcball
	Quaternion quat;
	mat44 mRotation;
	vec3 vUp(0,1,0);


	if (m_bcameraLeftHandle)
	{
		mRotation.setViewLookAtLH(  pvEyePt, pvLookatPt, vUp    );
	}
	else
	{
		mRotation.setViewLookAtRH(  pvEyePt, pvLookatPt, vUp    );
	}


	quat.setRotationMatrix(mRotation);

	m_ViewArcBall.setQuatNow( quat );

	// Set the radius according to the distance
	vec3 vEyeToPoint;
	//#pragma message("ks777::devhelp::model_view_camera::setViewParams  �������� �����������"   __FILE__)
	vEyeToPoint =  pvEyePt -   pvLookatPt;

	setRadius( vEyeToPoint.length()  );   

	// View information changed. FrameMove should be called.
	m_bDragSinceLastUpdate = true;
}


// Call this from your message proc so this class can handle window messages
LRESULT model_view_camera::handleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    base_camera::handleMessages( hWnd, uMsg, wParam, lParam );

    if( ( (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK ) && m_nRotateModelButtonMask & MOUSE_LEFT_BUTTON) ||
        ( (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK ) && m_nRotateModelButtonMask & MOUSE_MIDDLE_BUTTON) ||
        ( (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK ) && m_nRotateModelButtonMask & MOUSE_RIGHT_BUTTON) )
    {
        int iMouseX = (short)LOWORD(lParam);
        int iMouseY = (short)HIWORD(lParam);
        m_WorldArcBall.onBegin( iMouseX, iMouseY );
    }

    if( ( (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK ) && m_nRotateCameraButtonMask & MOUSE_LEFT_BUTTON) ||
        ( (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK ) && m_nRotateCameraButtonMask & MOUSE_MIDDLE_BUTTON) ||
        ( (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK ) && m_nRotateCameraButtonMask & MOUSE_RIGHT_BUTTON) )
    {
        int iMouseX = (short)LOWORD(lParam);
        int iMouseY = (short)HIWORD(lParam);
        m_ViewArcBall.onBegin( iMouseX, iMouseY );
    }

    if( uMsg == WM_MOUSEMOVE )
    {
        int iMouseX = (short)LOWORD(lParam);
        int iMouseY = (short)HIWORD(lParam);
        m_WorldArcBall.onMove( iMouseX, iMouseY );
        m_ViewArcBall.onMove( iMouseX, iMouseY );
    }

    if( (uMsg == WM_LBUTTONUP && m_nRotateModelButtonMask & MOUSE_LEFT_BUTTON) ||
        (uMsg == WM_MBUTTONUP && m_nRotateModelButtonMask & MOUSE_MIDDLE_BUTTON) ||
        (uMsg == WM_RBUTTONUP && m_nRotateModelButtonMask & MOUSE_RIGHT_BUTTON) )
    {
        m_WorldArcBall.onEnd();
    }

    if( (uMsg == WM_LBUTTONUP && m_nRotateCameraButtonMask & MOUSE_LEFT_BUTTON) ||
        (uMsg == WM_MBUTTONUP && m_nRotateCameraButtonMask & MOUSE_MIDDLE_BUTTON) ||
        (uMsg == WM_RBUTTONUP && m_nRotateCameraButtonMask & MOUSE_RIGHT_BUTTON) )
    {
        m_ViewArcBall.onEnd();
    }

    if( uMsg == WM_CAPTURECHANGED )
    {
        if( (HWND)lParam != hWnd )
        {
            if( (m_nRotateModelButtonMask & MOUSE_LEFT_BUTTON) ||
                (m_nRotateModelButtonMask & MOUSE_MIDDLE_BUTTON) ||
                (m_nRotateModelButtonMask & MOUSE_RIGHT_BUTTON) )
            {
                m_WorldArcBall.onEnd();
            }
        
            if( (m_nRotateCameraButtonMask & MOUSE_LEFT_BUTTON) ||
                (m_nRotateCameraButtonMask & MOUSE_MIDDLE_BUTTON) ||
                (m_nRotateCameraButtonMask & MOUSE_RIGHT_BUTTON) )
            {
                m_ViewArcBall.onEnd();
            }
        }
    }

    if( uMsg == WM_LBUTTONDOWN ||
        uMsg == WM_LBUTTONDBLCLK ||
        uMsg == WM_MBUTTONDOWN ||
        uMsg == WM_MBUTTONDBLCLK ||
        uMsg == WM_RBUTTONDOWN ||
        uMsg == WM_RBUTTONDBLCLK ||
        uMsg == WM_LBUTTONUP ||
        uMsg == WM_MBUTTONUP ||
        uMsg == WM_RBUTTONUP ||
        uMsg == WM_MOUSEWHEEL ||
        uMsg == WM_MOUSEMOVE )
    {
        m_bDragSinceLastUpdate = true;
    }

    return FALSE;
}



}

