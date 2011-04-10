// sq_webkit.h

// http://trac.webkit.org/browser/trunk/WebKitTools/WinLauncher
// http://siphon9.net/loune/2009/07/compiling-webkitcairo-on-windows-with-visual-c-express/

#pragma once

class CSqWebkit
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqWebkit )
	_SQBIND_CLASS_CTOR_END( CSqWebkit )

	/// Default constructor
	CSqWebkit();

	/// Destructor
	~CSqWebkit();

	/** \addtogroup CSqWebkit
		@{
	*/

	/// Releases resources
	void Destroy();

	/** @} */

private:


};
SQBIND_DECLARE_INSTANCE( CSqWebkit, CSqWebkit );

